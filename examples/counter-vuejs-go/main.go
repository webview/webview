package main

//go:generate go-bindata -o assets.go app.js vue.min.js styles.css

import (
	"fmt"
	"log"
	"strings"

	"github.com/zserge/webview"
)

type Counter struct {
	Value int `json:"value"`
}

func (c *Counter) Add(n int) {
	c.Value = c.Value + int(n)
	log.Println(c.Value)
}

func (c *Counter) Reset() {
	c.Value = 0
}

func main() {
	w := webview.New(webview.Settings{
		Title: "Controller binding example",
	})
	defer w.Exit()

	w.Dispatch(func() {
		_, err := w.Bind("counter", &Counter{})
		if err != nil {
			log.Fatal(err)
		}
		css := MustAsset("styles.css")
		w.Eval(fmt.Sprintf(`
			(function(){
				var n=document.createElement('style');
				n.innerHTML="%s";
				document.body.appendChild(n);
		})()`, strings.Replace(string(css), "\n", "\\\n", -1)))
		w.Eval(string(MustAsset("vue.min.js")))
		w.Eval(string(MustAsset("app.js")))
	})
	w.Run()
}
