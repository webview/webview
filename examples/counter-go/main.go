package main

import (
	"fmt"
	"html/template"

	"github.com/zserge/webview"
)

type Counter struct {
	Value int `json:"value"`
}

func (c *Counter) Add(n int) {
	c.Value = c.Value + int(n)
}

func (c *Counter) Reset() {
	c.Value = 0
}

func main() {
	w := webview.New(webview.Settings{
		Title: "Click counter: " + UIFrameworkName,
	})
	defer w.Exit()

	w.Dispatch(func() {
		// Inject controller
		w.Bind("counter", &Counter{})

		// Inject CSS
		w.Eval(fmt.Sprintf(`(function(css){
			var style = document.createElement('style');
			var head = document.head || document.getElementsByTagName('head')[0];
			if (style.styleSheet) {
				style.styleSheet.cssText = css;
			} else {
				style.appendChild(document.createTextNode(css));
			}
			head.appendChild(style);
		})("%s")`, template.JSEscapeString(string(MustAsset("js/styles.css")))))

		// Inject web UI framework and app UI code
		LoadUIFramework(w)
	})
	w.Run()
}
