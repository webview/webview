#ifndef WEBVIEW_H
#define WEBVIEW_H

static int webview(const char *title, const char *url, int w, int h,
		   int resizable);

#if defined(WEBVIEW_GTK)
#include <gtk/gtk.h>
#include <webkit/webkit.h>

static void webview_desroy_cb(GtkWidget *widget, GtkWidget *window) {
  (void)widget;
  (void)window;
  gtk_main_quit();
}

static gboolean webview_context_menu_cb(WebKitWebView *webview,
					GtkWidget *default_menu,
					WebKitHitTestResult *hit_test_result,
					gboolean triggered_with_keyboard,
					gpointer userdata) {
  (void)webview;
  (void)default_menu;
  (void)hit_test_result;
  (void)triggered_with_keyboard;
  (void)userdata;
  return TRUE;
}

static int webview(const char *title, const char *url, int width, int height,
		   int resizable) {
  GtkWidget *window;
  GtkWidget *scroller;
  GtkWidget *webview;

  if (gtk_init_check(0, NULL) == FALSE) {
    return -1;
  }

  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(window), title);

  if (resizable) {
    gtk_window_set_default_size(GTK_WINDOW(window), width, height);
  } else {
    gtk_widget_set_size_request(window, width, height);
  }
  gtk_window_set_resizable(GTK_WINDOW(window), !!resizable);
  gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);

  scroller = gtk_scrolled_window_new(NULL, NULL);
  gtk_container_add(GTK_CONTAINER(window), scroller);

  webview = webkit_web_view_new();
  webkit_web_view_load_uri(WEBKIT_WEB_VIEW(webview), url);
  gtk_container_add(GTK_CONTAINER(scroller), webview);

  gtk_widget_show_all(window);

  g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(webview_desroy_cb),
		   NULL);
  g_signal_connect(G_OBJECT(webview), "context-menu",
		   G_CALLBACK(webview_context_menu_cb), NULL);

  gtk_main();
  return 0;
}

#elif defined(WEBVIEW_WINAPI)
#include <windows.h>

#include <exdisp.h>
#include <mshtmhst.h>
#include <mshtml.h>

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "oleaut32.lib")

typedef struct {
  IOleInPlaceFrame frame;
  HWND window;
} _IOleInPlaceFrameEx;

typedef struct {
  IOleInPlaceSite inplace;
  _IOleInPlaceFrameEx frame;
} _IOleInPlaceSiteEx;

typedef struct { IDocHostUIHandler ui; } _IDocHostUIHandlerEx;

typedef struct {
  IOleClientSite client;
  _IOleInPlaceSiteEx inplace;
  _IDocHostUIHandlerEx ui;
} _IOleClientSiteEx;

static HRESULT STDMETHODCALLTYPE Site_AddRef(IOleClientSite FAR *This) {
  return 1;
}
static HRESULT STDMETHODCALLTYPE Site_Release(IOleClientSite FAR *This) {
  return 1;
}
static HRESULT STDMETHODCALLTYPE Site_SaveObject(IOleClientSite FAR *This) {
  return E_NOTIMPL;
}
static HRESULT STDMETHODCALLTYPE Site_GetMoniker(IOleClientSite FAR *This,
						 DWORD dwAssign,
						 DWORD dwWhichMoniker,
						 IMoniker **ppmk) {
  return E_NOTIMPL;
}
static HRESULT STDMETHODCALLTYPE
Site_GetContainer(IOleClientSite FAR *This, LPOLECONTAINER FAR *ppContainer) {
  *ppContainer = 0;
  return E_NOINTERFACE;
}
static HRESULT STDMETHODCALLTYPE Site_ShowObject(IOleClientSite FAR *This) {
  return NOERROR;
}
static HRESULT STDMETHODCALLTYPE Site_OnShowWindow(IOleClientSite FAR *This,
						   BOOL fShow) {
  return E_NOTIMPL;
}
static HRESULT STDMETHODCALLTYPE
Site_RequestNewObjectLayout(IOleClientSite FAR *This) {
  return E_NOTIMPL;
}
static HRESULT STDMETHODCALLTYPE Site_QueryInterface(IOleClientSite FAR *This,
						     REFIID riid,
						     void **ppvObject) {
  if (!memcmp(riid, &IID_IUnknown, sizeof(GUID)) ||
      !memcmp(riid, &IID_IOleClientSite, sizeof(GUID)))
    *ppvObject = &((_IOleClientSiteEx *)This)->client;
  else if (!memcmp(riid, &IID_IOleInPlaceSite, sizeof(GUID)))
    *ppvObject = &((_IOleClientSiteEx *)This)->inplace;
  else if (!memcmp(riid, &IID_IDocHostUIHandler, sizeof(GUID)))
    *ppvObject = &((_IOleClientSiteEx *)This)->ui;
  else {
    *ppvObject = 0;
    return (E_NOINTERFACE);
  }
  return S_OK;
}
static HRESULT STDMETHODCALLTYPE InPlace_QueryInterface(
    IOleInPlaceSite FAR *This, REFIID riid, LPVOID FAR *ppvObj) {
  return (Site_QueryInterface(
      (IOleClientSite *)((char *)This - sizeof(IOleClientSite)), riid, ppvObj));
}
static HRESULT STDMETHODCALLTYPE InPlace_AddRef(IOleInPlaceSite FAR *This) {
  return (1);
}
static HRESULT STDMETHODCALLTYPE InPlace_Release(IOleInPlaceSite FAR *This) {
  return (1);
}
static HRESULT STDMETHODCALLTYPE InPlace_GetWindow(IOleInPlaceSite FAR *This,
						   HWND FAR *lphwnd) {
  *lphwnd = ((_IOleInPlaceSiteEx FAR *)This)->frame.window;
  return S_OK;
}
static HRESULT STDMETHODCALLTYPE
InPlace_ContextSensitiveHelp(IOleInPlaceSite FAR *This, BOOL fEnterMode) {
  return E_NOTIMPL;
}
static HRESULT STDMETHODCALLTYPE
InPlace_CanInPlaceActivate(IOleInPlaceSite FAR *This) {
  return S_OK;
}
static HRESULT STDMETHODCALLTYPE
InPlace_OnInPlaceActivate(IOleInPlaceSite FAR *This) {
  return S_OK;
}
static HRESULT STDMETHODCALLTYPE
InPlace_OnUIActivate(IOleInPlaceSite FAR *This) {
  return S_OK;
}
static HRESULT STDMETHODCALLTYPE InPlace_GetWindowContext(
    IOleInPlaceSite FAR *This, LPOLEINPLACEFRAME FAR *lplpFrame,
    LPOLEINPLACEUIWINDOW FAR *lplpDoc, LPRECT lprcPosRect, LPRECT lprcClipRect,
    LPOLEINPLACEFRAMEINFO lpFrameInfo) {
  *lplpFrame = (LPOLEINPLACEFRAME) & ((_IOleInPlaceSiteEx *)This)->frame;
  *lplpDoc = 0;
  lpFrameInfo->fMDIApp = FALSE;
  lpFrameInfo->hwndFrame = ((_IOleInPlaceFrameEx *)*lplpFrame)->window;
  lpFrameInfo->haccel = 0;
  lpFrameInfo->cAccelEntries = 0;
  return S_OK;
}
static HRESULT STDMETHODCALLTYPE InPlace_Scroll(IOleInPlaceSite FAR *This,
						SIZE scrollExtent) {
  return E_NOTIMPL;
}
static HRESULT STDMETHODCALLTYPE
InPlace_OnUIDeactivate(IOleInPlaceSite FAR *This, BOOL fUndoable) {
  return S_OK;
}
static HRESULT STDMETHODCALLTYPE
InPlace_OnInPlaceDeactivate(IOleInPlaceSite FAR *This) {
  return S_OK;
}
static HRESULT STDMETHODCALLTYPE
InPlace_DiscardUndoState(IOleInPlaceSite FAR *This) {
  return E_NOTIMPL;
}
static HRESULT STDMETHODCALLTYPE
InPlace_DeactivateAndUndo(IOleInPlaceSite FAR *This) {
  return E_NOTIMPL;
}
static HRESULT STDMETHODCALLTYPE
InPlace_OnPosRectChange(IOleInPlaceSite FAR *This, LPCRECT lprcPosRect) {
  IOleObject *browserObject;
  IOleInPlaceObject *inplace;
  browserObject = *((IOleObject **)((char *)This - sizeof(IOleObject *) -
				    sizeof(IOleClientSite)));
  if (!browserObject->lpVtbl->QueryInterface(
	  browserObject, &IID_IOleInPlaceObject, (void **)&inplace)) {
    inplace->lpVtbl->SetObjectRects(inplace, lprcPosRect, lprcPosRect);
    inplace->lpVtbl->Release(inplace);
  }
  return S_OK;
}
static HRESULT STDMETHODCALLTYPE Frame_QueryInterface(
    IOleInPlaceFrame FAR *This, REFIID riid, LPVOID FAR *ppvObj) {
  return E_NOTIMPL;
}
static HRESULT STDMETHODCALLTYPE Frame_AddRef(IOleInPlaceFrame FAR *This) {
  return (1);
}
static HRESULT STDMETHODCALLTYPE Frame_Release(IOleInPlaceFrame FAR *This) {
  return (1);
}
static HRESULT STDMETHODCALLTYPE Frame_GetWindow(IOleInPlaceFrame FAR *This,
						 HWND FAR *lphwnd) {
  *lphwnd = ((_IOleInPlaceFrameEx *)This)->window;
  return S_OK;
}
static HRESULT STDMETHODCALLTYPE
Frame_ContextSensitiveHelp(IOleInPlaceFrame FAR *This, BOOL fEnterMode) {
  return E_NOTIMPL;
}
static HRESULT STDMETHODCALLTYPE Frame_GetBorder(IOleInPlaceFrame FAR *This,
						 LPRECT lprectBorder) {
  return E_NOTIMPL;
}
static HRESULT STDMETHODCALLTYPE Frame_RequestBorderSpace(
    IOleInPlaceFrame FAR *This, LPCBORDERWIDTHS pborderwidths) {
  return E_NOTIMPL;
}
static HRESULT STDMETHODCALLTYPE Frame_SetBorderSpace(
    IOleInPlaceFrame FAR *This, LPCBORDERWIDTHS pborderwidths) {
  return E_NOTIMPL;
}
static HRESULT STDMETHODCALLTYPE Frame_SetActiveObject(
    IOleInPlaceFrame FAR *This, IOleInPlaceActiveObject *pActiveObject,
    LPCOLESTR pszObjName) {
  return S_OK;
}
static HRESULT STDMETHODCALLTYPE
Frame_InsertMenus(IOleInPlaceFrame FAR *This, HMENU hmenuShared,
		  LPOLEMENUGROUPWIDTHS lpMenuWidths) {
  return E_NOTIMPL;
}
static HRESULT STDMETHODCALLTYPE Frame_SetMenu(IOleInPlaceFrame FAR *This,
					       HMENU hmenuShared,
					       HOLEMENU holemenu,
					       HWND hwndActiveObject) {
  return S_OK;
}
static HRESULT STDMETHODCALLTYPE Frame_RemoveMenus(IOleInPlaceFrame FAR *This,
						   HMENU hmenuShared) {
  return E_NOTIMPL;
}
static HRESULT STDMETHODCALLTYPE Frame_SetStatusText(IOleInPlaceFrame FAR *This,
						     LPCOLESTR pszStatusText) {
  return S_OK;
}
static HRESULT STDMETHODCALLTYPE
Frame_EnableModeless(IOleInPlaceFrame FAR *This, BOOL fEnable) {
  return S_OK;
}
static HRESULT STDMETHODCALLTYPE
Frame_TranslateAccelerator(IOleInPlaceFrame FAR *This, LPMSG lpmsg, WORD wID) {
  return E_NOTIMPL;
}
static HRESULT STDMETHODCALLTYPE UI_QueryInterface(IDocHostUIHandler FAR *This,
						   REFIID riid,
						   LPVOID FAR *ppvObj) {
  return (Site_QueryInterface((IOleClientSite *)((char *)This -
						 sizeof(IOleClientSite) -
						 sizeof(_IOleInPlaceSiteEx)),
			      riid, ppvObj));
}
static HRESULT STDMETHODCALLTYPE UI_AddRef(IDocHostUIHandler FAR *This) {
  return 1;
}
static HRESULT STDMETHODCALLTYPE UI_Release(IDocHostUIHandler FAR *This) {
  return 1;
}
static HRESULT STDMETHODCALLTYPE UI_ShowContextMenu(
    IDocHostUIHandler FAR *This, DWORD dwID, POINT __RPC_FAR *ppt,
    IUnknown __RPC_FAR *pcmdtReserved, IDispatch __RPC_FAR *pdispReserved) {
  return S_OK;
}
static HRESULT STDMETHODCALLTYPE
UI_GetHostInfo(IDocHostUIHandler FAR *This, DOCHOSTUIINFO __RPC_FAR *pInfo) {
  pInfo->cbSize = sizeof(DOCHOSTUIINFO);
  pInfo->dwFlags = DOCHOSTUIFLAG_NO3DBORDER;
  pInfo->dwDoubleClick = DOCHOSTUIDBLCLK_DEFAULT;
  return S_OK;
}
static HRESULT STDMETHODCALLTYPE UI_ShowUI(
    IDocHostUIHandler FAR *This, DWORD dwID,
    IOleInPlaceActiveObject __RPC_FAR *pActiveObject,
    IOleCommandTarget __RPC_FAR *pCommandTarget,
    IOleInPlaceFrame __RPC_FAR *pFrame, IOleInPlaceUIWindow __RPC_FAR *pDoc) {
  return S_OK;
}
static HRESULT STDMETHODCALLTYPE UI_HideUI(IDocHostUIHandler FAR *This) {
  return S_OK;
}
static HRESULT STDMETHODCALLTYPE UI_UpdateUI(IDocHostUIHandler FAR *This) {
  return S_OK;
}
static HRESULT STDMETHODCALLTYPE UI_EnableModeless(IDocHostUIHandler FAR *This,
						   BOOL fEnable) {
  return S_OK;
}
static HRESULT STDMETHODCALLTYPE
UI_OnDocWindowActivate(IDocHostUIHandler FAR *This, BOOL fActivate) {
  return S_OK;
}
static HRESULT STDMETHODCALLTYPE
UI_OnFrameWindowActivate(IDocHostUIHandler FAR *This, BOOL fActivate) {
  return S_OK;
}
static HRESULT STDMETHODCALLTYPE
UI_ResizeBorder(IDocHostUIHandler FAR *This, LPCRECT prcBorder,
		IOleInPlaceUIWindow __RPC_FAR *pUIWindow, BOOL fRameWindow) {
  return S_OK;
}
static HRESULT STDMETHODCALLTYPE
UI_TranslateAccelerator(IDocHostUIHandler FAR *This, LPMSG lpMsg,
			const GUID __RPC_FAR *pguidCmdGroup, DWORD nCmdID) {
  return S_FALSE;
}
static HRESULT STDMETHODCALLTYPE UI_GetOptionKeyPath(
    IDocHostUIHandler FAR *This, LPOLESTR __RPC_FAR *pchKey, DWORD dw) {
  return S_FALSE;
}
static HRESULT STDMETHODCALLTYPE UI_GetDropTarget(
    IDocHostUIHandler FAR *This, IDropTarget __RPC_FAR *pDropTarget,
    IDropTarget __RPC_FAR *__RPC_FAR *ppDropTarget) {
  return S_FALSE;
}
static HRESULT STDMETHODCALLTYPE UI_GetExternal(
    IDocHostUIHandler FAR *This, IDispatch __RPC_FAR *__RPC_FAR *ppDispatch) {
  *ppDispatch = 0;
  return S_FALSE;
}
static HRESULT STDMETHODCALLTYPE UI_TranslateUrl(
    IDocHostUIHandler FAR *This, DWORD dwTranslate, OLECHAR __RPC_FAR *pchURLIn,
    OLECHAR __RPC_FAR *__RPC_FAR *ppchURLOut) {
  *ppchURLOut = 0;
  return S_FALSE;
}
static HRESULT STDMETHODCALLTYPE
UI_FilterDataObject(IDocHostUIHandler FAR *This, IDataObject __RPC_FAR *pDO,
		    IDataObject __RPC_FAR *__RPC_FAR *ppDORet) {
  *ppDORet = 0;
  return S_FALSE;
}

static const TCHAR *classname = "WebView";
static const SAFEARRAYBOUND ArrayBound = {1, 0};

static IOleClientSiteVtbl MyIOleClientSiteTable = {
    Site_QueryInterface, Site_AddRef,       Site_Release,
    Site_SaveObject,     Site_GetMoniker,   Site_GetContainer,
    Site_ShowObject,     Site_OnShowWindow, Site_RequestNewObjectLayout};
static IOleInPlaceSiteVtbl MyIOleInPlaceSiteTable = {
    InPlace_QueryInterface,
    InPlace_AddRef,
    InPlace_Release,
    InPlace_GetWindow,
    InPlace_ContextSensitiveHelp,
    InPlace_CanInPlaceActivate,
    InPlace_OnInPlaceActivate,
    InPlace_OnUIActivate,
    InPlace_GetWindowContext,
    InPlace_Scroll,
    InPlace_OnUIDeactivate,
    InPlace_OnInPlaceDeactivate,
    InPlace_DiscardUndoState,
    InPlace_DeactivateAndUndo,
    InPlace_OnPosRectChange};

static IOleInPlaceFrameVtbl MyIOleInPlaceFrameTable = {
    Frame_QueryInterface,
    Frame_AddRef,
    Frame_Release,
    Frame_GetWindow,
    Frame_ContextSensitiveHelp,
    Frame_GetBorder,
    Frame_RequestBorderSpace,
    Frame_SetBorderSpace,
    Frame_SetActiveObject,
    Frame_InsertMenus,
    Frame_SetMenu,
    Frame_RemoveMenus,
    Frame_SetStatusText,
    Frame_EnableModeless,
    Frame_TranslateAccelerator};

static IDocHostUIHandlerVtbl MyIDocHostUIHandlerTable = {
    UI_QueryInterface,
    UI_AddRef,
    UI_Release,
    UI_ShowContextMenu,
    UI_GetHostInfo,
    UI_ShowUI,
    UI_HideUI,
    UI_UpdateUI,
    UI_EnableModeless,
    UI_OnDocWindowActivate,
    UI_OnFrameWindowActivate,
    UI_ResizeBorder,
    UI_TranslateAccelerator,
    UI_GetOptionKeyPath,
    UI_GetDropTarget,
    UI_GetExternal,
    UI_TranslateUrl,
    UI_FilterDataObject};

static void UnEmbedBrowserObject(HWND hwnd) {
  IOleObject **browserHandle;
  IOleObject *browserObject;
  if ((browserHandle = (IOleObject **)GetWindowLongPtr(hwnd, GWLP_USERDATA))) {
    browserObject = *browserHandle;
    browserObject->lpVtbl->Close(browserObject, OLECLOSE_NOSAVE);
    browserObject->lpVtbl->Release(browserObject);
    GlobalFree(browserHandle);
  }
}

static int EmbedBrowserObject(HWND hwnd) {
  LPCLASSFACTORY pClassFactory;
  IOleObject *browserObject;
  IWebBrowser2 *webBrowser2;
  RECT rect;
  char *ptr;
  _IOleClientSiteEx *_iOleClientSiteEx;
  ptr = (char *)GlobalAlloc(GMEM_FIXED,
			    sizeof(_IOleClientSiteEx) + sizeof(IOleObject *));
  if (ptr == NULL) {
    return -1;
  }
  _iOleClientSiteEx = (_IOleClientSiteEx *)(ptr + sizeof(IOleObject *));
  _iOleClientSiteEx->client.lpVtbl = &MyIOleClientSiteTable;
  _iOleClientSiteEx->inplace.inplace.lpVtbl = &MyIOleInPlaceSiteTable;
  _iOleClientSiteEx->inplace.frame.frame.lpVtbl = &MyIOleInPlaceFrameTable;
  _iOleClientSiteEx->inplace.frame.window = hwnd;
  _iOleClientSiteEx->ui.ui.lpVtbl = &MyIDocHostUIHandlerTable;
  pClassFactory = 0;
  if (!CoGetClassObject(&CLSID_WebBrowser,
			CLSCTX_INPROC_SERVER | CLSCTX_INPROC_HANDLER, NULL,
			&IID_IClassFactory, (void **)&pClassFactory) &&
      pClassFactory) {
    if (!pClassFactory->lpVtbl->CreateInstance(
	    pClassFactory, 0, &IID_IOleObject, &browserObject)) {
      pClassFactory->lpVtbl->Release(pClassFactory);
      *((IOleObject **)ptr) = browserObject;
      SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG)ptr);
      if (!browserObject->lpVtbl->SetClientSite(
	      browserObject, (IOleClientSite *)_iOleClientSiteEx)) {
	browserObject->lpVtbl->SetHostNames(browserObject, L"My Host Name", 0);

	GetClientRect(hwnd, &rect);
	if (!OleSetContainedObject((struct IUnknown *)browserObject, TRUE) &&

	    !browserObject->lpVtbl->DoVerb(browserObject, OLEIVERB_SHOW, NULL,
					   (IOleClientSite *)_iOleClientSiteEx,
					   -1, hwnd, &rect) &&

	    !browserObject->lpVtbl->QueryInterface(
		browserObject, &IID_IWebBrowser2, (void **)&webBrowser2)) {
	  webBrowser2->lpVtbl->put_Left(webBrowser2, 0);
	  webBrowser2->lpVtbl->put_Top(webBrowser2, 0);
	  webBrowser2->lpVtbl->put_Width(webBrowser2, rect.right);
	  webBrowser2->lpVtbl->put_Height(webBrowser2, rect.bottom);

	  webBrowser2->lpVtbl->Release(webBrowser2);
	  return (0);
	}
      }
      UnEmbedBrowserObject(hwnd);
      return -1;
    }
    pClassFactory->lpVtbl->Release(pClassFactory);
    GlobalFree(ptr);
    return -1;
  }
  GlobalFree(ptr);
  return -1;
}

#if 0
static long DisplayHTMLStr(HWND hwnd, LPCTSTR string) {
  IWebBrowser2 *webBrowser2;
  LPDISPATCH lpDispatch;
  IHTMLDocument2 *htmlDoc2;
  IOleObject *browserObject;
  SAFEARRAY *sfArray;
  VARIANT myURL;
  VARIANT *pVar;
  BSTR bstr;
  browserObject = *((IOleObject **)GetWindowLong(hwnd, GWL_USERDATA));
  bstr = 0;
  if (!browserObject->lpVtbl->QueryInterface(browserObject, &IID_IWebBrowser2,
					     (void **)&webBrowser2)) {
    VariantInit(&myURL);
    myURL.vt = VT_BSTR;
    myURL.bstrVal = SysAllocString(L"about:blank");
    webBrowser2->lpVtbl->Navigate2(webBrowser2, &myURL, 0, 0, 0, 0);
    VariantClear(&myURL);
    if (!webBrowser2->lpVtbl->get_Document(webBrowser2, &lpDispatch)) {
      if (!lpDispatch->lpVtbl->QueryInterface(lpDispatch, &IID_IHTMLDocument2,
					      (void **)&htmlDoc2)) {
	if ((sfArray = SafeArrayCreate(VT_VARIANT, 1,
				       (SAFEARRAYBOUND *)&ArrayBound))) {
	  if (!SafeArrayAccessData(sfArray, (void **)&pVar)) {
	    pVar->vt = VT_BSTR;
#ifndef UNICODE
	    {
	      wchar_t *buffer;
	      DWORD size;
	      size = MultiByteToWideChar(CP_ACP, 0, string, -1, 0, 0);
	      if (!(buffer = (wchar_t *)GlobalAlloc(GMEM_FIXED,
						    sizeof(wchar_t) * size)))
		goto bad;
	      MultiByteToWideChar(CP_ACP, 0, string, -1, buffer, size);
	      bstr = SysAllocString(buffer);
	      GlobalFree(buffer);
	    }
#else
	    bstr = SysAllocString(string);
#endif
	    if ((pVar->bstrVal = bstr)) {
	      htmlDoc2->lpVtbl->write(htmlDoc2, sfArray);
	      htmlDoc2->lpVtbl->close(htmlDoc2);
	    }
	  }
	  SafeArrayDestroy(sfArray);
	}
      bad:
	htmlDoc2->lpVtbl->Release(htmlDoc2);
      }
      lpDispatch->lpVtbl->Release(lpDispatch);
    }
    webBrowser2->lpVtbl->Release(webBrowser2);
  }
  if (bstr)
    return (0);
  return (-1);
}
#endif

static long DisplayHTMLPage(HWND hwnd, LPTSTR webPageName) {
  IWebBrowser2 *webBrowser2;
  VARIANT myURL;
  IOleObject *browserObject;
  browserObject = *((IOleObject **)GetWindowLongPtr(hwnd, GWLP_USERDATA));
  if (!browserObject->lpVtbl->QueryInterface(browserObject, &IID_IWebBrowser2,
					     (void **)&webBrowser2)) {
    VariantInit(&myURL);
    myURL.vt = VT_BSTR;
#ifndef UNICODE
    {
      wchar_t *buffer;
      DWORD size;
      size = MultiByteToWideChar(CP_ACP, 0, webPageName, -1, 0, 0);
      if (!(buffer =
		(wchar_t *)GlobalAlloc(GMEM_FIXED, sizeof(wchar_t) * size)))
	goto badalloc;
      MultiByteToWideChar(CP_ACP, 0, webPageName, -1, buffer, size);
      myURL.bstrVal = SysAllocString(buffer);
      GlobalFree(buffer);
    }
#else
    myURL.bstrVal = SysAllocString(webPageName);
#endif
    if (!myURL.bstrVal) {
    badalloc:
      webBrowser2->lpVtbl->Release(webBrowser2);
      return (-6);
    }
    webBrowser2->lpVtbl->Navigate2(webBrowser2, &myURL, 0, 0, 0, 0);
    VariantClear(&myURL);
    webBrowser2->lpVtbl->Release(webBrowser2);
    return (0);
  }
  return (-5);
}

static LRESULT CALLBACK wndproc(HWND hwnd, UINT uMsg, WPARAM wParam,
				LPARAM lParam) {
  switch (uMsg) {
  case WM_CREATE:
    return EmbedBrowserObject(hwnd);
  case WM_DESTROY:
    UnEmbedBrowserObject(hwnd);
    PostQuitMessage(0);
    return TRUE;
  }
  return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

static int webview(const char *title, const char *url, int width, int height,
		   int resizable) {
  MSG msg;
  WNDCLASSEX wc;
  HINSTANCE hInstance;
  STARTUPINFO info;
  DWORD style;
  IWebBrowser2 *browser;
  RECT rect;

  hInstance = GetModuleHandle(NULL);
  if (hInstance == NULL) {
    return -1;
  }
  GetStartupInfo(&info);
  if (OleInitialize(NULL) != S_OK) {
    return -1;
  }
  ZeroMemory(&wc, sizeof(WNDCLASSEX));
  wc.cbSize = sizeof(WNDCLASSEX);
  wc.hInstance = hInstance;
  wc.lpfnWndProc = wndproc;
  wc.lpszClassName = classname;
  RegisterClassEx(&wc);

  style = WS_OVERLAPPEDWINDOW;
  if (!resizable) {
    style = WS_OVERLAPPED | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU;
  }

  GetClientRect(GetDesktopWindow(), &rect);
  rect.left = (rect.right/2) - (width/2);
  rect.top = (rect.bottom/2) - (height/2);

  msg.hwnd =
      CreateWindowEx(0, classname, title, style, rect.left, rect.top,
		     width, height, HWND_DESKTOP, NULL, hInstance, 0);
  if (msg.hwnd == 0) {
    OleUninitialize();
    return -1;
  }

  SetWindowText(msg.hwnd, title);

  DisplayHTMLPage(msg.hwnd, (LPTSTR)url);
  ShowWindow(msg.hwnd, info.wShowWindow);
  UpdateWindow(msg.hwnd);
  SetFocus(msg.hwnd);

  while (GetMessage(&msg, 0, 0, 0) == 1) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
  OleUninitialize();
  return 0;
}

#elif defined(WEBVIEW_COCOA)
#import <Cocoa/Cocoa.h>
#import <WebKit/WebKit.h>

@interface WebViewApp : NSObject <NSApplicationDelegate>
@end
@implementation WebViewApp
- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)app {
  return YES;
}
@end

static int webview(const char *title, const char *url, int width, int height,
		   int resizable) {
  NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
  [NSApplication sharedApplication];
  WebViewApp *app = [WebViewApp new];
  [NSApp setDelegate:app];

  NSString *nsTitle = [NSString stringWithUTF8String:title];
  NSRect r = NSMakeRect(0, 0, width, height);
  NSUInteger style = NSWindowStyleMaskTitled | NSWindowStyleMaskClosable |
		     NSWindowStyleMaskMiniaturizable;
  if (resizable) {
    style = style | NSWindowStyleMaskResizable;
  }
  NSWindow *w = [[NSWindow alloc] initWithContentRect:r
					    styleMask:style
					      backing:NSBackingStoreBuffered
						defer:NO];
  [w autorelease];
  [w setTitle:nsTitle];

  WebView *webview =
      [[WebView alloc] initWithFrame:r frameName:@"WebView" groupName:nil];
  NSURL *nsURL = [NSURL URLWithString:[NSString stringWithUTF8String:url]];
  [[webview mainFrame] loadRequest:[NSURLRequest requestWithURL:nsURL]];

  [webview setAutoresizesSubviews:YES];
  [webview setAutoresizingMask:NSViewWidthSizable | NSViewHeightSizable];
  [[w contentView] addSubview:webview];

  [w orderFrontRegardless];
  [NSApp run];
  [pool drain];
  return 0;
}
#else
#error "Define one of: WEBVIEW_GTK, WEBVIEW_COCOA or WEBVIEW_WINAPI"
#endif

#endif /* WEBVIEW_H */
