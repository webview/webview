// +build !vue,!react

package main

//go:generate go-bindata -o assets.go js/picodom/... js/styles.css

import (
	"github.com/zserge/webview"
)

var UIFrameworkName = "Picodom"

func LoadUIFramework(w webview.WebView) {
	// Inject Picodom.js
	w.Eval(string(MustAsset("js/picodom/vendor/picodom.js")))
	// Inject app code
	w.Eval(string(MustAsset("js/picodom/app.js")))
}
