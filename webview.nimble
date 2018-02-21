# Package

version       = "0.1.0"
author        = "oskca"
description   = "Nim bindings for zserge\'s webview"
license       = "MIT"
skipDirs      = @["tests"]

# Dependencies

requires "nim >= 0.17.2"

task test, "a simple test case":
    exec "nim c -r examples/bindEx-nim/bindEx.nim"

task docs, "generate doc":
    exec "nim doc2 -o:webview.html webview.nim"

task clean, "clean tmp files":
    exec "rm -rf nimcache"
