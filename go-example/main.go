package main

import (
	"html/template"
	"log"
	"net"
	"net/http"
	"os"
	"os/user"

	"github.com/zserge/webview"
)

var tmpl = template.Must(template.New("").Parse(`
<html>
	<head>
		<style>
		* { margin: 0; padding: 0; box-sizing: border-box; font-family: Helvetica, Arial, sans-serif; }
		body { color: #ffffff; background-color: #03a9f4; text-decoration: uppercase; font-size: 24px; }
		h1 { text-align: center; font-weight: normal}
		form { margin-left: auto; margin-right: auto; margin-top: 50px; width: 300px; }
		input[type="submit"] {
				border: 0 none;
				cursor: pointer;
				margin-top: 1em;
				background-color: #ffffff;
				color: #03a9f4;
				width: 100%;
				height: 2em;
				font-size: 24px;
				text-transform: uppercase;
		}
		</style>
	</head>
	<body>
	  <form action="/exit">
			<h1>Hello, {{ .Name }}!</h1>
			<input type="submit" value="Exit" />
		</form>
	</body>
</html>
`))

func main() {
	ln, err := net.Listen("tcp", "127.0.0.1:0")
	if err != nil {
		log.Fatal(err)
	}
	defer ln.Close()

	go func() {
		http.HandleFunc("/exit", func(w http.ResponseWriter, r *http.Request) {
			os.Exit(0)
		})
		http.HandleFunc("/", func(w http.ResponseWriter, r *http.Request) {
			u, _ := user.Current()
			tmpl.Execute(w, u)
		})
		log.Fatal(http.Serve(ln, nil))
	}()

	webview.Open("Hello", "http://"+ln.Addr().String(), 400, 300, false)
}
