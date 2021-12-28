package main

import "github.com/phillvancejr/webview"

func main() {
    w := webview.New()
    defer w.Destroy()

    w.SetSize(300, 100, webview.HintFixed)
    w.Center()
    w.NoCtx()
    w.Topmost(true)
    w.SetTitle("Hello Go Webview!")
    w.Navigate(`data:text/html,<h1>Hello Webview!</h1>`)
    w.Run()
}
