package main

import (
    "github.com/phillvancejr/webview"
    "fmt"
)

const (
    width = 500
    height = 500
)

func main() {
    w := webview.New(false)
    defer w.Destroy()
    w.SetSize(width,height, webview.HintNone)
    w.SetTitle("Go Canvas")

    content := fmt.Sprintf(`
    <body style="margin:0px;overflow:hidden;">
    <canvas></canvas>
    </body>
    <script>
        const width = %d
        const height = %d
        c = document.querySelector('canvas')
        c.width = width
        c.height = height
        ctx = c.getContext('2d')
        ctx.fillStyle='black'
        ctx.fillRect(0,0,width,height)

        function draw(x,y,w,h) {
            ctx.fillStyle='white'
            ctx.fillRect(x,y,w,h)
        }

        // notify main thread that dom is loaded
        domLoaded()
        testing()
    </script>
    `, width, height)
    // channel for domLoaded notification
    domLoaded := make(chan bool)
    // this function allows JS to notify us when the dom is loaded
    w.Bind("domLoaded", func(){
        domLoaded <- true 
    })

    // run in goroutine to avoid blocking 
    go func() {
        // wait for dom to load
        <-domLoaded
        // w.Dispatch means do something on the main thread
        // you need this when calling JS from goroutines
        w.Dispatch(func(){
            // call the JS draw function
            w.Eval("draw(10,10, 50,50)") 
            w.Eval("ctx.font='100px Arial';ctx.fillText('Hello Go!', 30, 200)")
        })
    }()
    
    w.Navigate("data:text/html,"+content)
    w.Run()
}
