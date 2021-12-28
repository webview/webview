package main

import "github.com/phillvancejr/webview"

func main() {
	w := webview.New()
	defer w.Destroy()
	w.SetSize(800, 600, webview.HintNone)
	w.Center()	// Center the window
	w.NoCtx()	// remove the right click context menu
	w.SetTitle("Minimal Go webview example")
    w.Navigate("https://en.wikipedia.org/wiki/Go_(programming_language)")
	w.Run()
}
