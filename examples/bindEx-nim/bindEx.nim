import webview
import strutils 
import os

const indexHTML = """
<!doctype html>
<html>
    <head>
        <meta http-equiv="X-UA-Compatible" content="IE=edge">
    </head>
    <body>
        <button onclick="api.close()">Close</button>
        <button onclick="api.open()">Open</button>
        <button onclick="api.opendir()">Open directory</button>
        <button onclick="api.save()">Save</button>
        <button onclick="api.message()">Message</button>
        <button onclick="api.info()">info</button>
        <button onclick="api.warn()">warn</button>
        <button onclick="api.error()">error</button>
        <button onclick="api.changeColor()">changeColor</button>
        <button onclick="api.toggleFullScreen()">toggleFullScreen</button>
        <button onclick="api.changeTitle(document.getElementById('new-title').value)">
            Change title
        </button>
        <input id="new-title" type="text" />
    </body>
</html>
"""

let fn="$1/xxx.html"%[getTempDir()]
writeFile(fn, indexHTML)
var w = newWebView("Simple window demo2", "file://" & fn)
var fullScreen = true
w.bindProcs("api"):
    proc open() = echo w.dialogOpen()
    proc save() = echo w.dialogSave()
    proc opendir() = echo w.dialogOpen(flag=dFlagDir)
    proc message() = w.msg("hello", "message")
    proc info() = w.info("hello", "info")
    proc warn() = w.warn("hello", "warn")
    proc error() = w.error("hello", "error")
    proc changeTitle(title: string) = w.setTitle(title)
    proc close() = w.terminate()
    proc changeColor() = w.setColor(210,210,210,100)
    proc toggleFullScreen() = fullScreen = not w.setFullscreen(fullScreen)

# w.setFullscreen()
w.run()
w.exit()
removeFile(fn)
