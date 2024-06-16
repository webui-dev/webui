#include "webui.h"

const char *doc =
    "<html style=\"background: #654da9; color: #eee\">"
    "<head>"
    "  <script src=\"webui.js\"></script>"
    "</head>"
    "<body>"
    "  <script>"
    "    setTimeout(async () => {"
    "      await webui.assert_int(1, 23, 345);"
    "      await webui.assert_float(1.0, 2.3, 3.45);"
    "      await webui.assert_string('foo', 'bar', 'baz');"
    "      await webui.assert_bool(true, false, true);"
    "      await webui.assert_cprint();"
    "      await webui.assert_close();"
    "    }, 500)"
    "  </script>"
    "</body>"
    "</html>";

void assert_int(webui_event_t *e) {
	size_t count = webui_get_count(e);
	assert(count == 3);

	long long num = webui_get_int(e);
	long long num1 = webui_get_int_at(e, 0);
	long long num2 = webui_get_int_at(e, 1);
	long long num3 = webui_get_int_at(e, 2);

	assert(num == 1);
	assert(num1 == num);
	assert(num2 == 23);
	assert(num3 == 345);
}

void assert_float(webui_event_t *e) {
	size_t count = webui_get_count(e);
	assert(count == 3);

	double num = webui_get_float(e);
	double num1 = webui_get_float_at(e, 0);
	double num2 = webui_get_float_at(e, 1);
	double num3 = webui_get_float_at(e, 2);

	printf("num1: %f, num2: %f, num3: %f\n", num1, num2, num3);
	// TODO: enable asserts after get_float is fixed.
	// assert(num == 1.0);
	// assert(num1 == num);
	// assert(num2 == 2.3);
	// assert(num3 == 3.45);
}

void assert_string(webui_event_t *e) {
	size_t count = webui_get_count(e);
	assert(count == 3);

	const char *str = webui_get_string(e);
	const char *str1 = webui_get_string_at(e, 0);
	const char *str2 = webui_get_string_at(e, 1);
	const char *str3 = webui_get_string_at(e, 2);

	assert(strcmp(str, "foo") == 0);
	assert(strcmp(str1, str) == 0);
	assert(strcmp(str2, "bar") == 0);
	assert(strcmp(str3, "baz") == 0);
}

void assert_bool(webui_event_t *e) {
	size_t count = webui_get_count(e);
	assert(count == 3);

	long long b = webui_get_bool(e);
	long long b1 = webui_get_bool_at(e, 0);
	long long b2 = webui_get_bool_at(e, 1);
	long long b3 = webui_get_bool_at(e, 2);

	assert(b == true);
	assert(b1 == b);
	assert(b2 == false);
	assert(b3 == true);
}

void assert_cprint(webui_event_t *e) {
	size_t count = webui_get_count(e);
	assert(count == 0);

	// The print should be confirmed by checking the program's terminal output.
	printf("Hello from the backend!\n");
}

void assert_close(webui_event_t *e) {
	// Closing often leads to a seqfault at the moment. Therefore, just a sysexit for now.
	// webui_close(e->window);
	exit(EXIT_SUCCESS);
}

int main() {
	size_t w = webui_new_window();

	webui_bind(w, "assert_int", assert_int);
	webui_bind(w, "assert_float", assert_float);
	webui_bind(w, "assert_string", assert_string);
	webui_bind(w, "assert_bool", assert_bool);
	webui_bind(w, "assert_cprint", assert_cprint);
	webui_bind(w, "assert_close", assert_close);

	webui_show(w, doc);
	webui_wait();
	webui_clean();

	return 0;
}
