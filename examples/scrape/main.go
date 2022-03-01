// This example shows how to use the webview
// package to scrape data from JavaScript.
package main

import (
	"fmt"
	"os"
	"time"

	"github.com/webview/webview"
)

const account = "CloudNativeFdn"

func main() {
	w := webview.New(false)
	defer w.Destroy()
	w.SetTitle("scape example")
	w.SetSize(800, 600, webview.HintNone)
	w.Navigate("https://twitter.com/" + account)
	w.Bind("getFollowers", callback)

	w.Dispatch(func() {
		go func() {
			time.Sleep(time.Second * 4)
			w.Eval("getFollowers(document.body.querySelectorAll('[href=\"/" + account + "/followers\"]')[0].text);")
		}()
	})
	w.Run()
}

func callback(followers string) {
	fmt.Println(account, "has", followers)
	os.Exit(0)
}
