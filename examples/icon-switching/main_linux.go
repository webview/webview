// This example shows you how to change the icon in the webview on Linux

package main

/*
#cgo linux pkg-config: gtk+-3.0

#include <gtk/gtk.h>

void set_external_icon(void *ptr, char* iconPath) {
	gtk_window_set_icon_from_file(ptr, iconPath, NULL);
}
*/
import "C"
import (
	"time"
	"unsafe"

	"github.com/webview/webview"
)

// Set icon from external file
func setExternalIcon(w webview.WebView, iconPath string) {

	hwnd := w.Window()
	cstr := C.CString(iconPath)

	defer C.free(unsafe.Pointer(cstr))

	C.set_external_icon(hwnd, cstr)
}

func main() {
	w := webview.New(false)

	setExternalIcon(w, "icons/png/fish.png")

	go func() {
		time.Sleep(3 * time.Second)
		setExternalIcon(w, "icons/png/tiger.png")
		time.Sleep(3 * time.Second)
		setExternalIcon(w, "icons/png/zebra.png")
		time.Sleep(3 * time.Second)
		setExternalIcon(w, "icons/png/peacock.png")
	}()

	defer w.Destroy()
	w.SetTitle("Icon example")
	w.SetSize(800, 600, webview.HintNone)
	w.Navigate("https://duckduckgo.com")

	w.Run()
}
