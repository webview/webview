package main

import "github.com/zserge/webview"

func main() {
	webview.Open("Hello", "https://golang.org", 800, 600, true)
}
