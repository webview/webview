// This example shows you how to change the icon in the webview on Windows
//
// To build:
// - Install Go-Winres: go install github.com/tc-hib/go-winres@latest
// - Generate syso files: go-winres make
// - Compile exe: go build .

package main

/*
#include <windows.h>

void set_external_icon(const void *ptr, char* iconPath) {

	HICON iconBig = LoadImage(NULL, iconPath, IMAGE_ICON, GetSystemMetrics(SM_CXICON  ), GetSystemMetrics(SM_CXICON  ), LR_LOADFROMFILE);
	HICON iconSml = LoadImage(NULL, iconPath, IMAGE_ICON, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CXSMICON), LR_LOADFROMFILE);

	if (iconSml) SendMessage((HWND)ptr, WM_SETICON, ICON_SMALL, (LPARAM)iconSml);
	if (iconBig) SendMessage((HWND)ptr, WM_SETICON, ICON_BIG  , (LPARAM)iconBig);
}

void set_resource_icon(const void *ptr, char* name) {

	HINSTANCE hInstance = GetModuleHandle(NULL);

	HICON iconBig = (HICON)LoadImage(hInstance, name, IMAGE_ICON, GetSystemMetrics(SM_CXICON  ), GetSystemMetrics(SM_CXICON  ), LR_DEFAULTCOLOR);
	HICON iconSml = (HICON)LoadImage(hInstance, name, IMAGE_ICON, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);


	if (iconSml) SendMessage((HWND)ptr, WM_SETICON, ICON_SMALL, (LPARAM)iconSml);
	if (iconBig) SendMessage((HWND)ptr, WM_SETICON, ICON_BIG  , (LPARAM)iconBig);
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

// Set icon from resources embedded in rsrc_windows_amd64.syso / rsrc_windows_386.syso
// In this example, syso files were generated using https://github.com/tc-hib/go-winres
func setResourceIcon(w webview.WebView, resourceName string) {

	hwnd := w.Window()
	cstr := C.CString(resourceName)

	defer C.free(unsafe.Pointer(cstr))

	C.set_resource_icon(hwnd, cstr)
}

func main() {
	w := webview.New(false)

	setResourceIcon(w, "FISH")

	go func() {
		time.Sleep(3 * time.Second)
		setResourceIcon(w, "TIGER")
		time.Sleep(3 * time.Second)
		setExternalIcon(w, "icons/ico/zebra.ico")
		time.Sleep(3 * time.Second)
		setExternalIcon(w, "icons/ico/peacock.ico")
	}()

	defer w.Destroy()
	w.SetTitle("Icon example")
	w.SetSize(800, 600, webview.HintNone)
	w.Navigate("https://duckduckgo.com")

	w.Run()
}
