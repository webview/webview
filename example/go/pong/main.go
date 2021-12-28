package main

import (
	"bytes"
	"fmt"
	"github.com/phillvancejr/webview"
	"log"
	"math"
	"math/rand"
	"text/template"
	"time"
)

var (
	// webview instance
	wv webview.WebView

	// game settings
	game = struct {
		Title  string
		Width  int
		Height int
		// ball speed
		bSpeed float64
		// ball speed
		pSpeed float64
	}{
		Title:  "Pong",
		Width:  500,
		Height: 300,
		bSpeed: 150.0,
		pSpeed: 200.0,
	}

	// html content
	content = `
    <body style="margin:0px;overflow:hidden;">
        <canvas></canvas>
    </body>
    <script>
        // the {{ .Width }} is using Go's templating format
        const width = {{ .Width }}
        const height = {{ .Height }}
        let started = false
        let lastTime

        c = document.querySelector('canvas')
        c.width = width
        c.height = height

        ctx = c.getContext('2d')

        function clear() {
            ctx.fillStyle='black'
            ctx.fillRect(0,0,width,height)
        }

        function rect(x, y, w, h) {
            ctx.fillStyle='white'
            ctx.fillRect(x,y,w,h)
        }
        // key handers
        window.onkeydown = e => {
            e.preventDefault()
            // start game on space key
            if (e.keyCode == 32 && !started) {
                started = true
                start()
                lastTime = Date.now()
                loop()
            }
            // pass keyCode and pressed bool to Go onKey function
            onKey(e.keyCode, true)

        }
        window.onkeyup = e => {
            e.preventDefault()
            // pass keyCode and pressed bool to Go onKey function
            onKey(e.keyCode, false)

        }
        
        // render loop
        function loop() {
            requestAnimationFrame(loop)
            let now = Date.now()
            // calculate delta time
            let dt = (now - lastTime) / 1000
            lastTime = now
            // call update with delta time
            update(dt)
            // call draw function
            draw()
        }

        // draw first frame so we have something before pressing space
        resetBall()
        draw()
    </script>
    `
)

const (
	// paddle width
	pW = 10.0
	// paddle height
	pH = 50.0
	// ball size
	bSize = pW
	// arrow keys
	up   = 38
	down = 40
)

var (
	// entity variables
	// player X
	pX = pW
	// player Y
	pY = float64(game.Height/2 - pH/2)
	// cpu X
	cpuX = float64(game.Width - pW*2)
	// cpu Y
	cpuY = float64(game.Height/2 - pH/2)
	// center ball at beginning
	// ball X
	bX = 0.0
	// ball Y
	bY = 0.0
	// ball X Direction
	bXD = 0.0
	// ball Y Direction
	bYD = 0.0
	// movement variables
	mUp   = false
	mDown = false
)

func main() {
	fmt.Println("Click on the Window and press Space to Start\nuse up and down arrow to move")
	// seed the random engine
	rand.Seed(time.Now().Unix())

	// insert game settings into html string using Golang's text/template library
	t, e := template.New("").Parse(content)
	if e != nil {
		log.Fatal(e)
	}
	// byte buffer to store new content string with game settings inserted
	var buf bytes.Buffer
	// execute the template
	e = t.Execute(&buf, game)
	if e != nil {
		log.Fatal(e)
	}
	// assign content to the updated html with the game settings inserted
	content = buf.String()
	// create webview window
	wv = webview.New()
	wv.SetSize(game.Width, game.Height, webview.HintFixed)
	wv.Center()
	wv.NoCtx()
	wv.SetTitle(game.Title)

	// expose functions via Bind
	wv.Bind("start", start)
	wv.Bind("resetBall", resetBall)
	wv.Bind("update", update)
	wv.Bind("draw", draw)
	wv.Bind("onKey", onKey)
	// navigate to content string
	wv.Navigate("data:text/html," + content)
	wv.Run()
}

func onKey(key int, pressed bool) {
	move := true
	if !pressed {
		move = false
	}

	if key == up {
		mUp = move
	}

	if key == down {
		mDown = move
	}
}

// helper function for keeping things in bounds
func clamp(val, min, max float64) float64 {
	return math.Min(math.Max(val, min), max)
}

// put the ball center
func resetBall() {
	// center ball
	bX = float64(game.Width/2 - bSize/2)
	bY = float64(game.Height/2 - bSize/2)
}

// start the game
func start() {
	// random dir
	dirs := [2]float64{-1.0, 1.0}
	bXD = dirs[rand.Intn(2)]
	bYD = dirs[rand.Intn(2)]
}

// these functions are exposed to and called from JS
func update(dt float64) {
	// update ball
	bX += bXD * game.bSpeed * dt
	bY += bYD * game.bSpeed * dt
	//fmt.Printf("bSpeed: %f bXD: %f bX: %f\n", bSpeed, bXD,bX)
	// update cpu
	cpuY = bY - pH/2

	// update player
	if mUp {
		pY -= game.pSpeed * dt
	}

	if mDown {
		pY += game.pSpeed * dt
	}

	// collision and clamping
	// ball with wall
	ballXMax := float64(game.Width - bSize)
	ballYMax := float64(game.Height - bSize)
	bX = clamp(bX, 0, ballXMax)
	bY = clamp(bY, 0, ballYMax)

	// reset the ball if it hits the left or right wall
	if bX == 0 || bX == ballXMax {
		resetBall()
	}
	// reverse its direction if it hits a ceiling or floor
	if bY == 0 || bY == ballYMax {
		bYD *= -1.0
	}

	// ball with paddle
	// player
	if bX <= pX+pW && bY >= pY && bY <= pY+pH && bXD == -1 {
		bXD *= -1
	}
	// cpu
	if bX+bSize >= cpuX && bY >= cpuY && bY <= cpuY+pH && bXD == 1 {
		bXD *= -1
	}

	// paddle with ceiling/floor
	pY = clamp(pY, 0, float64(game.Height-pH))
	cpuY = clamp(cpuY, 0, float64(game.Height-pH))

}

func draw() {
	// clear screen to black
	clear()
	// draw ball
	rect(bX, bY, bSize, bSize)
	// draw player
	rect(pX, pY, pW, pH)
	// draw cpu
	rect(cpuX, cpuY, pW, pH)
}

// these functions wrap JS functions to make it easier to use them from Go
func clear() {
	wv.Eval("clear()")
}

func rect(x, y, w, h float64) {
	wv.Eval(fmt.Sprintf("rect(%f,%f,%f,%f)", x, y, w, h))
}
