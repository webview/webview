package main

import (
	"time"

	"github.com/webview/webview"
)

const html = `<button id="increment">Tap me</button>
<div>You tapped <span id="count">0</span> time(s).</div>
<button id="compute">Compute</button>
<div>Result of computation: <span id="compute-result">0</span></div>
<script>
  const [incrementElement, countElement, computeElement, computeResultElement] =
    document.querySelectorAll("#increment, #count, #compute, #compute-result");
  document.addEventListener("DOMContentLoaded", () => {
    incrementElement.addEventListener("click", () => {
      window.increment().then(result => {
        countElement.textContent = result.count;
      });
    });
    computeElement.addEventListener("click", () => {
      computeElement.disabled = true;
      window.compute(6, 7).then(result => {
		console.log(result);
        computeResultElement.textContent = result;
        computeElement.disabled = false;
      });
    });
  });
</script>`

type IncrementResult struct {
	Count uint `json:"count"`
}

func main() {
	var count uint = 0
	w := webview.New(false)
	defer w.Destroy()
	w.SetTitle("Bind Example")
	w.SetSize(480, 320, webview.HintNone)

	// A binding that increments a value and immediately returns the new value.
	w.Bind("increment", func() IncrementResult {
		count++
		return IncrementResult{Count: count}
	})

	// A binding that performs a long-running computation and returns the result
	w.Bind("compute", func(a, b int) chan webview.BindCallbackResult {
		ch := make(chan webview.BindCallbackResult)
		go func() {
			defer close(ch)
			time.Sleep(1 * time.Second)
			ch <- webview.BindCallbackResult{Value: a * b, Error: nil}
		}()
		return ch
	})

	w.SetHtml(html)
	w.Run()
}
