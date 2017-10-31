package main

//go:generate go-bindata -o assets.go app.jsx styles.css react.production.min.js react-dom.production.min.js babel.min.js

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
		Title: "ReactJS click counter example",
	})
	defer w.Exit()

	w.Dispatch(func() {
		// Inject controller
		w.Bind("counter", &Counter{})

		// Inject CSS
		w.Eval(fmt.Sprintf(`(function(){
			var n = document.createElement('style');
			n.innerHTML="%s";
			document.body.appendChild(n);
		})()`, template.JSEscapeString(string(MustAsset("styles.css")))))

		// Inject ReactJS
		w.Eval(string(MustAsset("react.production.min.js")))
		w.Eval(string(MustAsset("react-dom.production.min.js")))

		// Inject our app code
		w.Eval(fmt.Sprintf(`(function(){
			var n=document.createElement('script');
			n.type = 'text/babel';
			n.appendChild(document.createTextNode("%s"));
			document.body.appendChild(n);
		})()`, template.JSEscapeString(string(MustAsset("app.jsx")))))

		// Inject and run Babel to convert ES6/JSX into ES5
		w.Eval(string(MustAsset("babel.min.js")))
		w.Eval(`Babel.transformScriptTags()`)
	})
	w.Run()
}
