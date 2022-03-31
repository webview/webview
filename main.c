// +build ignore

#include <stdio.h>
#include <stdlib.h>
#define WEBVIEW_HEADER
#include "webview.h"

void myFunc(const char *seq, const char *req, void *arg) {
	printf("Params: %s\n", req);
}

#ifdef WIN32
int WINAPI WinMain(HINSTANCE hInt, HINSTANCE hPrevInst, LPSTR lpCmdLine,
                   int nCmdShow) {
#else
int main() {
#endif
	webview_t w = webview_create(0, NULL);
	webview_set_title(w, "Webview Example");
	webview_set_size(w, 480, 320, WEBVIEW_HINT_NONE);
	webview_bind(w, "myFunc", myFunc, NULL);
	webview_navigate(w, "data:text/html, <button onclick='myFunc(\"Foo bar\")'>Click Me</button>");
	webview_run(w);
	webview_destroy(w);
	return 0;
}

