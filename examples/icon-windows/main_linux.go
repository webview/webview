// This example shows you how to change the icon in the webview on Linux

package main

/*
#cgo CFLAGS: -pthread -I/usr/include/gtk-3.0 -I/usr/include/at-spi2-atk/2.0 -I/usr/include/at-spi-2.0 -I/usr/include/dbus-1.0 -I/usr/lib/x86_64-linux-gnu/dbus-1.0/include -I/usr/include/gtk-3.0 -I/usr/include/gio-unix-2.0/ -I/usr/include/cairo -I/usr/include/pango-1.0 -I/usr/include/harfbuzz -I/usr/include/pango-1.0 -I/usr/include/atk-1.0 -I/usr/include/cairo -I/usr/include/pixman-1 -I/usr/include/freetype2 -I/usr/include/libpng16 -I/usr/include/freetype2 -I/usr/include/libpng16 -I/usr/include/gdk-pixbuf-2.0 -I/usr/include/libpng16 -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include
#cgo LDFLAGS:  -lgtk-3 -lgdk-3 -lpangocairo-1.0 -lpango-1.0 -latk-1.0 -lcairo-gobject -lcairo -lgdk_pixbuf-2.0 -lgio-2.0 -lgobject-2.0 -lglib-2.0

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
