package main

import (
    "github.com/phillvancejr/webview"
)



func main() {
    w := webview.NewWindow(true,nil)
    defer w.Destroy()
    w.SetSize(500,500, webview.HintNone)
    w.Center()
    w.NoCtx()
    w.SetTitle("Go Moving Square")

    content := webview.EscapeJs(`
    <canvas width=500 height=500 style="margin:0px;overflow:hidden;"></canvas>
    <script>
        ctx = document.querySelector('canvas').getContext('2d")
        ctx.fillStyle='black'
        ctx.fillRect(0,0,500,500)

        function draw(x,y,w,h) {
            ctx.fillStyle='white'
            ctx.fillRect(x,y,w,h)
        }
    </script>
    `)
    w.Navigate("data:text/html,"+content)
    w.Run()
}
