// WebUI C - Stress Test
//
// Interactive stress test suite. A status dashboard shows every stage
// against every mode, and each stage opens its own window(s) to stress
// one part of WebUI. Windows of passed stages stay open and connected,
// showing a green "TEST PASSED", and get closed when the mode ends.
//
// The whole suite runs in four modes:
//   1. Browser  + asynchronous events
//   2. Browser  + blocking events
//   3. WebView  + asynchronous events
//   4. WebView  + blocking events
//
// The browser modes run first. WebUI is then cleaned completely, and a
// second dashboard is opened as a WebView for the WebView modes, so
// `webui_wait()` runs the native UI loop those modes need. The run
// stops at the first failing stage.
//
// Usage:
//   ./main        Wait for the user to click Start
//   ./main auto   Start the test automatically
//
// Result files (created next to the executable):
//   test_pass.txt    every mode passed
//   test_error.txt   a stage failed

#include "webui.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#include <direct.h>
#else
#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>
#endif

#define STAGES 15
#define MODES 4
#define MAX_KEPT 64
#define BIG_ARG_LEN 262144
#define BIG_RUN_LEN 307200
#define BIG_SCRIPT_LEN 1048576

static size_t status_win = 0;

static volatile long g_start = 0;
static volatile long g_abort = 0;
static volatile long g_run_one = -1; // Index of a single stage to run
static volatile long g_busy = 0; // A stage is running right now
static volatile long g_exited = 0; // The user clicked Exit
static volatile long g_finished = 0; // 1 = all passed, 2 = a stage failed
static volatile long g_ready = 0;
static volatile long g_report = -1;
static volatile long g_bind_hits = 0;
static volatile long g_kamikaze = 0;

static volatile long g_stage = -1; // Stage being run, for progress reports
static volatile long g_prog_last = -1;
static unsigned long long g_stage_start = 0; // When the running stage began
static volatile long g_switch_mode = 0; // Reload the dashboard in the other mode
static volatile long g_auto_advance = 0; // The switch continues a full run
static volatile long g_mode = 0; // Mode being run, a column of the matrix
static volatile long g_phase = 0; // 0 = browser dashboard, 1 = webview dashboard

// Result of every stage in every mode, so the second phase can
// redraw what the first phase already did
static char g_results[MODES][STAGES];
static unsigned long long g_times[MODES][STAGES];

// Windows kept open by the passed stages of the running mode
static size_t g_kept[MAX_KEPT];
static size_t g_kept_count = 0;

static const char* mode_names[MODES] = {
	"Browser, async events",
	"Browser, blocking events",
	"WebView, async events",
	"WebView, blocking events"
};

static char page_buf[8192];
static char big_js[BIG_RUN_LEN + 128];
static char big_buf[BIG_SCRIPT_LEN + 128];

// The number in front of every stage is how many times it repeats
// the operation it tests
static const char* stage_names[STAGES] = {
	"x1 Dashboard connection",
	"x3 Open / close / reopen window",
	"x2000 C to JS calls (webui_run / webui_script)",
	"x401 JS to C calls and arguments",
	"x2 Large payloads (multi-packet)",
	"x40 Many bindings",
	"x8 Navigation / content reload",
	"x6 Multi-window churn",
	"x1 Port change reload (webui_set_port)",
	"x2 Live root folder change",
	"x1 Folder monitor auto-reload",
	"x24 Concurrent create / destroy",
	"x200 Window reclaim (create / destroy)",
	"x1 Destroy from callback",
	"x5 Rapid open / close"
};

static unsigned long long now_ms(void) {
	#ifdef _WIN32
	return (unsigned long long)GetTickCount64();
	#else
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return ((unsigned long long)ts.tv_sec * 1000ULL) + ((unsigned long long)ts.tv_nsec / 1000000ULL);
	#endif
}

static void sleep_ms(unsigned long ms) {
	#ifdef _WIN32
	Sleep((DWORD)ms);
	#else
	struct timespec ts;
	ts.tv_sec = (time_t)(ms / 1000);
	ts.tv_nsec = (long)((ms % 1000) * 1000000L);
	nanosleep(&ts, NULL);
	#endif
}

static bool wait_long(volatile long* v, long want, unsigned long timeout_ms) {
	unsigned long waited = 0;
	while (*v != want) {
		if (g_abort || waited >= timeout_ms)
			return false;
		sleep_ms(25);
		waited += 25;
	}
	return true;
}

static bool wait_at_least(volatile long* v, long want, unsigned long timeout_ms) {
	unsigned long waited = 0;
	while (*v < want) {
		if (g_abort || waited >= timeout_ms)
			return false;
		sleep_ms(25);
		waited += 25;
	}
	return true;
}

static bool wait_hidden(size_t w, unsigned long timeout_ms) {
	unsigned long waited = 0;
	while (webui_is_shown(w)) {
		if (g_abort || waited >= timeout_ms)
			return false;
		sleep_ms(25);
		waited += 25;
	}
	return true;
}

static void status_run(const char* fmt, ...) {
	char js[1024];
	va_list ap;
	va_start(ap, fmt);
	vsnprintf(js, sizeof(js), fmt, ap);
	va_end(ap);
	webui_run(status_win, js);
}

// Stage windows follow the mode: a browser window, or a WebView one.
// Server-only windows are not affected, they have no UI at all.
static bool mode_is_webview(void) {
	return (g_mode >= 2);
}

static bool show_stage_window(size_t w, const char* content) {
	if (mode_is_webview())
		return webui_show_browser(w, content, Webview);
	return webui_show(w, content);
}

// Stage windows are kept small, many of them are open at the same time
static void stage_window_size(size_t w) {
	if (w > 0)
		webui_set_size(w, 400, 800);
}

static void keep_window(size_t w) {
	if (g_kept_count < MAX_KEPT)
		g_kept[g_kept_count++] = w;
}

// Close the windows that the passed stages of this mode kept open,
// so every mode starts from a clean desktop
static void close_kept_windows(void) {
	for (size_t n = 0; n < g_kept_count; n++)
		webui_destroy(g_kept[n]);
	g_kept_count = 0;
}

static void stage_begin(int i) {
	printf("[%d/%d] %s...\n", i + 1, STAGES, stage_names[i]);
	fflush(stdout);
	g_stage = i;
	g_prog_last = -1;
	g_stage_start = now_ms();
	status_run("setCell(%ld,%d,'RUNNING','run');", (long)g_mode, i);
}

// Show how far a stage got, as a counter with a small bar. Stages
// without a known number of steps simply never call this, and keep
// showing RUNNING. Safe to call from any thread.
static void stage_progress(long done, long total) {
	if (g_stage < 0 || g_abort || total < 1)
		return;
	if (done == g_prog_last)
		return;
	g_prog_last = done;
	status_run("setProgress(%ld,%ld,%ld,%ld);", (long)g_mode, (long)g_stage, done, total);
}

static bool stage_end(int i, bool ok, const char* detail) {

	unsigned long long spent = now_ms() - g_stage_start;
	g_stage = -1;
	g_results[g_mode][i] = (ok ? 'P' : 'F');
	g_times[g_mode][i] = spent;

	char label[32];
	if (ok)
		snprintf(label, sizeof(label), "PASS %llu.%03llus", (spent / 1000ULL), (spent % 1000ULL));
	else
		snprintf(label, sizeof(label), "FAIL");

	printf("[%d/%d] %s -> %s in %llu.%03llu s (%s)\n", i + 1, STAGES, stage_names[i],
		ok ? "PASS" : "FAIL", (spent / 1000ULL), (spent % 1000ULL), detail);
	fflush(stdout);
	status_run("setCell(%ld,%d,'%s','%s');", (long)g_mode, i, label, ok ? "pass" : "fail");
	if (!ok)
		status_run("setBanner('%s -> %s: %s','fail');", mode_names[g_mode], stage_names[i], detail);
	return ok;
}

// Every stage page gets `ready(f)`: it runs `f` once, as soon as the
// connection to the backend is established. This is the connect pattern
// recommended by the WebUI documentation, plus a fallback for the case
// where the bridge connected before the callback got registered.
static const char* make_page(const char* title, const char* script) {
	snprintf(page_buf, sizeof(page_buf),
		"<html><head><script src=\"webui.js\"></script><style>"
		"body{font-family:sans-serif;background:#141824;color:#dde;padding:24px}"
		"h2{margin:0 0 8px;font-size:18px}#out{color:#8a92b2}"
		"</style></head><body><h2>%s</h2><div id=\"out\">Working...</div>"
		"<script>"
		"function ready(f){"
		"let done=false;"
		"const go=function(){if(!done){done=true;f();}};"
		"document.addEventListener('DOMContentLoaded',function(){"
		"if(typeof webui!=='undefined'){"
		"webui.setEventCallback(function(e){if(e==webui.event.CONNECTED)go();});"
		"if(webui.isConnected())go();"
		"}});}"
		"%s"
		"</script></body></html>",
		title, script);
	return page_buf;
}

// Turn a finished stage window into a big green "TEST PASSED" page.
// The page is not reloaded, so the WebSocket connection stays alive.
static void mark_passed(size_t w, const char* label) {
	keep_window(w);
	char js[640];
	snprintf(js, sizeof(js),
		"document.body.innerHTML=\"<div style='display:flex;height:90vh;align-items:center;"
		"justify-content:center;flex-direction:column'>"
		"<div style='font-size:44px;font-weight:800;color:#7ce38b'>TEST PASSED</div>"
		"<div style='margin-top:10px;font-size:15px;color:#9aa0b8'>%s</div>"
		"<div style='margin-top:4px;font-size:13px;color:#5b6180'>This window is still connected</div>"
		"</div>\";document.body.style.background='#0c2913';", label);
	webui_run(w, js);
}

static void cb_start(webui_event_t* e) {
	(void)e;
	if (g_busy)
		return;
	g_start = 1;
}

// -- Manual window control ------------------------------------------
// These actions change a window in a way only a human can judge, so
// the code just reports what the API returned: a boolean, or nothing
// at all for the functions that return void.

typedef struct {
	const char* label;
	bool returns_bool;
}
ctrl_action_t;

static const ctrl_action_t ctrl_actions[] = {
	{"show", true},
	{"show_webview", true},
	{"is_shown", true},
	{"close", false},
	{"destroy", false},
	{"set_center", false},
	{"minimize", false},
	{"maximize", false},
	{"set_size 200x200", false},
	{"set_size 400x400", false},
	{"set_size 800x600", false},
	{"set_position 0,0", false},
	{"set_position 400,200", false},
	{"set_minimum_size 300x300", false},
	{"set_hide true", false},
	{"set_hide false", false},
	{"focus", false},
	{"set_kiosk true", false},
	{"set_kiosk false", false},
	{"set_frameless true", false},
	{"set_frameless false", false},
	{"set_transparent true", false},
	{"set_transparent false", false},
	{"set_resizable true", false},
	{"set_resizable false", false},
	{"set_high_contrast true", false},
	{"set_high_contrast false", false},
	{"navigate (own URL)", false},
	{"set_icon (red dot)", false},
	{"set_port 33500", true}
};

#define CTRL_ACTIONS ((int)(sizeof(ctrl_actions) / sizeof(ctrl_actions[0])))

static size_t g_ctrl_win = 0;

static const char* ctrl_page =
	"<html><head><title>WebUI Manual Test Window</title>"
	"<script src=\"webui.js\"></script><style>"
	"body{font-family:sans-serif;background:#3a1f4d;color:#f0e6f7;padding:20px;margin:0}"
	"h2{margin:0 0 6px;font-size:17px}p{color:#c0a8d0;font-size:13px}"
	"</style></head><body><h2>Manual test window</h2>"
	"<p>This window belongs to the manual control panel only. The automatic "
	"stages never touch it. Pick an action in the dashboard and click Run.</p>"
	"</body></html>";

// The one and only window the manual control panel acts on. It is
// created with the dashboard, and is never used by any stage, so what
// happens to it is always the result of the action just requested.
static size_t ctrl_new_window(void) {
	g_ctrl_win = webui_new_window();
	webui_set_size(g_ctrl_win, 400, 300);
	webui_set_position(g_ctrl_win, 700, 250);
	return g_ctrl_win;
}

static size_t ctrl_window(void) {
	// Only ever recreated after the `destroy` action consumed it
	if (g_ctrl_win == 0)
		return ctrl_new_window();
	return g_ctrl_win;
}

static void ctrl_fill_actions(void) {
	status_run("ctrlClear();");
	for (int i = 0; i < CTRL_ACTIONS; i++)
		status_run("ctrlAdd('%s');", ctrl_actions[i].label);
}

static void cb_ctrl_run(webui_event_t* e) {

	int idx = (int)webui_get_int(e);
	if (idx < 0 || idx >= CTRL_ACTIONS)
		return;

	size_t w = ctrl_window();
	bool result = false;

	switch (idx) {
		case 0: result = show_stage_window(w, ctrl_page); break;
		case 1: result = webui_show_wv(w, ctrl_page); break;
		case 2: result = webui_is_shown(w); break;
		case 3: webui_close(w); break;
		case 4:
			webui_destroy(w);
			// Keep exactly one manual window ready at all times
			g_ctrl_win = 0;
			ctrl_new_window();
			break;
		case 5: webui_set_center(w); break;
		case 6: webui_minimize(w); break;
		case 7: webui_maximize(w); break;
		case 8: webui_set_size(w, 200, 200); break;
		case 9: webui_set_size(w, 400, 400); break;
		case 10: webui_set_size(w, 800, 600); break;
		case 11: webui_set_position(w, 0, 0); break;
		case 12: webui_set_position(w, 400, 200); break;
		case 13: webui_set_minimum_size(w, 300, 300); break;
		case 14: webui_set_hide(w, true); break;
		case 15: webui_set_hide(w, false); break;
		case 16: webui_focus(w); break;
		case 17: webui_set_kiosk(w, true); break;
		case 18: webui_set_kiosk(w, false); break;
		case 19: webui_set_frameless(w, true); break;
		case 20: webui_set_frameless(w, false); break;
		case 21: webui_set_transparent(w, true); break;
		case 22: webui_set_transparent(w, false); break;
		case 23: webui_set_resizable(w, true); break;
		case 24: webui_set_resizable(w, false); break;
		case 25: webui_set_high_contrast(w, true); break;
		case 26: webui_set_high_contrast(w, false); break;
		case 27: {
			const char* url = webui_get_url(w);
			if (url != NULL && url[0] != '\0')
				webui_navigate(w, url);
			break;
		}
		case 28: webui_set_icon(w,
			"<svg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 32 32'>"
			"<circle cx='16' cy='16' r='14' fill='#ff4050'/></svg>", "image/svg+xml");
			break;
		case 29: result = webui_set_port(w, 33500); break;
		default: break;
	}

	const char* text = "VOID";
	const char* cls = "ok";
	if (ctrl_actions[idx].returns_bool) {
		text = (result ? "TRUE" : "FALSE");
		cls = (result ? "ok" : "bad");
	}

	printf("[Manual] %s -> %s\n", ctrl_actions[idx].label, text);
	fflush(stdout);
	status_run("setCtrl('%s','%s');", text, cls);
}

static void cb_switch_mode(webui_event_t* e) {
	(void)e;
	if (g_busy)
		return;
	g_switch_mode = 1;
}

static void cb_run_stage(webui_event_t* e) {
	if (g_busy)
		return;
	long idx = (long)webui_get_int(e);
	if (idx >= 0 && idx < STAGES)
		g_run_one = idx;
}

static void cb_exit(webui_event_t* e) {
	(void)e;
	g_exited = 1;
	g_abort = 1;
	webui_exit();
}

static void cb_ready(webui_event_t* e) {
	g_ready = (long)webui_get_int(e);
}

static void cb_report(webui_event_t* e) {
	g_report = (long)webui_get_int(e);
}

static void cb_ready_inc(webui_event_t* e) {
	(void)e;
	g_ready++;
}

static void cb_report_char(webui_event_t* e) {
	const char* s = webui_get_string(e);
	g_report = (s != NULL && s[0] != '\0') ? (long)s[0] : -2;
}

static void cb_echo(webui_event_t* e) {
	webui_return_int(e, webui_get_int(e) * 2);
}

static void cb_big_arg(webui_event_t* e) {
	const char* s = webui_get_string(e);
	size_t len = (s != NULL ? strlen(s) : 0);
	webui_return_bool(e, (len == BIG_ARG_LEN && s[0] == 'B' && s[len - 1] == 'B'));
}

static void cb_bind_hit(webui_event_t* e) {
	g_bind_hits++;
	stage_progress(g_bind_hits, 40);
	webui_return_int(e, 1);
}

static void cb_progress(webui_event_t* e) {
	stage_progress((long)webui_get_int(e), 401);
}

static void cb_kamikaze(webui_event_t* e) {
	g_kamikaze = 1;
	webui_destroy(e->window);
}

static bool stage_dashboard(int i) {
	stage_begin(i);
	char detail[256];
	const char* url = webui_get_url(status_win);
	snprintf(detail, sizeof(detail), "Connected at %s", (url != NULL ? url : "?"));
	return stage_end(i, webui_is_shown(status_win), detail);
}

static bool stage_reopen(int i) {
	stage_begin(i);
	bool ok = true;
	char detail[256] = "3 open/close cycles, same URL every time";
	char first_url[256] = {0};
	size_t w = webui_new_window();
	stage_window_size(w);
	webui_bind(w, "stage_ready", cb_ready);
	for (int c = 0; c < 3 && ok; c++) {
		g_ready = 0;
		if (!show_stage_window(w, make_page("Reopen cycle", "ready(function(){webui.call('stage_ready',1);});"))) {
			snprintf(detail, sizeof(detail), "Show failed at cycle %d", c + 1);
			ok = false;
			break;
		}
		if (!wait_long(&g_ready, 1, 15000)) {
			snprintf(detail, sizeof(detail), "No page signal at cycle %d", c + 1);
			ok = false;
			break;
		}
		const char* url = webui_get_url(w);
		if (url == NULL || url[0] == '\0') {
			snprintf(detail, sizeof(detail), "Empty URL at cycle %d", c + 1);
			ok = false;
			break;
		}
		if (first_url[0] == '\0') {
			snprintf(first_url, sizeof(first_url), "%s", url);
		} else if (strcmp(first_url, url) != 0) {
			snprintf(detail, sizeof(detail), "URL changed at cycle %d", c + 1);
			ok = false;
			break;
		}
		if (c < 2) {
			// Close and reopen. The last cycle keeps the window open
			webui_close(w);
			if (!wait_hidden(w, 10000)) {
				snprintf(detail, sizeof(detail), "Window did not close at cycle %d", c + 1);
				ok = false;
				break;
			}
			sleep_ms(400);
		}
		stage_progress(c + 1, 3);
	}
	if (ok)
		mark_passed(w, stage_names[i]);
	return stage_end(i, ok, detail);
}

static bool stage_c_to_js(int i) {
	stage_begin(i);
	bool ok = true;
	char detail[256] = "2000 webui_run + 5 webui_script checks";
	char resp[64];
	size_t w = webui_new_window();
	stage_window_size(w);
	webui_bind(w, "stage_ready", cb_ready);
	g_ready = 0;
	if (!show_stage_window(w, make_page("C to JS",
		"var count=0; ready(function(){webui.call('stage_ready',1);});"))) {
		snprintf(detail, sizeof(detail), "Show failed");
		ok = false;
	}
	if (ok && !wait_long(&g_ready, 1, 15000)) {
		snprintf(detail, sizeof(detail), "No page signal");
		ok = false;
	}
	if (ok) {
		for (int n = 0; n < 2000; n++) {
			webui_run(w, "count++;");
			if ((n % 100) == 0)
				stage_progress(n, 2000);
		}
		stage_progress(2000, 2000);
		memset(resp, 0, sizeof(resp));
		if (!webui_script(w, "return count;", 30, resp, sizeof(resp)) || strcmp(resp, "2000") != 0) {
			snprintf(detail, sizeof(detail), "Counter is [%s], expected [2000]", resp);
			ok = false;
		}
	}
	if (ok) {
		memset(resp, 0, sizeof(resp));
		if (!webui_script(w, "return 'hello';", 10, resp, sizeof(resp)) || strcmp(resp, "hello") != 0)
			ok = false;
		memset(resp, 0, sizeof(resp));
		if (!webui_script(w, "return 40 + 2;", 10, resp, sizeof(resp)) || strcmp(resp, "42") != 0)
			ok = false;
		memset(resp, 0, sizeof(resp));
		if (!webui_script(w, "return true;", 10, resp, sizeof(resp)) || strcmp(resp, "true") != 0)
			ok = false;
		memset(resp, 0, sizeof(resp));
		if (webui_script(w, "return no_such_variable.x;", 10, resp, sizeof(resp)))
			ok = false;
		if (!ok)
			snprintf(detail, sizeof(detail), "A webui_script result mismatched");
	}
	if (ok)
		mark_passed(w, stage_names[i]);
	return stage_end(i, ok, detail);
}

static bool stage_js_to_c(int i) {
	stage_begin(i);
	bool ok = true;
	char detail[256] = "400 echo calls + 256KB argument";
	size_t w = webui_new_window();
	stage_window_size(w);
	webui_bind(w, "stage_ready", cb_ready);
	webui_bind(w, "echo", cb_echo);
	webui_bind(w, "big_arg", cb_big_arg);
	webui_bind(w, "report", cb_report);
	webui_bind(w, "progress", cb_progress);
	g_ready = 0;
	g_report = -1;
	if (!show_stage_window(w, make_page("JS to C",
		"ready(async function(){"
		"webui.call('stage_ready',1);"
		"let ok=0;"
		"for(let i=0;i<400;i++){const r=await webui.call('echo',i);if(Number(r)===i*2)ok++;"
		"if((i%20)===0)webui.call('progress',i);}"
		"const big='B'.repeat(262144);"
		"if((await webui.call('big_arg',big))==='1')ok++;"
		"webui.call('report',ok);});"))) {
		snprintf(detail, sizeof(detail), "Show failed");
		ok = false;
	}
	if (ok && !wait_long(&g_ready, 1, 15000)) {
		snprintf(detail, sizeof(detail), "No page signal");
		ok = false;
	}
	if (ok && !wait_long(&g_report, 401, 60000)) {
		snprintf(detail, sizeof(detail), "Got %ld of 401 round trips", g_report);
		ok = false;
	}
	if (ok)
		mark_passed(w, stage_names[i]);
	return stage_end(i, ok, detail);
}

static bool stage_big_payloads(int i) {
	stage_begin(i);
	bool ok = true;
	char detail[256] = "1MB JS to C + 300KB C to JS";
	size_t w = webui_new_window();
	stage_window_size(w);
	webui_bind(w, "stage_ready", cb_ready);
	webui_bind(w, "report", cb_report);
	g_ready = 0;
	if (!show_stage_window(w, make_page("Large payloads", "ready(function(){webui.call('stage_ready',1);});"))) {
		snprintf(detail, sizeof(detail), "Show failed");
		ok = false;
	}
	if (ok && !wait_long(&g_ready, 1, 15000)) {
		snprintf(detail, sizeof(detail), "No page signal");
		ok = false;
	}
	if (ok) {
		memset(big_buf, 0, sizeof(big_buf));
		if (!webui_script(w, "return 'A'.repeat(1048576);", 60, big_buf, sizeof(big_buf))) {
			snprintf(detail, sizeof(detail), "1MB webui_script failed");
			ok = false;
		} else {
			size_t len = strlen(big_buf);
			if (len != BIG_SCRIPT_LEN || big_buf[0] != 'A' || big_buf[len - 1] != 'A') {
				snprintf(detail, sizeof(detail), "1MB response corrupted (%zu bytes)", len);
				ok = false;
			}
		}
		if (ok)
			stage_progress(1, 2);
	}
	if (ok) {
		g_report = -1;
		size_t pos = 0;
		strcpy(big_js, "var s='");
		pos = strlen(big_js);
		memset(big_js + pos, 'C', BIG_RUN_LEN);
		pos += BIG_RUN_LEN;
		big_js[pos] = '\0';
		strcat(big_js, "'; webui.call('report', s.length);");
		webui_run(w, big_js);
		if (!wait_long(&g_report, BIG_RUN_LEN, 30000)) {
			snprintf(detail, sizeof(detail), "300KB webui_run not confirmed (%ld)", g_report);
			ok = false;
		}
		if (ok)
			stage_progress(2, 2);
	}
	if (ok)
		mark_passed(w, stage_names[i]);
	return stage_end(i, ok, detail);
}

static bool stage_bindings(int i) {
	stage_begin(i);
	bool ok = true;
	char detail[256] = "40 bound functions each called once";
	size_t w = webui_new_window();
	stage_window_size(w);
	webui_bind(w, "stage_ready", cb_ready);
	webui_bind(w, "report", cb_report);
	for (int n = 0; n < 40; n++) {
		char name[16];
		snprintf(name, sizeof(name), "fn_%d", n);
		webui_bind(w, name, cb_bind_hit);
	}
	g_ready = 0;
	g_report = -1;
	g_bind_hits = 0;
	if (!show_stage_window(w, make_page("Many bindings",
		"ready(async function(){"
		"webui.call('stage_ready',1);"
		"let s=0;"
		"for(let i=0;i<40;i++){s+=Number(await webui.call('fn_'+i));}"
		"webui.call('report',s);});"))) {
		snprintf(detail, sizeof(detail), "Show failed");
		ok = false;
	}
	if (ok && !wait_long(&g_ready, 1, 15000)) {
		snprintf(detail, sizeof(detail), "No page signal");
		ok = false;
	}
	if (ok && (!wait_long(&g_report, 40, 30000) || g_bind_hits != 40)) {
		snprintf(detail, sizeof(detail), "JS confirmed %ld, C counted %ld of 40", g_report, g_bind_hits);
		ok = false;
	}
	if (ok)
		mark_passed(w, stage_names[i]);
	return stage_end(i, ok, detail);
}

static bool stage_navigation(int i) {
	stage_begin(i);
	bool ok = true;
	char detail[256] = "8 content reloads on one window";
	size_t w = webui_new_window();
	stage_window_size(w);
	webui_bind(w, "stage_ready", cb_ready);
	for (int p = 1; p <= 8 && ok; p++) {
		char title[32];
		char script[128];
		g_ready = 0;
		snprintf(title, sizeof(title), "Page %d", p);
		snprintf(script, sizeof(script), "ready(function(){webui.call('stage_ready',%d);});", p);
		if (!show_stage_window(w, make_page(title, script))) {
			snprintf(detail, sizeof(detail), "Show failed at page %d", p);
			ok = false;
			break;
		}
		if (!wait_long(&g_ready, p, 15000)) {
			snprintf(detail, sizeof(detail), "Page %d never signaled", p);
			ok = false;
			break;
		}
		stage_progress(p, 8);
	}
	if (ok)
		mark_passed(w, stage_names[i]);
	return stage_end(i, ok, detail);
}

static bool stage_multi_window(int i) {
	stage_begin(i);
	bool ok = true;
	char detail[256] = "2 rounds of 3 parallel windows";
	for (int round = 0; round < 2 && ok; round++) {
		size_t ws[3] = {0, 0, 0};
		for (int n = 0; n < 3 && ok; n++) {
			char title[32];
			char script[128];
			ws[n] = webui_new_window();
	stage_window_size(ws[n]);
			webui_bind(ws[n], "stage_ready", cb_ready);
			g_ready = 0;
			snprintf(title, sizeof(title), "Window %d.%d", round + 1, n + 1);
			snprintf(script, sizeof(script), "ready(function(){webui.call('stage_ready',%d);});", n + 1);
			if (!show_stage_window(ws[n], make_page(title, script)) || !wait_long(&g_ready, n + 1, 15000)) {
				snprintf(detail, sizeof(detail), "Window %d.%d failed to open", round + 1, n + 1);
				ok = false;
			}
			stage_progress((round * 3) + n + 1, 6);
		}
		if (ok) {
			for (int n = 0; n < 3 && ok; n++) {
				if (!webui_is_shown(ws[n])) {
					snprintf(detail, sizeof(detail), "Window %d.%d not shown", round + 1, n + 1);
					ok = false;
				}
			}
		}
		if (round == 0) {
			// First round gets closed, the churn part of the test
			for (int n = 0; n < 3; n++) {
				if (ws[n] > 0) {
					webui_close(ws[n]);
					wait_hidden(ws[n], 10000);
					webui_destroy(ws[n]);
				}
			}
		} else if (ok) {
			// Last round stays open and connected
			for (int n = 0; n < 3; n++)
				mark_passed(ws[n], stage_names[i]);
		}
	}
	return stage_end(i, ok, detail);
}

static bool stage_port_reload(int i) {
	stage_begin(i);
	bool ok = true;
	char detail[256] = "Window moved itself to the new port, still connected";
	size_t w = webui_new_window();
	stage_window_size(w);
	webui_bind(w, "stage_ready", cb_ready);
	g_ready = 0;
	if (!show_stage_window(w, make_page("Port reload", "ready(function(){webui.call('stage_ready',1);});")) ||
		!wait_long(&g_ready, 1, 15000)) {
		snprintf(detail, sizeof(detail), "First show failed");
		ok = false;
	}
	char old_url[256] = {0};
	if (ok) {
		stage_progress(1, 3);
		const char* u = webui_get_url(w);
		snprintf(old_url, sizeof(old_url), "%s", (u != NULL ? u : ""));
	}
	size_t new_port = 0;
	if (ok) {
		// The window is not closed for this: it moves itself to the
		// new port as soon as the new server answers, and signals
		// again from there
		g_ready = 0;
		for (size_t p = 33000; p < 33100; p++) {
			if (webui_set_port(w, p)) {
				new_port = p;
				break;
			}
		}
		if (new_port == 0) {
			snprintf(detail, sizeof(detail), "No free test port found");
			ok = false;
		}
	}
	if (ok) {
		stage_progress(2, 3);
		// This blocks until the pending reload is done,
		// so the returned URL is the one of the new server
		const char* url = webui_get_url(w);
		char expect[32];
		snprintf(expect, sizeof(expect), ":%zu", new_port);
		if (url == NULL || strstr(url, expect) == NULL) {
			snprintf(detail, sizeof(detail), "URL [%s] is not on port %zu", (url != NULL ? url : "?"), new_port);
			ok = false;
		} else if (strcmp(old_url, url) == 0) {
			snprintf(detail, sizeof(detail), "URL did not change after the reload");
			ok = false;
		}
	}
	if (ok && !wait_long(&g_ready, 1, 25000)) {
		snprintf(detail, sizeof(detail), "Window did not come back on port %zu", new_port);
		ok = false;
	}
	if (ok && !webui_is_shown(w)) {
		snprintf(detail, sizeof(detail), "Window is not connected after the reload");
		ok = false;
	}
	if (ok)
		stage_progress(3, 3);
	if (ok)
		mark_passed(w, stage_names[i]);
	return stage_end(i, ok, detail);
}

static void write_file(const char* path, const char* content) {
	FILE* f = fopen(path, "w");
	if (f != NULL) {
		fputs(content, f);
		fclose(f);
	}
}

static bool stage_root_folder(int i) {
	stage_begin(i);
	bool ok = true;
	char detail[256] = "webui_set_root_folder() applied live, no reload";

	#ifdef _WIN32
	_mkdir("st_root_a");
	_mkdir("st_root_b");
	#else
	mkdir("st_root_a", 0755);
	mkdir("st_root_b", 0755);
	#endif
	write_file("st_root_a/data.txt", "AAA");
	write_file("st_root_b/data.txt", "BBB");
	write_file("st_root_a/index.html",
		"<html><head><script src=\"webui.js\"></script></head><body>Root folder test"
		"<script>"
		"async function grab(){const r=await fetch('data.txt?x='+Date.now());"
		"const t=await r.text();webui.call('report_char',t);}"
		"function ready(f){"
		"let done=false;"
		"const go=function(){if(!done){done=true;f();}};"
		"document.addEventListener('DOMContentLoaded',function(){"
		"if(typeof webui!=='undefined'){"
		"webui.setEventCallback(function(e){if(e==webui.event.CONNECTED)go();});"
		"if(webui.isConnected())go();"
		"}});}"
		"ready(function(){webui.call('stage_ready',1);grab();});"
		"</script></body></html>");

	size_t w = webui_new_window();
	stage_window_size(w);
	webui_bind(w, "stage_ready", cb_ready);
	webui_bind(w, "report_char", cb_report_char);
	g_ready = 0;
	g_report = -1;
	if (!webui_set_root_folder(w, "./st_root_a")) {
		snprintf(detail, sizeof(detail), "set_root_folder before show failed");
		ok = false;
	}
	if (ok && (!show_stage_window(w, "index.html") || !wait_long(&g_ready, 1, 15000))) {
		snprintf(detail, sizeof(detail), "Show from folder A failed");
		ok = false;
	}
	if (ok && !wait_long(&g_report, 'A', 15000)) {
		snprintf(detail, sizeof(detail), "Folder A content mismatch (%ld)", g_report);
		ok = false;
	}
	if (ok)
		stage_progress(1, 2);
	if (ok && !webui_set_root_folder(w, "./st_root_b")) {
		snprintf(detail, sizeof(detail), "Live set_root_folder failed");
		ok = false;
	}
	if (ok) {
		g_report = -1;
		webui_run(w, "grab();");
		if (!wait_long(&g_report, 'B', 15000)) {
			snprintf(detail, sizeof(detail), "Folder B content mismatch (%ld)", g_report);
			ok = false;
		}
		if (ok)
			stage_progress(2, 2);
	}
	if (ok)
		mark_passed(w, stage_names[i]);
	return stage_end(i, ok, detail);
}

static bool stage_folder_monitor(int i) {
	stage_begin(i);
	bool ok = true;
	char detail[256] = "File change reloaded the page, monitor stopped clean";

	#ifdef _WIN32
	_mkdir("st_monitor");
	#else
	mkdir("st_monitor", 0755);
	#endif
	write_file("st_monitor/data.txt", "one");
	write_file("st_monitor/index.html",
		"<html><head><script src=\"webui.js\"></script></head><body>Folder monitor test"
		"<script>"
		"function ready(f){"
		"let done=false;"
		"const go=function(){if(!done){done=true;f();}};"
		"document.addEventListener('DOMContentLoaded',function(){"
		"if(typeof webui!=='undefined'){"
		"webui.setEventCallback(function(e){if(e==webui.event.CONNECTED)go();});"
		"if(webui.isConnected())go();"
		"}});}"
		"ready(function(){webui.call('stage_ready',1);});"
		"</script></body></html>");

	webui_set_config(folder_monitor, true);

	size_t w = webui_new_window();
	stage_window_size(w);
	webui_bind(w, "stage_ready", cb_ready_inc);
	g_ready = 0;
	if (!webui_set_root_folder(w, "./st_monitor")) {
		snprintf(detail, sizeof(detail), "set_root_folder failed");
		ok = false;
	}
	if (ok && (!show_stage_window(w, "index.html") || !wait_at_least(&g_ready, 1, 15000))) {
		snprintf(detail, sizeof(detail), "Show failed");
		ok = false;
	}
	if (ok)
		stage_progress(1, 2);
	if (ok) {
		// Changing a file in the folder must reload the page,
		// which connects again and signals a second time
		sleep_ms(1000);
		write_file("st_monitor/data.txt", "two");
		if (!wait_at_least(&g_ready, 2, 20000)) {
			snprintf(detail, sizeof(detail), "Page was not reloaded on file change");
			ok = false;
		}
		if (ok)
			stage_progress(2, 2);
	}
	if (ok) {
		// The monitor thread has to stop cooperatively. If it had to be
		// killed, or if it hung, this destroy would take much longer.
		time_t t0 = time(NULL);
		webui_destroy(w);
		double spent = difftime(time(NULL), t0);
		if (spent > 5.0) {
			snprintf(detail, sizeof(detail), "Destroy took %.0f seconds, monitor did not stop", spent);
			ok = false;
		}
	}
	webui_set_config(folder_monitor, false);

	if (ok) {
		// The tested window is destroyed, so keep a marker window
		size_t w2 = webui_new_window();
	stage_window_size(w2);
		webui_bind(w2, "stage_ready", cb_ready);
		g_ready = 0;
		if (!show_stage_window(w2, make_page("Folder monitor", "ready(function(){webui.call('stage_ready',1);});"))
			|| !wait_long(&g_ready, 1, 15000)) {
			snprintf(detail, sizeof(detail), "Marker window failed to open");
			ok = false;
		} else {
			mark_passed(w2, stage_names[i]);
		}
	}
	return stage_end(i, ok, detail);
}

static bool stage_reclaim(int i) {
	stage_begin(i);
	bool ok = true;
	char detail[256] = "200 windows created and destroyed, memory reclaimed";
	const char* page = "<html><head><script src=\"webui.js\"></script></head><body>Reclaim</body></html>";

	for (int n = 0; n < 200 && ok; n++) {
		size_t w = webui_new_window();
	stage_window_size(w);
		if (w == 0) {
			snprintf(detail, sizeof(detail), "Window creation failed at %d", n + 1);
			ok = false;
			break;
		}
		if (!webui_show_browser(w, page, NoBrowser)) {
			snprintf(detail, sizeof(detail), "Headless show failed at %d", n + 1);
			webui_destroy(w);
			ok = false;
			break;
		}
		webui_destroy(w);
		if ((n % 5) == 0)
			stage_progress(n, 200);
		if (g_abort)
			break;
	}
	if (ok)
		stage_progress(200, 200);
	if (ok) {
		size_t w = webui_new_window();
	stage_window_size(w);
		webui_bind(w, "stage_ready", cb_ready);
		g_ready = 0;
		if (!show_stage_window(w, make_page("Window reclaim", "ready(function(){webui.call('stage_ready',1);});"))
			|| !wait_long(&g_ready, 1, 15000)) {
			snprintf(detail, sizeof(detail), "Marker window failed to open");
			ok = false;
		} else {
			mark_passed(w, stage_names[i]);
		}
	}
	return stage_end(i, ok, detail);
}

// A fixed page for the concurrent workers: `make_page()`
// uses a shared buffer, so it's not thread-safe
static const char* churn_page =
	"<html><head><script src=\"webui.js\"></script></head><body>Churn</body></html>";

typedef struct {
	bool ok;
	volatile long done;
} churn_arg_t;

#ifdef _WIN32
static DWORD WINAPI churn_worker(LPVOID arg)
#else
static void* churn_worker(void* arg)
#endif
{
	churn_arg_t* state = (churn_arg_t*)arg;
	state->ok = true;
	for (int n = 0; n < 6; n++) {
		size_t w = webui_new_window();
	stage_window_size(w);
		if (w == 0) {
			state->ok = false;
			break;
		}
		webui_bind(w, "stage_ready", cb_ready);
		if (!webui_show_browser(w, churn_page, NoBrowser)) {
			state->ok = false;
			break;
		}
		sleep_ms(15);
		webui_destroy(w);
		// Only this worker writes this counter, the
		// driver thread sums them up for the progress
		state->done++;
	}
	#ifdef _WIN32
	return 0;
	#else
	return NULL;
	#endif
}

static bool stage_concurrent(int i) {
	stage_begin(i);
	bool ok = true;
	char detail[256] = "4 threads x 6 headless windows, no serialization";
	churn_arg_t state[4];
	memset(state, 0, sizeof(state));
	#ifdef _WIN32
	HANDLE th[4];
	for (int n = 0; n < 4; n++)
		th[n] = CreateThread(NULL, 0, churn_worker, &state[n], 0, NULL);
	#else
	pthread_t th[4];
	for (int n = 0; n < 4; n++)
		pthread_create(&th[n], NULL, churn_worker, &state[n]);
	#endif

	// Report the progress while the workers run
	for (int guard = 0; guard < 1200; guard++) {
		long done = state[0].done + state[1].done + state[2].done + state[3].done;
		stage_progress(done, 24);
		if (done >= 24 || g_abort)
			break;
		sleep_ms(50);
	}

	#ifdef _WIN32
	WaitForMultipleObjects(4, th, TRUE, INFINITE);
	for (int n = 0; n < 4; n++)
		CloseHandle(th[n]);
	#else
	for (int n = 0; n < 4; n++)
		pthread_join(th[n], NULL);
	#endif
	stage_progress(state[0].done + state[1].done + state[2].done + state[3].done, 24);

	if (!state[0].ok || !state[1].ok || !state[2].ok || !state[3].ok) {
		snprintf(detail, sizeof(detail), "A concurrent worker failed");
		ok = false;
	}
	if (ok) {
		size_t w = webui_new_window();
	stage_window_size(w);
		webui_bind(w, "stage_ready", cb_ready);
		g_ready = 0;
		if (!show_stage_window(w, make_page("Concurrent churn", "ready(function(){webui.call('stage_ready',1);});")) ||
			!wait_long(&g_ready, 1, 15000)) {
			snprintf(detail, sizeof(detail), "Marker window failed to open");
			ok = false;
		} else {
			mark_passed(w, stage_names[i]);
		}
	}
	return stage_end(i, ok, detail);
}

static bool stage_destroy_from_callback(int i) {
	stage_begin(i);
	bool ok = true;
	char detail[256] = "webui_destroy() called inside its own callback";
	size_t w = webui_new_window();
	stage_window_size(w);
	webui_bind(w, "stage_ready", cb_ready);
	webui_bind(w, "kamikaze", cb_kamikaze);
	g_ready = 0;
	g_kamikaze = 0;
	// The page only reports that it is connected here. Destroying the
	// window from the connect callback itself would race `webui_show()`,
	// which returns the connection status: the window would already be
	// gone by the time it returns, and the show would look failed.
	if (!show_stage_window(w, make_page("Destroy from callback", "ready(function(){webui.call('stage_ready',1);});"))) {
		snprintf(detail, sizeof(detail), "Show failed");
		ok = false;
	}
	if (ok && !wait_long(&g_ready, 1, 15000)) {
		snprintf(detail, sizeof(detail), "No page signal");
		ok = false;
	}
	if (ok) {
		// Now let the page call the backend function
		// that destroys this very window
		webui_run(w, "webui.call('kamikaze');");
	}
	if (ok && !wait_long(&g_kamikaze, 1, 15000)) {
		snprintf(detail, sizeof(detail), "Callback never ran");
		ok = false;
	}
	if (ok && !wait_hidden(w, 10000)) {
		snprintf(detail, sizeof(detail), "Window still shown after destroy");
		ok = false;
	}
	if (ok) {
		// The tested window is gone by design. Open a fresh one to
		// keep this stage visible among the connected windows
		sleep_ms(500);
		size_t w2 = webui_new_window();
	stage_window_size(w2);
		webui_bind(w2, "stage_ready", cb_ready);
		g_ready = 0;
		if (!show_stage_window(w2, make_page("Destroy from callback", "ready(function(){webui.call('stage_ready',1);});"))
			|| !wait_long(&g_ready, 1, 15000)) {
			snprintf(detail, sizeof(detail), "Replacement window failed to open");
			ok = false;
		} else {
			mark_passed(w2, stage_names[i]);
		}
	}
	return stage_end(i, ok, detail);
}

static bool stage_rapid_open_close(int i) {
	stage_begin(i);
	bool ok = true;
	char detail[256] = "5 immediate open/close cycles";
	size_t w = webui_new_window();
	stage_window_size(w);
	webui_bind(w, "stage_ready", cb_ready);
	for (int c = 0; c < 5 && ok; c++) {
		g_ready = 0;
		if (!show_stage_window(w, make_page("Rapid cycle", "ready(function(){webui.call('stage_ready',1);});"))) {
			snprintf(detail, sizeof(detail), "Show failed at cycle %d", c + 1);
			ok = false;
			break;
		}
		if (!wait_long(&g_ready, 1, 15000)) {
			snprintf(detail, sizeof(detail), "No page signal at cycle %d", c + 1);
			ok = false;
			break;
		}
		if (c < 4) {
			// Close and reopen. The last cycle keeps the window open
			webui_close(w);
			if (!wait_hidden(w, 10000)) {
				snprintf(detail, sizeof(detail), "Window did not close at cycle %d", c + 1);
				ok = false;
				break;
			}
		}
		stage_progress(c + 1, 5);
	}
	if (ok)
		mark_passed(w, stage_names[i]);
	return stage_end(i, ok, detail);
}

typedef bool (*stage_fn)(int);
static const stage_fn stage_fns[STAGES] = {
	stage_dashboard,
	stage_reopen,
	stage_c_to_js,
	stage_js_to_c,
	stage_big_payloads,
	stage_bindings,
	stage_navigation,
	stage_multi_window,
	stage_port_reload,
	stage_root_folder,
	stage_folder_monitor,
	stage_concurrent,
	stage_reclaim,
	stage_destroy_from_callback,
	stage_rapid_open_close
};

// Repaint the results this run already produced. Used by the second
// phase, whose dashboard is a brand new window.
static void repaint_results(void) {
	for (int m = 0; m < MODES; m++) {
		for (int s = 0; s < STAGES; s++) {
			char r = g_results[m][s];
			if (r == 'P') {
				unsigned long long spent = g_times[m][s];
				status_run("setCell(%d,%d,'PASS %llu.%03llus','pass');",
					m, s, (spent / 1000ULL), (spent % 1000ULL));
			}
			else if (r == 'F')
				status_run("setCell(%d,%d,'FAIL','fail');", m, s);
			else if (r == 'S')
				status_run("setCell(%d,%d,'SKIP','skip');", m, s);
		}
	}
}

// Run every stage of one mode, in order, stopping at the first failure
static bool run_mode(int mode) {

	g_mode = mode;
	webui_set_config(ui_event_blocking, ((mode % 2) == 1));

	printf("=== MODE %d/%d: %s ===\n", mode + 1, MODES, mode_names[mode]);
	fflush(stdout);
	status_run("setBanner('Mode %d/%d: %s','');", mode + 1, MODES, mode_names[mode]);

	int i;
	bool all = true;
	for (i = 0; i < STAGES && all && !g_abort; i++)
		all = stage_fns[i](i);

	if (g_abort)
		return false;

	if (!all) {
		for (int k = i; k < STAGES; k++) {
			g_results[mode][k] = 'S';
			status_run("setCell(%d,%d,'SKIP','skip');", mode, k);
		}
		printf("MODE %d STOPPED: stage %d failed\n", mode + 1, i);
		fflush(stdout);
		return false;
	}

	printf("MODE %d PASSED\n", mode + 1);
	fflush(stdout);

	// Every mode starts from a clean desktop
	status_run("setBanner('Mode %d/%d passed, closing its windows...','');", mode + 1, MODES);
	close_kept_windows();
	sleep_ms(1000);
	return true;
}

// Run the two modes of the current phase
static bool run_phase(void) {

	int first = (int)(g_phase * 2);
	for (int m = first; m < first + 2; m++) {
		if (!run_mode(m))
			return false;
	}
	return true;
}

// Run one stage alone, in the first mode of the current phase. This does
// not touch the result files.
static void run_single_stage(int idx) {

	g_mode = g_phase * 2;
	webui_set_config(ui_event_blocking, false);
	status_run("setBanner('Running stage %d alone, in %s...','');", idx + 1, mode_names[g_mode]);

	bool ok = stage_fns[idx](idx);

	if (g_abort)
		return;

	printf("SINGLE STAGE %d -> %s\n", idx + 1, ok ? "PASS" : "FAIL");
	fflush(stdout);
	status_run("setBanner('Stage %d %s (single run, result files unchanged)','%s');",
		idx + 1, ok ? "passed" : "failed", ok ? "pass" : "fail");
}

#ifdef _WIN32
static DWORD WINAPI driver(LPVOID arg)
#else
static void* driver(void* arg)
#endif
{
	(void)arg;

	// Serve run requests until this phase ends. A request comes from
	// the Start button, from a stage Run button, or from auto mode.
	while (!g_abort) {

		if (g_start) {
			g_start = 0;
			g_busy = 1;
			bool ok = run_phase();
			g_busy = 0;

			if (g_abort)
				break;

			if (!ok) {
				// Stop the whole run, and keep this dashboard open
				// so the failure stays visible
				g_finished = 2;
				FILE* f = fopen("test_error.txt", "w");
				if (f != NULL)
					fclose(f);
				status_run("setBusy(false);");
				continue;
			}

			if (g_phase == 0) {
				// The browser modes are done, the WebView modes are
				// next, and this run carries on into them
				status_run("setBanner('Browser modes passed, switching to WebView...','pass');");
				sleep_ms(1500);
				g_auto_advance = 1;
				g_switch_mode = 1;
				continue;
			}

			// Everything is done
			g_finished = 1;
			FILE* f = fopen("test_pass.txt", "w");
			if (f != NULL)
				fclose(f);
			printf("ALL %d MODES PASSED\n", MODES);
			fflush(stdout);
			status_run("setBanner('ALL %d MODES PASSED - every green window is still connected','pass');", MODES);
			status_run("setBusy(false);");
		}
		else if (g_run_one >= 0) {
			int idx = (int)g_run_one;
			g_run_one = -1;
			g_busy = 1;
			run_single_stage(idx);
			g_busy = 0;
			status_run("setBusy(false);");
		}
		else if (g_switch_mode) {
			// Close everything of this mode, so `webui_wait()` returns
			// and a fresh dashboard opens in the other mode
			close_kept_windows();
			if (g_ctrl_win > 0) {
				webui_destroy(g_ctrl_win);
				g_ctrl_win = 0;
			}
			webui_destroy(status_win);
			break;
		}
		else {
			sleep_ms(50);
		}
	}

	#ifdef _WIN32
	return 0;
	#else
	return NULL;
	#endif
}

static const char* status_page =
	"<html><head><script src=\"webui.js\"></script><style>"
	"body{font-family:'Segoe UI',sans-serif;background:#0f1220;color:#e8e8ef;margin:0;padding:24px}"
	"h1{font-size:20px;margin:0 0 12px}"
	"button{font-size:14px;font-weight:600;padding:8px 22px;margin-right:8px;border:0;"
	"border-radius:6px;cursor:pointer;background:#3d63dd;color:#fff}"
	"button:disabled{background:#2b3050;color:#8a92b2;cursor:default}"
	"#bexit{background:#5a2530}"
	"button.run{font-size:12px;font-weight:600;padding:4px 14px;margin:0;background:#2f3c6b}"
	"button.run:hover:enabled{background:#3d63dd}"
	".top{display:flex;justify-content:space-between;align-items:flex-start;gap:20px}"
	".mode{text-align:center;padding-top:6px}"
	"#modelbl{font-size:15px;font-weight:700;color:#ffd866;margin-bottom:8px}"
	"#bmode{background:#3d4d8a}"
	".ctrl{background:#171b2e;border:1px solid #23274a;border-radius:8px;padding:12px 14px;min-width:270px}"
	".ctrl .t{color:#9aa0b8;font-size:12px;margin-bottom:8px}"
	".ctrl select{width:100%;padding:6px;border-radius:6px;background:#0f1220;color:#e8e8ef;"
	"border:1px solid #2b3050;font-size:13px}"
	".ctrl .row{display:flex;align-items:center;gap:10px;margin-top:8px}"
	".ctrl button{padding:6px 18px;font-size:13px;margin:0}"
	"#ctrlres{font-weight:800;font-size:15px;color:#5b6180}"
	"#ctrlres.ok{color:#7ce38b}#ctrlres.bad{color:#ff8089}"
	"#banner{padding:10px 14px;border-radius:8px;background:#2b3050;margin:14px 0;font-weight:600}"
	"#banner.pass{background:#12391f;color:#7ce38b}"
	"#banner.fail{background:#46151a;color:#ff8089}"
	"table{width:100%;border-collapse:collapse}"
	"td,th{padding:7px 8px;border-bottom:1px solid #23274a;text-align:left;font-size:13px}"
	"th{color:#9aa0b8;font-size:12px}"
	".st{width:92px;font-weight:600;color:#5b6180}"
	".st.pass{color:#7ce38b}.st.fail{color:#ff8089}.st.run{color:#ffd866}.st.skip{color:#8a92b2}"
	".bar{height:4px;background:#23274a;border-radius:2px;margin-top:5px;overflow:hidden}"
	".bar i{display:block;height:4px;background:#ffd866;border-radius:2px}"
	"</style></head><body>"
	"<div class=\"top\">"
	"<div>"
	"<h1>WebUI Stress Test</h1>"
	"<div>"
	"<button id=\"bstart\" onclick=\"setBusy(true);webui.call('start_test');\">Start</button>"
	"<button id=\"bexit\" onclick=\"webui.call('app_exit');\">Exit</button>"
	"</div>"
	"</div>"
	"<div class=\"mode\">"
	"<div id=\"modelbl\">Web Browser Mode</div>"
	"<button id=\"bmode\" onclick=\"setBusy(true);webui.call('switch_mode');\">Reload in WebView Mode</button>"
	"</div>"
	"<div class=\"ctrl\">"
	"<div class=\"t\">Manual test window (judge it by looking)</div>"
	"<select id=\"act\"></select>"
	"<div class=\"row\">"
	"<button onclick=\"webui.call('ctrl_run',document.getElementById('act').selectedIndex);\">Run</button>"
	"<span id=\"ctrlres\">-</span>"
	"</div>"
	"</div>"
	"</div>"
	"<div id=\"banner\">Ready. Start runs every stage in all 4 modes</div>"
	"<table id=\"tbl\"><tr><th>#</th><th>Stage</th>"
	"<th>Browser<br>async</th><th>Browser<br>blocking</th>"
	"<th>WebView<br>async</th><th>WebView<br>blocking</th><th></th></tr></table>"
	"<script>"
	"const names=['x1 Dashboard connection','x3 Open / close / reopen window',"
	"'x2000 C to JS calls (webui_run / webui_script)','x401 JS to C calls and arguments',"
	"'x2 Large payloads (multi-packet)','x40 Many bindings','x8 Navigation / content reload',"
	"'x6 Multi-window churn','x1 Port change reload (webui_set_port)','x2 Live root folder change',"
	"'x1 Folder monitor auto-reload','x24 Concurrent create / destroy',"
	"'x200 Window reclaim (create / destroy)','x1 Destroy from callback','x5 Rapid open / close'];"
	"const MODES=4;"
	"const tbl=document.getElementById('tbl');"
	"names.forEach((n,i)=>{const r=tbl.insertRow(-1);r.insertCell(0).textContent=i+1;"
	"r.insertCell(1).textContent=n;"
	"for(let m=0;m<MODES;m++){const s=r.insertCell(2+m);s.className='st';s.id='c'+m+'_'+i;"
	"s.textContent='PENDING';}"
	"const a=r.insertCell(2+MODES);const b=document.createElement('button');b.className='run';"
	"b.textContent='Run';b.onclick=function(){setBusy(true);webui.call('run_stage',i);};"
	"a.appendChild(b);});"
	"function setCell(m,i,txt,cls){const s=document.getElementById('c'+m+'_'+i);"
	"if(!s)return;s.textContent=txt;s.className='st '+(cls||'');}"
	"function setProgress(m,i,done,total){const s=document.getElementById('c'+m+'_'+i);"
	"if(!s)return;const pct=total>0?Math.round((done*100)/total):0;s.className='st run';"
	"s.innerHTML=done+'/'+total+'<div class=\"bar\"><i style=\"width:'+pct+'%\"></i></div>';}"
	"function setBanner(t,cls){const b=document.getElementById('banner');"
	"b.textContent=t;b.className=cls||'';}"
	"function setBusy(b){document.getElementById('bstart').disabled=b;"
	"document.getElementById('bmode').disabled=b;"
	"document.querySelectorAll('button.run').forEach(x=>{x.disabled=b;});}"
	"function setMode(m){"
	"document.getElementById('modelbl').textContent=m?'WebView Mode':'Web Browser Mode';"
	"document.getElementById('bmode').textContent=m?'Reload in Web Browser Mode':'Reload in WebView Mode';}"
	"function ctrlClear(){document.getElementById('act').innerHTML='';}"
	"function ctrlAdd(t){const o=document.createElement('option');o.textContent=t;"
	"document.getElementById('act').appendChild(o);}"
	"function setCtrl(t,cls){const s=document.getElementById('ctrlres');"
	"s.textContent=t;s.className=cls||'';}"
	"</script></body></html>";

int main(int argc, char* argv[]) {

	bool autorun = (argc > 1 && argv[1] != NULL && strcmp(argv[1], "auto") == 0);

	remove("test_pass.txt");
	remove("test_error.txt");

	printf("WebUI Stress Test%s\n", autorun ? " (auto mode)" : "");
	fflush(stdout);

	// The dashboard runs in a browser for the browser modes, then the
	// whole library is cleaned and a new dashboard is opened as a
	// WebView for the WebView modes. A WebView dashboard is what gives
	// `webui_wait()` the native UI loop those modes need. The same
	// switch happens whenever the user asks for the other mode.
	bool webview_failed = false;
	for (;;) {

		bool phase = (g_phase != 0);
		g_start = 0;
		g_busy = 0;
		g_run_one = -1;
		g_switch_mode = 0;
		g_kept_count = 0;
		g_ctrl_win = 0;

		status_win = webui_new_window();

		// The dashboard always shows up at the same place
		webui_set_size(status_win, 1000, 700);
		webui_set_position(status_win, 100, 100);

		webui_bind(status_win, "start_test", cb_start);
		webui_bind(status_win, "run_stage", cb_run_stage);
		webui_bind(status_win, "ctrl_run", cb_ctrl_run);
		webui_bind(status_win, "switch_mode", cb_switch_mode);
		webui_bind(status_win, "app_exit", cb_exit);

		bool shown = (!phase ?
			webui_show(status_win, status_page) :
			webui_show_browser(status_win, status_page, Webview)
		);

		if (!shown) {
			if (!phase) {
				printf("Could not open the status window\n");
				fflush(stdout);
				FILE* f = fopen("test_error.txt", "w");
				if (f != NULL)
					fclose(f);
				return 1;
			}
			// No WebView available on this machine
			printf("WebView is not available, modes 3 and 4 are skipped\n");
			fflush(stdout);
			for (int m = 2; m < MODES; m++) {
				for (int s = 0; s < STAGES; s++) {
					if (g_results[m][s] == 0)
						g_results[m][s] = 'S';
				}
			}
			if (webview_failed)
				break;
			// Go back to the browser dashboard, so the app stays
			// usable. The run cannot continue, so it does not.
			webview_failed = true;
			g_auto_advance = 0;
			g_phase = 0;
			webui_clean();
			sleep_ms(500);
			continue;
		}

		// The manual control list is filled from the table in this
		// file, so the dashboard cannot go out of sync with it
		ctrl_fill_actions();

		// The dedicated manual test window, created once per phase
		ctrl_new_window();

		// Tell the dashboard which mode it is showing, and repaint
		// whatever the previous modes already did
		sleep_ms(300);
		status_run("setMode(%d);", (phase ? 1 : 0));
		repaint_results();

		// Only a run that moved here by itself carries on. A mode the
		// user asked for just opens, and waits for what they do next.
		if (g_auto_advance) {
			g_auto_advance = 0;
			if (!g_exited)
				g_start = 1;
		}
		else if (!phase && autorun && (g_finished == 0)) {
			g_start = 1;
		}

		#ifdef _WIN32
		HANDLE th = CreateThread(NULL, 0, driver, NULL, 0, NULL);
		#else
		pthread_t th;
		pthread_create(&th, NULL, driver, NULL);
		#endif

		webui_wait();
		g_abort = 1;

		#ifdef _WIN32
		if (th != NULL) {
			WaitForSingleObject(th, INFINITE);
			CloseHandle(th);
		}
		#else
		pthread_join(th, NULL);
		#endif

		g_abort = 0;

		// The dashboard is gone. Either the other mode was asked for,
		// or this is the end of the run.
		if (!g_switch_mode || g_exited)
			break;

		g_switch_mode = 0;
		g_phase = (phase ? 0 : 1);

		printf("Cleaning, and reloading in %s mode...\n", (g_phase ? "WebView" : "Web Browser"));
		fflush(stdout);
		webui_clean();
		sleep_ms(500);
	}

	webui_clean();
	return (g_finished == 1 ? 0 : 1);
}

#if defined(_MSC_VER)
int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow) { return main(__argc, __argv); }
#endif
