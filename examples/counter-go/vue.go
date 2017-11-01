// +build vue

package main

//go:generate go-bindata -o assets.go js/vue/... js/styles.css

import (
	"github.com/zserge/webview"
)

var UIFrameworkName = "VueJS"

func LoadUIFramework(w webview.WebView) {
	// Inject Vue.js
	w.Eval(string(MustAsset("js/vue/vendor/vue.min.js")))
	// Inject app code
	w.Eval(string(MustAsset("js/vue/app.js")))
}
