package main

import (
	"fmt"
	"time"

	"github.com/webview/webview"
)

const html = `<button id="increment">Tap me</button>
<div>You tapped <span id="count">0</span> time(s).</div>
<hr />
<label for="name">Enter your name:</label>
<input id="name" type="text" name="name" />
<button id="submitBtn" type="submit">Submit</button>
<div id="result"></div>
<script>
	const [incrementElement, countElement, nameElement, submitElement, resultElement] =
	document.querySelectorAll("#increment, #count, #name, #submitBtn, #result");
	document.addEventListener("DOMContentLoaded", () => {
		incrementElement.addEventListener("click", () => {
			window.increment().then(result => {
				countElement.textContent = result.count;
			});
		});

		submitElement.addEventListener("click", () => {
			submitBtn.disabled = true;
			resultElement.textContent = "Loading...";
			var name = nameElement.value;
			window.sendName(name).then(result => {
				resultElement.textContent = result.name;
				submitBtn.disabled = false;
			});
		});
	});
</script>`

type IncrementResult struct {
	Count uint `json:"count"`
}

type SendNameResult struct {
	Name string `json:"name"`
}

func main() {
	var count uint = 0
	w := webview.New(false)
	defer w.Destroy()
	w.SetTitle("Bind Example")
	w.SetSize(480, 320, webview.HintNone)

	// A binding that increments a value and immediately returns the new value.
	w.Bind("increment", func(id string) {
		count++

		fmt.Println("incrementing count to", count)

		w.Return(
			id,
			webview.StatusSuccess,
			IncrementResult{Count: count},
		)
	})

	// A binding that returns a string after a delay.
	w.Bind("sendName", func(id string, name string) {
		go func() {
			time.Sleep(2 * time.Second)

			var nameString string

			if len(name) > 0 {
				nameString = name
			} else {
				nameString = "World"
			}

			w.Return(id, webview.StatusSuccess, SendNameResult{Name: nameString})
		}()
	})

	w.SetHtml(html)
	w.Run()
}
