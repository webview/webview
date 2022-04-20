package main

import (
	"github.com/phillvancejr/webview"
	"embed"
)

//go:embed web
var web embed.FS

func main() {
	webview.App{
		Width: 500,
		Height: 500,
		Title: "Webview App Test",
		// webview.App.Content must be an embed.FS
		Content: web,
		// the root folder containing the content
		ContentRoot: "web",
	}.Run()
}
