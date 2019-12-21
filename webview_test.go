package webview

import (
	"log"
	"testing"
)

func Example() {
	w := New(true)
	defer w.Destroy()
	w.Navigate("https://github.com")
	w.SetTitle("Hello")
	w.Dispatch(func() {
		log.Println("Hello dispatch")
		w.Terminate()
	})
	w.Run()
}

func TestExample(t *testing.T) {
	if !testing.Verbose() {
		return
	}
	Example()
}
