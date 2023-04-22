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
	w.SetTitle("Hello")
	w.Bind("noop", func(id string) {
		log.Println("hello")
		w.Return(id, StatusSuccess, "hello")
	})
	w.Bind("add", func(id string, a, b int) {
		w.Return(id, StatusSuccess, a+b)
	})
	w.Bind("quit", func() {
		w.Terminate()
	})
	w.SetHtml(`<!doctype html>
		<html>
			<body>hello</body>
			<script>
				window.onload = function() {
					document.body.innerText = ` + "`hello, ${navigator.userAgent}`" + `;
					noop().then(function(res) {
						console.log('noop res', res);
						add(1, 2).then(function(res) {
							console.log('add res', res);
							quit();
						});
					});
				};
			</script>
		</html>
	)`)
	w.Run()
}

func TestMain(m *testing.M) {
	flag.Parse()
	if testing.Verbose() {
		Example()
	}
	os.Exit(m.Run())
}
