package webview

import (
	"flag"
	"log"
	"os"
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

func TestMain(m *testing.M) {
	flag.Parse()
	if testing.Verbose() {
		Example()
	}
	os.Exit(m.Run())
}
