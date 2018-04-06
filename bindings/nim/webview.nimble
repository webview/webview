# Package

version       = "0.1.0"
author        = "zserge"
description   = "Nim bindings for webview"
license       = "MIT"
skipDirs      = @["examples"]

# Dependencies

requires "nim >= 0.17.2"

task test, "a simple test case":
    exec "nim c -r ../../examples/bindEx-nim/bindEx.nim"

task docs, "generate doc":
    exec "nim doc2 -o:webview.html webview.nim"

task clean, "clean tmp files":
    exec "rm -rf nimcache"
