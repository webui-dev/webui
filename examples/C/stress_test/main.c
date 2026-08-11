// WebUI C - Stress Test
//
// Interactive stress test suite. The first window is a live status
// dashboard with Start / Exit buttons. Every stage opens its own
// window(s) and stresses one part of WebUI. The run stops at the first
// failing stage. Windows of passed stages stay open and connected,
// showing a green "TEST PASSED", to prove WebUI stays stable with many
// connected windows.
//
// Usage:
//   ./main        Wait for the user to click Start
//   ./main auto   Start the test automatically
//
// Result files (created next to the executable):
//   test_pass.txt    every stage passed
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

#define STAGES 13
#define BIG_ARG_LEN 262144
#define BIG_RUN_LEN 307200
#define BIG_SCRIPT_LEN 1048576

static size_t status_win = 0;

static volatile long g_start = 0;
static volatile long g_abort = 0;
static volatile long g_run_one = -1; // Index of a single stage to run
static volatile long g_busy = 0; // A stage is running right now
static volatile long g_finished = 0; // 1 = all passed, 2 = a stage failed
static volatile long g_ready = 0;
static volatile long g_report = -1;
static volatile long g_bind_hits = 0;
static volatile long g_kamikaze = 0;

static char page_buf[8192];
static char big_js[BIG_RUN_LEN + 128];
static char big_buf[BIG_SCRIPT_LEN + 128];

static const char* stage_names[STAGES] = {
	"Dashboard connection",
	"Open / close / reopen window",
	"C to JS calls (webui_run / webui_script)",
	"JS to C calls and arguments",
	"Large payloads (multi-packet)",
	"Many bindings",
	"Navigation / content reload",
	"Multi-window churn",
	"Port change reload (webui_set_port)",
	"Live root folder change",
	"Concurrent create / destroy",
	"Destroy from callback",
	"Rapid open / close"
};

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

static void stage_begin(int i) {
	printf("[%d/%d] %s...\n", i + 1, STAGES, stage_names[i]);
	fflush(stdout);
	status_run("setStage(%d,'RUNNING','');", i);
}

static bool stage_end(int i, bool ok, const char* detail) {
	printf("[%d/%d] %s -> %s (%s)\n", i + 1, STAGES, stage_names[i], ok ? "PASS" : "FAIL", detail);
	fflush(stdout);
	status_run("setStage(%d,'%s','%s');", i, ok ? "PASS" : "FAIL", detail);
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

static void cb_run_stage(webui_event_t* e) {
	if (g_busy)
		return;
	long idx = (long)webui_get_int(e);
	if (idx >= 0 && idx < STAGES)
		g_run_one = idx;
}

static void cb_exit(webui_event_t* e) {
	(void)e;
	g_abort = 1;
	g_start = 1;
	webui_exit();
}

static void cb_ready(webui_event_t* e) {
	g_ready = (long)webui_get_int(e);
}

static void cb_report(webui_event_t* e) {
	g_report = (long)webui_get_int(e);
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
	webui_return_int(e, 1);
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
	webui_bind(w, "stage_ready", cb_ready);
	for (int c = 0; c < 3 && ok; c++) {
		g_ready = 0;
		if (!webui_show(w, make_page("Reopen cycle", "ready(function(){webui.call('stage_ready',1);});"))) {
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
	webui_bind(w, "stage_ready", cb_ready);
	g_ready = 0;
	if (!webui_show(w, make_page("C to JS",
		"var count=0; ready(function(){webui.call('stage_ready',1);});"))) {
		snprintf(detail, sizeof(detail), "Show failed");
		ok = false;
	}
	if (ok && !wait_long(&g_ready, 1, 15000)) {
		snprintf(detail, sizeof(detail), "No page signal");
		ok = false;
	}
	if (ok) {
		for (int n = 0; n < 2000; n++)
			webui_run(w, "count++;");
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
	webui_bind(w, "stage_ready", cb_ready);
	webui_bind(w, "echo", cb_echo);
	webui_bind(w, "big_arg", cb_big_arg);
	webui_bind(w, "report", cb_report);
	g_ready = 0;
	g_report = -1;
	if (!webui_show(w, make_page("JS to C",
		"ready(async function(){"
		"webui.call('stage_ready',1);"
		"let ok=0;"
		"for(let i=0;i<400;i++){const r=await webui.call('echo',i);if(Number(r)===i*2)ok++;}"
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
	webui_bind(w, "stage_ready", cb_ready);
	webui_bind(w, "report", cb_report);
	g_ready = 0;
	if (!webui_show(w, make_page("Large payloads", "ready(function(){webui.call('stage_ready',1);});"))) {
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
	if (!webui_show(w, make_page("Many bindings",
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
	webui_bind(w, "stage_ready", cb_ready);
	for (int p = 1; p <= 8 && ok; p++) {
		char title[32];
		char script[128];
		g_ready = 0;
		snprintf(title, sizeof(title), "Page %d", p);
		snprintf(script, sizeof(script), "ready(function(){webui.call('stage_ready',%d);});", p);
		if (!webui_show(w, make_page(title, script))) {
			snprintf(detail, sizeof(detail), "Show failed at page %d", p);
			ok = false;
			break;
		}
		if (!wait_long(&g_ready, p, 15000)) {
			snprintf(detail, sizeof(detail), "Page %d never signaled", p);
			ok = false;
			break;
		}
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
			webui_bind(ws[n], "stage_ready", cb_ready);
			g_ready = 0;
			snprintf(title, sizeof(title), "Window %d.%d", round + 1, n + 1);
			snprintf(script, sizeof(script), "ready(function(){webui.call('stage_ready',%d);});", n + 1);
			if (!webui_show(ws[n], make_page(title, script)) || !wait_long(&g_ready, n + 1, 15000)) {
				snprintf(detail, sizeof(detail), "Window %d.%d failed to open", round + 1, n + 1);
				ok = false;
			}
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
	char detail[256] = "webui_set_port() reloaded a live window server";
	size_t w = webui_new_window();
	webui_bind(w, "stage_ready", cb_ready);
	g_ready = 0;
	if (!webui_show(w, make_page("Port reload", "ready(function(){webui.call('stage_ready',1);});")) ||
		!wait_long(&g_ready, 1, 15000)) {
		snprintf(detail, sizeof(detail), "First show failed");
		ok = false;
	}
	char old_url[256] = {0};
	if (ok) {
		const char* u = webui_get_url(w);
		snprintf(old_url, sizeof(old_url), "%s", (u != NULL ? u : ""));
	}
	size_t new_port = 0;
	if (ok) {
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
	if (ok && !wait_hidden(w, 15000)) {
		snprintf(detail, sizeof(detail), "Window did not close for the reload");
		ok = false;
	}
	if (ok) {
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
	if (ok) {
		g_ready = 0;
		if (!webui_show(w, make_page("Port reload", "ready(function(){webui.call('stage_ready',1);});")) ||
			!wait_long(&g_ready, 1, 15000)) {
			snprintf(detail, sizeof(detail), "Re-show on the new port failed");
			ok = false;
		}
	}
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
	webui_bind(w, "stage_ready", cb_ready);
	webui_bind(w, "report_char", cb_report_char);
	g_ready = 0;
	g_report = -1;
	if (!webui_set_root_folder(w, "./st_root_a")) {
		snprintf(detail, sizeof(detail), "set_root_folder before show failed");
		ok = false;
	}
	if (ok && (!webui_show(w, "index.html") || !wait_long(&g_ready, 1, 15000))) {
		snprintf(detail, sizeof(detail), "Show from folder A failed");
		ok = false;
	}
	if (ok && !wait_long(&g_report, 'A', 15000)) {
		snprintf(detail, sizeof(detail), "Folder A content mismatch (%ld)", g_report);
		ok = false;
	}
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
	}
	if (ok)
		mark_passed(w, stage_names[i]);
	return stage_end(i, ok, detail);
}

// A fixed page for the concurrent workers: `make_page()`
// uses a shared buffer, so it's not thread-safe
static const char* churn_page =
	"<html><head><script src=\"webui.js\"></script></head><body>Churn</body></html>";

#ifdef _WIN32
static DWORD WINAPI churn_worker(LPVOID arg)
#else
static void* churn_worker(void* arg)
#endif
{
	bool* ok = (bool*)arg;
	*ok = true;
	for (int n = 0; n < 6; n++) {
		size_t w = webui_new_window();
		if (w == 0) {
			*ok = false;
			break;
		}
		webui_bind(w, "stage_ready", cb_ready);
		if (!webui_show_browser(w, churn_page, NoBrowser)) {
			*ok = false;
			break;
		}
		sleep_ms(15);
		webui_destroy(w);
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
	bool oks[4] = {false, false, false, false};
	#ifdef _WIN32
	HANDLE th[4];
	for (int n = 0; n < 4; n++)
		th[n] = CreateThread(NULL, 0, churn_worker, &oks[n], 0, NULL);
	WaitForMultipleObjects(4, th, TRUE, INFINITE);
	for (int n = 0; n < 4; n++)
		CloseHandle(th[n]);
	#else
	pthread_t th[4];
	for (int n = 0; n < 4; n++)
		pthread_create(&th[n], NULL, churn_worker, &oks[n]);
	for (int n = 0; n < 4; n++)
		pthread_join(th[n], NULL);
	#endif
	if (!oks[0] || !oks[1] || !oks[2] || !oks[3]) {
		snprintf(detail, sizeof(detail), "A concurrent worker failed");
		ok = false;
	}
	if (ok) {
		size_t w = webui_new_window();
		webui_bind(w, "stage_ready", cb_ready);
		g_ready = 0;
		if (!webui_show(w, make_page("Concurrent churn", "ready(function(){webui.call('stage_ready',1);});")) ||
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
	webui_bind(w, "stage_ready", cb_ready);
	webui_bind(w, "kamikaze", cb_kamikaze);
	g_ready = 0;
	g_kamikaze = 0;
	// The page only reports that it is connected here. Destroying the
	// window from the connect callback itself would race `webui_show()`,
	// which returns the connection status: the window would already be
	// gone by the time it returns, and the show would look failed.
	if (!webui_show(w, make_page("Destroy from callback", "ready(function(){webui.call('stage_ready',1);});"))) {
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
		webui_bind(w2, "stage_ready", cb_ready);
		g_ready = 0;
		if (!webui_show(w2, make_page("Destroy from callback", "ready(function(){webui.call('stage_ready',1);});"))
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
	webui_bind(w, "stage_ready", cb_ready);
	for (int c = 0; c < 5 && ok; c++) {
		g_ready = 0;
		if (!webui_show(w, make_page("Rapid cycle", "ready(function(){webui.call('stage_ready',1);});"))) {
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
	stage_concurrent,
	stage_destroy_from_callback,
	stage_rapid_open_close
};

// Run every stage in order, and stop at the first failure. Only a full
// run writes the result files, they tell if the whole suite passed.
static void run_all_stages(void) {

	remove("test_pass.txt");
	remove("test_error.txt");
	status_run("resetStages();");
	status_run("setBanner('Running all stages...','');");

	int i;
	bool all = true;
	for (i = 0; i < STAGES && all && !g_abort; i++)
		all = stage_fns[i](i);

	if (g_abort)
		return;

	if (all) {
		g_finished = 1;
		FILE* f = fopen("test_pass.txt", "w");
		if (f != NULL)
			fclose(f);
		printf("ALL %d STAGES PASSED\n", STAGES);
		status_run("setBanner('ALL %d STAGES PASSED - every green window is still connected','pass');", STAGES);
	} else {
		g_finished = 2;
		for (int k = i; k < STAGES; k++)
			status_run("setStage(%d,'SKIP','Stopped after a failure');", k);
		FILE* f = fopen("test_error.txt", "w");
		if (f != NULL)
			fclose(f);
		printf("STOPPED: stage %d failed\n", i);
		status_run("setBanner('STOPPED - stage %d failed','fail');", i);
	}
	fflush(stdout);
}

// Run one stage alone, for a user who wants to test a single case.
// This does not touch the result files.
static void run_single_stage(int idx) {

	status_run("setBanner('Running stage %d alone...','');", idx + 1);

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

	// Serve run requests until the app exits. A request comes from
	// the Start button, from a stage Run button, or from auto mode.
	while (!g_abort) {

		if (g_start) {
			g_start = 0;
			g_busy = 1;
			run_all_stages();
			g_busy = 0;
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
	"#banner{padding:10px 14px;border-radius:8px;background:#2b3050;margin:14px 0;font-weight:600}"
	"#banner.pass{background:#12391f;color:#7ce38b}"
	"#banner.fail{background:#46151a;color:#ff8089}"
	"table{width:100%;border-collapse:collapse}"
	"td,th{padding:8px 10px;border-bottom:1px solid #23274a;text-align:left;font-size:14px}"
	".st{width:110px;font-weight:600}"
	".st.pass{color:#7ce38b}.st.fail{color:#ff8089}.st.run{color:#ffd866}.st.skip{color:#8a92b2}"
	".dt{color:#9aa0b8}"
	"</style></head><body>"
	"<h1>WebUI Stress Test</h1>"
	"<div>"
	"<button id=\"bstart\" onclick=\"setBusy(true);webui.call('start_test');\">Start</button>"
	"<button id=\"bexit\" onclick=\"webui.call('app_exit');\">Exit</button>"
	"</div>"
	"<div id=\"banner\">Ready. Click Start to run every stage, or Run to test a single stage</div>"
	"<table id=\"tbl\"><tr><th>#</th><th>Stage</th><th class=\"st\">Status</th><th>Details</th><th></th></tr></table>"
	"<script>"
	"const names=['Dashboard connection','Open / close / reopen window',"
	"'C to JS calls (webui_run / webui_script)','JS to C calls and arguments',"
	"'Large payloads (multi-packet)','Many bindings','Navigation / content reload',"
	"'Multi-window churn','Port change reload (webui_set_port)','Live root folder change',"
	"'Concurrent create / destroy','Destroy from callback','Rapid open / close'];"
	"const tbl=document.getElementById('tbl');"
	"names.forEach((n,i)=>{const r=tbl.insertRow(-1);r.insertCell(0).textContent=i+1;"
	"r.insertCell(1).textContent=n;const s=r.insertCell(2);s.className='st';s.id='st'+i;"
	"s.textContent='PENDING';const d=r.insertCell(3);d.className='dt';d.id='dt'+i;"
	"const a=r.insertCell(4);const b=document.createElement('button');b.className='run';"
	"b.textContent='Run';b.onclick=function(){setBusy(true);webui.call('run_stage',i);};"
	"a.appendChild(b);});"
	"function setStage(i,st,dt){const s=document.getElementById('st'+i);s.textContent=st;"
	"s.className='st '+(st=='PASS'?'pass':st=='FAIL'?'fail':st=='SKIP'?'skip':st=='PENDING'?'':'run');"
	"if(dt!==undefined)document.getElementById('dt'+i).textContent=dt;}"
	"function setBanner(t,cls){const b=document.getElementById('banner');"
	"b.textContent=t;b.className=cls||'';}"
	"function setBusy(b){document.getElementById('bstart').disabled=b;"
	"document.querySelectorAll('button.run').forEach(x=>{x.disabled=b;});}"
	"function resetStages(){for(let i=0;i<names.length;i++)setStage(i,'PENDING','');}"
	"</script></body></html>";

int main(int argc, char* argv[]) {

	bool autorun = (argc > 1 && argv[1] != NULL && strcmp(argv[1], "auto") == 0);

	remove("test_pass.txt");
	remove("test_error.txt");

	printf("WebUI Stress Test%s\n", autorun ? " (auto mode)" : "");
	fflush(stdout);

	status_win = webui_new_window();
	webui_bind(status_win, "start_test", cb_start);
	webui_bind(status_win, "run_stage", cb_run_stage);
	webui_bind(status_win, "app_exit", cb_exit);
	if (!webui_show(status_win, status_page)) {
		printf("Could not open the status window\n");
		FILE* f = fopen("test_error.txt", "w");
		if (f != NULL)
			fclose(f);
		return 1;
	}

	if (autorun)
		g_start = 1;

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

	webui_clean();
	return (g_finished == 1 ? 0 : 1);
}

#if defined(_MSC_VER)
int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow) { return main(__argc, __argv); }
#endif
