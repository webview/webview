package main

import (
	"fmt"
	"html/template"

	"github.com/zserge/webview"
)

// Counter is a simple example of automatic Go-to-JS data binding
type Counter struct {
	Value int `json:"value"`
}

// Add increases the value of a counter by n
func (c *Counter) Add(n int) {
	c.Value = c.Value + int(n)
}

// Reset sets the value of a counter back to zero
func (c *Counter) Reset() {
	c.Value = 0
}

func main() {
	w := webview.New(webview.Settings{
		Title: "Click counter: " + uiFrameworkName,
	})
	defer w.Exit()

	w.Dispatch(func() {
		// Inject controller
		w.Bind("counter", &Counter{})

		// Inject CSS
		w.Eval(fmt.Sprintf(`(function(css){
			var style = document.createElement('style');
			var head = document.head || document.getElementsByTagName('head')[0];
			style.setAttribute('type', 'text/css');
			if (style.styleSheet) {
				style.styleSheet.cssText = css;
			} else {
				style.appendChild(document.createTextNode(css));
			}
			head.appendChild(style);
		})("%s")`, template.JSEscapeString(string(MustAsset("js/styles.css")))))

		// Inject web UI framework and app UI code
		loadUIFramework(w)
	})
	w.Run()
}
