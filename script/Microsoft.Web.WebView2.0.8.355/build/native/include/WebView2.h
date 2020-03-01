

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.xx.xxxx */
/* at a redacted point in time
 */
/* Compiler settings for ../../edge_embedded_browser/client/win/current/webview2.idl:
    Oicf, W1, Zp8, env=Win64 (32b run), target_arch=AMD64 8.xx.xxxx 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif /* __RPCNDR_H_VERSION__ */

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __webview2_h__
#define __webview2_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IWebView2WebView_FWD_DEFINED__
#define __IWebView2WebView_FWD_DEFINED__
typedef interface IWebView2WebView IWebView2WebView;

#endif 	/* __IWebView2WebView_FWD_DEFINED__ */


#ifndef __IWebView2WebView2_FWD_DEFINED__
#define __IWebView2WebView2_FWD_DEFINED__
typedef interface IWebView2WebView2 IWebView2WebView2;

#endif 	/* __IWebView2WebView2_FWD_DEFINED__ */


#ifndef __IWebView2WebView3_FWD_DEFINED__
#define __IWebView2WebView3_FWD_DEFINED__
typedef interface IWebView2WebView3 IWebView2WebView3;

#endif 	/* __IWebView2WebView3_FWD_DEFINED__ */


#ifndef __IWebView2WebView4_FWD_DEFINED__
#define __IWebView2WebView4_FWD_DEFINED__
typedef interface IWebView2WebView4 IWebView2WebView4;

#endif 	/* __IWebView2WebView4_FWD_DEFINED__ */


#ifndef __IWebView2WebView5_FWD_DEFINED__
#define __IWebView2WebView5_FWD_DEFINED__
typedef interface IWebView2WebView5 IWebView2WebView5;

#endif 	/* __IWebView2WebView5_FWD_DEFINED__ */


#ifndef __IWebView2Deferral_FWD_DEFINED__
#define __IWebView2Deferral_FWD_DEFINED__
typedef interface IWebView2Deferral IWebView2Deferral;

#endif 	/* __IWebView2Deferral_FWD_DEFINED__ */


#ifndef __IWebView2Settings_FWD_DEFINED__
#define __IWebView2Settings_FWD_DEFINED__
typedef interface IWebView2Settings IWebView2Settings;

#endif 	/* __IWebView2Settings_FWD_DEFINED__ */


#ifndef __IWebView2Settings2_FWD_DEFINED__
#define __IWebView2Settings2_FWD_DEFINED__
typedef interface IWebView2Settings2 IWebView2Settings2;

#endif 	/* __IWebView2Settings2_FWD_DEFINED__ */


#ifndef __IWebView2ProcessFailedEventArgs_FWD_DEFINED__
#define __IWebView2ProcessFailedEventArgs_FWD_DEFINED__
typedef interface IWebView2ProcessFailedEventArgs IWebView2ProcessFailedEventArgs;

#endif 	/* __IWebView2ProcessFailedEventArgs_FWD_DEFINED__ */


#ifndef __IWebView2ProcessFailedEventHandler_FWD_DEFINED__
#define __IWebView2ProcessFailedEventHandler_FWD_DEFINED__
typedef interface IWebView2ProcessFailedEventHandler IWebView2ProcessFailedEventHandler;

#endif 	/* __IWebView2ProcessFailedEventHandler_FWD_DEFINED__ */


#ifndef __IWebView2ZoomFactorChangedEventHandler_FWD_DEFINED__
#define __IWebView2ZoomFactorChangedEventHandler_FWD_DEFINED__
typedef interface IWebView2ZoomFactorChangedEventHandler IWebView2ZoomFactorChangedEventHandler;

#endif 	/* __IWebView2ZoomFactorChangedEventHandler_FWD_DEFINED__ */


#ifndef __IWebView2HttpHeadersCollectionIterator_FWD_DEFINED__
#define __IWebView2HttpHeadersCollectionIterator_FWD_DEFINED__
typedef interface IWebView2HttpHeadersCollectionIterator IWebView2HttpHeadersCollectionIterator;

#endif 	/* __IWebView2HttpHeadersCollectionIterator_FWD_DEFINED__ */


#ifndef __IWebView2HttpRequestHeaders_FWD_DEFINED__
#define __IWebView2HttpRequestHeaders_FWD_DEFINED__
typedef interface IWebView2HttpRequestHeaders IWebView2HttpRequestHeaders;

#endif 	/* __IWebView2HttpRequestHeaders_FWD_DEFINED__ */


#ifndef __IWebView2HttpResponseHeaders_FWD_DEFINED__
#define __IWebView2HttpResponseHeaders_FWD_DEFINED__
typedef interface IWebView2HttpResponseHeaders IWebView2HttpResponseHeaders;

#endif 	/* __IWebView2HttpResponseHeaders_FWD_DEFINED__ */


#ifndef __IWebView2WebResourceRequest_FWD_DEFINED__
#define __IWebView2WebResourceRequest_FWD_DEFINED__
typedef interface IWebView2WebResourceRequest IWebView2WebResourceRequest;

#endif 	/* __IWebView2WebResourceRequest_FWD_DEFINED__ */


#ifndef __IWebView2WebResourceResponse_FWD_DEFINED__
#define __IWebView2WebResourceResponse_FWD_DEFINED__
typedef interface IWebView2WebResourceResponse IWebView2WebResourceResponse;

#endif 	/* __IWebView2WebResourceResponse_FWD_DEFINED__ */


#ifndef __IWebView2NavigationStartingEventArgs_FWD_DEFINED__
#define __IWebView2NavigationStartingEventArgs_FWD_DEFINED__
typedef interface IWebView2NavigationStartingEventArgs IWebView2NavigationStartingEventArgs;

#endif 	/* __IWebView2NavigationStartingEventArgs_FWD_DEFINED__ */


#ifndef __IWebView2NavigationStartingEventHandler_FWD_DEFINED__
#define __IWebView2NavigationStartingEventHandler_FWD_DEFINED__
typedef interface IWebView2NavigationStartingEventHandler IWebView2NavigationStartingEventHandler;

#endif 	/* __IWebView2NavigationStartingEventHandler_FWD_DEFINED__ */


#ifndef __IWebView2DocumentStateChangedEventArgs_FWD_DEFINED__
#define __IWebView2DocumentStateChangedEventArgs_FWD_DEFINED__
typedef interface IWebView2DocumentStateChangedEventArgs IWebView2DocumentStateChangedEventArgs;

#endif 	/* __IWebView2DocumentStateChangedEventArgs_FWD_DEFINED__ */


#ifndef __IWebView2DocumentStateChangedEventHandler_FWD_DEFINED__
#define __IWebView2DocumentStateChangedEventHandler_FWD_DEFINED__
typedef interface IWebView2DocumentStateChangedEventHandler IWebView2DocumentStateChangedEventHandler;

#endif 	/* __IWebView2DocumentStateChangedEventHandler_FWD_DEFINED__ */


#ifndef __IWebView2ScriptDialogOpeningEventArgs_FWD_DEFINED__
#define __IWebView2ScriptDialogOpeningEventArgs_FWD_DEFINED__
typedef interface IWebView2ScriptDialogOpeningEventArgs IWebView2ScriptDialogOpeningEventArgs;

#endif 	/* __IWebView2ScriptDialogOpeningEventArgs_FWD_DEFINED__ */


#ifndef __IWebView2ScriptDialogOpeningEventHandler_FWD_DEFINED__
#define __IWebView2ScriptDialogOpeningEventHandler_FWD_DEFINED__
typedef interface IWebView2ScriptDialogOpeningEventHandler IWebView2ScriptDialogOpeningEventHandler;

#endif 	/* __IWebView2ScriptDialogOpeningEventHandler_FWD_DEFINED__ */


#ifndef __IWebView2NavigationCompletedEventArgs_FWD_DEFINED__
#define __IWebView2NavigationCompletedEventArgs_FWD_DEFINED__
typedef interface IWebView2NavigationCompletedEventArgs IWebView2NavigationCompletedEventArgs;

#endif 	/* __IWebView2NavigationCompletedEventArgs_FWD_DEFINED__ */


#ifndef __IWebView2NavigationCompletedEventHandler_FWD_DEFINED__
#define __IWebView2NavigationCompletedEventHandler_FWD_DEFINED__
typedef interface IWebView2NavigationCompletedEventHandler IWebView2NavigationCompletedEventHandler;

#endif 	/* __IWebView2NavigationCompletedEventHandler_FWD_DEFINED__ */


#ifndef __IWebView2PermissionRequestedEventArgs_FWD_DEFINED__
#define __IWebView2PermissionRequestedEventArgs_FWD_DEFINED__
typedef interface IWebView2PermissionRequestedEventArgs IWebView2PermissionRequestedEventArgs;

#endif 	/* __IWebView2PermissionRequestedEventArgs_FWD_DEFINED__ */


#ifndef __IWebView2PermissionRequestedEventHandler_FWD_DEFINED__
#define __IWebView2PermissionRequestedEventHandler_FWD_DEFINED__
typedef interface IWebView2PermissionRequestedEventHandler IWebView2PermissionRequestedEventHandler;

#endif 	/* __IWebView2PermissionRequestedEventHandler_FWD_DEFINED__ */


#ifndef __IWebView2AddScriptToExecuteOnDocumentCreatedCompletedHandler_FWD_DEFINED__
#define __IWebView2AddScriptToExecuteOnDocumentCreatedCompletedHandler_FWD_DEFINED__
typedef interface IWebView2AddScriptToExecuteOnDocumentCreatedCompletedHandler IWebView2AddScriptToExecuteOnDocumentCreatedCompletedHandler;

#endif 	/* __IWebView2AddScriptToExecuteOnDocumentCreatedCompletedHandler_FWD_DEFINED__ */


#ifndef __IWebView2ExecuteScriptCompletedHandler_FWD_DEFINED__
#define __IWebView2ExecuteScriptCompletedHandler_FWD_DEFINED__
typedef interface IWebView2ExecuteScriptCompletedHandler IWebView2ExecuteScriptCompletedHandler;

#endif 	/* __IWebView2ExecuteScriptCompletedHandler_FWD_DEFINED__ */


#ifndef __IWebView2WebResourceRequestedEventArgs_FWD_DEFINED__
#define __IWebView2WebResourceRequestedEventArgs_FWD_DEFINED__
typedef interface IWebView2WebResourceRequestedEventArgs IWebView2WebResourceRequestedEventArgs;

#endif 	/* __IWebView2WebResourceRequestedEventArgs_FWD_DEFINED__ */


#ifndef __IWebView2WebResourceRequestedEventArgs2_FWD_DEFINED__
#define __IWebView2WebResourceRequestedEventArgs2_FWD_DEFINED__
typedef interface IWebView2WebResourceRequestedEventArgs2 IWebView2WebResourceRequestedEventArgs2;

#endif 	/* __IWebView2WebResourceRequestedEventArgs2_FWD_DEFINED__ */


#ifndef __IWebView2WebResourceRequestedEventHandler_FWD_DEFINED__
#define __IWebView2WebResourceRequestedEventHandler_FWD_DEFINED__
typedef interface IWebView2WebResourceRequestedEventHandler IWebView2WebResourceRequestedEventHandler;

#endif 	/* __IWebView2WebResourceRequestedEventHandler_FWD_DEFINED__ */


#ifndef __IWebView2CapturePreviewCompletedHandler_FWD_DEFINED__
#define __IWebView2CapturePreviewCompletedHandler_FWD_DEFINED__
typedef interface IWebView2CapturePreviewCompletedHandler IWebView2CapturePreviewCompletedHandler;

#endif 	/* __IWebView2CapturePreviewCompletedHandler_FWD_DEFINED__ */


#ifndef __IWebView2FocusChangedEventHandler_FWD_DEFINED__
#define __IWebView2FocusChangedEventHandler_FWD_DEFINED__
typedef interface IWebView2FocusChangedEventHandler IWebView2FocusChangedEventHandler;

#endif 	/* __IWebView2FocusChangedEventHandler_FWD_DEFINED__ */


#ifndef __IWebView2MoveFocusRequestedEventArgs_FWD_DEFINED__
#define __IWebView2MoveFocusRequestedEventArgs_FWD_DEFINED__
typedef interface IWebView2MoveFocusRequestedEventArgs IWebView2MoveFocusRequestedEventArgs;

#endif 	/* __IWebView2MoveFocusRequestedEventArgs_FWD_DEFINED__ */


#ifndef __IWebView2MoveFocusRequestedEventHandler_FWD_DEFINED__
#define __IWebView2MoveFocusRequestedEventHandler_FWD_DEFINED__
typedef interface IWebView2MoveFocusRequestedEventHandler IWebView2MoveFocusRequestedEventHandler;

#endif 	/* __IWebView2MoveFocusRequestedEventHandler_FWD_DEFINED__ */


#ifndef __IWebView2WebMessageReceivedEventArgs_FWD_DEFINED__
#define __IWebView2WebMessageReceivedEventArgs_FWD_DEFINED__
typedef interface IWebView2WebMessageReceivedEventArgs IWebView2WebMessageReceivedEventArgs;

#endif 	/* __IWebView2WebMessageReceivedEventArgs_FWD_DEFINED__ */


#ifndef __IWebView2WebMessageReceivedEventHandler_FWD_DEFINED__
#define __IWebView2WebMessageReceivedEventHandler_FWD_DEFINED__
typedef interface IWebView2WebMessageReceivedEventHandler IWebView2WebMessageReceivedEventHandler;

#endif 	/* __IWebView2WebMessageReceivedEventHandler_FWD_DEFINED__ */


#ifndef __IWebView2DevToolsProtocolEventReceivedEventArgs_FWD_DEFINED__
#define __IWebView2DevToolsProtocolEventReceivedEventArgs_FWD_DEFINED__
typedef interface IWebView2DevToolsProtocolEventReceivedEventArgs IWebView2DevToolsProtocolEventReceivedEventArgs;

#endif 	/* __IWebView2DevToolsProtocolEventReceivedEventArgs_FWD_DEFINED__ */


#ifndef __IWebView2DevToolsProtocolEventReceivedEventHandler_FWD_DEFINED__
#define __IWebView2DevToolsProtocolEventReceivedEventHandler_FWD_DEFINED__
typedef interface IWebView2DevToolsProtocolEventReceivedEventHandler IWebView2DevToolsProtocolEventReceivedEventHandler;

#endif 	/* __IWebView2DevToolsProtocolEventReceivedEventHandler_FWD_DEFINED__ */


#ifndef __IWebView2CallDevToolsProtocolMethodCompletedHandler_FWD_DEFINED__
#define __IWebView2CallDevToolsProtocolMethodCompletedHandler_FWD_DEFINED__
typedef interface IWebView2CallDevToolsProtocolMethodCompletedHandler IWebView2CallDevToolsProtocolMethodCompletedHandler;

#endif 	/* __IWebView2CallDevToolsProtocolMethodCompletedHandler_FWD_DEFINED__ */


#ifndef __IWebView2CreateWebViewCompletedHandler_FWD_DEFINED__
#define __IWebView2CreateWebViewCompletedHandler_FWD_DEFINED__
typedef interface IWebView2CreateWebViewCompletedHandler IWebView2CreateWebViewCompletedHandler;

#endif 	/* __IWebView2CreateWebViewCompletedHandler_FWD_DEFINED__ */


#ifndef __IWebView2NewWindowRequestedEventArgs_FWD_DEFINED__
#define __IWebView2NewWindowRequestedEventArgs_FWD_DEFINED__
typedef interface IWebView2NewWindowRequestedEventArgs IWebView2NewWindowRequestedEventArgs;

#endif 	/* __IWebView2NewWindowRequestedEventArgs_FWD_DEFINED__ */


#ifndef __IWebView2NewWindowRequestedEventHandler_FWD_DEFINED__
#define __IWebView2NewWindowRequestedEventHandler_FWD_DEFINED__
typedef interface IWebView2NewWindowRequestedEventHandler IWebView2NewWindowRequestedEventHandler;

#endif 	/* __IWebView2NewWindowRequestedEventHandler_FWD_DEFINED__ */


#ifndef __IWebView2DocumentTitleChangedEventHandler_FWD_DEFINED__
#define __IWebView2DocumentTitleChangedEventHandler_FWD_DEFINED__
typedef interface IWebView2DocumentTitleChangedEventHandler IWebView2DocumentTitleChangedEventHandler;

#endif 	/* __IWebView2DocumentTitleChangedEventHandler_FWD_DEFINED__ */


#ifndef __IWebView2AcceleratorKeyPressedEventArgs_FWD_DEFINED__
#define __IWebView2AcceleratorKeyPressedEventArgs_FWD_DEFINED__
typedef interface IWebView2AcceleratorKeyPressedEventArgs IWebView2AcceleratorKeyPressedEventArgs;

#endif 	/* __IWebView2AcceleratorKeyPressedEventArgs_FWD_DEFINED__ */


#ifndef __IWebView2AcceleratorKeyPressedEventHandler_FWD_DEFINED__
#define __IWebView2AcceleratorKeyPressedEventHandler_FWD_DEFINED__
typedef interface IWebView2AcceleratorKeyPressedEventHandler IWebView2AcceleratorKeyPressedEventHandler;

#endif 	/* __IWebView2AcceleratorKeyPressedEventHandler_FWD_DEFINED__ */


#ifndef __IWebView2NewVersionAvailableEventArgs_FWD_DEFINED__
#define __IWebView2NewVersionAvailableEventArgs_FWD_DEFINED__
typedef interface IWebView2NewVersionAvailableEventArgs IWebView2NewVersionAvailableEventArgs;

#endif 	/* __IWebView2NewVersionAvailableEventArgs_FWD_DEFINED__ */


#ifndef __IWebView2NewVersionAvailableEventHandler_FWD_DEFINED__
#define __IWebView2NewVersionAvailableEventHandler_FWD_DEFINED__
typedef interface IWebView2NewVersionAvailableEventHandler IWebView2NewVersionAvailableEventHandler;

#endif 	/* __IWebView2NewVersionAvailableEventHandler_FWD_DEFINED__ */


#ifndef __IWebView2ContainsFullScreenElementChangedEventHandler_FWD_DEFINED__
#define __IWebView2ContainsFullScreenElementChangedEventHandler_FWD_DEFINED__
typedef interface IWebView2ContainsFullScreenElementChangedEventHandler IWebView2ContainsFullScreenElementChangedEventHandler;

#endif 	/* __IWebView2ContainsFullScreenElementChangedEventHandler_FWD_DEFINED__ */


#ifndef __IWebView2Environment_FWD_DEFINED__
#define __IWebView2Environment_FWD_DEFINED__
typedef interface IWebView2Environment IWebView2Environment;

#endif 	/* __IWebView2Environment_FWD_DEFINED__ */


#ifndef __IWebView2Environment2_FWD_DEFINED__
#define __IWebView2Environment2_FWD_DEFINED__
typedef interface IWebView2Environment2 IWebView2Environment2;

#endif 	/* __IWebView2Environment2_FWD_DEFINED__ */


#ifndef __IWebView2Environment3_FWD_DEFINED__
#define __IWebView2Environment3_FWD_DEFINED__
typedef interface IWebView2Environment3 IWebView2Environment3;

#endif 	/* __IWebView2Environment3_FWD_DEFINED__ */


#ifndef __IWebView2CreateWebView2EnvironmentCompletedHandler_FWD_DEFINED__
#define __IWebView2CreateWebView2EnvironmentCompletedHandler_FWD_DEFINED__
typedef interface IWebView2CreateWebView2EnvironmentCompletedHandler IWebView2CreateWebView2EnvironmentCompletedHandler;

#endif 	/* __IWebView2CreateWebView2EnvironmentCompletedHandler_FWD_DEFINED__ */


/* header files for imported files */
#include "objidl.h"
#include "oaidl.h"
#include "EventToken.h"

#ifdef __cplusplus
extern "C"{
#endif 


/* interface __MIDL_itf_webview2_0000_0000 */
/* [local] */ 






















































extern RPC_IF_HANDLE __MIDL_itf_webview2_0000_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_webview2_0000_0000_v0_0_s_ifspec;

#ifndef __IWebView2WebView_INTERFACE_DEFINED__
#define __IWebView2WebView_INTERFACE_DEFINED__

/* interface IWebView2WebView */
/* [unique][object][uuid] */ 

typedef /* [v1_enum] */ 
enum WEBVIEW2_CAPTURE_PREVIEW_IMAGE_FORMAT
    {
        WEBVIEW2_CAPTURE_PREVIEW_IMAGE_FORMAT_PNG	= 0,
        WEBVIEW2_CAPTURE_PREVIEW_IMAGE_FORMAT_JPEG	= ( WEBVIEW2_CAPTURE_PREVIEW_IMAGE_FORMAT_PNG + 1 ) 
    } 	WEBVIEW2_CAPTURE_PREVIEW_IMAGE_FORMAT;

typedef /* [v1_enum] */ 
enum WEBVIEW2_SCRIPT_DIALOG_KIND
    {
        WEBVIEW2_SCRIPT_DIALOG_KIND_ALERT	= 0,
        WEBVIEW2_SCRIPT_DIALOG_KIND_CONFIRM	= ( WEBVIEW2_SCRIPT_DIALOG_KIND_ALERT + 1 ) ,
        WEBVIEW2_SCRIPT_DIALOG_KIND_PROMPT	= ( WEBVIEW2_SCRIPT_DIALOG_KIND_CONFIRM + 1 ) 
    } 	WEBVIEW2_SCRIPT_DIALOG_KIND;

typedef /* [v1_enum] */ 
enum WEBVIEW2_PROCESS_FAILED_KIND
    {
        WEBVIEW2_PROCESS_FAILED_KIND_BROWSER_PROCESS_EXITED	= 0,
        WEBVIEW2_PROCESS_FAILED_KIND_RENDER_PROCESS_EXITED	= ( WEBVIEW2_PROCESS_FAILED_KIND_BROWSER_PROCESS_EXITED + 1 ) ,
        WEBVIEW2_PROCESS_FAILED_KIND_RENDER_PROCESS_UNRESPONSIVE	= ( WEBVIEW2_PROCESS_FAILED_KIND_RENDER_PROCESS_EXITED + 1 ) 
    } 	WEBVIEW2_PROCESS_FAILED_KIND;

typedef /* [v1_enum] */ 
enum WEBVIEW2_PERMISSION_TYPE
    {
        WEBVIEW2_PERMISSION_TYPE_UNKNOWN_PERMISSION	= 0,
        WEBVIEW2_PERMISSION_TYPE_MICROPHONE	= ( WEBVIEW2_PERMISSION_TYPE_UNKNOWN_PERMISSION + 1 ) ,
        WEBVIEW2_PERMISSION_TYPE_CAMERA	= ( WEBVIEW2_PERMISSION_TYPE_MICROPHONE + 1 ) ,
        WEBVIEW2_PERMISSION_TYPE_GEOLOCATION	= ( WEBVIEW2_PERMISSION_TYPE_CAMERA + 1 ) ,
        WEBVIEW2_PERMISSION_TYPE_NOTIFICATIONS	= ( WEBVIEW2_PERMISSION_TYPE_GEOLOCATION + 1 ) ,
        WEBVIEW2_PERMISSION_TYPE_OTHER_SENSORS	= ( WEBVIEW2_PERMISSION_TYPE_NOTIFICATIONS + 1 ) ,
        WEBVIEW2_PERMISSION_TYPE_CLIPBOARD_READ	= ( WEBVIEW2_PERMISSION_TYPE_OTHER_SENSORS + 1 ) 
    } 	WEBVIEW2_PERMISSION_TYPE;

typedef /* [v1_enum] */ 
enum WEBVIEW2_PERMISSION_STATE
    {
        WEBVIEW2_PERMISSION_STATE_DEFAULT	= 0,
        WEBVIEW2_PERMISSION_STATE_ALLOW	= ( WEBVIEW2_PERMISSION_STATE_DEFAULT + 1 ) ,
        WEBVIEW2_PERMISSION_STATE_DENY	= ( WEBVIEW2_PERMISSION_STATE_ALLOW + 1 ) 
    } 	WEBVIEW2_PERMISSION_STATE;

typedef /* [v1_enum] */ 
enum WEBVIEW2_MOVE_FOCUS_REASON
    {
        WEBVIEW2_MOVE_FOCUS_REASON_PROGRAMMATIC	= 0,
        WEBVIEW2_MOVE_FOCUS_REASON_NEXT	= ( WEBVIEW2_MOVE_FOCUS_REASON_PROGRAMMATIC + 1 ) ,
        WEBVIEW2_MOVE_FOCUS_REASON_PREVIOUS	= ( WEBVIEW2_MOVE_FOCUS_REASON_NEXT + 1 ) 
    } 	WEBVIEW2_MOVE_FOCUS_REASON;

typedef /* [v1_enum] */ 
enum WEBVIEW2_WEB_ERROR_STATUS
    {
        WEBVIEW2_WEB_ERROR_STATUS_UNKNOWN	= 0,
        WEBVIEW2_WEB_ERROR_STATUS_CERTIFICATE_COMMON_NAME_IS_INCORRECT	= ( WEBVIEW2_WEB_ERROR_STATUS_UNKNOWN + 1 ) ,
        WEBVIEW2_WEB_ERROR_STATUS_CERTIFICATE_EXPIRED	= ( WEBVIEW2_WEB_ERROR_STATUS_CERTIFICATE_COMMON_NAME_IS_INCORRECT + 1 ) ,
        WEBVIEW2_WEB_ERROR_STATUS_CLIENT_CERTIFICATE_CONTAINS_ERRORS	= ( WEBVIEW2_WEB_ERROR_STATUS_CERTIFICATE_EXPIRED + 1 ) ,
        WEBVIEW2_WEB_ERROR_STATUS_CERTIFICATE_REVOKED	= ( WEBVIEW2_WEB_ERROR_STATUS_CLIENT_CERTIFICATE_CONTAINS_ERRORS + 1 ) ,
        WEBVIEW2_WEB_ERROR_STATUS_CERTIFICATE_IS_INVALID	= ( WEBVIEW2_WEB_ERROR_STATUS_CERTIFICATE_REVOKED + 1 ) ,
        WEBVIEW2_WEB_ERROR_STATUS_SERVER_UNREACHABLE	= ( WEBVIEW2_WEB_ERROR_STATUS_CERTIFICATE_IS_INVALID + 1 ) ,
        WEBVIEW2_WEB_ERROR_STATUS_TIMEOUT	= ( WEBVIEW2_WEB_ERROR_STATUS_SERVER_UNREACHABLE + 1 ) ,
        WEBVIEW2_WEB_ERROR_STATUS_ERROR_HTTP_INVALID_SERVER_RESPONSE	= ( WEBVIEW2_WEB_ERROR_STATUS_TIMEOUT + 1 ) ,
        WEBVIEW2_WEB_ERROR_STATUS_CONNECTION_ABORTED	= ( WEBVIEW2_WEB_ERROR_STATUS_ERROR_HTTP_INVALID_SERVER_RESPONSE + 1 ) ,
        WEBVIEW2_WEB_ERROR_STATUS_CONNECTION_RESET	= ( WEBVIEW2_WEB_ERROR_STATUS_CONNECTION_ABORTED + 1 ) ,
        WEBVIEW2_WEB_ERROR_STATUS_DISCONNECTED	= ( WEBVIEW2_WEB_ERROR_STATUS_CONNECTION_RESET + 1 ) ,
        WEBVIEW2_WEB_ERROR_STATUS_CANNOT_CONNECT	= ( WEBVIEW2_WEB_ERROR_STATUS_DISCONNECTED + 1 ) ,
        WEBVIEW2_WEB_ERROR_STATUS_HOST_NAME_NOT_RESOLVED	= ( WEBVIEW2_WEB_ERROR_STATUS_CANNOT_CONNECT + 1 ) ,
        WEBVIEW2_WEB_ERROR_STATUS_OPERATION_CANCELED	= ( WEBVIEW2_WEB_ERROR_STATUS_HOST_NAME_NOT_RESOLVED + 1 ) ,
        WEBVIEW2_WEB_ERROR_STATUS_REDIRECT_FAILED	= ( WEBVIEW2_WEB_ERROR_STATUS_OPERATION_CANCELED + 1 ) ,
        WEBVIEW2_WEB_ERROR_STATUS_UNEXPECTED_ERROR	= ( WEBVIEW2_WEB_ERROR_STATUS_REDIRECT_FAILED + 1 ) 
    } 	WEBVIEW2_WEB_ERROR_STATUS;

typedef /* [v1_enum] */ 
enum WEBVIEW2_WEB_RESOURCE_CONTEXT
    {
        WEBVIEW2_WEB_RESOURCE_CONTEXT_ALL	= 0,
        WEBVIEW2_WEB_RESOURCE_CONTEXT_DOCUMENT	= ( WEBVIEW2_WEB_RESOURCE_CONTEXT_ALL + 1 ) ,
        WEBVIEW2_WEB_RESOURCE_CONTEXT_STYLESHEET	= ( WEBVIEW2_WEB_RESOURCE_CONTEXT_DOCUMENT + 1 ) ,
        WEBVIEW2_WEB_RESOURCE_CONTEXT_IMAGE	= ( WEBVIEW2_WEB_RESOURCE_CONTEXT_STYLESHEET + 1 ) ,
        WEBVIEW2_WEB_RESOURCE_CONTEXT_MEDIA	= ( WEBVIEW2_WEB_RESOURCE_CONTEXT_IMAGE + 1 ) ,
        WEBVIEW2_WEB_RESOURCE_CONTEXT_FONT	= ( WEBVIEW2_WEB_RESOURCE_CONTEXT_MEDIA + 1 ) ,
        WEBVIEW2_WEB_RESOURCE_CONTEXT_SCRIPT	= ( WEBVIEW2_WEB_RESOURCE_CONTEXT_FONT + 1 ) ,
        WEBVIEW2_WEB_RESOURCE_CONTEXT_XML_HTTP_REQUEST	= ( WEBVIEW2_WEB_RESOURCE_CONTEXT_SCRIPT + 1 ) ,
        WEBVIEW2_WEB_RESOURCE_CONTEXT_FETCH	= ( WEBVIEW2_WEB_RESOURCE_CONTEXT_XML_HTTP_REQUEST + 1 ) ,
        WEBVIEW2_WEB_RESOURCE_CONTEXT_TEXT_TRACK	= ( WEBVIEW2_WEB_RESOURCE_CONTEXT_FETCH + 1 ) ,
        WEBVIEW2_WEB_RESOURCE_CONTEXT_EVENT_SOURCE	= ( WEBVIEW2_WEB_RESOURCE_CONTEXT_TEXT_TRACK + 1 ) ,
        WEBVIEW2_WEB_RESOURCE_CONTEXT_WEBSOCKET	= ( WEBVIEW2_WEB_RESOURCE_CONTEXT_EVENT_SOURCE + 1 ) ,
        WEBVIEW2_WEB_RESOURCE_CONTEXT_MANIFEST	= ( WEBVIEW2_WEB_RESOURCE_CONTEXT_WEBSOCKET + 1 ) ,
        WEBVIEW2_WEB_RESOURCE_CONTEXT_SIGNED_EXCHANGE	= ( WEBVIEW2_WEB_RESOURCE_CONTEXT_MANIFEST + 1 ) ,
        WEBVIEW2_WEB_RESOURCE_CONTEXT_PING	= ( WEBVIEW2_WEB_RESOURCE_CONTEXT_SIGNED_EXCHANGE + 1 ) ,
        WEBVIEW2_WEB_RESOURCE_CONTEXT_CSP_VIOLATION_REPORT	= ( WEBVIEW2_WEB_RESOURCE_CONTEXT_PING + 1 ) ,
        WEBVIEW2_WEB_RESOURCE_CONTEXT_OTHER	= ( WEBVIEW2_WEB_RESOURCE_CONTEXT_CSP_VIOLATION_REPORT + 1 ) 
    } 	WEBVIEW2_WEB_RESOURCE_CONTEXT;


EXTERN_C const IID IID_IWebView2WebView;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("76711B9E-8D56-4806-8485-35250BB2384F")
    IWebView2WebView : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE get_Settings( 
            /* [retval][out] */ IWebView2Settings **settings) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE get_Source( 
            /* [retval][out] */ LPWSTR *uri) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Navigate( 
            /* [in] */ LPCWSTR uri) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE MoveFocus( 
            /* [in] */ WEBVIEW2_MOVE_FOCUS_REASON reason) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE NavigateToString( 
            /* [in] */ LPCWSTR htmlContent) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE add_NavigationStarting( 
            /* [in] */ IWebView2NavigationStartingEventHandler *eventHandler,
            /* [out] */ EventRegistrationToken *token) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE remove_NavigationStarting( 
            /* [in] */ EventRegistrationToken token) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE add_DocumentStateChanged( 
            /* [in] */ IWebView2DocumentStateChangedEventHandler *eventHandler,
            /* [out] */ EventRegistrationToken *token) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE remove_DocumentStateChanged( 
            /* [in] */ EventRegistrationToken token) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE add_NavigationCompleted( 
            /* [in] */ IWebView2NavigationCompletedEventHandler *eventHandler,
            /* [out] */ EventRegistrationToken *token) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE remove_NavigationCompleted( 
            /* [in] */ EventRegistrationToken token) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE add_FrameNavigationStarting( 
            /* [in] */ IWebView2NavigationStartingEventHandler *eventHandler,
            /* [out] */ EventRegistrationToken *token) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE remove_FrameNavigationStarting( 
            /* [in] */ EventRegistrationToken token) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE add_MoveFocusRequested( 
            /* [in] */ IWebView2MoveFocusRequestedEventHandler *eventHandler,
            /* [out] */ EventRegistrationToken *token) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE remove_MoveFocusRequested( 
            /* [in] */ EventRegistrationToken token) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE add_GotFocus( 
            /* [in] */ IWebView2FocusChangedEventHandler *eventHandler,
            /* [out] */ EventRegistrationToken *token) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE remove_GotFocus( 
            /* [in] */ EventRegistrationToken token) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE add_LostFocus( 
            /* [in] */ IWebView2FocusChangedEventHandler *eventHandler,
            /* [out] */ EventRegistrationToken *token) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE remove_LostFocus( 
            /* [in] */ EventRegistrationToken token) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE add_WebResourceRequested_deprecated( 
            /* [in] */ LPCWSTR *const urlFilter,
            /* [in] */ WEBVIEW2_WEB_RESOURCE_CONTEXT *const resourceContextFilter,
            /* [in] */ SIZE_T filterLength,
            /* [in] */ IWebView2WebResourceRequestedEventHandler *eventHandler,
            /* [out] */ EventRegistrationToken *token) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE remove_WebResourceRequested( 
            /* [in] */ EventRegistrationToken token) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE add_ScriptDialogOpening( 
            /* [in] */ IWebView2ScriptDialogOpeningEventHandler *eventHandler,
            /* [out] */ EventRegistrationToken *token) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE remove_ScriptDialogOpening( 
            /* [in] */ EventRegistrationToken token) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE add_ZoomFactorChanged( 
            /* [in] */ IWebView2ZoomFactorChangedEventHandler *eventHandler,
            /* [out] */ EventRegistrationToken *token) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE remove_ZoomFactorChanged( 
            /* [in] */ EventRegistrationToken token) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE add_PermissionRequested( 
            /* [in] */ IWebView2PermissionRequestedEventHandler *eventHandler,
            /* [out] */ EventRegistrationToken *token) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE remove_PermissionRequested( 
            /* [in] */ EventRegistrationToken token) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE add_ProcessFailed( 
            /* [in] */ IWebView2ProcessFailedEventHandler *eventHandler,
            /* [out] */ EventRegistrationToken *token) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE remove_ProcessFailed( 
            /* [in] */ EventRegistrationToken token) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE AddScriptToExecuteOnDocumentCreated( 
            /* [in] */ LPCWSTR javaScript,
            /* [in] */ IWebView2AddScriptToExecuteOnDocumentCreatedCompletedHandler *handler) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RemoveScriptToExecuteOnDocumentCreated( 
            /* [in] */ LPCWSTR id) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ExecuteScript( 
            /* [in] */ LPCWSTR javaScript,
            /* [in] */ IWebView2ExecuteScriptCompletedHandler *handler) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CapturePreview( 
            /* [in] */ WEBVIEW2_CAPTURE_PREVIEW_IMAGE_FORMAT imageFormat,
            /* [in] */ IStream *imageStream,
            /* [in] */ IWebView2CapturePreviewCompletedHandler *handler) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Reload( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE get_Bounds( 
            /* [retval][out] */ RECT *bounds) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE put_Bounds( 
            /* [in] */ RECT bounds) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE get_ZoomFactor( 
            /* [retval][out] */ double *zoomFactor) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE put_ZoomFactor( 
            /* [in] */ double zoomFactor) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE get_IsVisible( 
            /* [retval][out] */ BOOL *isVisible) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE put_IsVisible( 
            /* [in] */ BOOL isVisible) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE PostWebMessageAsJson( 
            /* [in] */ LPCWSTR webMessageAsJson) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE PostWebMessageAsString( 
            /* [in] */ LPCWSTR webMessageAsString) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE add_WebMessageReceived( 
            /* [in] */ IWebView2WebMessageReceivedEventHandler *handler,
            /* [out] */ EventRegistrationToken *token) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE remove_WebMessageReceived( 
            /* [in] */ EventRegistrationToken token) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Close( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CallDevToolsProtocolMethod( 
            /* [in] */ LPCWSTR methodName,
            /* [in] */ LPCWSTR parametersAsJson,
            /* [in] */ IWebView2CallDevToolsProtocolMethodCompletedHandler *handler) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE add_DevToolsProtocolEventReceived( 
            /* [in] */ LPCWSTR eventName,
            /* [in] */ IWebView2DevToolsProtocolEventReceivedEventHandler *handler,
            /* [out] */ EventRegistrationToken *token) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE remove_DevToolsProtocolEventReceived( 
            /* [in] */ LPCWSTR eventName,
            /* [in] */ EventRegistrationToken token) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE get_BrowserProcessId( 
            /* [retval][out] */ UINT32 *value) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE get_CanGoBack( 
            /* [retval][out] */ BOOL *canGoBack) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE get_CanGoForward( 
            /* [retval][out] */ BOOL *canGoForward) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GoBack( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GoForward( void) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IWebView2WebViewVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IWebView2WebView * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IWebView2WebView * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IWebView2WebView * This);
        
        HRESULT ( STDMETHODCALLTYPE *get_Settings )( 
            IWebView2WebView * This,
            /* [retval][out] */ IWebView2Settings **settings);
        
        HRESULT ( STDMETHODCALLTYPE *get_Source )( 
            IWebView2WebView * This,
            /* [retval][out] */ LPWSTR *uri);
        
        HRESULT ( STDMETHODCALLTYPE *Navigate )( 
            IWebView2WebView * This,
            /* [in] */ LPCWSTR uri);
        
        HRESULT ( STDMETHODCALLTYPE *MoveFocus )( 
            IWebView2WebView * This,
            /* [in] */ WEBVIEW2_MOVE_FOCUS_REASON reason);
        
        HRESULT ( STDMETHODCALLTYPE *NavigateToString )( 
            IWebView2WebView * This,
            /* [in] */ LPCWSTR htmlContent);
        
        HRESULT ( STDMETHODCALLTYPE *add_NavigationStarting )( 
            IWebView2WebView * This,
            /* [in] */ IWebView2NavigationStartingEventHandler *eventHandler,
            /* [out] */ EventRegistrationToken *token);
        
        HRESULT ( STDMETHODCALLTYPE *remove_NavigationStarting )( 
            IWebView2WebView * This,
            /* [in] */ EventRegistrationToken token);
        
        HRESULT ( STDMETHODCALLTYPE *add_DocumentStateChanged )( 
            IWebView2WebView * This,
            /* [in] */ IWebView2DocumentStateChangedEventHandler *eventHandler,
            /* [out] */ EventRegistrationToken *token);
        
        HRESULT ( STDMETHODCALLTYPE *remove_DocumentStateChanged )( 
            IWebView2WebView * This,
            /* [in] */ EventRegistrationToken token);
        
        HRESULT ( STDMETHODCALLTYPE *add_NavigationCompleted )( 
            IWebView2WebView * This,
            /* [in] */ IWebView2NavigationCompletedEventHandler *eventHandler,
            /* [out] */ EventRegistrationToken *token);
        
        HRESULT ( STDMETHODCALLTYPE *remove_NavigationCompleted )( 
            IWebView2WebView * This,
            /* [in] */ EventRegistrationToken token);
        
        HRESULT ( STDMETHODCALLTYPE *add_FrameNavigationStarting )( 
            IWebView2WebView * This,
            /* [in] */ IWebView2NavigationStartingEventHandler *eventHandler,
            /* [out] */ EventRegistrationToken *token);
        
        HRESULT ( STDMETHODCALLTYPE *remove_FrameNavigationStarting )( 
            IWebView2WebView * This,
            /* [in] */ EventRegistrationToken token);
        
        HRESULT ( STDMETHODCALLTYPE *add_MoveFocusRequested )( 
            IWebView2WebView * This,
            /* [in] */ IWebView2MoveFocusRequestedEventHandler *eventHandler,
            /* [out] */ EventRegistrationToken *token);
        
        HRESULT ( STDMETHODCALLTYPE *remove_MoveFocusRequested )( 
            IWebView2WebView * This,
            /* [in] */ EventRegistrationToken token);
        
        HRESULT ( STDMETHODCALLTYPE *add_GotFocus )( 
            IWebView2WebView * This,
            /* [in] */ IWebView2FocusChangedEventHandler *eventHandler,
            /* [out] */ EventRegistrationToken *token);
        
        HRESULT ( STDMETHODCALLTYPE *remove_GotFocus )( 
            IWebView2WebView * This,
            /* [in] */ EventRegistrationToken token);
        
        HRESULT ( STDMETHODCALLTYPE *add_LostFocus )( 
            IWebView2WebView * This,
            /* [in] */ IWebView2FocusChangedEventHandler *eventHandler,
            /* [out] */ EventRegistrationToken *token);
        
        HRESULT ( STDMETHODCALLTYPE *remove_LostFocus )( 
            IWebView2WebView * This,
            /* [in] */ EventRegistrationToken token);
        
        HRESULT ( STDMETHODCALLTYPE *add_WebResourceRequested_deprecated )( 
            IWebView2WebView * This,
            /* [in] */ LPCWSTR *const urlFilter,
            /* [in] */ WEBVIEW2_WEB_RESOURCE_CONTEXT *const resourceContextFilter,
            /* [in] */ SIZE_T filterLength,
            /* [in] */ IWebView2WebResourceRequestedEventHandler *eventHandler,
            /* [out] */ EventRegistrationToken *token);
        
        HRESULT ( STDMETHODCALLTYPE *remove_WebResourceRequested )( 
            IWebView2WebView * This,
            /* [in] */ EventRegistrationToken token);
        
        HRESULT ( STDMETHODCALLTYPE *add_ScriptDialogOpening )( 
            IWebView2WebView * This,
            /* [in] */ IWebView2ScriptDialogOpeningEventHandler *eventHandler,
            /* [out] */ EventRegistrationToken *token);
        
        HRESULT ( STDMETHODCALLTYPE *remove_ScriptDialogOpening )( 
            IWebView2WebView * This,
            /* [in] */ EventRegistrationToken token);
        
        HRESULT ( STDMETHODCALLTYPE *add_ZoomFactorChanged )( 
            IWebView2WebView * This,
            /* [in] */ IWebView2ZoomFactorChangedEventHandler *eventHandler,
            /* [out] */ EventRegistrationToken *token);
        
        HRESULT ( STDMETHODCALLTYPE *remove_ZoomFactorChanged )( 
            IWebView2WebView * This,
            /* [in] */ EventRegistrationToken token);
        
        HRESULT ( STDMETHODCALLTYPE *add_PermissionRequested )( 
            IWebView2WebView * This,
            /* [in] */ IWebView2PermissionRequestedEventHandler *eventHandler,
            /* [out] */ EventRegistrationToken *token);
        
        HRESULT ( STDMETHODCALLTYPE *remove_PermissionRequested )( 
            IWebView2WebView * This,
            /* [in] */ EventRegistrationToken token);
        
        HRESULT ( STDMETHODCALLTYPE *add_ProcessFailed )( 
            IWebView2WebView * This,
            /* [in] */ IWebView2ProcessFailedEventHandler *eventHandler,
            /* [out] */ EventRegistrationToken *token);
        
        HRESULT ( STDMETHODCALLTYPE *remove_ProcessFailed )( 
            IWebView2WebView * This,
            /* [in] */ EventRegistrationToken token);
        
        HRESULT ( STDMETHODCALLTYPE *AddScriptToExecuteOnDocumentCreated )( 
            IWebView2WebView * This,
            /* [in] */ LPCWSTR javaScript,
            /* [in] */ IWebView2AddScriptToExecuteOnDocumentCreatedCompletedHandler *handler);
        
        HRESULT ( STDMETHODCALLTYPE *RemoveScriptToExecuteOnDocumentCreated )( 
            IWebView2WebView * This,
            /* [in] */ LPCWSTR id);
        
        HRESULT ( STDMETHODCALLTYPE *ExecuteScript )( 
            IWebView2WebView * This,
            /* [in] */ LPCWSTR javaScript,
            /* [in] */ IWebView2ExecuteScriptCompletedHandler *handler);
        
        HRESULT ( STDMETHODCALLTYPE *CapturePreview )( 
            IWebView2WebView * This,
            /* [in] */ WEBVIEW2_CAPTURE_PREVIEW_IMAGE_FORMAT imageFormat,
            /* [in] */ IStream *imageStream,
            /* [in] */ IWebView2CapturePreviewCompletedHandler *handler);
        
        HRESULT ( STDMETHODCALLTYPE *Reload )( 
            IWebView2WebView * This);
        
        HRESULT ( STDMETHODCALLTYPE *get_Bounds )( 
            IWebView2WebView * This,
            /* [retval][out] */ RECT *bounds);
        
        HRESULT ( STDMETHODCALLTYPE *put_Bounds )( 
            IWebView2WebView * This,
            /* [in] */ RECT bounds);
        
        HRESULT ( STDMETHODCALLTYPE *get_ZoomFactor )( 
            IWebView2WebView * This,
            /* [retval][out] */ double *zoomFactor);
        
        HRESULT ( STDMETHODCALLTYPE *put_ZoomFactor )( 
            IWebView2WebView * This,
            /* [in] */ double zoomFactor);
        
        HRESULT ( STDMETHODCALLTYPE *get_IsVisible )( 
            IWebView2WebView * This,
            /* [retval][out] */ BOOL *isVisible);
        
        HRESULT ( STDMETHODCALLTYPE *put_IsVisible )( 
            IWebView2WebView * This,
            /* [in] */ BOOL isVisible);
        
        HRESULT ( STDMETHODCALLTYPE *PostWebMessageAsJson )( 
            IWebView2WebView * This,
            /* [in] */ LPCWSTR webMessageAsJson);
        
        HRESULT ( STDMETHODCALLTYPE *PostWebMessageAsString )( 
            IWebView2WebView * This,
            /* [in] */ LPCWSTR webMessageAsString);
        
        HRESULT ( STDMETHODCALLTYPE *add_WebMessageReceived )( 
            IWebView2WebView * This,
            /* [in] */ IWebView2WebMessageReceivedEventHandler *handler,
            /* [out] */ EventRegistrationToken *token);
        
        HRESULT ( STDMETHODCALLTYPE *remove_WebMessageReceived )( 
            IWebView2WebView * This,
            /* [in] */ EventRegistrationToken token);
        
        HRESULT ( STDMETHODCALLTYPE *Close )( 
            IWebView2WebView * This);
        
        HRESULT ( STDMETHODCALLTYPE *CallDevToolsProtocolMethod )( 
            IWebView2WebView * This,
            /* [in] */ LPCWSTR methodName,
            /* [in] */ LPCWSTR parametersAsJson,
            /* [in] */ IWebView2CallDevToolsProtocolMethodCompletedHandler *handler);
        
        HRESULT ( STDMETHODCALLTYPE *add_DevToolsProtocolEventReceived )( 
            IWebView2WebView * This,
            /* [in] */ LPCWSTR eventName,
            /* [in] */ IWebView2DevToolsProtocolEventReceivedEventHandler *handler,
            /* [out] */ EventRegistrationToken *token);
        
        HRESULT ( STDMETHODCALLTYPE *remove_DevToolsProtocolEventReceived )( 
            IWebView2WebView * This,
            /* [in] */ LPCWSTR eventName,
            /* [in] */ EventRegistrationToken token);
        
        HRESULT ( STDMETHODCALLTYPE *get_BrowserProcessId )( 
            IWebView2WebView * This,
            /* [retval][out] */ UINT32 *value);
        
        HRESULT ( STDMETHODCALLTYPE *get_CanGoBack )( 
            IWebView2WebView * This,
            /* [retval][out] */ BOOL *canGoBack);
        
        HRESULT ( STDMETHODCALLTYPE *get_CanGoForward )( 
            IWebView2WebView * This,
            /* [retval][out] */ BOOL *canGoForward);
        
        HRESULT ( STDMETHODCALLTYPE *GoBack )( 
            IWebView2WebView * This);
        
        HRESULT ( STDMETHODCALLTYPE *GoForward )( 
            IWebView2WebView * This);
        
        END_INTERFACE
    } IWebView2WebViewVtbl;

    interface IWebView2WebView
    {
        CONST_VTBL struct IWebView2WebViewVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWebView2WebView_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IWebView2WebView_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IWebView2WebView_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IWebView2WebView_get_Settings(This,settings)	\
    ( (This)->lpVtbl -> get_Settings(This,settings) ) 

#define IWebView2WebView_get_Source(This,uri)	\
    ( (This)->lpVtbl -> get_Source(This,uri) ) 

#define IWebView2WebView_Navigate(This,uri)	\
    ( (This)->lpVtbl -> Navigate(This,uri) ) 

#define IWebView2WebView_MoveFocus(This,reason)	\
    ( (This)->lpVtbl -> MoveFocus(This,reason) ) 

#define IWebView2WebView_NavigateToString(This,htmlContent)	\
    ( (This)->lpVtbl -> NavigateToString(This,htmlContent) ) 

#define IWebView2WebView_add_NavigationStarting(This,eventHandler,token)	\
    ( (This)->lpVtbl -> add_NavigationStarting(This,eventHandler,token) ) 

#define IWebView2WebView_remove_NavigationStarting(This,token)	\
    ( (This)->lpVtbl -> remove_NavigationStarting(This,token) ) 

#define IWebView2WebView_add_DocumentStateChanged(This,eventHandler,token)	\
    ( (This)->lpVtbl -> add_DocumentStateChanged(This,eventHandler,token) ) 

#define IWebView2WebView_remove_DocumentStateChanged(This,token)	\
    ( (This)->lpVtbl -> remove_DocumentStateChanged(This,token) ) 

#define IWebView2WebView_add_NavigationCompleted(This,eventHandler,token)	\
    ( (This)->lpVtbl -> add_NavigationCompleted(This,eventHandler,token) ) 

#define IWebView2WebView_remove_NavigationCompleted(This,token)	\
    ( (This)->lpVtbl -> remove_NavigationCompleted(This,token) ) 

#define IWebView2WebView_add_FrameNavigationStarting(This,eventHandler,token)	\
    ( (This)->lpVtbl -> add_FrameNavigationStarting(This,eventHandler,token) ) 

#define IWebView2WebView_remove_FrameNavigationStarting(This,token)	\
    ( (This)->lpVtbl -> remove_FrameNavigationStarting(This,token) ) 

#define IWebView2WebView_add_MoveFocusRequested(This,eventHandler,token)	\
    ( (This)->lpVtbl -> add_MoveFocusRequested(This,eventHandler,token) ) 

#define IWebView2WebView_remove_MoveFocusRequested(This,token)	\
    ( (This)->lpVtbl -> remove_MoveFocusRequested(This,token) ) 

#define IWebView2WebView_add_GotFocus(This,eventHandler,token)	\
    ( (This)->lpVtbl -> add_GotFocus(This,eventHandler,token) ) 

#define IWebView2WebView_remove_GotFocus(This,token)	\
    ( (This)->lpVtbl -> remove_GotFocus(This,token) ) 

#define IWebView2WebView_add_LostFocus(This,eventHandler,token)	\
    ( (This)->lpVtbl -> add_LostFocus(This,eventHandler,token) ) 

#define IWebView2WebView_remove_LostFocus(This,token)	\
    ( (This)->lpVtbl -> remove_LostFocus(This,token) ) 

#define IWebView2WebView_add_WebResourceRequested_deprecated(This,urlFilter,resourceContextFilter,filterLength,eventHandler,token)	\
    ( (This)->lpVtbl -> add_WebResourceRequested_deprecated(This,urlFilter,resourceContextFilter,filterLength,eventHandler,token) ) 

#define IWebView2WebView_remove_WebResourceRequested(This,token)	\
    ( (This)->lpVtbl -> remove_WebResourceRequested(This,token) ) 

#define IWebView2WebView_add_ScriptDialogOpening(This,eventHandler,token)	\
    ( (This)->lpVtbl -> add_ScriptDialogOpening(This,eventHandler,token) ) 

#define IWebView2WebView_remove_ScriptDialogOpening(This,token)	\
    ( (This)->lpVtbl -> remove_ScriptDialogOpening(This,token) ) 

#define IWebView2WebView_add_ZoomFactorChanged(This,eventHandler,token)	\
    ( (This)->lpVtbl -> add_ZoomFactorChanged(This,eventHandler,token) ) 

#define IWebView2WebView_remove_ZoomFactorChanged(This,token)	\
    ( (This)->lpVtbl -> remove_ZoomFactorChanged(This,token) ) 

#define IWebView2WebView_add_PermissionRequested(This,eventHandler,token)	\
    ( (This)->lpVtbl -> add_PermissionRequested(This,eventHandler,token) ) 

#define IWebView2WebView_remove_PermissionRequested(This,token)	\
    ( (This)->lpVtbl -> remove_PermissionRequested(This,token) ) 

#define IWebView2WebView_add_ProcessFailed(This,eventHandler,token)	\
    ( (This)->lpVtbl -> add_ProcessFailed(This,eventHandler,token) ) 

#define IWebView2WebView_remove_ProcessFailed(This,token)	\
    ( (This)->lpVtbl -> remove_ProcessFailed(This,token) ) 

#define IWebView2WebView_AddScriptToExecuteOnDocumentCreated(This,javaScript,handler)	\
    ( (This)->lpVtbl -> AddScriptToExecuteOnDocumentCreated(This,javaScript,handler) ) 

#define IWebView2WebView_RemoveScriptToExecuteOnDocumentCreated(This,id)	\
    ( (This)->lpVtbl -> RemoveScriptToExecuteOnDocumentCreated(This,id) ) 

#define IWebView2WebView_ExecuteScript(This,javaScript,handler)	\
    ( (This)->lpVtbl -> ExecuteScript(This,javaScript,handler) ) 

#define IWebView2WebView_CapturePreview(This,imageFormat,imageStream,handler)	\
    ( (This)->lpVtbl -> CapturePreview(This,imageFormat,imageStream,handler) ) 

#define IWebView2WebView_Reload(This)	\
    ( (This)->lpVtbl -> Reload(This) ) 

#define IWebView2WebView_get_Bounds(This,bounds)	\
    ( (This)->lpVtbl -> get_Bounds(This,bounds) ) 

#define IWebView2WebView_put_Bounds(This,bounds)	\
    ( (This)->lpVtbl -> put_Bounds(This,bounds) ) 

#define IWebView2WebView_get_ZoomFactor(This,zoomFactor)	\
    ( (This)->lpVtbl -> get_ZoomFactor(This,zoomFactor) ) 

#define IWebView2WebView_put_ZoomFactor(This,zoomFactor)	\
    ( (This)->lpVtbl -> put_ZoomFactor(This,zoomFactor) ) 

#define IWebView2WebView_get_IsVisible(This,isVisible)	\
    ( (This)->lpVtbl -> get_IsVisible(This,isVisible) ) 

#define IWebView2WebView_put_IsVisible(This,isVisible)	\
    ( (This)->lpVtbl -> put_IsVisible(This,isVisible) ) 

#define IWebView2WebView_PostWebMessageAsJson(This,webMessageAsJson)	\
    ( (This)->lpVtbl -> PostWebMessageAsJson(This,webMessageAsJson) ) 

#define IWebView2WebView_PostWebMessageAsString(This,webMessageAsString)	\
    ( (This)->lpVtbl -> PostWebMessageAsString(This,webMessageAsString) ) 

#define IWebView2WebView_add_WebMessageReceived(This,handler,token)	\
    ( (This)->lpVtbl -> add_WebMessageReceived(This,handler,token) ) 

#define IWebView2WebView_remove_WebMessageReceived(This,token)	\
    ( (This)->lpVtbl -> remove_WebMessageReceived(This,token) ) 

#define IWebView2WebView_Close(This)	\
    ( (This)->lpVtbl -> Close(This) ) 

#define IWebView2WebView_CallDevToolsProtocolMethod(This,methodName,parametersAsJson,handler)	\
    ( (This)->lpVtbl -> CallDevToolsProtocolMethod(This,methodName,parametersAsJson,handler) ) 

#define IWebView2WebView_add_DevToolsProtocolEventReceived(This,eventName,handler,token)	\
    ( (This)->lpVtbl -> add_DevToolsProtocolEventReceived(This,eventName,handler,token) ) 

#define IWebView2WebView_remove_DevToolsProtocolEventReceived(This,eventName,token)	\
    ( (This)->lpVtbl -> remove_DevToolsProtocolEventReceived(This,eventName,token) ) 

#define IWebView2WebView_get_BrowserProcessId(This,value)	\
    ( (This)->lpVtbl -> get_BrowserProcessId(This,value) ) 

#define IWebView2WebView_get_CanGoBack(This,canGoBack)	\
    ( (This)->lpVtbl -> get_CanGoBack(This,canGoBack) ) 

#define IWebView2WebView_get_CanGoForward(This,canGoForward)	\
    ( (This)->lpVtbl -> get_CanGoForward(This,canGoForward) ) 

#define IWebView2WebView_GoBack(This)	\
    ( (This)->lpVtbl -> GoBack(This) ) 

#define IWebView2WebView_GoForward(This)	\
    ( (This)->lpVtbl -> GoForward(This) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IWebView2WebView_INTERFACE_DEFINED__ */


#ifndef __IWebView2WebView2_INTERFACE_DEFINED__
#define __IWebView2WebView2_INTERFACE_DEFINED__

/* interface IWebView2WebView2 */
/* [unique][object][uuid] */ 


EXTERN_C const IID IID_IWebView2WebView2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("BB2DA827-0632-4ED6-8EDA-3F9E561767CA")
    IWebView2WebView2 : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Stop( void) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IWebView2WebView2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IWebView2WebView2 * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IWebView2WebView2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IWebView2WebView2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *Stop )( 
            IWebView2WebView2 * This);
        
        END_INTERFACE
    } IWebView2WebView2Vtbl;

    interface IWebView2WebView2
    {
        CONST_VTBL struct IWebView2WebView2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWebView2WebView2_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IWebView2WebView2_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IWebView2WebView2_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IWebView2WebView2_Stop(This)	\
    ( (This)->lpVtbl -> Stop(This) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IWebView2WebView2_INTERFACE_DEFINED__ */


#ifndef __IWebView2WebView3_INTERFACE_DEFINED__
#define __IWebView2WebView3_INTERFACE_DEFINED__

/* interface IWebView2WebView3 */
/* [unique][object][uuid] */ 


EXTERN_C const IID IID_IWebView2WebView3;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("A16A5FF1-C23B-4489-8752-8568A1BED09C")
    IWebView2WebView3 : public IWebView2WebView
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Stop( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE add_NewWindowRequested( 
            /* [in] */ IWebView2NewWindowRequestedEventHandler *eventHandler,
            /* [out] */ EventRegistrationToken *token) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE remove_NewWindowRequested( 
            /* [in] */ EventRegistrationToken token) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE add_DocumentTitleChanged( 
            /* [in] */ IWebView2DocumentTitleChangedEventHandler *eventHandler,
            /* [out] */ EventRegistrationToken *token) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE remove_DocumentTitleChanged( 
            /* [in] */ EventRegistrationToken token) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE get_DocumentTitle( 
            /* [out] */ LPWSTR *title) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IWebView2WebView3Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IWebView2WebView3 * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IWebView2WebView3 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IWebView2WebView3 * This);
        
        HRESULT ( STDMETHODCALLTYPE *get_Settings )( 
            IWebView2WebView3 * This,
            /* [retval][out] */ IWebView2Settings **settings);
        
        HRESULT ( STDMETHODCALLTYPE *get_Source )( 
            IWebView2WebView3 * This,
            /* [retval][out] */ LPWSTR *uri);
        
        HRESULT ( STDMETHODCALLTYPE *Navigate )( 
            IWebView2WebView3 * This,
            /* [in] */ LPCWSTR uri);
        
        HRESULT ( STDMETHODCALLTYPE *MoveFocus )( 
            IWebView2WebView3 * This,
            /* [in] */ WEBVIEW2_MOVE_FOCUS_REASON reason);
        
        HRESULT ( STDMETHODCALLTYPE *NavigateToString )( 
            IWebView2WebView3 * This,
            /* [in] */ LPCWSTR htmlContent);
        
        HRESULT ( STDMETHODCALLTYPE *add_NavigationStarting )( 
            IWebView2WebView3 * This,
            /* [in] */ IWebView2NavigationStartingEventHandler *eventHandler,
            /* [out] */ EventRegistrationToken *token);
        
        HRESULT ( STDMETHODCALLTYPE *remove_NavigationStarting )( 
            IWebView2WebView3 * This,
            /* [in] */ EventRegistrationToken token);
        
        HRESULT ( STDMETHODCALLTYPE *add_DocumentStateChanged )( 
            IWebView2WebView3 * This,
            /* [in] */ IWebView2DocumentStateChangedEventHandler *eventHandler,
            /* [out] */ EventRegistrationToken *token);
        
        HRESULT ( STDMETHODCALLTYPE *remove_DocumentStateChanged )( 
            IWebView2WebView3 * This,
            /* [in] */ EventRegistrationToken token);
        
        HRESULT ( STDMETHODCALLTYPE *add_NavigationCompleted )( 
            IWebView2WebView3 * This,
            /* [in] */ IWebView2NavigationCompletedEventHandler *eventHandler,
            /* [out] */ EventRegistrationToken *token);
        
        HRESULT ( STDMETHODCALLTYPE *remove_NavigationCompleted )( 
            IWebView2WebView3 * This,
            /* [in] */ EventRegistrationToken token);
        
        HRESULT ( STDMETHODCALLTYPE *add_FrameNavigationStarting )( 
            IWebView2WebView3 * This,
            /* [in] */ IWebView2NavigationStartingEventHandler *eventHandler,
            /* [out] */ EventRegistrationToken *token);
        
        HRESULT ( STDMETHODCALLTYPE *remove_FrameNavigationStarting )( 
            IWebView2WebView3 * This,
            /* [in] */ EventRegistrationToken token);
        
        HRESULT ( STDMETHODCALLTYPE *add_MoveFocusRequested )( 
            IWebView2WebView3 * This,
            /* [in] */ IWebView2MoveFocusRequestedEventHandler *eventHandler,
            /* [out] */ EventRegistrationToken *token);
        
        HRESULT ( STDMETHODCALLTYPE *remove_MoveFocusRequested )( 
            IWebView2WebView3 * This,
            /* [in] */ EventRegistrationToken token);
        
        HRESULT ( STDMETHODCALLTYPE *add_GotFocus )( 
            IWebView2WebView3 * This,
            /* [in] */ IWebView2FocusChangedEventHandler *eventHandler,
            /* [out] */ EventRegistrationToken *token);
        
        HRESULT ( STDMETHODCALLTYPE *remove_GotFocus )( 
            IWebView2WebView3 * This,
            /* [in] */ EventRegistrationToken token);
        
        HRESULT ( STDMETHODCALLTYPE *add_LostFocus )( 
            IWebView2WebView3 * This,
            /* [in] */ IWebView2FocusChangedEventHandler *eventHandler,
            /* [out] */ EventRegistrationToken *token);
        
        HRESULT ( STDMETHODCALLTYPE *remove_LostFocus )( 
            IWebView2WebView3 * This,
            /* [in] */ EventRegistrationToken token);
        
        HRESULT ( STDMETHODCALLTYPE *add_WebResourceRequested_deprecated )( 
            IWebView2WebView3 * This,
            /* [in] */ LPCWSTR *const urlFilter,
            /* [in] */ WEBVIEW2_WEB_RESOURCE_CONTEXT *const resourceContextFilter,
            /* [in] */ SIZE_T filterLength,
            /* [in] */ IWebView2WebResourceRequestedEventHandler *eventHandler,
            /* [out] */ EventRegistrationToken *token);
        
        HRESULT ( STDMETHODCALLTYPE *remove_WebResourceRequested )( 
            IWebView2WebView3 * This,
            /* [in] */ EventRegistrationToken token);
        
        HRESULT ( STDMETHODCALLTYPE *add_ScriptDialogOpening )( 
            IWebView2WebView3 * This,
            /* [in] */ IWebView2ScriptDialogOpeningEventHandler *eventHandler,
            /* [out] */ EventRegistrationToken *token);
        
        HRESULT ( STDMETHODCALLTYPE *remove_ScriptDialogOpening )( 
            IWebView2WebView3 * This,
            /* [in] */ EventRegistrationToken token);
        
        HRESULT ( STDMETHODCALLTYPE *add_ZoomFactorChanged )( 
            IWebView2WebView3 * This,
            /* [in] */ IWebView2ZoomFactorChangedEventHandler *eventHandler,
            /* [out] */ EventRegistrationToken *token);
        
        HRESULT ( STDMETHODCALLTYPE *remove_ZoomFactorChanged )( 
            IWebView2WebView3 * This,
            /* [in] */ EventRegistrationToken token);
        
        HRESULT ( STDMETHODCALLTYPE *add_PermissionRequested )( 
            IWebView2WebView3 * This,
            /* [in] */ IWebView2PermissionRequestedEventHandler *eventHandler,
            /* [out] */ EventRegistrationToken *token);
        
        HRESULT ( STDMETHODCALLTYPE *remove_PermissionRequested )( 
            IWebView2WebView3 * This,
            /* [in] */ EventRegistrationToken token);
        
        HRESULT ( STDMETHODCALLTYPE *add_ProcessFailed )( 
            IWebView2WebView3 * This,
            /* [in] */ IWebView2ProcessFailedEventHandler *eventHandler,
            /* [out] */ EventRegistrationToken *token);
        
        HRESULT ( STDMETHODCALLTYPE *remove_ProcessFailed )( 
            IWebView2WebView3 * This,
            /* [in] */ EventRegistrationToken token);
        
        HRESULT ( STDMETHODCALLTYPE *AddScriptToExecuteOnDocumentCreated )( 
            IWebView2WebView3 * This,
            /* [in] */ LPCWSTR javaScript,
            /* [in] */ IWebView2AddScriptToExecuteOnDocumentCreatedCompletedHandler *handler);
        
        HRESULT ( STDMETHODCALLTYPE *RemoveScriptToExecuteOnDocumentCreated )( 
            IWebView2WebView3 * This,
            /* [in] */ LPCWSTR id);
        
        HRESULT ( STDMETHODCALLTYPE *ExecuteScript )( 
            IWebView2WebView3 * This,
            /* [in] */ LPCWSTR javaScript,
            /* [in] */ IWebView2ExecuteScriptCompletedHandler *handler);
        
        HRESULT ( STDMETHODCALLTYPE *CapturePreview )( 
            IWebView2WebView3 * This,
            /* [in] */ WEBVIEW2_CAPTURE_PREVIEW_IMAGE_FORMAT imageFormat,
            /* [in] */ IStream *imageStream,
            /* [in] */ IWebView2CapturePreviewCompletedHandler *handler);
        
        HRESULT ( STDMETHODCALLTYPE *Reload )( 
            IWebView2WebView3 * This);
        
        HRESULT ( STDMETHODCALLTYPE *get_Bounds )( 
            IWebView2WebView3 * This,
            /* [retval][out] */ RECT *bounds);
        
        HRESULT ( STDMETHODCALLTYPE *put_Bounds )( 
            IWebView2WebView3 * This,
            /* [in] */ RECT bounds);
        
        HRESULT ( STDMETHODCALLTYPE *get_ZoomFactor )( 
            IWebView2WebView3 * This,
            /* [retval][out] */ double *zoomFactor);
        
        HRESULT ( STDMETHODCALLTYPE *put_ZoomFactor )( 
            IWebView2WebView3 * This,
            /* [in] */ double zoomFactor);
        
        HRESULT ( STDMETHODCALLTYPE *get_IsVisible )( 
            IWebView2WebView3 * This,
            /* [retval][out] */ BOOL *isVisible);
        
        HRESULT ( STDMETHODCALLTYPE *put_IsVisible )( 
            IWebView2WebView3 * This,
            /* [in] */ BOOL isVisible);
        
        HRESULT ( STDMETHODCALLTYPE *PostWebMessageAsJson )( 
            IWebView2WebView3 * This,
            /* [in] */ LPCWSTR webMessageAsJson);
        
        HRESULT ( STDMETHODCALLTYPE *PostWebMessageAsString )( 
            IWebView2WebView3 * This,
            /* [in] */ LPCWSTR webMessageAsString);
        
        HRESULT ( STDMETHODCALLTYPE *add_WebMessageReceived )( 
            IWebView2WebView3 * This,
            /* [in] */ IWebView2WebMessageReceivedEventHandler *handler,
            /* [out] */ EventRegistrationToken *token);
        
        HRESULT ( STDMETHODCALLTYPE *remove_WebMessageReceived )( 
            IWebView2WebView3 * This,
            /* [in] */ EventRegistrationToken token);
        
        HRESULT ( STDMETHODCALLTYPE *Close )( 
            IWebView2WebView3 * This);
        
        HRESULT ( STDMETHODCALLTYPE *CallDevToolsProtocolMethod )( 
            IWebView2WebView3 * This,
            /* [in] */ LPCWSTR methodName,
            /* [in] */ LPCWSTR parametersAsJson,
            /* [in] */ IWebView2CallDevToolsProtocolMethodCompletedHandler *handler);
        
        HRESULT ( STDMETHODCALLTYPE *add_DevToolsProtocolEventReceived )( 
            IWebView2WebView3 * This,
            /* [in] */ LPCWSTR eventName,
            /* [in] */ IWebView2DevToolsProtocolEventReceivedEventHandler *handler,
            /* [out] */ EventRegistrationToken *token);
        
        HRESULT ( STDMETHODCALLTYPE *remove_DevToolsProtocolEventReceived )( 
            IWebView2WebView3 * This,
            /* [in] */ LPCWSTR eventName,
            /* [in] */ EventRegistrationToken token);
        
        HRESULT ( STDMETHODCALLTYPE *get_BrowserProcessId )( 
            IWebView2WebView3 * This,
            /* [retval][out] */ UINT32 *value);
        
        HRESULT ( STDMETHODCALLTYPE *get_CanGoBack )( 
            IWebView2WebView3 * This,
            /* [retval][out] */ BOOL *canGoBack);
        
        HRESULT ( STDMETHODCALLTYPE *get_CanGoForward )( 
            IWebView2WebView3 * This,
            /* [retval][out] */ BOOL *canGoForward);
        
        HRESULT ( STDMETHODCALLTYPE *GoBack )( 
            IWebView2WebView3 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GoForward )( 
            IWebView2WebView3 * This);
        
        HRESULT ( STDMETHODCALLTYPE *Stop )( 
            IWebView2WebView3 * This);
        
        HRESULT ( STDMETHODCALLTYPE *add_NewWindowRequested )( 
            IWebView2WebView3 * This,
            /* [in] */ IWebView2NewWindowRequestedEventHandler *eventHandler,
            /* [out] */ EventRegistrationToken *token);
        
        HRESULT ( STDMETHODCALLTYPE *remove_NewWindowRequested )( 
            IWebView2WebView3 * This,
            /* [in] */ EventRegistrationToken token);
        
        HRESULT ( STDMETHODCALLTYPE *add_DocumentTitleChanged )( 
            IWebView2WebView3 * This,
            /* [in] */ IWebView2DocumentTitleChangedEventHandler *eventHandler,
            /* [out] */ EventRegistrationToken *token);
        
        HRESULT ( STDMETHODCALLTYPE *remove_DocumentTitleChanged )( 
            IWebView2WebView3 * This,
            /* [in] */ EventRegistrationToken token);
        
        HRESULT ( STDMETHODCALLTYPE *get_DocumentTitle )( 
            IWebView2WebView3 * This,
            /* [out] */ LPWSTR *title);
        
        END_INTERFACE
    } IWebView2WebView3Vtbl;

    interface IWebView2WebView3
    {
        CONST_VTBL struct IWebView2WebView3Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWebView2WebView3_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IWebView2WebView3_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IWebView2WebView3_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IWebView2WebView3_get_Settings(This,settings)	\
    ( (This)->lpVtbl -> get_Settings(This,settings) ) 

#define IWebView2WebView3_get_Source(This,uri)	\
    ( (This)->lpVtbl -> get_Source(This,uri) ) 

#define IWebView2WebView3_Navigate(This,uri)	\
    ( (This)->lpVtbl -> Navigate(This,uri) ) 

#define IWebView2WebView3_MoveFocus(This,reason)	\
    ( (This)->lpVtbl -> MoveFocus(This,reason) ) 

#define IWebView2WebView3_NavigateToString(This,htmlContent)	\
    ( (This)->lpVtbl -> NavigateToString(This,htmlContent) ) 

#define IWebView2WebView3_add_NavigationStarting(This,eventHandler,token)	\
    ( (This)->lpVtbl -> add_NavigationStarting(This,eventHandler,token) ) 

#define IWebView2WebView3_remove_NavigationStarting(This,token)	\
    ( (This)->lpVtbl -> remove_NavigationStarting(This,token) ) 

#define IWebView2WebView3_add_DocumentStateChanged(This,eventHandler,token)	\
    ( (This)->lpVtbl -> add_DocumentStateChanged(This,eventHandler,token) ) 

#define IWebView2WebView3_remove_DocumentStateChanged(This,token)	\
    ( (This)->lpVtbl -> remove_DocumentStateChanged(This,token) ) 

#define IWebView2WebView3_add_NavigationCompleted(This,eventHandler,token)	\
    ( (This)->lpVtbl -> add_NavigationCompleted(This,eventHandler,token) ) 

#define IWebView2WebView3_remove_NavigationCompleted(This,token)	\
    ( (This)->lpVtbl -> remove_NavigationCompleted(This,token) ) 

#define IWebView2WebView3_add_FrameNavigationStarting(This,eventHandler,token)	\
    ( (This)->lpVtbl -> add_FrameNavigationStarting(This,eventHandler,token) ) 

#define IWebView2WebView3_remove_FrameNavigationStarting(This,token)	\
    ( (This)->lpVtbl -> remove_FrameNavigationStarting(This,token) ) 

#define IWebView2WebView3_add_MoveFocusRequested(This,eventHandler,token)	\
    ( (This)->lpVtbl -> add_MoveFocusRequested(This,eventHandler,token) ) 

#define IWebView2WebView3_remove_MoveFocusRequested(This,token)	\
    ( (This)->lpVtbl -> remove_MoveFocusRequested(This,token) ) 

#define IWebView2WebView3_add_GotFocus(This,eventHandler,token)	\
    ( (This)->lpVtbl -> add_GotFocus(This,eventHandler,token) ) 

#define IWebView2WebView3_remove_GotFocus(This,token)	\
    ( (This)->lpVtbl -> remove_GotFocus(This,token) ) 

#define IWebView2WebView3_add_LostFocus(This,eventHandler,token)	\
    ( (This)->lpVtbl -> add_LostFocus(This,eventHandler,token) ) 

#define IWebView2WebView3_remove_LostFocus(This,token)	\
    ( (This)->lpVtbl -> remove_LostFocus(This,token) ) 

#define IWebView2WebView3_add_WebResourceRequested_deprecated(This,urlFilter,resourceContextFilter,filterLength,eventHandler,token)	\
    ( (This)->lpVtbl -> add_WebResourceRequested_deprecated(This,urlFilter,resourceContextFilter,filterLength,eventHandler,token) ) 

#define IWebView2WebView3_remove_WebResourceRequested(This,token)	\
    ( (This)->lpVtbl -> remove_WebResourceRequested(This,token) ) 

#define IWebView2WebView3_add_ScriptDialogOpening(This,eventHandler,token)	\
    ( (This)->lpVtbl -> add_ScriptDialogOpening(This,eventHandler,token) ) 

#define IWebView2WebView3_remove_ScriptDialogOpening(This,token)	\
    ( (This)->lpVtbl -> remove_ScriptDialogOpening(This,token) ) 

#define IWebView2WebView3_add_ZoomFactorChanged(This,eventHandler,token)	\
    ( (This)->lpVtbl -> add_ZoomFactorChanged(This,eventHandler,token) ) 

#define IWebView2WebView3_remove_ZoomFactorChanged(This,token)	\
    ( (This)->lpVtbl -> remove_ZoomFactorChanged(This,token) ) 

#define IWebView2WebView3_add_PermissionRequested(This,eventHandler,token)	\
    ( (This)->lpVtbl -> add_PermissionRequested(This,eventHandler,token) ) 

#define IWebView2WebView3_remove_PermissionRequested(This,token)	\
    ( (This)->lpVtbl -> remove_PermissionRequested(This,token) ) 

#define IWebView2WebView3_add_ProcessFailed(This,eventHandler,token)	\
    ( (This)->lpVtbl -> add_ProcessFailed(This,eventHandler,token) ) 

#define IWebView2WebView3_remove_ProcessFailed(This,token)	\
    ( (This)->lpVtbl -> remove_ProcessFailed(This,token) ) 

#define IWebView2WebView3_AddScriptToExecuteOnDocumentCreated(This,javaScript,handler)	\
    ( (This)->lpVtbl -> AddScriptToExecuteOnDocumentCreated(This,javaScript,handler) ) 

#define IWebView2WebView3_RemoveScriptToExecuteOnDocumentCreated(This,id)	\
    ( (This)->lpVtbl -> RemoveScriptToExecuteOnDocumentCreated(This,id) ) 

#define IWebView2WebView3_ExecuteScript(This,javaScript,handler)	\
    ( (This)->lpVtbl -> ExecuteScript(This,javaScript,handler) ) 

#define IWebView2WebView3_CapturePreview(This,imageFormat,imageStream,handler)	\
    ( (This)->lpVtbl -> CapturePreview(This,imageFormat,imageStream,handler) ) 

#define IWebView2WebView3_Reload(This)	\
    ( (This)->lpVtbl -> Reload(This) ) 

#define IWebView2WebView3_get_Bounds(This,bounds)	\
    ( (This)->lpVtbl -> get_Bounds(This,bounds) ) 

#define IWebView2WebView3_put_Bounds(This,bounds)	\
    ( (This)->lpVtbl -> put_Bounds(This,bounds) ) 

#define IWebView2WebView3_get_ZoomFactor(This,zoomFactor)	\
    ( (This)->lpVtbl -> get_ZoomFactor(This,zoomFactor) ) 

#define IWebView2WebView3_put_ZoomFactor(This,zoomFactor)	\
    ( (This)->lpVtbl -> put_ZoomFactor(This,zoomFactor) ) 

#define IWebView2WebView3_get_IsVisible(This,isVisible)	\
    ( (This)->lpVtbl -> get_IsVisible(This,isVisible) ) 

#define IWebView2WebView3_put_IsVisible(This,isVisible)	\
    ( (This)->lpVtbl -> put_IsVisible(This,isVisible) ) 

#define IWebView2WebView3_PostWebMessageAsJson(This,webMessageAsJson)	\
    ( (This)->lpVtbl -> PostWebMessageAsJson(This,webMessageAsJson) ) 

#define IWebView2WebView3_PostWebMessageAsString(This,webMessageAsString)	\
    ( (This)->lpVtbl -> PostWebMessageAsString(This,webMessageAsString) ) 

#define IWebView2WebView3_add_WebMessageReceived(This,handler,token)	\
    ( (This)->lpVtbl -> add_WebMessageReceived(This,handler,token) ) 

#define IWebView2WebView3_remove_WebMessageReceived(This,token)	\
    ( (This)->lpVtbl -> remove_WebMessageReceived(This,token) ) 

#define IWebView2WebView3_Close(This)	\
    ( (This)->lpVtbl -> Close(This) ) 

#define IWebView2WebView3_CallDevToolsProtocolMethod(This,methodName,parametersAsJson,handler)	\
    ( (This)->lpVtbl -> CallDevToolsProtocolMethod(This,methodName,parametersAsJson,handler) ) 

#define IWebView2WebView3_add_DevToolsProtocolEventReceived(This,eventName,handler,token)	\
    ( (This)->lpVtbl -> add_DevToolsProtocolEventReceived(This,eventName,handler,token) ) 

#define IWebView2WebView3_remove_DevToolsProtocolEventReceived(This,eventName,token)	\
    ( (This)->lpVtbl -> remove_DevToolsProtocolEventReceived(This,eventName,token) ) 

#define IWebView2WebView3_get_BrowserProcessId(This,value)	\
    ( (This)->lpVtbl -> get_BrowserProcessId(This,value) ) 

#define IWebView2WebView3_get_CanGoBack(This,canGoBack)	\
    ( (This)->lpVtbl -> get_CanGoBack(This,canGoBack) ) 

#define IWebView2WebView3_get_CanGoForward(This,canGoForward)	\
    ( (This)->lpVtbl -> get_CanGoForward(This,canGoForward) ) 

#define IWebView2WebView3_GoBack(This)	\
    ( (This)->lpVtbl -> GoBack(This) ) 

#define IWebView2WebView3_GoForward(This)	\
    ( (This)->lpVtbl -> GoForward(This) ) 


#define IWebView2WebView3_Stop(This)	\
    ( (This)->lpVtbl -> Stop(This) ) 

#define IWebView2WebView3_add_NewWindowRequested(This,eventHandler,token)	\
    ( (This)->lpVtbl -> add_NewWindowRequested(This,eventHandler,token) ) 

#define IWebView2WebView3_remove_NewWindowRequested(This,token)	\
    ( (This)->lpVtbl -> remove_NewWindowRequested(This,token) ) 

#define IWebView2WebView3_add_DocumentTitleChanged(This,eventHandler,token)	\
    ( (This)->lpVtbl -> add_DocumentTitleChanged(This,eventHandler,token) ) 

#define IWebView2WebView3_remove_DocumentTitleChanged(This,token)	\
    ( (This)->lpVtbl -> remove_DocumentTitleChanged(This,token) ) 

#define IWebView2WebView3_get_DocumentTitle(This,title)	\
    ( (This)->lpVtbl -> get_DocumentTitle(This,title) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IWebView2WebView3_INTERFACE_DEFINED__ */


#ifndef __IWebView2WebView4_INTERFACE_DEFINED__
#define __IWebView2WebView4_INTERFACE_DEFINED__

/* interface IWebView2WebView4 */
/* [unique][object][uuid] */ 

typedef /* [v1_enum] */ 
enum WEBVIEW2_KEY_EVENT_TYPE
    {
        WEBVIEW2_KEY_EVENT_TYPE_KEY_DOWN	= 0,
        WEBVIEW2_KEY_EVENT_TYPE_KEY_UP	= ( WEBVIEW2_KEY_EVENT_TYPE_KEY_DOWN + 1 ) ,
        WEBVIEW2_KEY_EVENT_TYPE_SYSTEM_KEY_DOWN	= ( WEBVIEW2_KEY_EVENT_TYPE_KEY_UP + 1 ) ,
        WEBVIEW2_KEY_EVENT_TYPE_SYSTEM_KEY_UP	= ( WEBVIEW2_KEY_EVENT_TYPE_SYSTEM_KEY_DOWN + 1 ) 
    } 	WEBVIEW2_KEY_EVENT_TYPE;

typedef struct WEBVIEW2_PHYSICAL_KEY_STATUS
    {
    UINT32 RepeatCount;
    UINT32 ScanCode;
    BOOL IsExtendedKey;
    BOOL IsMenuKeyDown;
    BOOL WasKeyDown;
    BOOL IsKeyReleased;
    } 	WEBVIEW2_PHYSICAL_KEY_STATUS;


EXTERN_C const IID IID_IWebView2WebView4;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("E8B04ED2-FA61-4CBB-B303-786088177555")
    IWebView2WebView4 : public IWebView2WebView3
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE AddRemoteObject( 
            /* [in] */ LPCWSTR name,
            /* [in] */ VARIANT *object) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RemoveRemoteObject( 
            /* [in] */ LPCWSTR name) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OpenDevToolsWindow( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE add_AcceleratorKeyPressed( 
            /* [in] */ IWebView2AcceleratorKeyPressedEventHandler *eventHandler,
            /* [out] */ EventRegistrationToken *token) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE remove_AcceleratorKeyPressed( 
            /* [in] */ EventRegistrationToken token) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IWebView2WebView4Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IWebView2WebView4 * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IWebView2WebView4 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IWebView2WebView4 * This);
        
        HRESULT ( STDMETHODCALLTYPE *get_Settings )( 
            IWebView2WebView4 * This,
            /* [retval][out] */ IWebView2Settings **settings);
        
        HRESULT ( STDMETHODCALLTYPE *get_Source )( 
            IWebView2WebView4 * This,
            /* [retval][out] */ LPWSTR *uri);
        
        HRESULT ( STDMETHODCALLTYPE *Navigate )( 
            IWebView2WebView4 * This,
            /* [in] */ LPCWSTR uri);
        
        HRESULT ( STDMETHODCALLTYPE *MoveFocus )( 
            IWebView2WebView4 * This,
            /* [in] */ WEBVIEW2_MOVE_FOCUS_REASON reason);
        
        HRESULT ( STDMETHODCALLTYPE *NavigateToString )( 
            IWebView2WebView4 * This,
            /* [in] */ LPCWSTR htmlContent);
        
        HRESULT ( STDMETHODCALLTYPE *add_NavigationStarting )( 
            IWebView2WebView4 * This,
            /* [in] */ IWebView2NavigationStartingEventHandler *eventHandler,
            /* [out] */ EventRegistrationToken *token);
        
        HRESULT ( STDMETHODCALLTYPE *remove_NavigationStarting )( 
            IWebView2WebView4 * This,
            /* [in] */ EventRegistrationToken token);
        
        HRESULT ( STDMETHODCALLTYPE *add_DocumentStateChanged )( 
            IWebView2WebView4 * This,
            /* [in] */ IWebView2DocumentStateChangedEventHandler *eventHandler,
            /* [out] */ EventRegistrationToken *token);
        
        HRESULT ( STDMETHODCALLTYPE *remove_DocumentStateChanged )( 
            IWebView2WebView4 * This,
            /* [in] */ EventRegistrationToken token);
        
        HRESULT ( STDMETHODCALLTYPE *add_NavigationCompleted )( 
            IWebView2WebView4 * This,
            /* [in] */ IWebView2NavigationCompletedEventHandler *eventHandler,
            /* [out] */ EventRegistrationToken *token);
        
        HRESULT ( STDMETHODCALLTYPE *remove_NavigationCompleted )( 
            IWebView2WebView4 * This,
            /* [in] */ EventRegistrationToken token);
        
        HRESULT ( STDMETHODCALLTYPE *add_FrameNavigationStarting )( 
            IWebView2WebView4 * This,
            /* [in] */ IWebView2NavigationStartingEventHandler *eventHandler,
            /* [out] */ EventRegistrationToken *token);
        
        HRESULT ( STDMETHODCALLTYPE *remove_FrameNavigationStarting )( 
            IWebView2WebView4 * This,
            /* [in] */ EventRegistrationToken token);
        
        HRESULT ( STDMETHODCALLTYPE *add_MoveFocusRequested )( 
            IWebView2WebView4 * This,
            /* [in] */ IWebView2MoveFocusRequestedEventHandler *eventHandler,
            /* [out] */ EventRegistrationToken *token);
        
        HRESULT ( STDMETHODCALLTYPE *remove_MoveFocusRequested )( 
            IWebView2WebView4 * This,
            /* [in] */ EventRegistrationToken token);
        
        HRESULT ( STDMETHODCALLTYPE *add_GotFocus )( 
            IWebView2WebView4 * This,
            /* [in] */ IWebView2FocusChangedEventHandler *eventHandler,
            /* [out] */ EventRegistrationToken *token);
        
        HRESULT ( STDMETHODCALLTYPE *remove_GotFocus )( 
            IWebView2WebView4 * This,
            /* [in] */ EventRegistrationToken token);
        
        HRESULT ( STDMETHODCALLTYPE *add_LostFocus )( 
            IWebView2WebView4 * This,
            /* [in] */ IWebView2FocusChangedEventHandler *eventHandler,
            /* [out] */ EventRegistrationToken *token);
        
        HRESULT ( STDMETHODCALLTYPE *remove_LostFocus )( 
            IWebView2WebView4 * This,
            /* [in] */ EventRegistrationToken token);
        
        HRESULT ( STDMETHODCALLTYPE *add_WebResourceRequested_deprecated )( 
            IWebView2WebView4 * This,
            /* [in] */ LPCWSTR *const urlFilter,
            /* [in] */ WEBVIEW2_WEB_RESOURCE_CONTEXT *const resourceContextFilter,
            /* [in] */ SIZE_T filterLength,
            /* [in] */ IWebView2WebResourceRequestedEventHandler *eventHandler,
            /* [out] */ EventRegistrationToken *token);
        
        HRESULT ( STDMETHODCALLTYPE *remove_WebResourceRequested )( 
            IWebView2WebView4 * This,
            /* [in] */ EventRegistrationToken token);
        
        HRESULT ( STDMETHODCALLTYPE *add_ScriptDialogOpening )( 
            IWebView2WebView4 * This,
            /* [in] */ IWebView2ScriptDialogOpeningEventHandler *eventHandler,
            /* [out] */ EventRegistrationToken *token);
        
        HRESULT ( STDMETHODCALLTYPE *remove_ScriptDialogOpening )( 
            IWebView2WebView4 * This,
            /* [in] */ EventRegistrationToken token);
        
        HRESULT ( STDMETHODCALLTYPE *add_ZoomFactorChanged )( 
            IWebView2WebView4 * This,
            /* [in] */ IWebView2ZoomFactorChangedEventHandler *eventHandler,
            /* [out] */ EventRegistrationToken *token);
        
        HRESULT ( STDMETHODCALLTYPE *remove_ZoomFactorChanged )( 
            IWebView2WebView4 * This,
            /* [in] */ EventRegistrationToken token);
        
        HRESULT ( STDMETHODCALLTYPE *add_PermissionRequested )( 
            IWebView2WebView4 * This,
            /* [in] */ IWebView2PermissionRequestedEventHandler *eventHandler,
            /* [out] */ EventRegistrationToken *token);
        
        HRESULT ( STDMETHODCALLTYPE *remove_PermissionRequested )( 
            IWebView2WebView4 * This,
            /* [in] */ EventRegistrationToken token);
        
        HRESULT ( STDMETHODCALLTYPE *add_ProcessFailed )( 
            IWebView2WebView4 * This,
            /* [in] */ IWebView2ProcessFailedEventHandler *eventHandler,
            /* [out] */ EventRegistrationToken *token);
        
        HRESULT ( STDMETHODCALLTYPE *remove_ProcessFailed )( 
            IWebView2WebView4 * This,
            /* [in] */ EventRegistrationToken token);
        
        HRESULT ( STDMETHODCALLTYPE *AddScriptToExecuteOnDocumentCreated )( 
            IWebView2WebView4 * This,
            /* [in] */ LPCWSTR javaScript,
            /* [in] */ IWebView2AddScriptToExecuteOnDocumentCreatedCompletedHandler *handler);
        
        HRESULT ( STDMETHODCALLTYPE *RemoveScriptToExecuteOnDocumentCreated )( 
            IWebView2WebView4 * This,
            /* [in] */ LPCWSTR id);
        
        HRESULT ( STDMETHODCALLTYPE *ExecuteScript )( 
            IWebView2WebView4 * This,
            /* [in] */ LPCWSTR javaScript,
            /* [in] */ IWebView2ExecuteScriptCompletedHandler *handler);
        
        HRESULT ( STDMETHODCALLTYPE *CapturePreview )( 
            IWebView2WebView4 * This,
            /* [in] */ WEBVIEW2_CAPTURE_PREVIEW_IMAGE_FORMAT imageFormat,
            /* [in] */ IStream *imageStream,
            /* [in] */ IWebView2CapturePreviewCompletedHandler *handler);
        
        HRESULT ( STDMETHODCALLTYPE *Reload )( 
            IWebView2WebView4 * This);
        
        HRESULT ( STDMETHODCALLTYPE *get_Bounds )( 
            IWebView2WebView4 * This,
            /* [retval][out] */ RECT *bounds);
        
        HRESULT ( STDMETHODCALLTYPE *put_Bounds )( 
            IWebView2WebView4 * This,
            /* [in] */ RECT bounds);
        
        HRESULT ( STDMETHODCALLTYPE *get_ZoomFactor )( 
            IWebView2WebView4 * This,
            /* [retval][out] */ double *zoomFactor);
        
        HRESULT ( STDMETHODCALLTYPE *put_ZoomFactor )( 
            IWebView2WebView4 * This,
            /* [in] */ double zoomFactor);
        
        HRESULT ( STDMETHODCALLTYPE *get_IsVisible )( 
            IWebView2WebView4 * This,
            /* [retval][out] */ BOOL *isVisible);
        
        HRESULT ( STDMETHODCALLTYPE *put_IsVisible )( 
            IWebView2WebView4 * This,
            /* [in] */ BOOL isVisible);
        
        HRESULT ( STDMETHODCALLTYPE *PostWebMessageAsJson )( 
            IWebView2WebView4 * This,
            /* [in] */ LPCWSTR webMessageAsJson);
        
        HRESULT ( STDMETHODCALLTYPE *PostWebMessageAsString )( 
            IWebView2WebView4 * This,
            /* [in] */ LPCWSTR webMessageAsString);
        
        HRESULT ( STDMETHODCALLTYPE *add_WebMessageReceived )( 
            IWebView2WebView4 * This,
            /* [in] */ IWebView2WebMessageReceivedEventHandler *handler,
            /* [out] */ EventRegistrationToken *token);
        
        HRESULT ( STDMETHODCALLTYPE *remove_WebMessageReceived )( 
            IWebView2WebView4 * This,
            /* [in] */ EventRegistrationToken token);
        
        HRESULT ( STDMETHODCALLTYPE *Close )( 
            IWebView2WebView4 * This);
        
        HRESULT ( STDMETHODCALLTYPE *CallDevToolsProtocolMethod )( 
            IWebView2WebView4 * This,
            /* [in] */ LPCWSTR methodName,
            /* [in] */ LPCWSTR parametersAsJson,
            /* [in] */ IWebView2CallDevToolsProtocolMethodCompletedHandler *handler);
        
        HRESULT ( STDMETHODCALLTYPE *add_DevToolsProtocolEventReceived )( 
            IWebView2WebView4 * This,
            /* [in] */ LPCWSTR eventName,
            /* [in] */ IWebView2DevToolsProtocolEventReceivedEventHandler *handler,
            /* [out] */ EventRegistrationToken *token);
        
        HRESULT ( STDMETHODCALLTYPE *remove_DevToolsProtocolEventReceived )( 
            IWebView2WebView4 * This,
            /* [in] */ LPCWSTR eventName,
            /* [in] */ EventRegistrationToken token);
        
        HRESULT ( STDMETHODCALLTYPE *get_BrowserProcessId )( 
            IWebView2WebView4 * This,
            /* [retval][out] */ UINT32 *value);
        
        HRESULT ( STDMETHODCALLTYPE *get_CanGoBack )( 
            IWebView2WebView4 * This,
            /* [retval][out] */ BOOL *canGoBack);
        
        HRESULT ( STDMETHODCALLTYPE *get_CanGoForward )( 
            IWebView2WebView4 * This,
            /* [retval][out] */ BOOL *canGoForward);
        
        HRESULT ( STDMETHODCALLTYPE *GoBack )( 
            IWebView2WebView4 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GoForward )( 
            IWebView2WebView4 * This);
        
        HRESULT ( STDMETHODCALLTYPE *Stop )( 
            IWebView2WebView4 * This);
        
        HRESULT ( STDMETHODCALLTYPE *add_NewWindowRequested )( 
            IWebView2WebView4 * This,
            /* [in] */ IWebView2NewWindowRequestedEventHandler *eventHandler,
            /* [out] */ EventRegistrationToken *token);
        
        HRESULT ( STDMETHODCALLTYPE *remove_NewWindowRequested )( 
            IWebView2WebView4 * This,
            /* [in] */ EventRegistrationToken token);
        
        HRESULT ( STDMETHODCALLTYPE *add_DocumentTitleChanged )( 
            IWebView2WebView4 * This,
            /* [in] */ IWebView2DocumentTitleChangedEventHandler *eventHandler,
            /* [out] */ EventRegistrationToken *token);
        
        HRESULT ( STDMETHODCALLTYPE *remove_DocumentTitleChanged )( 
            IWebView2WebView4 * This,
            /* [in] */ EventRegistrationToken token);
        
        HRESULT ( STDMETHODCALLTYPE *get_DocumentTitle )( 
            IWebView2WebView4 * This,
            /* [out] */ LPWSTR *title);
        
        HRESULT ( STDMETHODCALLTYPE *AddRemoteObject )( 
            IWebView2WebView4 * This,
            /* [in] */ LPCWSTR name,
            /* [in] */ VARIANT *object);
        
        HRESULT ( STDMETHODCALLTYPE *RemoveRemoteObject )( 
            IWebView2WebView4 * This,
            /* [in] */ LPCWSTR name);
        
        HRESULT ( STDMETHODCALLTYPE *OpenDevToolsWindow )( 
            IWebView2WebView4 * This);
        
        HRESULT ( STDMETHODCALLTYPE *add_AcceleratorKeyPressed )( 
            IWebView2WebView4 * This,
            /* [in] */ IWebView2AcceleratorKeyPressedEventHandler *eventHandler,
            /* [out] */ EventRegistrationToken *token);
        
        HRESULT ( STDMETHODCALLTYPE *remove_AcceleratorKeyPressed )( 
            IWebView2WebView4 * This,
            /* [in] */ EventRegistrationToken token);
        
        END_INTERFACE
    } IWebView2WebView4Vtbl;

    interface IWebView2WebView4
    {
        CONST_VTBL struct IWebView2WebView4Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWebView2WebView4_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IWebView2WebView4_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IWebView2WebView4_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IWebView2WebView4_get_Settings(This,settings)	\
    ( (This)->lpVtbl -> get_Settings(This,settings) ) 

#define IWebView2WebView4_get_Source(This,uri)	\
    ( (This)->lpVtbl -> get_Source(This,uri) ) 

#define IWebView2WebView4_Navigate(This,uri)	\
    ( (This)->lpVtbl -> Navigate(This,uri) ) 

#define IWebView2WebView4_MoveFocus(This,reason)	\
    ( (This)->lpVtbl -> MoveFocus(This,reason) ) 

#define IWebView2WebView4_NavigateToString(This,htmlContent)	\
    ( (This)->lpVtbl -> NavigateToString(This,htmlContent) ) 

#define IWebView2WebView4_add_NavigationStarting(This,eventHandler,token)	\
    ( (This)->lpVtbl -> add_NavigationStarting(This,eventHandler,token) ) 

#define IWebView2WebView4_remove_NavigationStarting(This,token)	\
    ( (This)->lpVtbl -> remove_NavigationStarting(This,token) ) 

#define IWebView2WebView4_add_DocumentStateChanged(This,eventHandler,token)	\
    ( (This)->lpVtbl -> add_DocumentStateChanged(This,eventHandler,token) ) 

#define IWebView2WebView4_remove_DocumentStateChanged(This,token)	\
    ( (This)->lpVtbl -> remove_DocumentStateChanged(This,token) ) 

#define IWebView2WebView4_add_NavigationCompleted(This,eventHandler,token)	\
    ( (This)->lpVtbl -> add_NavigationCompleted(This,eventHandler,token) ) 

#define IWebView2WebView4_remove_NavigationCompleted(This,token)	\
    ( (This)->lpVtbl -> remove_NavigationCompleted(This,token) ) 

#define IWebView2WebView4_add_FrameNavigationStarting(This,eventHandler,token)	\
    ( (This)->lpVtbl -> add_FrameNavigationStarting(This,eventHandler,token) ) 

#define IWebView2WebView4_remove_FrameNavigationStarting(This,token)	\
    ( (This)->lpVtbl -> remove_FrameNavigationStarting(This,token) ) 

#define IWebView2WebView4_add_MoveFocusRequested(This,eventHandler,token)	\
    ( (This)->lpVtbl -> add_MoveFocusRequested(This,eventHandler,token) ) 

#define IWebView2WebView4_remove_MoveFocusRequested(This,token)	\
    ( (This)->lpVtbl -> remove_MoveFocusRequested(This,token) ) 

#define IWebView2WebView4_add_GotFocus(This,eventHandler,token)	\
    ( (This)->lpVtbl -> add_GotFocus(This,eventHandler,token) ) 

#define IWebView2WebView4_remove_GotFocus(This,token)	\
    ( (This)->lpVtbl -> remove_GotFocus(This,token) ) 

#define IWebView2WebView4_add_LostFocus(This,eventHandler,token)	\
    ( (This)->lpVtbl -> add_LostFocus(This,eventHandler,token) ) 

#define IWebView2WebView4_remove_LostFocus(This,token)	\
    ( (This)->lpVtbl -> remove_LostFocus(This,token) ) 

#define IWebView2WebView4_add_WebResourceRequested_deprecated(This,urlFilter,resourceContextFilter,filterLength,eventHandler,token)	\
    ( (This)->lpVtbl -> add_WebResourceRequested_deprecated(This,urlFilter,resourceContextFilter,filterLength,eventHandler,token) ) 

#define IWebView2WebView4_remove_WebResourceRequested(This,token)	\
    ( (This)->lpVtbl -> remove_WebResourceRequested(This,token) ) 

#define IWebView2WebView4_add_ScriptDialogOpening(This,eventHandler,token)	\
    ( (This)->lpVtbl -> add_ScriptDialogOpening(This,eventHandler,token) ) 

#define IWebView2WebView4_remove_ScriptDialogOpening(This,token)	\
    ( (This)->lpVtbl -> remove_ScriptDialogOpening(This,token) ) 

#define IWebView2WebView4_add_ZoomFactorChanged(This,eventHandler,token)	\
    ( (This)->lpVtbl -> add_ZoomFactorChanged(This,eventHandler,token) ) 

#define IWebView2WebView4_remove_ZoomFactorChanged(This,token)	\
    ( (This)->lpVtbl -> remove_ZoomFactorChanged(This,token) ) 

#define IWebView2WebView4_add_PermissionRequested(This,eventHandler,token)	\
    ( (This)->lpVtbl -> add_PermissionRequested(This,eventHandler,token) ) 

#define IWebView2WebView4_remove_PermissionRequested(This,token)	\
    ( (This)->lpVtbl -> remove_PermissionRequested(This,token) ) 

#define IWebView2WebView4_add_ProcessFailed(This,eventHandler,token)	\
    ( (This)->lpVtbl -> add_ProcessFailed(This,eventHandler,token) ) 

#define IWebView2WebView4_remove_ProcessFailed(This,token)	\
    ( (This)->lpVtbl -> remove_ProcessFailed(This,token) ) 

#define IWebView2WebView4_AddScriptToExecuteOnDocumentCreated(This,javaScript,handler)	\
    ( (This)->lpVtbl -> AddScriptToExecuteOnDocumentCreated(This,javaScript,handler) ) 

#define IWebView2WebView4_RemoveScriptToExecuteOnDocumentCreated(This,id)	\
    ( (This)->lpVtbl -> RemoveScriptToExecuteOnDocumentCreated(This,id) ) 

#define IWebView2WebView4_ExecuteScript(This,javaScript,handler)	\
    ( (This)->lpVtbl -> ExecuteScript(This,javaScript,handler) ) 

#define IWebView2WebView4_CapturePreview(This,imageFormat,imageStream,handler)	\
    ( (This)->lpVtbl -> CapturePreview(This,imageFormat,imageStream,handler) ) 

#define IWebView2WebView4_Reload(This)	\
    ( (This)->lpVtbl -> Reload(This) ) 

#define IWebView2WebView4_get_Bounds(This,bounds)	\
    ( (This)->lpVtbl -> get_Bounds(This,bounds) ) 

#define IWebView2WebView4_put_Bounds(This,bounds)	\
    ( (This)->lpVtbl -> put_Bounds(This,bounds) ) 

#define IWebView2WebView4_get_ZoomFactor(This,zoomFactor)	\
    ( (This)->lpVtbl -> get_ZoomFactor(This,zoomFactor) ) 

#define IWebView2WebView4_put_ZoomFactor(This,zoomFactor)	\
    ( (This)->lpVtbl -> put_ZoomFactor(This,zoomFactor) ) 

#define IWebView2WebView4_get_IsVisible(This,isVisible)	\
    ( (This)->lpVtbl -> get_IsVisible(This,isVisible) ) 

#define IWebView2WebView4_put_IsVisible(This,isVisible)	\
    ( (This)->lpVtbl -> put_IsVisible(This,isVisible) ) 

#define IWebView2WebView4_PostWebMessageAsJson(This,webMessageAsJson)	\
    ( (This)->lpVtbl -> PostWebMessageAsJson(This,webMessageAsJson) ) 

#define IWebView2WebView4_PostWebMessageAsString(This,webMessageAsString)	\
    ( (This)->lpVtbl -> PostWebMessageAsString(This,webMessageAsString) ) 

#define IWebView2WebView4_add_WebMessageReceived(This,handler,token)	\
    ( (This)->lpVtbl -> add_WebMessageReceived(This,handler,token) ) 

#define IWebView2WebView4_remove_WebMessageReceived(This,token)	\
    ( (This)->lpVtbl -> remove_WebMessageReceived(This,token) ) 

#define IWebView2WebView4_Close(This)	\
    ( (This)->lpVtbl -> Close(This) ) 

#define IWebView2WebView4_CallDevToolsProtocolMethod(This,methodName,parametersAsJson,handler)	\
    ( (This)->lpVtbl -> CallDevToolsProtocolMethod(This,methodName,parametersAsJson,handler) ) 

#define IWebView2WebView4_add_DevToolsProtocolEventReceived(This,eventName,handler,token)	\
    ( (This)->lpVtbl -> add_DevToolsProtocolEventReceived(This,eventName,handler,token) ) 

#define IWebView2WebView4_remove_DevToolsProtocolEventReceived(This,eventName,token)	\
    ( (This)->lpVtbl -> remove_DevToolsProtocolEventReceived(This,eventName,token) ) 

#define IWebView2WebView4_get_BrowserProcessId(This,value)	\
    ( (This)->lpVtbl -> get_BrowserProcessId(This,value) ) 

#define IWebView2WebView4_get_CanGoBack(This,canGoBack)	\
    ( (This)->lpVtbl -> get_CanGoBack(This,canGoBack) ) 

#define IWebView2WebView4_get_CanGoForward(This,canGoForward)	\
    ( (This)->lpVtbl -> get_CanGoForward(This,canGoForward) ) 

#define IWebView2WebView4_GoBack(This)	\
    ( (This)->lpVtbl -> GoBack(This) ) 

#define IWebView2WebView4_GoForward(This)	\
    ( (This)->lpVtbl -> GoForward(This) ) 


#define IWebView2WebView4_Stop(This)	\
    ( (This)->lpVtbl -> Stop(This) ) 

#define IWebView2WebView4_add_NewWindowRequested(This,eventHandler,token)	\
    ( (This)->lpVtbl -> add_NewWindowRequested(This,eventHandler,token) ) 

#define IWebView2WebView4_remove_NewWindowRequested(This,token)	\
    ( (This)->lpVtbl -> remove_NewWindowRequested(This,token) ) 

#define IWebView2WebView4_add_DocumentTitleChanged(This,eventHandler,token)	\
    ( (This)->lpVtbl -> add_DocumentTitleChanged(This,eventHandler,token) ) 

#define IWebView2WebView4_remove_DocumentTitleChanged(This,token)	\
    ( (This)->lpVtbl -> remove_DocumentTitleChanged(This,token) ) 

#define IWebView2WebView4_get_DocumentTitle(This,title)	\
    ( (This)->lpVtbl -> get_DocumentTitle(This,title) ) 


#define IWebView2WebView4_AddRemoteObject(This,name,object)	\
    ( (This)->lpVtbl -> AddRemoteObject(This,name,object) ) 

#define IWebView2WebView4_RemoveRemoteObject(This,name)	\
    ( (This)->lpVtbl -> RemoveRemoteObject(This,name) ) 

#define IWebView2WebView4_OpenDevToolsWindow(This)	\
    ( (This)->lpVtbl -> OpenDevToolsWindow(This) ) 

#define IWebView2WebView4_add_AcceleratorKeyPressed(This,eventHandler,token)	\
    ( (This)->lpVtbl -> add_AcceleratorKeyPressed(This,eventHandler,token) ) 

#define IWebView2WebView4_remove_AcceleratorKeyPressed(This,token)	\
    ( (This)->lpVtbl -> remove_AcceleratorKeyPressed(This,token) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IWebView2WebView4_INTERFACE_DEFINED__ */


#ifndef __IWebView2WebView5_INTERFACE_DEFINED__
#define __IWebView2WebView5_INTERFACE_DEFINED__

/* interface IWebView2WebView5 */
/* [unique][object][uuid] */ 


EXTERN_C const IID IID_IWebView2WebView5;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("E55144F5-A16F-43D8-9580-1E5227152EDF")
    IWebView2WebView5 : public IWebView2WebView4
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE add_ContainsFullScreenElementChanged( 
            /* [in] */ IWebView2ContainsFullScreenElementChangedEventHandler *eventHandler,
            /* [out] */ EventRegistrationToken *token) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE remove_ContainsFullScreenElementChanged( 
            /* [in] */ EventRegistrationToken token) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE get_ContainsFullScreenElement( 
            /* [retval][out] */ BOOL *containsFullScreenElement) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE add_WebResourceRequested( 
            /* [in] */ IWebView2WebResourceRequestedEventHandler *eventHandler,
            /* [out] */ EventRegistrationToken *token) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE AddWebResourceRequestedFilter( 
            /* [in] */ const LPCWSTR uri,
            /* [in] */ const WEBVIEW2_WEB_RESOURCE_CONTEXT resourceContext) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RemoveWebResourceRequestedFilter( 
            /* [in] */ const LPCWSTR uri,
            /* [in] */ const WEBVIEW2_WEB_RESOURCE_CONTEXT resourceContext) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IWebView2WebView5Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IWebView2WebView5 * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IWebView2WebView5 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IWebView2WebView5 * This);
        
        HRESULT ( STDMETHODCALLTYPE *get_Settings )( 
            IWebView2WebView5 * This,
            /* [retval][out] */ IWebView2Settings **settings);
        
        HRESULT ( STDMETHODCALLTYPE *get_Source )( 
            IWebView2WebView5 * This,
            /* [retval][out] */ LPWSTR *uri);
        
        HRESULT ( STDMETHODCALLTYPE *Navigate )( 
            IWebView2WebView5 * This,
            /* [in] */ LPCWSTR uri);
        
        HRESULT ( STDMETHODCALLTYPE *MoveFocus )( 
            IWebView2WebView5 * This,
            /* [in] */ WEBVIEW2_MOVE_FOCUS_REASON reason);
        
        HRESULT ( STDMETHODCALLTYPE *NavigateToString )( 
            IWebView2WebView5 * This,
            /* [in] */ LPCWSTR htmlContent);
        
        HRESULT ( STDMETHODCALLTYPE *add_NavigationStarting )( 
            IWebView2WebView5 * This,
            /* [in] */ IWebView2NavigationStartingEventHandler *eventHandler,
            /* [out] */ EventRegistrationToken *token);
        
        HRESULT ( STDMETHODCALLTYPE *remove_NavigationStarting )( 
            IWebView2WebView5 * This,
            /* [in] */ EventRegistrationToken token);
        
        HRESULT ( STDMETHODCALLTYPE *add_DocumentStateChanged )( 
            IWebView2WebView5 * This,
            /* [in] */ IWebView2DocumentStateChangedEventHandler *eventHandler,
            /* [out] */ EventRegistrationToken *token);
        
        HRESULT ( STDMETHODCALLTYPE *remove_DocumentStateChanged )( 
            IWebView2WebView5 * This,
            /* [in] */ EventRegistrationToken token);
        
        HRESULT ( STDMETHODCALLTYPE *add_NavigationCompleted )( 
            IWebView2WebView5 * This,
            /* [in] */ IWebView2NavigationCompletedEventHandler *eventHandler,
            /* [out] */ EventRegistrationToken *token);
        
        HRESULT ( STDMETHODCALLTYPE *remove_NavigationCompleted )( 
            IWebView2WebView5 * This,
            /* [in] */ EventRegistrationToken token);
        
        HRESULT ( STDMETHODCALLTYPE *add_FrameNavigationStarting )( 
            IWebView2WebView5 * This,
            /* [in] */ IWebView2NavigationStartingEventHandler *eventHandler,
            /* [out] */ EventRegistrationToken *token);
        
        HRESULT ( STDMETHODCALLTYPE *remove_FrameNavigationStarting )( 
            IWebView2WebView5 * This,
            /* [in] */ EventRegistrationToken token);
        
        HRESULT ( STDMETHODCALLTYPE *add_MoveFocusRequested )( 
            IWebView2WebView5 * This,
            /* [in] */ IWebView2MoveFocusRequestedEventHandler *eventHandler,
            /* [out] */ EventRegistrationToken *token);
        
        HRESULT ( STDMETHODCALLTYPE *remove_MoveFocusRequested )( 
            IWebView2WebView5 * This,
            /* [in] */ EventRegistrationToken token);
        
        HRESULT ( STDMETHODCALLTYPE *add_GotFocus )( 
            IWebView2WebView5 * This,
            /* [in] */ IWebView2FocusChangedEventHandler *eventHandler,
            /* [out] */ EventRegistrationToken *token);
        
        HRESULT ( STDMETHODCALLTYPE *remove_GotFocus )( 
            IWebView2WebView5 * This,
            /* [in] */ EventRegistrationToken token);
        
        HRESULT ( STDMETHODCALLTYPE *add_LostFocus )( 
            IWebView2WebView5 * This,
            /* [in] */ IWebView2FocusChangedEventHandler *eventHandler,
            /* [out] */ EventRegistrationToken *token);
        
        HRESULT ( STDMETHODCALLTYPE *remove_LostFocus )( 
            IWebView2WebView5 * This,
            /* [in] */ EventRegistrationToken token);
        
        HRESULT ( STDMETHODCALLTYPE *add_WebResourceRequested_deprecated )( 
            IWebView2WebView5 * This,
            /* [in] */ LPCWSTR *const urlFilter,
            /* [in] */ WEBVIEW2_WEB_RESOURCE_CONTEXT *const resourceContextFilter,
            /* [in] */ SIZE_T filterLength,
            /* [in] */ IWebView2WebResourceRequestedEventHandler *eventHandler,
            /* [out] */ EventRegistrationToken *token);
        
        HRESULT ( STDMETHODCALLTYPE *remove_WebResourceRequested )( 
            IWebView2WebView5 * This,
            /* [in] */ EventRegistrationToken token);
        
        HRESULT ( STDMETHODCALLTYPE *add_ScriptDialogOpening )( 
            IWebView2WebView5 * This,
            /* [in] */ IWebView2ScriptDialogOpeningEventHandler *eventHandler,
            /* [out] */ EventRegistrationToken *token);
        
        HRESULT ( STDMETHODCALLTYPE *remove_ScriptDialogOpening )( 
            IWebView2WebView5 * This,
            /* [in] */ EventRegistrationToken token);
        
        HRESULT ( STDMETHODCALLTYPE *add_ZoomFactorChanged )( 
            IWebView2WebView5 * This,
            /* [in] */ IWebView2ZoomFactorChangedEventHandler *eventHandler,
            /* [out] */ EventRegistrationToken *token);
        
        HRESULT ( STDMETHODCALLTYPE *remove_ZoomFactorChanged )( 
            IWebView2WebView5 * This,
            /* [in] */ EventRegistrationToken token);
        
        HRESULT ( STDMETHODCALLTYPE *add_PermissionRequested )( 
            IWebView2WebView5 * This,
            /* [in] */ IWebView2PermissionRequestedEventHandler *eventHandler,
            /* [out] */ EventRegistrationToken *token);
        
        HRESULT ( STDMETHODCALLTYPE *remove_PermissionRequested )( 
            IWebView2WebView5 * This,
            /* [in] */ EventRegistrationToken token);
        
        HRESULT ( STDMETHODCALLTYPE *add_ProcessFailed )( 
            IWebView2WebView5 * This,
            /* [in] */ IWebView2ProcessFailedEventHandler *eventHandler,
            /* [out] */ EventRegistrationToken *token);
        
        HRESULT ( STDMETHODCALLTYPE *remove_ProcessFailed )( 
            IWebView2WebView5 * This,
            /* [in] */ EventRegistrationToken token);
        
        HRESULT ( STDMETHODCALLTYPE *AddScriptToExecuteOnDocumentCreated )( 
            IWebView2WebView5 * This,
            /* [in] */ LPCWSTR javaScript,
            /* [in] */ IWebView2AddScriptToExecuteOnDocumentCreatedCompletedHandler *handler);
        
        HRESULT ( STDMETHODCALLTYPE *RemoveScriptToExecuteOnDocumentCreated )( 
            IWebView2WebView5 * This,
            /* [in] */ LPCWSTR id);
        
        HRESULT ( STDMETHODCALLTYPE *ExecuteScript )( 
            IWebView2WebView5 * This,
            /* [in] */ LPCWSTR javaScript,
            /* [in] */ IWebView2ExecuteScriptCompletedHandler *handler);
        
        HRESULT ( STDMETHODCALLTYPE *CapturePreview )( 
            IWebView2WebView5 * This,
            /* [in] */ WEBVIEW2_CAPTURE_PREVIEW_IMAGE_FORMAT imageFormat,
            /* [in] */ IStream *imageStream,
            /* [in] */ IWebView2CapturePreviewCompletedHandler *handler);
        
        HRESULT ( STDMETHODCALLTYPE *Reload )( 
            IWebView2WebView5 * This);
        
        HRESULT ( STDMETHODCALLTYPE *get_Bounds )( 
            IWebView2WebView5 * This,
            /* [retval][out] */ RECT *bounds);
        
        HRESULT ( STDMETHODCALLTYPE *put_Bounds )( 
            IWebView2WebView5 * This,
            /* [in] */ RECT bounds);
        
        HRESULT ( STDMETHODCALLTYPE *get_ZoomFactor )( 
            IWebView2WebView5 * This,
            /* [retval][out] */ double *zoomFactor);
        
        HRESULT ( STDMETHODCALLTYPE *put_ZoomFactor )( 
            IWebView2WebView5 * This,
            /* [in] */ double zoomFactor);
        
        HRESULT ( STDMETHODCALLTYPE *get_IsVisible )( 
            IWebView2WebView5 * This,
            /* [retval][out] */ BOOL *isVisible);
        
        HRESULT ( STDMETHODCALLTYPE *put_IsVisible )( 
            IWebView2WebView5 * This,
            /* [in] */ BOOL isVisible);
        
        HRESULT ( STDMETHODCALLTYPE *PostWebMessageAsJson )( 
            IWebView2WebView5 * This,
            /* [in] */ LPCWSTR webMessageAsJson);
        
        HRESULT ( STDMETHODCALLTYPE *PostWebMessageAsString )( 
            IWebView2WebView5 * This,
            /* [in] */ LPCWSTR webMessageAsString);
        
        HRESULT ( STDMETHODCALLTYPE *add_WebMessageReceived )( 
            IWebView2WebView5 * This,
            /* [in] */ IWebView2WebMessageReceivedEventHandler *handler,
            /* [out] */ EventRegistrationToken *token);
        
        HRESULT ( STDMETHODCALLTYPE *remove_WebMessageReceived )( 
            IWebView2WebView5 * This,
            /* [in] */ EventRegistrationToken token);
        
        HRESULT ( STDMETHODCALLTYPE *Close )( 
            IWebView2WebView5 * This);
        
        HRESULT ( STDMETHODCALLTYPE *CallDevToolsProtocolMethod )( 
            IWebView2WebView5 * This,
            /* [in] */ LPCWSTR methodName,
            /* [in] */ LPCWSTR parametersAsJson,
            /* [in] */ IWebView2CallDevToolsProtocolMethodCompletedHandler *handler);
        
        HRESULT ( STDMETHODCALLTYPE *add_DevToolsProtocolEventReceived )( 
            IWebView2WebView5 * This,
            /* [in] */ LPCWSTR eventName,
            /* [in] */ IWebView2DevToolsProtocolEventReceivedEventHandler *handler,
            /* [out] */ EventRegistrationToken *token);
        
        HRESULT ( STDMETHODCALLTYPE *remove_DevToolsProtocolEventReceived )( 
            IWebView2WebView5 * This,
            /* [in] */ LPCWSTR eventName,
            /* [in] */ EventRegistrationToken token);
        
        HRESULT ( STDMETHODCALLTYPE *get_BrowserProcessId )( 
            IWebView2WebView5 * This,
            /* [retval][out] */ UINT32 *value);
        
        HRESULT ( STDMETHODCALLTYPE *get_CanGoBack )( 
            IWebView2WebView5 * This,
            /* [retval][out] */ BOOL *canGoBack);
        
        HRESULT ( STDMETHODCALLTYPE *get_CanGoForward )( 
            IWebView2WebView5 * This,
            /* [retval][out] */ BOOL *canGoForward);
        
        HRESULT ( STDMETHODCALLTYPE *GoBack )( 
            IWebView2WebView5 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GoForward )( 
            IWebView2WebView5 * This);
        
        HRESULT ( STDMETHODCALLTYPE *Stop )( 
            IWebView2WebView5 * This);
        
        HRESULT ( STDMETHODCALLTYPE *add_NewWindowRequested )( 
            IWebView2WebView5 * This,
            /* [in] */ IWebView2NewWindowRequestedEventHandler *eventHandler,
            /* [out] */ EventRegistrationToken *token);
        
        HRESULT ( STDMETHODCALLTYPE *remove_NewWindowRequested )( 
            IWebView2WebView5 * This,
            /* [in] */ EventRegistrationToken token);
        
        HRESULT ( STDMETHODCALLTYPE *add_DocumentTitleChanged )( 
            IWebView2WebView5 * This,
            /* [in] */ IWebView2DocumentTitleChangedEventHandler *eventHandler,
            /* [out] */ EventRegistrationToken *token);
        
        HRESULT ( STDMETHODCALLTYPE *remove_DocumentTitleChanged )( 
            IWebView2WebView5 * This,
            /* [in] */ EventRegistrationToken token);
        
        HRESULT ( STDMETHODCALLTYPE *get_DocumentTitle )( 
            IWebView2WebView5 * This,
            /* [out] */ LPWSTR *title);
        
        HRESULT ( STDMETHODCALLTYPE *AddRemoteObject )( 
            IWebView2WebView5 * This,
            /* [in] */ LPCWSTR name,
            /* [in] */ VARIANT *object);
        
        HRESULT ( STDMETHODCALLTYPE *RemoveRemoteObject )( 
            IWebView2WebView5 * This,
            /* [in] */ LPCWSTR name);
        
        HRESULT ( STDMETHODCALLTYPE *OpenDevToolsWindow )( 
            IWebView2WebView5 * This);
        
        HRESULT ( STDMETHODCALLTYPE *add_AcceleratorKeyPressed )( 
            IWebView2WebView5 * This,
            /* [in] */ IWebView2AcceleratorKeyPressedEventHandler *eventHandler,
            /* [out] */ EventRegistrationToken *token);
        
        HRESULT ( STDMETHODCALLTYPE *remove_AcceleratorKeyPressed )( 
            IWebView2WebView5 * This,
            /* [in] */ EventRegistrationToken token);
        
        HRESULT ( STDMETHODCALLTYPE *add_ContainsFullScreenElementChanged )( 
            IWebView2WebView5 * This,
            /* [in] */ IWebView2ContainsFullScreenElementChangedEventHandler *eventHandler,
            /* [out] */ EventRegistrationToken *token);
        
        HRESULT ( STDMETHODCALLTYPE *remove_ContainsFullScreenElementChanged )( 
            IWebView2WebView5 * This,
            /* [in] */ EventRegistrationToken token);
        
        HRESULT ( STDMETHODCALLTYPE *get_ContainsFullScreenElement )( 
            IWebView2WebView5 * This,
            /* [retval][out] */ BOOL *containsFullScreenElement);
        
        HRESULT ( STDMETHODCALLTYPE *add_WebResourceRequested )( 
            IWebView2WebView5 * This,
            /* [in] */ IWebView2WebResourceRequestedEventHandler *eventHandler,
            /* [out] */ EventRegistrationToken *token);
        
        HRESULT ( STDMETHODCALLTYPE *AddWebResourceRequestedFilter )( 
            IWebView2WebView5 * This,
            /* [in] */ const LPCWSTR uri,
            /* [in] */ const WEBVIEW2_WEB_RESOURCE_CONTEXT resourceContext);
        
        HRESULT ( STDMETHODCALLTYPE *RemoveWebResourceRequestedFilter )( 
            IWebView2WebView5 * This,
            /* [in] */ const LPCWSTR uri,
            /* [in] */ const WEBVIEW2_WEB_RESOURCE_CONTEXT resourceContext);
        
        END_INTERFACE
    } IWebView2WebView5Vtbl;

    interface IWebView2WebView5
    {
        CONST_VTBL struct IWebView2WebView5Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWebView2WebView5_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IWebView2WebView5_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IWebView2WebView5_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IWebView2WebView5_get_Settings(This,settings)	\
    ( (This)->lpVtbl -> get_Settings(This,settings) ) 

#define IWebView2WebView5_get_Source(This,uri)	\
    ( (This)->lpVtbl -> get_Source(This,uri) ) 

#define IWebView2WebView5_Navigate(This,uri)	\
    ( (This)->lpVtbl -> Navigate(This,uri) ) 

#define IWebView2WebView5_MoveFocus(This,reason)	\
    ( (This)->lpVtbl -> MoveFocus(This,reason) ) 

#define IWebView2WebView5_NavigateToString(This,htmlContent)	\
    ( (This)->lpVtbl -> NavigateToString(This,htmlContent) ) 

#define IWebView2WebView5_add_NavigationStarting(This,eventHandler,token)	\
    ( (This)->lpVtbl -> add_NavigationStarting(This,eventHandler,token) ) 

#define IWebView2WebView5_remove_NavigationStarting(This,token)	\
    ( (This)->lpVtbl -> remove_NavigationStarting(This,token) ) 

#define IWebView2WebView5_add_DocumentStateChanged(This,eventHandler,token)	\
    ( (This)->lpVtbl -> add_DocumentStateChanged(This,eventHandler,token) ) 

#define IWebView2WebView5_remove_DocumentStateChanged(This,token)	\
    ( (This)->lpVtbl -> remove_DocumentStateChanged(This,token) ) 

#define IWebView2WebView5_add_NavigationCompleted(This,eventHandler,token)	\
    ( (This)->lpVtbl -> add_NavigationCompleted(This,eventHandler,token) ) 

#define IWebView2WebView5_remove_NavigationCompleted(This,token)	\
    ( (This)->lpVtbl -> remove_NavigationCompleted(This,token) ) 

#define IWebView2WebView5_add_FrameNavigationStarting(This,eventHandler,token)	\
    ( (This)->lpVtbl -> add_FrameNavigationStarting(This,eventHandler,token) ) 

#define IWebView2WebView5_remove_FrameNavigationStarting(This,token)	\
    ( (This)->lpVtbl -> remove_FrameNavigationStarting(This,token) ) 

#define IWebView2WebView5_add_MoveFocusRequested(This,eventHandler,token)	\
    ( (This)->lpVtbl -> add_MoveFocusRequested(This,eventHandler,token) ) 

#define IWebView2WebView5_remove_MoveFocusRequested(This,token)	\
    ( (This)->lpVtbl -> remove_MoveFocusRequested(This,token) ) 

#define IWebView2WebView5_add_GotFocus(This,eventHandler,token)	\
    ( (This)->lpVtbl -> add_GotFocus(This,eventHandler,token) ) 

#define IWebView2WebView5_remove_GotFocus(This,token)	\
    ( (This)->lpVtbl -> remove_GotFocus(This,token) ) 

#define IWebView2WebView5_add_LostFocus(This,eventHandler,token)	\
    ( (This)->lpVtbl -> add_LostFocus(This,eventHandler,token) ) 

#define IWebView2WebView5_remove_LostFocus(This,token)	\
    ( (This)->lpVtbl -> remove_LostFocus(This,token) ) 

#define IWebView2WebView5_add_WebResourceRequested_deprecated(This,urlFilter,resourceContextFilter,filterLength,eventHandler,token)	\
    ( (This)->lpVtbl -> add_WebResourceRequested_deprecated(This,urlFilter,resourceContextFilter,filterLength,eventHandler,token) ) 

#define IWebView2WebView5_remove_WebResourceRequested(This,token)	\
    ( (This)->lpVtbl -> remove_WebResourceRequested(This,token) ) 

#define IWebView2WebView5_add_ScriptDialogOpening(This,eventHandler,token)	\
    ( (This)->lpVtbl -> add_ScriptDialogOpening(This,eventHandler,token) ) 

#define IWebView2WebView5_remove_ScriptDialogOpening(This,token)	\
    ( (This)->lpVtbl -> remove_ScriptDialogOpening(This,token) ) 

#define IWebView2WebView5_add_ZoomFactorChanged(This,eventHandler,token)	\
    ( (This)->lpVtbl -> add_ZoomFactorChanged(This,eventHandler,token) ) 

#define IWebView2WebView5_remove_ZoomFactorChanged(This,token)	\
    ( (This)->lpVtbl -> remove_ZoomFactorChanged(This,token) ) 

#define IWebView2WebView5_add_PermissionRequested(This,eventHandler,token)	\
    ( (This)->lpVtbl -> add_PermissionRequested(This,eventHandler,token) ) 

#define IWebView2WebView5_remove_PermissionRequested(This,token)	\
    ( (This)->lpVtbl -> remove_PermissionRequested(This,token) ) 

#define IWebView2WebView5_add_ProcessFailed(This,eventHandler,token)	\
    ( (This)->lpVtbl -> add_ProcessFailed(This,eventHandler,token) ) 

#define IWebView2WebView5_remove_ProcessFailed(This,token)	\
    ( (This)->lpVtbl -> remove_ProcessFailed(This,token) ) 

#define IWebView2WebView5_AddScriptToExecuteOnDocumentCreated(This,javaScript,handler)	\
    ( (This)->lpVtbl -> AddScriptToExecuteOnDocumentCreated(This,javaScript,handler) ) 

#define IWebView2WebView5_RemoveScriptToExecuteOnDocumentCreated(This,id)	\
    ( (This)->lpVtbl -> RemoveScriptToExecuteOnDocumentCreated(This,id) ) 

#define IWebView2WebView5_ExecuteScript(This,javaScript,handler)	\
    ( (This)->lpVtbl -> ExecuteScript(This,javaScript,handler) ) 

#define IWebView2WebView5_CapturePreview(This,imageFormat,imageStream,handler)	\
    ( (This)->lpVtbl -> CapturePreview(This,imageFormat,imageStream,handler) ) 

#define IWebView2WebView5_Reload(This)	\
    ( (This)->lpVtbl -> Reload(This) ) 

#define IWebView2WebView5_get_Bounds(This,bounds)	\
    ( (This)->lpVtbl -> get_Bounds(This,bounds) ) 

#define IWebView2WebView5_put_Bounds(This,bounds)	\
    ( (This)->lpVtbl -> put_Bounds(This,bounds) ) 

#define IWebView2WebView5_get_ZoomFactor(This,zoomFactor)	\
    ( (This)->lpVtbl -> get_ZoomFactor(This,zoomFactor) ) 

#define IWebView2WebView5_put_ZoomFactor(This,zoomFactor)	\
    ( (This)->lpVtbl -> put_ZoomFactor(This,zoomFactor) ) 

#define IWebView2WebView5_get_IsVisible(This,isVisible)	\
    ( (This)->lpVtbl -> get_IsVisible(This,isVisible) ) 

#define IWebView2WebView5_put_IsVisible(This,isVisible)	\
    ( (This)->lpVtbl -> put_IsVisible(This,isVisible) ) 

#define IWebView2WebView5_PostWebMessageAsJson(This,webMessageAsJson)	\
    ( (This)->lpVtbl -> PostWebMessageAsJson(This,webMessageAsJson) ) 

#define IWebView2WebView5_PostWebMessageAsString(This,webMessageAsString)	\
    ( (This)->lpVtbl -> PostWebMessageAsString(This,webMessageAsString) ) 

#define IWebView2WebView5_add_WebMessageReceived(This,handler,token)	\
    ( (This)->lpVtbl -> add_WebMessageReceived(This,handler,token) ) 

#define IWebView2WebView5_remove_WebMessageReceived(This,token)	\
    ( (This)->lpVtbl -> remove_WebMessageReceived(This,token) ) 

#define IWebView2WebView5_Close(This)	\
    ( (This)->lpVtbl -> Close(This) ) 

#define IWebView2WebView5_CallDevToolsProtocolMethod(This,methodName,parametersAsJson,handler)	\
    ( (This)->lpVtbl -> CallDevToolsProtocolMethod(This,methodName,parametersAsJson,handler) ) 

#define IWebView2WebView5_add_DevToolsProtocolEventReceived(This,eventName,handler,token)	\
    ( (This)->lpVtbl -> add_DevToolsProtocolEventReceived(This,eventName,handler,token) ) 

#define IWebView2WebView5_remove_DevToolsProtocolEventReceived(This,eventName,token)	\
    ( (This)->lpVtbl -> remove_DevToolsProtocolEventReceived(This,eventName,token) ) 

#define IWebView2WebView5_get_BrowserProcessId(This,value)	\
    ( (This)->lpVtbl -> get_BrowserProcessId(This,value) ) 

#define IWebView2WebView5_get_CanGoBack(This,canGoBack)	\
    ( (This)->lpVtbl -> get_CanGoBack(This,canGoBack) ) 

#define IWebView2WebView5_get_CanGoForward(This,canGoForward)	\
    ( (This)->lpVtbl -> get_CanGoForward(This,canGoForward) ) 

#define IWebView2WebView5_GoBack(This)	\
    ( (This)->lpVtbl -> GoBack(This) ) 

#define IWebView2WebView5_GoForward(This)	\
    ( (This)->lpVtbl -> GoForward(This) ) 


#define IWebView2WebView5_Stop(This)	\
    ( (This)->lpVtbl -> Stop(This) ) 

#define IWebView2WebView5_add_NewWindowRequested(This,eventHandler,token)	\
    ( (This)->lpVtbl -> add_NewWindowRequested(This,eventHandler,token) ) 

#define IWebView2WebView5_remove_NewWindowRequested(This,token)	\
    ( (This)->lpVtbl -> remove_NewWindowRequested(This,token) ) 

#define IWebView2WebView5_add_DocumentTitleChanged(This,eventHandler,token)	\
    ( (This)->lpVtbl -> add_DocumentTitleChanged(This,eventHandler,token) ) 

#define IWebView2WebView5_remove_DocumentTitleChanged(This,token)	\
    ( (This)->lpVtbl -> remove_DocumentTitleChanged(This,token) ) 

#define IWebView2WebView5_get_DocumentTitle(This,title)	\
    ( (This)->lpVtbl -> get_DocumentTitle(This,title) ) 


#define IWebView2WebView5_AddRemoteObject(This,name,object)	\
    ( (This)->lpVtbl -> AddRemoteObject(This,name,object) ) 

#define IWebView2WebView5_RemoveRemoteObject(This,name)	\
    ( (This)->lpVtbl -> RemoveRemoteObject(This,name) ) 

#define IWebView2WebView5_OpenDevToolsWindow(This)	\
    ( (This)->lpVtbl -> OpenDevToolsWindow(This) ) 

#define IWebView2WebView5_add_AcceleratorKeyPressed(This,eventHandler,token)	\
    ( (This)->lpVtbl -> add_AcceleratorKeyPressed(This,eventHandler,token) ) 

#define IWebView2WebView5_remove_AcceleratorKeyPressed(This,token)	\
    ( (This)->lpVtbl -> remove_AcceleratorKeyPressed(This,token) ) 


#define IWebView2WebView5_add_ContainsFullScreenElementChanged(This,eventHandler,token)	\
    ( (This)->lpVtbl -> add_ContainsFullScreenElementChanged(This,eventHandler,token) ) 

#define IWebView2WebView5_remove_ContainsFullScreenElementChanged(This,token)	\
    ( (This)->lpVtbl -> remove_ContainsFullScreenElementChanged(This,token) ) 

#define IWebView2WebView5_get_ContainsFullScreenElement(This,containsFullScreenElement)	\
    ( (This)->lpVtbl -> get_ContainsFullScreenElement(This,containsFullScreenElement) ) 

#define IWebView2WebView5_add_WebResourceRequested(This,eventHandler,token)	\
    ( (This)->lpVtbl -> add_WebResourceRequested(This,eventHandler,token) ) 

#define IWebView2WebView5_AddWebResourceRequestedFilter(This,uri,resourceContext)	\
    ( (This)->lpVtbl -> AddWebResourceRequestedFilter(This,uri,resourceContext) ) 

#define IWebView2WebView5_RemoveWebResourceRequestedFilter(This,uri,resourceContext)	\
    ( (This)->lpVtbl -> RemoveWebResourceRequestedFilter(This,uri,resourceContext) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IWebView2WebView5_INTERFACE_DEFINED__ */


#ifndef __IWebView2Deferral_INTERFACE_DEFINED__
#define __IWebView2Deferral_INTERFACE_DEFINED__

/* interface IWebView2Deferral */
/* [unique][object][uuid] */ 


EXTERN_C const IID IID_IWebView2Deferral;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("BD478C19-4706-4B1D-88B6-76DD39ACB7B1")
    IWebView2Deferral : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Complete( void) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IWebView2DeferralVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IWebView2Deferral * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IWebView2Deferral * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IWebView2Deferral * This);
        
        HRESULT ( STDMETHODCALLTYPE *Complete )( 
            IWebView2Deferral * This);
        
        END_INTERFACE
    } IWebView2DeferralVtbl;

    interface IWebView2Deferral
    {
        CONST_VTBL struct IWebView2DeferralVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWebView2Deferral_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IWebView2Deferral_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IWebView2Deferral_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IWebView2Deferral_Complete(This)	\
    ( (This)->lpVtbl -> Complete(This) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IWebView2Deferral_INTERFACE_DEFINED__ */


#ifndef __IWebView2Settings_INTERFACE_DEFINED__
#define __IWebView2Settings_INTERFACE_DEFINED__

/* interface IWebView2Settings */
/* [unique][object][uuid] */ 


EXTERN_C const IID IID_IWebView2Settings;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("A28CD108-3234-4B45-B390-7E871B504A96")
    IWebView2Settings : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE get_IsScriptEnabled( 
            /* [retval][out] */ BOOL *isScriptEnabled) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE put_IsScriptEnabled( 
            /* [in] */ BOOL isScriptEnabled) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE get_IsWebMessageEnabled( 
            /* [retval][out] */ BOOL *isWebMessageEnabled) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE put_IsWebMessageEnabled( 
            /* [in] */ BOOL isWebMessageEnabled) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE get_AreDefaultScriptDialogsEnabled( 
            /* [retval][out] */ BOOL *areDefaultScriptDialogsEnabled) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE put_AreDefaultScriptDialogsEnabled( 
            /* [in] */ BOOL areDefaultScriptDialogsEnabled) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE get_IsFullscreenAllowed_deprecated( 
            /* [retval][out] */ BOOL *isFullscreenAllowed) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE put_IsFullscreenAllowed_deprecated( 
            /* [in] */ BOOL isFullscreenAllowed) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE get_IsStatusBarEnabled( 
            /* [retval][out] */ BOOL *isStatusBarEnabled) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE put_IsStatusBarEnabled( 
            /* [in] */ BOOL isStatusBarEnabled) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE get_AreDevToolsEnabled( 
            /* [retval][out] */ BOOL *areDevToolsEnabled) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE put_AreDevToolsEnabled( 
            /* [in] */ BOOL areDevToolsEnabled) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IWebView2SettingsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IWebView2Settings * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IWebView2Settings * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IWebView2Settings * This);
        
        HRESULT ( STDMETHODCALLTYPE *get_IsScriptEnabled )( 
            IWebView2Settings * This,
            /* [retval][out] */ BOOL *isScriptEnabled);
        
        HRESULT ( STDMETHODCALLTYPE *put_IsScriptEnabled )( 
            IWebView2Settings * This,
            /* [in] */ BOOL isScriptEnabled);
        
        HRESULT ( STDMETHODCALLTYPE *get_IsWebMessageEnabled )( 
            IWebView2Settings * This,
            /* [retval][out] */ BOOL *isWebMessageEnabled);
        
        HRESULT ( STDMETHODCALLTYPE *put_IsWebMessageEnabled )( 
            IWebView2Settings * This,
            /* [in] */ BOOL isWebMessageEnabled);
        
        HRESULT ( STDMETHODCALLTYPE *get_AreDefaultScriptDialogsEnabled )( 
            IWebView2Settings * This,
            /* [retval][out] */ BOOL *areDefaultScriptDialogsEnabled);
        
        HRESULT ( STDMETHODCALLTYPE *put_AreDefaultScriptDialogsEnabled )( 
            IWebView2Settings * This,
            /* [in] */ BOOL areDefaultScriptDialogsEnabled);
        
        HRESULT ( STDMETHODCALLTYPE *get_IsFullscreenAllowed_deprecated )( 
            IWebView2Settings * This,
            /* [retval][out] */ BOOL *isFullscreenAllowed);
        
        HRESULT ( STDMETHODCALLTYPE *put_IsFullscreenAllowed_deprecated )( 
            IWebView2Settings * This,
            /* [in] */ BOOL isFullscreenAllowed);
        
        HRESULT ( STDMETHODCALLTYPE *get_IsStatusBarEnabled )( 
            IWebView2Settings * This,
            /* [retval][out] */ BOOL *isStatusBarEnabled);
        
        HRESULT ( STDMETHODCALLTYPE *put_IsStatusBarEnabled )( 
            IWebView2Settings * This,
            /* [in] */ BOOL isStatusBarEnabled);
        
        HRESULT ( STDMETHODCALLTYPE *get_AreDevToolsEnabled )( 
            IWebView2Settings * This,
            /* [retval][out] */ BOOL *areDevToolsEnabled);
        
        HRESULT ( STDMETHODCALLTYPE *put_AreDevToolsEnabled )( 
            IWebView2Settings * This,
            /* [in] */ BOOL areDevToolsEnabled);
        
        END_INTERFACE
    } IWebView2SettingsVtbl;

    interface IWebView2Settings
    {
        CONST_VTBL struct IWebView2SettingsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWebView2Settings_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IWebView2Settings_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IWebView2Settings_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IWebView2Settings_get_IsScriptEnabled(This,isScriptEnabled)	\
    ( (This)->lpVtbl -> get_IsScriptEnabled(This,isScriptEnabled) ) 

#define IWebView2Settings_put_IsScriptEnabled(This,isScriptEnabled)	\
    ( (This)->lpVtbl -> put_IsScriptEnabled(This,isScriptEnabled) ) 

#define IWebView2Settings_get_IsWebMessageEnabled(This,isWebMessageEnabled)	\
    ( (This)->lpVtbl -> get_IsWebMessageEnabled(This,isWebMessageEnabled) ) 

#define IWebView2Settings_put_IsWebMessageEnabled(This,isWebMessageEnabled)	\
    ( (This)->lpVtbl -> put_IsWebMessageEnabled(This,isWebMessageEnabled) ) 

#define IWebView2Settings_get_AreDefaultScriptDialogsEnabled(This,areDefaultScriptDialogsEnabled)	\
    ( (This)->lpVtbl -> get_AreDefaultScriptDialogsEnabled(This,areDefaultScriptDialogsEnabled) ) 

#define IWebView2Settings_put_AreDefaultScriptDialogsEnabled(This,areDefaultScriptDialogsEnabled)	\
    ( (This)->lpVtbl -> put_AreDefaultScriptDialogsEnabled(This,areDefaultScriptDialogsEnabled) ) 

#define IWebView2Settings_get_IsFullscreenAllowed_deprecated(This,isFullscreenAllowed)	\
    ( (This)->lpVtbl -> get_IsFullscreenAllowed_deprecated(This,isFullscreenAllowed) ) 

#define IWebView2Settings_put_IsFullscreenAllowed_deprecated(This,isFullscreenAllowed)	\
    ( (This)->lpVtbl -> put_IsFullscreenAllowed_deprecated(This,isFullscreenAllowed) ) 

#define IWebView2Settings_get_IsStatusBarEnabled(This,isStatusBarEnabled)	\
    ( (This)->lpVtbl -> get_IsStatusBarEnabled(This,isStatusBarEnabled) ) 

#define IWebView2Settings_put_IsStatusBarEnabled(This,isStatusBarEnabled)	\
    ( (This)->lpVtbl -> put_IsStatusBarEnabled(This,isStatusBarEnabled) ) 

#define IWebView2Settings_get_AreDevToolsEnabled(This,areDevToolsEnabled)	\
    ( (This)->lpVtbl -> get_AreDevToolsEnabled(This,areDevToolsEnabled) ) 

#define IWebView2Settings_put_AreDevToolsEnabled(This,areDevToolsEnabled)	\
    ( (This)->lpVtbl -> put_AreDevToolsEnabled(This,areDevToolsEnabled) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IWebView2Settings_INTERFACE_DEFINED__ */


#ifndef __IWebView2Settings2_INTERFACE_DEFINED__
#define __IWebView2Settings2_INTERFACE_DEFINED__

/* interface IWebView2Settings2 */
/* [unique][object][uuid] */ 


EXTERN_C const IID IID_IWebView2Settings2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("9FC76F96-CFD8-4C92-8EC5-9215E92EF3E8")
    IWebView2Settings2 : public IWebView2Settings
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE get_AreDefaultContextMenusEnabled( 
            /* [retval][out] */ BOOL *enabled) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE put_AreDefaultContextMenusEnabled( 
            /* [in] */ BOOL enabled) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IWebView2Settings2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IWebView2Settings2 * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IWebView2Settings2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IWebView2Settings2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *get_IsScriptEnabled )( 
            IWebView2Settings2 * This,
            /* [retval][out] */ BOOL *isScriptEnabled);
        
        HRESULT ( STDMETHODCALLTYPE *put_IsScriptEnabled )( 
            IWebView2Settings2 * This,
            /* [in] */ BOOL isScriptEnabled);
        
        HRESULT ( STDMETHODCALLTYPE *get_IsWebMessageEnabled )( 
            IWebView2Settings2 * This,
            /* [retval][out] */ BOOL *isWebMessageEnabled);
        
        HRESULT ( STDMETHODCALLTYPE *put_IsWebMessageEnabled )( 
            IWebView2Settings2 * This,
            /* [in] */ BOOL isWebMessageEnabled);
        
        HRESULT ( STDMETHODCALLTYPE *get_AreDefaultScriptDialogsEnabled )( 
            IWebView2Settings2 * This,
            /* [retval][out] */ BOOL *areDefaultScriptDialogsEnabled);
        
        HRESULT ( STDMETHODCALLTYPE *put_AreDefaultScriptDialogsEnabled )( 
            IWebView2Settings2 * This,
            /* [in] */ BOOL areDefaultScriptDialogsEnabled);
        
        HRESULT ( STDMETHODCALLTYPE *get_IsFullscreenAllowed_deprecated )( 
            IWebView2Settings2 * This,
            /* [retval][out] */ BOOL *isFullscreenAllowed);
        
        HRESULT ( STDMETHODCALLTYPE *put_IsFullscreenAllowed_deprecated )( 
            IWebView2Settings2 * This,
            /* [in] */ BOOL isFullscreenAllowed);
        
        HRESULT ( STDMETHODCALLTYPE *get_IsStatusBarEnabled )( 
            IWebView2Settings2 * This,
            /* [retval][out] */ BOOL *isStatusBarEnabled);
        
        HRESULT ( STDMETHODCALLTYPE *put_IsStatusBarEnabled )( 
            IWebView2Settings2 * This,
            /* [in] */ BOOL isStatusBarEnabled);
        
        HRESULT ( STDMETHODCALLTYPE *get_AreDevToolsEnabled )( 
            IWebView2Settings2 * This,
            /* [retval][out] */ BOOL *areDevToolsEnabled);
        
        HRESULT ( STDMETHODCALLTYPE *put_AreDevToolsEnabled )( 
            IWebView2Settings2 * This,
            /* [in] */ BOOL areDevToolsEnabled);
        
        HRESULT ( STDMETHODCALLTYPE *get_AreDefaultContextMenusEnabled )( 
            IWebView2Settings2 * This,
            /* [retval][out] */ BOOL *enabled);
        
        HRESULT ( STDMETHODCALLTYPE *put_AreDefaultContextMenusEnabled )( 
            IWebView2Settings2 * This,
            /* [in] */ BOOL enabled);
        
        END_INTERFACE
    } IWebView2Settings2Vtbl;

    interface IWebView2Settings2
    {
        CONST_VTBL struct IWebView2Settings2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWebView2Settings2_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IWebView2Settings2_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IWebView2Settings2_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IWebView2Settings2_get_IsScriptEnabled(This,isScriptEnabled)	\
    ( (This)->lpVtbl -> get_IsScriptEnabled(This,isScriptEnabled) ) 

#define IWebView2Settings2_put_IsScriptEnabled(This,isScriptEnabled)	\
    ( (This)->lpVtbl -> put_IsScriptEnabled(This,isScriptEnabled) ) 

#define IWebView2Settings2_get_IsWebMessageEnabled(This,isWebMessageEnabled)	\
    ( (This)->lpVtbl -> get_IsWebMessageEnabled(This,isWebMessageEnabled) ) 

#define IWebView2Settings2_put_IsWebMessageEnabled(This,isWebMessageEnabled)	\
    ( (This)->lpVtbl -> put_IsWebMessageEnabled(This,isWebMessageEnabled) ) 

#define IWebView2Settings2_get_AreDefaultScriptDialogsEnabled(This,areDefaultScriptDialogsEnabled)	\
    ( (This)->lpVtbl -> get_AreDefaultScriptDialogsEnabled(This,areDefaultScriptDialogsEnabled) ) 

#define IWebView2Settings2_put_AreDefaultScriptDialogsEnabled(This,areDefaultScriptDialogsEnabled)	\
    ( (This)->lpVtbl -> put_AreDefaultScriptDialogsEnabled(This,areDefaultScriptDialogsEnabled) ) 

#define IWebView2Settings2_get_IsFullscreenAllowed_deprecated(This,isFullscreenAllowed)	\
    ( (This)->lpVtbl -> get_IsFullscreenAllowed_deprecated(This,isFullscreenAllowed) ) 

#define IWebView2Settings2_put_IsFullscreenAllowed_deprecated(This,isFullscreenAllowed)	\
    ( (This)->lpVtbl -> put_IsFullscreenAllowed_deprecated(This,isFullscreenAllowed) ) 

#define IWebView2Settings2_get_IsStatusBarEnabled(This,isStatusBarEnabled)	\
    ( (This)->lpVtbl -> get_IsStatusBarEnabled(This,isStatusBarEnabled) ) 

#define IWebView2Settings2_put_IsStatusBarEnabled(This,isStatusBarEnabled)	\
    ( (This)->lpVtbl -> put_IsStatusBarEnabled(This,isStatusBarEnabled) ) 

#define IWebView2Settings2_get_AreDevToolsEnabled(This,areDevToolsEnabled)	\
    ( (This)->lpVtbl -> get_AreDevToolsEnabled(This,areDevToolsEnabled) ) 

#define IWebView2Settings2_put_AreDevToolsEnabled(This,areDevToolsEnabled)	\
    ( (This)->lpVtbl -> put_AreDevToolsEnabled(This,areDevToolsEnabled) ) 


#define IWebView2Settings2_get_AreDefaultContextMenusEnabled(This,enabled)	\
    ( (This)->lpVtbl -> get_AreDefaultContextMenusEnabled(This,enabled) ) 

#define IWebView2Settings2_put_AreDefaultContextMenusEnabled(This,enabled)	\
    ( (This)->lpVtbl -> put_AreDefaultContextMenusEnabled(This,enabled) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IWebView2Settings2_INTERFACE_DEFINED__ */


#ifndef __IWebView2ProcessFailedEventArgs_INTERFACE_DEFINED__
#define __IWebView2ProcessFailedEventArgs_INTERFACE_DEFINED__

/* interface IWebView2ProcessFailedEventArgs */
/* [unique][object][uuid] */ 


EXTERN_C const IID IID_IWebView2ProcessFailedEventArgs;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("6DABCFB8-8C7D-4515-893B-9766766900DA")
    IWebView2ProcessFailedEventArgs : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE get_ProcessFailedKind( 
            /* [retval][out] */ WEBVIEW2_PROCESS_FAILED_KIND *processFailedKind) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IWebView2ProcessFailedEventArgsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IWebView2ProcessFailedEventArgs * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IWebView2ProcessFailedEventArgs * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IWebView2ProcessFailedEventArgs * This);
        
        HRESULT ( STDMETHODCALLTYPE *get_ProcessFailedKind )( 
            IWebView2ProcessFailedEventArgs * This,
            /* [retval][out] */ WEBVIEW2_PROCESS_FAILED_KIND *processFailedKind);
        
        END_INTERFACE
    } IWebView2ProcessFailedEventArgsVtbl;

    interface IWebView2ProcessFailedEventArgs
    {
        CONST_VTBL struct IWebView2ProcessFailedEventArgsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWebView2ProcessFailedEventArgs_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IWebView2ProcessFailedEventArgs_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IWebView2ProcessFailedEventArgs_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IWebView2ProcessFailedEventArgs_get_ProcessFailedKind(This,processFailedKind)	\
    ( (This)->lpVtbl -> get_ProcessFailedKind(This,processFailedKind) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IWebView2ProcessFailedEventArgs_INTERFACE_DEFINED__ */


#ifndef __IWebView2ProcessFailedEventHandler_INTERFACE_DEFINED__
#define __IWebView2ProcessFailedEventHandler_INTERFACE_DEFINED__

/* interface IWebView2ProcessFailedEventHandler */
/* [unique][object][uuid] */ 


EXTERN_C const IID IID_IWebView2ProcessFailedEventHandler;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("011EC830-5DAF-4767-A099-C43DE1A925F4")
    IWebView2ProcessFailedEventHandler : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Invoke( 
            /* [in] */ IWebView2WebView *webview,
            /* [in] */ IWebView2ProcessFailedEventArgs *args) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IWebView2ProcessFailedEventHandlerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IWebView2ProcessFailedEventHandler * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IWebView2ProcessFailedEventHandler * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IWebView2ProcessFailedEventHandler * This);
        
        HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IWebView2ProcessFailedEventHandler * This,
            /* [in] */ IWebView2WebView *webview,
            /* [in] */ IWebView2ProcessFailedEventArgs *args);
        
        END_INTERFACE
    } IWebView2ProcessFailedEventHandlerVtbl;

    interface IWebView2ProcessFailedEventHandler
    {
        CONST_VTBL struct IWebView2ProcessFailedEventHandlerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWebView2ProcessFailedEventHandler_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IWebView2ProcessFailedEventHandler_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IWebView2ProcessFailedEventHandler_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IWebView2ProcessFailedEventHandler_Invoke(This,webview,args)	\
    ( (This)->lpVtbl -> Invoke(This,webview,args) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IWebView2ProcessFailedEventHandler_INTERFACE_DEFINED__ */


#ifndef __IWebView2ZoomFactorChangedEventHandler_INTERFACE_DEFINED__
#define __IWebView2ZoomFactorChangedEventHandler_INTERFACE_DEFINED__

/* interface IWebView2ZoomFactorChangedEventHandler */
/* [unique][object][uuid] */ 


EXTERN_C const IID IID_IWebView2ZoomFactorChangedEventHandler;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("A5C0B08B-25D7-4BAC-AD06-11783393088E")
    IWebView2ZoomFactorChangedEventHandler : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Invoke( 
            /* [in] */ IWebView2WebView *webview,
            /* [in] */ IUnknown *args) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IWebView2ZoomFactorChangedEventHandlerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IWebView2ZoomFactorChangedEventHandler * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IWebView2ZoomFactorChangedEventHandler * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IWebView2ZoomFactorChangedEventHandler * This);
        
        HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IWebView2ZoomFactorChangedEventHandler * This,
            /* [in] */ IWebView2WebView *webview,
            /* [in] */ IUnknown *args);
        
        END_INTERFACE
    } IWebView2ZoomFactorChangedEventHandlerVtbl;

    interface IWebView2ZoomFactorChangedEventHandler
    {
        CONST_VTBL struct IWebView2ZoomFactorChangedEventHandlerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWebView2ZoomFactorChangedEventHandler_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IWebView2ZoomFactorChangedEventHandler_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IWebView2ZoomFactorChangedEventHandler_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IWebView2ZoomFactorChangedEventHandler_Invoke(This,webview,args)	\
    ( (This)->lpVtbl -> Invoke(This,webview,args) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IWebView2ZoomFactorChangedEventHandler_INTERFACE_DEFINED__ */


#ifndef __IWebView2HttpHeadersCollectionIterator_INTERFACE_DEFINED__
#define __IWebView2HttpHeadersCollectionIterator_INTERFACE_DEFINED__

/* interface IWebView2HttpHeadersCollectionIterator */
/* [unique][object][uuid] */ 


EXTERN_C const IID IID_IWebView2HttpHeadersCollectionIterator;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("66a215e4-ca41-490b-884a-411ffb17cd1c")
    IWebView2HttpHeadersCollectionIterator : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetCurrentHeader( 
            /* [out] */ LPWSTR *name,
            /* [out] */ LPWSTR *value) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE MoveNext( 
            /* [out] */ BOOL *has_next) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IWebView2HttpHeadersCollectionIteratorVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IWebView2HttpHeadersCollectionIterator * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IWebView2HttpHeadersCollectionIterator * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IWebView2HttpHeadersCollectionIterator * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetCurrentHeader )( 
            IWebView2HttpHeadersCollectionIterator * This,
            /* [out] */ LPWSTR *name,
            /* [out] */ LPWSTR *value);
        
        HRESULT ( STDMETHODCALLTYPE *MoveNext )( 
            IWebView2HttpHeadersCollectionIterator * This,
            /* [out] */ BOOL *has_next);
        
        END_INTERFACE
    } IWebView2HttpHeadersCollectionIteratorVtbl;

    interface IWebView2HttpHeadersCollectionIterator
    {
        CONST_VTBL struct IWebView2HttpHeadersCollectionIteratorVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWebView2HttpHeadersCollectionIterator_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IWebView2HttpHeadersCollectionIterator_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IWebView2HttpHeadersCollectionIterator_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IWebView2HttpHeadersCollectionIterator_GetCurrentHeader(This,name,value)	\
    ( (This)->lpVtbl -> GetCurrentHeader(This,name,value) ) 

#define IWebView2HttpHeadersCollectionIterator_MoveNext(This,has_next)	\
    ( (This)->lpVtbl -> MoveNext(This,has_next) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IWebView2HttpHeadersCollectionIterator_INTERFACE_DEFINED__ */


#ifndef __IWebView2HttpRequestHeaders_INTERFACE_DEFINED__
#define __IWebView2HttpRequestHeaders_INTERFACE_DEFINED__

/* interface IWebView2HttpRequestHeaders */
/* [unique][object][uuid] */ 


EXTERN_C const IID IID_IWebView2HttpRequestHeaders;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("982be490-0252-44f3-9f33-376c04885a6d")
    IWebView2HttpRequestHeaders : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetHeader( 
            /* [in] */ LPCWSTR name,
            /* [out] */ LPWSTR *value) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Contains( 
            /* [in] */ LPCWSTR name,
            /* [retval][out] */ BOOL *contains) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetHeader( 
            /* [in] */ LPCWSTR name,
            /* [in] */ LPCWSTR value) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RemoveHeader( 
            /* [in] */ LPCWSTR name) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetIterator( 
            /* [out] */ IWebView2HttpHeadersCollectionIterator **iterator) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IWebView2HttpRequestHeadersVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IWebView2HttpRequestHeaders * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IWebView2HttpRequestHeaders * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IWebView2HttpRequestHeaders * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetHeader )( 
            IWebView2HttpRequestHeaders * This,
            /* [in] */ LPCWSTR name,
            /* [out] */ LPWSTR *value);
        
        HRESULT ( STDMETHODCALLTYPE *Contains )( 
            IWebView2HttpRequestHeaders * This,
            /* [in] */ LPCWSTR name,
            /* [retval][out] */ BOOL *contains);
        
        HRESULT ( STDMETHODCALLTYPE *SetHeader )( 
            IWebView2HttpRequestHeaders * This,
            /* [in] */ LPCWSTR name,
            /* [in] */ LPCWSTR value);
        
        HRESULT ( STDMETHODCALLTYPE *RemoveHeader )( 
            IWebView2HttpRequestHeaders * This,
            /* [in] */ LPCWSTR name);
        
        HRESULT ( STDMETHODCALLTYPE *GetIterator )( 
            IWebView2HttpRequestHeaders * This,
            /* [out] */ IWebView2HttpHeadersCollectionIterator **iterator);
        
        END_INTERFACE
    } IWebView2HttpRequestHeadersVtbl;

    interface IWebView2HttpRequestHeaders
    {
        CONST_VTBL struct IWebView2HttpRequestHeadersVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWebView2HttpRequestHeaders_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IWebView2HttpRequestHeaders_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IWebView2HttpRequestHeaders_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IWebView2HttpRequestHeaders_GetHeader(This,name,value)	\
    ( (This)->lpVtbl -> GetHeader(This,name,value) ) 

#define IWebView2HttpRequestHeaders_Contains(This,name,contains)	\
    ( (This)->lpVtbl -> Contains(This,name,contains) ) 

#define IWebView2HttpRequestHeaders_SetHeader(This,name,value)	\
    ( (This)->lpVtbl -> SetHeader(This,name,value) ) 

#define IWebView2HttpRequestHeaders_RemoveHeader(This,name)	\
    ( (This)->lpVtbl -> RemoveHeader(This,name) ) 

#define IWebView2HttpRequestHeaders_GetIterator(This,iterator)	\
    ( (This)->lpVtbl -> GetIterator(This,iterator) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IWebView2HttpRequestHeaders_INTERFACE_DEFINED__ */


#ifndef __IWebView2HttpResponseHeaders_INTERFACE_DEFINED__
#define __IWebView2HttpResponseHeaders_INTERFACE_DEFINED__

/* interface IWebView2HttpResponseHeaders */
/* [unique][object][uuid] */ 


EXTERN_C const IID IID_IWebView2HttpResponseHeaders;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("6d1a13a6-c677-41aa-852f-827b53f35301")
    IWebView2HttpResponseHeaders : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE AppendHeader( 
            /* [in] */ LPCWSTR name,
            /* [in] */ LPCWSTR value) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Contains( 
            /* [in] */ LPCWSTR name,
            /* [retval][out] */ BOOL *contains) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetHeaders( 
            /* [in] */ LPCWSTR name,
            /* [out] */ IWebView2HttpHeadersCollectionIterator **iterator) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetIterator( 
            /* [out] */ IWebView2HttpHeadersCollectionIterator **iterator) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IWebView2HttpResponseHeadersVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IWebView2HttpResponseHeaders * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IWebView2HttpResponseHeaders * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IWebView2HttpResponseHeaders * This);
        
        HRESULT ( STDMETHODCALLTYPE *AppendHeader )( 
            IWebView2HttpResponseHeaders * This,
            /* [in] */ LPCWSTR name,
            /* [in] */ LPCWSTR value);
        
        HRESULT ( STDMETHODCALLTYPE *Contains )( 
            IWebView2HttpResponseHeaders * This,
            /* [in] */ LPCWSTR name,
            /* [retval][out] */ BOOL *contains);
        
        HRESULT ( STDMETHODCALLTYPE *GetHeaders )( 
            IWebView2HttpResponseHeaders * This,
            /* [in] */ LPCWSTR name,
            /* [out] */ IWebView2HttpHeadersCollectionIterator **iterator);
        
        HRESULT ( STDMETHODCALLTYPE *GetIterator )( 
            IWebView2HttpResponseHeaders * This,
            /* [out] */ IWebView2HttpHeadersCollectionIterator **iterator);
        
        END_INTERFACE
    } IWebView2HttpResponseHeadersVtbl;

    interface IWebView2HttpResponseHeaders
    {
        CONST_VTBL struct IWebView2HttpResponseHeadersVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWebView2HttpResponseHeaders_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IWebView2HttpResponseHeaders_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IWebView2HttpResponseHeaders_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IWebView2HttpResponseHeaders_AppendHeader(This,name,value)	\
    ( (This)->lpVtbl -> AppendHeader(This,name,value) ) 

#define IWebView2HttpResponseHeaders_Contains(This,name,contains)	\
    ( (This)->lpVtbl -> Contains(This,name,contains) ) 

#define IWebView2HttpResponseHeaders_GetHeaders(This,name,iterator)	\
    ( (This)->lpVtbl -> GetHeaders(This,name,iterator) ) 

#define IWebView2HttpResponseHeaders_GetIterator(This,iterator)	\
    ( (This)->lpVtbl -> GetIterator(This,iterator) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IWebView2HttpResponseHeaders_INTERFACE_DEFINED__ */


#ifndef __IWebView2WebResourceRequest_INTERFACE_DEFINED__
#define __IWebView2WebResourceRequest_INTERFACE_DEFINED__

/* interface IWebView2WebResourceRequest */
/* [unique][object][uuid] */ 


EXTERN_C const IID IID_IWebView2WebResourceRequest;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("1b3f4122-34a0-4f5d-9089-af63c3afe375")
    IWebView2WebResourceRequest : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE get_Uri( 
            /* [retval][out] */ LPWSTR *uri) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE put_Uri( 
            /* [in] */ LPCWSTR uri) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE get_Method( 
            /* [retval][out] */ LPWSTR *method) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE put_Method( 
            /* [in] */ LPCWSTR method) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE get_Content( 
            /* [retval][out] */ IStream **content) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE put_Content( 
            /* [in] */ IStream *content) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE get_Headers( 
            /* [retval][out] */ IWebView2HttpRequestHeaders **headers) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IWebView2WebResourceRequestVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IWebView2WebResourceRequest * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IWebView2WebResourceRequest * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IWebView2WebResourceRequest * This);
        
        HRESULT ( STDMETHODCALLTYPE *get_Uri )( 
            IWebView2WebResourceRequest * This,
            /* [retval][out] */ LPWSTR *uri);
        
        HRESULT ( STDMETHODCALLTYPE *put_Uri )( 
            IWebView2WebResourceRequest * This,
            /* [in] */ LPCWSTR uri);
        
        HRESULT ( STDMETHODCALLTYPE *get_Method )( 
            IWebView2WebResourceRequest * This,
            /* [retval][out] */ LPWSTR *method);
        
        HRESULT ( STDMETHODCALLTYPE *put_Method )( 
            IWebView2WebResourceRequest * This,
            /* [in] */ LPCWSTR method);
        
        HRESULT ( STDMETHODCALLTYPE *get_Content )( 
            IWebView2WebResourceRequest * This,
            /* [retval][out] */ IStream **content);
        
        HRESULT ( STDMETHODCALLTYPE *put_Content )( 
            IWebView2WebResourceRequest * This,
            /* [in] */ IStream *content);
        
        HRESULT ( STDMETHODCALLTYPE *get_Headers )( 
            IWebView2WebResourceRequest * This,
            /* [retval][out] */ IWebView2HttpRequestHeaders **headers);
        
        END_INTERFACE
    } IWebView2WebResourceRequestVtbl;

    interface IWebView2WebResourceRequest
    {
        CONST_VTBL struct IWebView2WebResourceRequestVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWebView2WebResourceRequest_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IWebView2WebResourceRequest_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IWebView2WebResourceRequest_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IWebView2WebResourceRequest_get_Uri(This,uri)	\
    ( (This)->lpVtbl -> get_Uri(This,uri) ) 

#define IWebView2WebResourceRequest_put_Uri(This,uri)	\
    ( (This)->lpVtbl -> put_Uri(This,uri) ) 

#define IWebView2WebResourceRequest_get_Method(This,method)	\
    ( (This)->lpVtbl -> get_Method(This,method) ) 

#define IWebView2WebResourceRequest_put_Method(This,method)	\
    ( (This)->lpVtbl -> put_Method(This,method) ) 

#define IWebView2WebResourceRequest_get_Content(This,content)	\
    ( (This)->lpVtbl -> get_Content(This,content) ) 

#define IWebView2WebResourceRequest_put_Content(This,content)	\
    ( (This)->lpVtbl -> put_Content(This,content) ) 

#define IWebView2WebResourceRequest_get_Headers(This,headers)	\
    ( (This)->lpVtbl -> get_Headers(This,headers) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IWebView2WebResourceRequest_INTERFACE_DEFINED__ */


#ifndef __IWebView2WebResourceResponse_INTERFACE_DEFINED__
#define __IWebView2WebResourceResponse_INTERFACE_DEFINED__

/* interface IWebView2WebResourceResponse */
/* [unique][object][uuid] */ 


EXTERN_C const IID IID_IWebView2WebResourceResponse;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("297886a6-5fdf-472d-a97a-e336ecfe1352")
    IWebView2WebResourceResponse : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE get_Content( 
            /* [retval][out] */ IStream **content) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE put_Content( 
            /* [in] */ IStream *content) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE get_Headers( 
            /* [retval][out] */ IWebView2HttpResponseHeaders **headers) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE get_StatusCode( 
            /* [retval][out] */ int *statusCode) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE put_StatusCode( 
            /* [in] */ int statusCode) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE get_ReasonPhrase( 
            /* [retval][out] */ LPWSTR *reasonPhrase) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE put_ReasonPhrase( 
            /* [in] */ LPCWSTR reasonPhrase) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IWebView2WebResourceResponseVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IWebView2WebResourceResponse * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IWebView2WebResourceResponse * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IWebView2WebResourceResponse * This);
        
        HRESULT ( STDMETHODCALLTYPE *get_Content )( 
            IWebView2WebResourceResponse * This,
            /* [retval][out] */ IStream **content);
        
        HRESULT ( STDMETHODCALLTYPE *put_Content )( 
            IWebView2WebResourceResponse * This,
            /* [in] */ IStream *content);
        
        HRESULT ( STDMETHODCALLTYPE *get_Headers )( 
            IWebView2WebResourceResponse * This,
            /* [retval][out] */ IWebView2HttpResponseHeaders **headers);
        
        HRESULT ( STDMETHODCALLTYPE *get_StatusCode )( 
            IWebView2WebResourceResponse * This,
            /* [retval][out] */ int *statusCode);
        
        HRESULT ( STDMETHODCALLTYPE *put_StatusCode )( 
            IWebView2WebResourceResponse * This,
            /* [in] */ int statusCode);
        
        HRESULT ( STDMETHODCALLTYPE *get_ReasonPhrase )( 
            IWebView2WebResourceResponse * This,
            /* [retval][out] */ LPWSTR *reasonPhrase);
        
        HRESULT ( STDMETHODCALLTYPE *put_ReasonPhrase )( 
            IWebView2WebResourceResponse * This,
            /* [in] */ LPCWSTR reasonPhrase);
        
        END_INTERFACE
    } IWebView2WebResourceResponseVtbl;

    interface IWebView2WebResourceResponse
    {
        CONST_VTBL struct IWebView2WebResourceResponseVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWebView2WebResourceResponse_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IWebView2WebResourceResponse_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IWebView2WebResourceResponse_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IWebView2WebResourceResponse_get_Content(This,content)	\
    ( (This)->lpVtbl -> get_Content(This,content) ) 

#define IWebView2WebResourceResponse_put_Content(This,content)	\
    ( (This)->lpVtbl -> put_Content(This,content) ) 

#define IWebView2WebResourceResponse_get_Headers(This,headers)	\
    ( (This)->lpVtbl -> get_Headers(This,headers) ) 

#define IWebView2WebResourceResponse_get_StatusCode(This,statusCode)	\
    ( (This)->lpVtbl -> get_StatusCode(This,statusCode) ) 

#define IWebView2WebResourceResponse_put_StatusCode(This,statusCode)	\
    ( (This)->lpVtbl -> put_StatusCode(This,statusCode) ) 

#define IWebView2WebResourceResponse_get_ReasonPhrase(This,reasonPhrase)	\
    ( (This)->lpVtbl -> get_ReasonPhrase(This,reasonPhrase) ) 

#define IWebView2WebResourceResponse_put_ReasonPhrase(This,reasonPhrase)	\
    ( (This)->lpVtbl -> put_ReasonPhrase(This,reasonPhrase) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IWebView2WebResourceResponse_INTERFACE_DEFINED__ */


#ifndef __IWebView2NavigationStartingEventArgs_INTERFACE_DEFINED__
#define __IWebView2NavigationStartingEventArgs_INTERFACE_DEFINED__

/* interface IWebView2NavigationStartingEventArgs */
/* [unique][object][uuid] */ 


EXTERN_C const IID IID_IWebView2NavigationStartingEventArgs;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("9d7a1f73-8211-48c0-9119-686d1fb1ae02")
    IWebView2NavigationStartingEventArgs : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE get_Uri( 
            /* [retval][out] */ LPWSTR *uri) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE get_IsUserInitiated( 
            /* [retval][out] */ BOOL *isUserInitiated) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE get_IsRedirected( 
            /* [retval][out] */ BOOL *isRedirected) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE get_RequestHeaders( 
            /* [retval][out] */ IWebView2HttpRequestHeaders **requestHeaders) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE get_Cancel( 
            /* [retval][out] */ BOOL *cancel) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE put_Cancel( 
            /* [in] */ BOOL cancel) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IWebView2NavigationStartingEventArgsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IWebView2NavigationStartingEventArgs * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IWebView2NavigationStartingEventArgs * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IWebView2NavigationStartingEventArgs * This);
        
        HRESULT ( STDMETHODCALLTYPE *get_Uri )( 
            IWebView2NavigationStartingEventArgs * This,
            /* [retval][out] */ LPWSTR *uri);
        
        HRESULT ( STDMETHODCALLTYPE *get_IsUserInitiated )( 
            IWebView2NavigationStartingEventArgs * This,
            /* [retval][out] */ BOOL *isUserInitiated);
        
        HRESULT ( STDMETHODCALLTYPE *get_IsRedirected )( 
            IWebView2NavigationStartingEventArgs * This,
            /* [retval][out] */ BOOL *isRedirected);
        
        HRESULT ( STDMETHODCALLTYPE *get_RequestHeaders )( 
            IWebView2NavigationStartingEventArgs * This,
            /* [retval][out] */ IWebView2HttpRequestHeaders **requestHeaders);
        
        HRESULT ( STDMETHODCALLTYPE *get_Cancel )( 
            IWebView2NavigationStartingEventArgs * This,
            /* [retval][out] */ BOOL *cancel);
        
        HRESULT ( STDMETHODCALLTYPE *put_Cancel )( 
            IWebView2NavigationStartingEventArgs * This,
            /* [in] */ BOOL cancel);
        
        END_INTERFACE
    } IWebView2NavigationStartingEventArgsVtbl;

    interface IWebView2NavigationStartingEventArgs
    {
        CONST_VTBL struct IWebView2NavigationStartingEventArgsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWebView2NavigationStartingEventArgs_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IWebView2NavigationStartingEventArgs_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IWebView2NavigationStartingEventArgs_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IWebView2NavigationStartingEventArgs_get_Uri(This,uri)	\
    ( (This)->lpVtbl -> get_Uri(This,uri) ) 

#define IWebView2NavigationStartingEventArgs_get_IsUserInitiated(This,isUserInitiated)	\
    ( (This)->lpVtbl -> get_IsUserInitiated(This,isUserInitiated) ) 

#define IWebView2NavigationStartingEventArgs_get_IsRedirected(This,isRedirected)	\
    ( (This)->lpVtbl -> get_IsRedirected(This,isRedirected) ) 

#define IWebView2NavigationStartingEventArgs_get_RequestHeaders(This,requestHeaders)	\
    ( (This)->lpVtbl -> get_RequestHeaders(This,requestHeaders) ) 

#define IWebView2NavigationStartingEventArgs_get_Cancel(This,cancel)	\
    ( (This)->lpVtbl -> get_Cancel(This,cancel) ) 

#define IWebView2NavigationStartingEventArgs_put_Cancel(This,cancel)	\
    ( (This)->lpVtbl -> put_Cancel(This,cancel) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IWebView2NavigationStartingEventArgs_INTERFACE_DEFINED__ */


#ifndef __IWebView2NavigationStartingEventHandler_INTERFACE_DEFINED__
#define __IWebView2NavigationStartingEventHandler_INTERFACE_DEFINED__

/* interface IWebView2NavigationStartingEventHandler */
/* [unique][object][uuid] */ 


EXTERN_C const IID IID_IWebView2NavigationStartingEventHandler;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("34896570-DC04-40F9-A2DA-8582551A707D")
    IWebView2NavigationStartingEventHandler : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Invoke( 
            /* [in] */ IWebView2WebView *webview,
            /* [in] */ IWebView2NavigationStartingEventArgs *args) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IWebView2NavigationStartingEventHandlerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IWebView2NavigationStartingEventHandler * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IWebView2NavigationStartingEventHandler * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IWebView2NavigationStartingEventHandler * This);
        
        HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IWebView2NavigationStartingEventHandler * This,
            /* [in] */ IWebView2WebView *webview,
            /* [in] */ IWebView2NavigationStartingEventArgs *args);
        
        END_INTERFACE
    } IWebView2NavigationStartingEventHandlerVtbl;

    interface IWebView2NavigationStartingEventHandler
    {
        CONST_VTBL struct IWebView2NavigationStartingEventHandlerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWebView2NavigationStartingEventHandler_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IWebView2NavigationStartingEventHandler_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IWebView2NavigationStartingEventHandler_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IWebView2NavigationStartingEventHandler_Invoke(This,webview,args)	\
    ( (This)->lpVtbl -> Invoke(This,webview,args) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IWebView2NavigationStartingEventHandler_INTERFACE_DEFINED__ */


#ifndef __IWebView2DocumentStateChangedEventArgs_INTERFACE_DEFINED__
#define __IWebView2DocumentStateChangedEventArgs_INTERFACE_DEFINED__

/* interface IWebView2DocumentStateChangedEventArgs */
/* [unique][object][uuid] */ 


EXTERN_C const IID IID_IWebView2DocumentStateChangedEventArgs;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("3A38CB7F-EFC1-41B4-87FC-5AFCEE27C8ED")
    IWebView2DocumentStateChangedEventArgs : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE get_IsNewDocument( 
            /* [retval][out] */ BOOL *isNewDocument) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE get_IsErrorPage( 
            /* [retval][out] */ BOOL *isErrorPage) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IWebView2DocumentStateChangedEventArgsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IWebView2DocumentStateChangedEventArgs * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IWebView2DocumentStateChangedEventArgs * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IWebView2DocumentStateChangedEventArgs * This);
        
        HRESULT ( STDMETHODCALLTYPE *get_IsNewDocument )( 
            IWebView2DocumentStateChangedEventArgs * This,
            /* [retval][out] */ BOOL *isNewDocument);
        
        HRESULT ( STDMETHODCALLTYPE *get_IsErrorPage )( 
            IWebView2DocumentStateChangedEventArgs * This,
            /* [retval][out] */ BOOL *isErrorPage);
        
        END_INTERFACE
    } IWebView2DocumentStateChangedEventArgsVtbl;

    interface IWebView2DocumentStateChangedEventArgs
    {
        CONST_VTBL struct IWebView2DocumentStateChangedEventArgsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWebView2DocumentStateChangedEventArgs_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IWebView2DocumentStateChangedEventArgs_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IWebView2DocumentStateChangedEventArgs_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IWebView2DocumentStateChangedEventArgs_get_IsNewDocument(This,isNewDocument)	\
    ( (This)->lpVtbl -> get_IsNewDocument(This,isNewDocument) ) 

#define IWebView2DocumentStateChangedEventArgs_get_IsErrorPage(This,isErrorPage)	\
    ( (This)->lpVtbl -> get_IsErrorPage(This,isErrorPage) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IWebView2DocumentStateChangedEventArgs_INTERFACE_DEFINED__ */


#ifndef __IWebView2DocumentStateChangedEventHandler_INTERFACE_DEFINED__
#define __IWebView2DocumentStateChangedEventHandler_INTERFACE_DEFINED__

/* interface IWebView2DocumentStateChangedEventHandler */
/* [unique][object][uuid] */ 


EXTERN_C const IID IID_IWebView2DocumentStateChangedEventHandler;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("88E66305-3A5A-4E7F-9C76-2EBFC138CAFD")
    IWebView2DocumentStateChangedEventHandler : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Invoke( 
            /* [in] */ IWebView2WebView *webview,
            /* [in] */ IWebView2DocumentStateChangedEventArgs *args) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IWebView2DocumentStateChangedEventHandlerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IWebView2DocumentStateChangedEventHandler * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IWebView2DocumentStateChangedEventHandler * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IWebView2DocumentStateChangedEventHandler * This);
        
        HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IWebView2DocumentStateChangedEventHandler * This,
            /* [in] */ IWebView2WebView *webview,
            /* [in] */ IWebView2DocumentStateChangedEventArgs *args);
        
        END_INTERFACE
    } IWebView2DocumentStateChangedEventHandlerVtbl;

    interface IWebView2DocumentStateChangedEventHandler
    {
        CONST_VTBL struct IWebView2DocumentStateChangedEventHandlerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWebView2DocumentStateChangedEventHandler_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IWebView2DocumentStateChangedEventHandler_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IWebView2DocumentStateChangedEventHandler_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IWebView2DocumentStateChangedEventHandler_Invoke(This,webview,args)	\
    ( (This)->lpVtbl -> Invoke(This,webview,args) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IWebView2DocumentStateChangedEventHandler_INTERFACE_DEFINED__ */


#ifndef __IWebView2ScriptDialogOpeningEventArgs_INTERFACE_DEFINED__
#define __IWebView2ScriptDialogOpeningEventArgs_INTERFACE_DEFINED__

/* interface IWebView2ScriptDialogOpeningEventArgs */
/* [unique][object][uuid] */ 


EXTERN_C const IID IID_IWebView2ScriptDialogOpeningEventArgs;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("ABB0484E-8D4F-4BEA-9058-B0287221A976")
    IWebView2ScriptDialogOpeningEventArgs : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE get_Uri( 
            /* [retval][out] */ LPWSTR *uri) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE get_Kind( 
            /* [retval][out] */ WEBVIEW2_SCRIPT_DIALOG_KIND *kind) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE get_Message( 
            /* [retval][out] */ LPWSTR *message) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Accept( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE get_DefaultText( 
            /* [retval][out] */ LPWSTR *defaultText) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE get_ResultText( 
            /* [retval][out] */ LPWSTR *resultText) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE put_ResultText( 
            /* [in] */ LPCWSTR resultText) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetDeferral( 
            /* [retval][out] */ IWebView2Deferral **deferral) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IWebView2ScriptDialogOpeningEventArgsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IWebView2ScriptDialogOpeningEventArgs * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IWebView2ScriptDialogOpeningEventArgs * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IWebView2ScriptDialogOpeningEventArgs * This);
        
        HRESULT ( STDMETHODCALLTYPE *get_Uri )( 
            IWebView2ScriptDialogOpeningEventArgs * This,
            /* [retval][out] */ LPWSTR *uri);
        
        HRESULT ( STDMETHODCALLTYPE *get_Kind )( 
            IWebView2ScriptDialogOpeningEventArgs * This,
            /* [retval][out] */ WEBVIEW2_SCRIPT_DIALOG_KIND *kind);
        
        HRESULT ( STDMETHODCALLTYPE *get_Message )( 
            IWebView2ScriptDialogOpeningEventArgs * This,
            /* [retval][out] */ LPWSTR *message);
        
        HRESULT ( STDMETHODCALLTYPE *Accept )( 
            IWebView2ScriptDialogOpeningEventArgs * This);
        
        HRESULT ( STDMETHODCALLTYPE *get_DefaultText )( 
            IWebView2ScriptDialogOpeningEventArgs * This,
            /* [retval][out] */ LPWSTR *defaultText);
        
        HRESULT ( STDMETHODCALLTYPE *get_ResultText )( 
            IWebView2ScriptDialogOpeningEventArgs * This,
            /* [retval][out] */ LPWSTR *resultText);
        
        HRESULT ( STDMETHODCALLTYPE *put_ResultText )( 
            IWebView2ScriptDialogOpeningEventArgs * This,
            /* [in] */ LPCWSTR resultText);
        
        HRESULT ( STDMETHODCALLTYPE *GetDeferral )( 
            IWebView2ScriptDialogOpeningEventArgs * This,
            /* [retval][out] */ IWebView2Deferral **deferral);
        
        END_INTERFACE
    } IWebView2ScriptDialogOpeningEventArgsVtbl;

    interface IWebView2ScriptDialogOpeningEventArgs
    {
        CONST_VTBL struct IWebView2ScriptDialogOpeningEventArgsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWebView2ScriptDialogOpeningEventArgs_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IWebView2ScriptDialogOpeningEventArgs_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IWebView2ScriptDialogOpeningEventArgs_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IWebView2ScriptDialogOpeningEventArgs_get_Uri(This,uri)	\
    ( (This)->lpVtbl -> get_Uri(This,uri) ) 

#define IWebView2ScriptDialogOpeningEventArgs_get_Kind(This,kind)	\
    ( (This)->lpVtbl -> get_Kind(This,kind) ) 

#define IWebView2ScriptDialogOpeningEventArgs_get_Message(This,message)	\
    ( (This)->lpVtbl -> get_Message(This,message) ) 

#define IWebView2ScriptDialogOpeningEventArgs_Accept(This)	\
    ( (This)->lpVtbl -> Accept(This) ) 

#define IWebView2ScriptDialogOpeningEventArgs_get_DefaultText(This,defaultText)	\
    ( (This)->lpVtbl -> get_DefaultText(This,defaultText) ) 

#define IWebView2ScriptDialogOpeningEventArgs_get_ResultText(This,resultText)	\
    ( (This)->lpVtbl -> get_ResultText(This,resultText) ) 

#define IWebView2ScriptDialogOpeningEventArgs_put_ResultText(This,resultText)	\
    ( (This)->lpVtbl -> put_ResultText(This,resultText) ) 

#define IWebView2ScriptDialogOpeningEventArgs_GetDeferral(This,deferral)	\
    ( (This)->lpVtbl -> GetDeferral(This,deferral) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IWebView2ScriptDialogOpeningEventArgs_INTERFACE_DEFINED__ */


#ifndef __IWebView2ScriptDialogOpeningEventHandler_INTERFACE_DEFINED__
#define __IWebView2ScriptDialogOpeningEventHandler_INTERFACE_DEFINED__

/* interface IWebView2ScriptDialogOpeningEventHandler */
/* [unique][object][uuid] */ 


EXTERN_C const IID IID_IWebView2ScriptDialogOpeningEventHandler;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("8EAF9A50-2AF9-45DA-9AC5-F80F4147180E")
    IWebView2ScriptDialogOpeningEventHandler : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Invoke( 
            /* [in] */ IWebView2WebView *webview,
            /* [in] */ IWebView2ScriptDialogOpeningEventArgs *args) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IWebView2ScriptDialogOpeningEventHandlerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IWebView2ScriptDialogOpeningEventHandler * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IWebView2ScriptDialogOpeningEventHandler * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IWebView2ScriptDialogOpeningEventHandler * This);
        
        HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IWebView2ScriptDialogOpeningEventHandler * This,
            /* [in] */ IWebView2WebView *webview,
            /* [in] */ IWebView2ScriptDialogOpeningEventArgs *args);
        
        END_INTERFACE
    } IWebView2ScriptDialogOpeningEventHandlerVtbl;

    interface IWebView2ScriptDialogOpeningEventHandler
    {
        CONST_VTBL struct IWebView2ScriptDialogOpeningEventHandlerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWebView2ScriptDialogOpeningEventHandler_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IWebView2ScriptDialogOpeningEventHandler_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IWebView2ScriptDialogOpeningEventHandler_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IWebView2ScriptDialogOpeningEventHandler_Invoke(This,webview,args)	\
    ( (This)->lpVtbl -> Invoke(This,webview,args) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IWebView2ScriptDialogOpeningEventHandler_INTERFACE_DEFINED__ */


#ifndef __IWebView2NavigationCompletedEventArgs_INTERFACE_DEFINED__
#define __IWebView2NavigationCompletedEventArgs_INTERFACE_DEFINED__

/* interface IWebView2NavigationCompletedEventArgs */
/* [unique][object][uuid] */ 


EXTERN_C const IID IID_IWebView2NavigationCompletedEventArgs;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("48655B1F-3F52-4835-B7AA-7D95F7D7587E")
    IWebView2NavigationCompletedEventArgs : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE get_IsSuccess( 
            /* [retval][out] */ BOOL *isSuccess) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE get_WebErrorStatus( 
            /* [retval][out] */ WEBVIEW2_WEB_ERROR_STATUS *WEBVIEW2_WEB_ERROR_STATUS) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IWebView2NavigationCompletedEventArgsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IWebView2NavigationCompletedEventArgs * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IWebView2NavigationCompletedEventArgs * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IWebView2NavigationCompletedEventArgs * This);
        
        HRESULT ( STDMETHODCALLTYPE *get_IsSuccess )( 
            IWebView2NavigationCompletedEventArgs * This,
            /* [retval][out] */ BOOL *isSuccess);
        
        HRESULT ( STDMETHODCALLTYPE *get_WebErrorStatus )( 
            IWebView2NavigationCompletedEventArgs * This,
            /* [retval][out] */ WEBVIEW2_WEB_ERROR_STATUS *WEBVIEW2_WEB_ERROR_STATUS);
        
        END_INTERFACE
    } IWebView2NavigationCompletedEventArgsVtbl;

    interface IWebView2NavigationCompletedEventArgs
    {
        CONST_VTBL struct IWebView2NavigationCompletedEventArgsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWebView2NavigationCompletedEventArgs_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IWebView2NavigationCompletedEventArgs_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IWebView2NavigationCompletedEventArgs_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IWebView2NavigationCompletedEventArgs_get_IsSuccess(This,isSuccess)	\
    ( (This)->lpVtbl -> get_IsSuccess(This,isSuccess) ) 

#define IWebView2NavigationCompletedEventArgs_get_WebErrorStatus(This,WEBVIEW2_WEB_ERROR_STATUS)	\
    ( (This)->lpVtbl -> get_WebErrorStatus(This,WEBVIEW2_WEB_ERROR_STATUS) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IWebView2NavigationCompletedEventArgs_INTERFACE_DEFINED__ */


#ifndef __IWebView2NavigationCompletedEventHandler_INTERFACE_DEFINED__
#define __IWebView2NavigationCompletedEventHandler_INTERFACE_DEFINED__

/* interface IWebView2NavigationCompletedEventHandler */
/* [unique][object][uuid] */ 


EXTERN_C const IID IID_IWebView2NavigationCompletedEventHandler;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("DCEB3A27-C8C0-4DE7-889D-AF3DE80EDB3C")
    IWebView2NavigationCompletedEventHandler : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Invoke( 
            /* [in] */ IWebView2WebView *webview,
            /* [in] */ IWebView2NavigationCompletedEventArgs *args) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IWebView2NavigationCompletedEventHandlerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IWebView2NavigationCompletedEventHandler * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IWebView2NavigationCompletedEventHandler * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IWebView2NavigationCompletedEventHandler * This);
        
        HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IWebView2NavigationCompletedEventHandler * This,
            /* [in] */ IWebView2WebView *webview,
            /* [in] */ IWebView2NavigationCompletedEventArgs *args);
        
        END_INTERFACE
    } IWebView2NavigationCompletedEventHandlerVtbl;

    interface IWebView2NavigationCompletedEventHandler
    {
        CONST_VTBL struct IWebView2NavigationCompletedEventHandlerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWebView2NavigationCompletedEventHandler_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IWebView2NavigationCompletedEventHandler_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IWebView2NavigationCompletedEventHandler_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IWebView2NavigationCompletedEventHandler_Invoke(This,webview,args)	\
    ( (This)->lpVtbl -> Invoke(This,webview,args) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IWebView2NavigationCompletedEventHandler_INTERFACE_DEFINED__ */


#ifndef __IWebView2PermissionRequestedEventArgs_INTERFACE_DEFINED__
#define __IWebView2PermissionRequestedEventArgs_INTERFACE_DEFINED__

/* interface IWebView2PermissionRequestedEventArgs */
/* [unique][object][uuid] */ 


EXTERN_C const IID IID_IWebView2PermissionRequestedEventArgs;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("8D8DA0E4-A071-486F-85AA-31B4B2BADC61")
    IWebView2PermissionRequestedEventArgs : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE get_Uri( 
            /* [retval][out] */ LPWSTR *uri) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE get_PermissionType( 
            /* [retval][out] */ WEBVIEW2_PERMISSION_TYPE *value) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE get_IsUserInitiated( 
            /* [retval][out] */ BOOL *isUserInitiated) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE get_State( 
            /* [retval][out] */ WEBVIEW2_PERMISSION_STATE *value) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE put_State( 
            /* [in] */ WEBVIEW2_PERMISSION_STATE value) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetDeferral( 
            /* [retval][out] */ IWebView2Deferral **deferral) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IWebView2PermissionRequestedEventArgsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IWebView2PermissionRequestedEventArgs * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IWebView2PermissionRequestedEventArgs * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IWebView2PermissionRequestedEventArgs * This);
        
        HRESULT ( STDMETHODCALLTYPE *get_Uri )( 
            IWebView2PermissionRequestedEventArgs * This,
            /* [retval][out] */ LPWSTR *uri);
        
        HRESULT ( STDMETHODCALLTYPE *get_PermissionType )( 
            IWebView2PermissionRequestedEventArgs * This,
            /* [retval][out] */ WEBVIEW2_PERMISSION_TYPE *value);
        
        HRESULT ( STDMETHODCALLTYPE *get_IsUserInitiated )( 
            IWebView2PermissionRequestedEventArgs * This,
            /* [retval][out] */ BOOL *isUserInitiated);
        
        HRESULT ( STDMETHODCALLTYPE *get_State )( 
            IWebView2PermissionRequestedEventArgs * This,
            /* [retval][out] */ WEBVIEW2_PERMISSION_STATE *value);
        
        HRESULT ( STDMETHODCALLTYPE *put_State )( 
            IWebView2PermissionRequestedEventArgs * This,
            /* [in] */ WEBVIEW2_PERMISSION_STATE value);
        
        HRESULT ( STDMETHODCALLTYPE *GetDeferral )( 
            IWebView2PermissionRequestedEventArgs * This,
            /* [retval][out] */ IWebView2Deferral **deferral);
        
        END_INTERFACE
    } IWebView2PermissionRequestedEventArgsVtbl;

    interface IWebView2PermissionRequestedEventArgs
    {
        CONST_VTBL struct IWebView2PermissionRequestedEventArgsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWebView2PermissionRequestedEventArgs_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IWebView2PermissionRequestedEventArgs_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IWebView2PermissionRequestedEventArgs_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IWebView2PermissionRequestedEventArgs_get_Uri(This,uri)	\
    ( (This)->lpVtbl -> get_Uri(This,uri) ) 

#define IWebView2PermissionRequestedEventArgs_get_PermissionType(This,value)	\
    ( (This)->lpVtbl -> get_PermissionType(This,value) ) 

#define IWebView2PermissionRequestedEventArgs_get_IsUserInitiated(This,isUserInitiated)	\
    ( (This)->lpVtbl -> get_IsUserInitiated(This,isUserInitiated) ) 

#define IWebView2PermissionRequestedEventArgs_get_State(This,value)	\
    ( (This)->lpVtbl -> get_State(This,value) ) 

#define IWebView2PermissionRequestedEventArgs_put_State(This,value)	\
    ( (This)->lpVtbl -> put_State(This,value) ) 

#define IWebView2PermissionRequestedEventArgs_GetDeferral(This,deferral)	\
    ( (This)->lpVtbl -> GetDeferral(This,deferral) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IWebView2PermissionRequestedEventArgs_INTERFACE_DEFINED__ */


#ifndef __IWebView2PermissionRequestedEventHandler_INTERFACE_DEFINED__
#define __IWebView2PermissionRequestedEventHandler_INTERFACE_DEFINED__

/* interface IWebView2PermissionRequestedEventHandler */
/* [unique][object][uuid] */ 


EXTERN_C const IID IID_IWebView2PermissionRequestedEventHandler;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("C5DA3C20-95AC-4345-B3C9-5FCA3B92C9DB")
    IWebView2PermissionRequestedEventHandler : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Invoke( 
            /* [in] */ IWebView2WebView *webview,
            /* [in] */ IWebView2PermissionRequestedEventArgs *args) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IWebView2PermissionRequestedEventHandlerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IWebView2PermissionRequestedEventHandler * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IWebView2PermissionRequestedEventHandler * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IWebView2PermissionRequestedEventHandler * This);
        
        HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IWebView2PermissionRequestedEventHandler * This,
            /* [in] */ IWebView2WebView *webview,
            /* [in] */ IWebView2PermissionRequestedEventArgs *args);
        
        END_INTERFACE
    } IWebView2PermissionRequestedEventHandlerVtbl;

    interface IWebView2PermissionRequestedEventHandler
    {
        CONST_VTBL struct IWebView2PermissionRequestedEventHandlerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWebView2PermissionRequestedEventHandler_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IWebView2PermissionRequestedEventHandler_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IWebView2PermissionRequestedEventHandler_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IWebView2PermissionRequestedEventHandler_Invoke(This,webview,args)	\
    ( (This)->lpVtbl -> Invoke(This,webview,args) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IWebView2PermissionRequestedEventHandler_INTERFACE_DEFINED__ */


#ifndef __IWebView2AddScriptToExecuteOnDocumentCreatedCompletedHandler_INTERFACE_DEFINED__
#define __IWebView2AddScriptToExecuteOnDocumentCreatedCompletedHandler_INTERFACE_DEFINED__

/* interface IWebView2AddScriptToExecuteOnDocumentCreatedCompletedHandler */
/* [unique][object][uuid] */ 


EXTERN_C const IID IID_IWebView2AddScriptToExecuteOnDocumentCreatedCompletedHandler;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("EE07AA7F-5DAF-4C00-9C0B-5F736213C92D")
    IWebView2AddScriptToExecuteOnDocumentCreatedCompletedHandler : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Invoke( 
            /* [in] */ HRESULT errorCode,
            /* [in] */ LPCWSTR id) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IWebView2AddScriptToExecuteOnDocumentCreatedCompletedHandlerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IWebView2AddScriptToExecuteOnDocumentCreatedCompletedHandler * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IWebView2AddScriptToExecuteOnDocumentCreatedCompletedHandler * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IWebView2AddScriptToExecuteOnDocumentCreatedCompletedHandler * This);
        
        HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IWebView2AddScriptToExecuteOnDocumentCreatedCompletedHandler * This,
            /* [in] */ HRESULT errorCode,
            /* [in] */ LPCWSTR id);
        
        END_INTERFACE
    } IWebView2AddScriptToExecuteOnDocumentCreatedCompletedHandlerVtbl;

    interface IWebView2AddScriptToExecuteOnDocumentCreatedCompletedHandler
    {
        CONST_VTBL struct IWebView2AddScriptToExecuteOnDocumentCreatedCompletedHandlerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWebView2AddScriptToExecuteOnDocumentCreatedCompletedHandler_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IWebView2AddScriptToExecuteOnDocumentCreatedCompletedHandler_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IWebView2AddScriptToExecuteOnDocumentCreatedCompletedHandler_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IWebView2AddScriptToExecuteOnDocumentCreatedCompletedHandler_Invoke(This,errorCode,id)	\
    ( (This)->lpVtbl -> Invoke(This,errorCode,id) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IWebView2AddScriptToExecuteOnDocumentCreatedCompletedHandler_INTERFACE_DEFINED__ */


#ifndef __IWebView2ExecuteScriptCompletedHandler_INTERFACE_DEFINED__
#define __IWebView2ExecuteScriptCompletedHandler_INTERFACE_DEFINED__

/* interface IWebView2ExecuteScriptCompletedHandler */
/* [unique][object][uuid] */ 


EXTERN_C const IID IID_IWebView2ExecuteScriptCompletedHandler;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("F5AC0E3B-8B92-45E5-ABEF-DB8518EFFF27")
    IWebView2ExecuteScriptCompletedHandler : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Invoke( 
            /* [in] */ HRESULT errorCode,
            /* [in] */ LPCWSTR resultObjectAsJson) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IWebView2ExecuteScriptCompletedHandlerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IWebView2ExecuteScriptCompletedHandler * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IWebView2ExecuteScriptCompletedHandler * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IWebView2ExecuteScriptCompletedHandler * This);
        
        HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IWebView2ExecuteScriptCompletedHandler * This,
            /* [in] */ HRESULT errorCode,
            /* [in] */ LPCWSTR resultObjectAsJson);
        
        END_INTERFACE
    } IWebView2ExecuteScriptCompletedHandlerVtbl;

    interface IWebView2ExecuteScriptCompletedHandler
    {
        CONST_VTBL struct IWebView2ExecuteScriptCompletedHandlerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWebView2ExecuteScriptCompletedHandler_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IWebView2ExecuteScriptCompletedHandler_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IWebView2ExecuteScriptCompletedHandler_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IWebView2ExecuteScriptCompletedHandler_Invoke(This,errorCode,resultObjectAsJson)	\
    ( (This)->lpVtbl -> Invoke(This,errorCode,resultObjectAsJson) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IWebView2ExecuteScriptCompletedHandler_INTERFACE_DEFINED__ */


#ifndef __IWebView2WebResourceRequestedEventArgs_INTERFACE_DEFINED__
#define __IWebView2WebResourceRequestedEventArgs_INTERFACE_DEFINED__

/* interface IWebView2WebResourceRequestedEventArgs */
/* [unique][object][uuid] */ 


EXTERN_C const IID IID_IWebView2WebResourceRequestedEventArgs;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("D8B1DD71-B9AD-4EEB-ABE3-87E7EFC5D37F")
    IWebView2WebResourceRequestedEventArgs : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE get_Request( 
            /* [retval][out] */ IWebView2WebResourceRequest **request) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE get_Response( 
            /* [retval][out] */ IWebView2WebResourceResponse **response) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE put_Response( 
            /* [in] */ IWebView2WebResourceResponse *response) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetDeferral( 
            /* [retval][out] */ IWebView2Deferral **deferral) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IWebView2WebResourceRequestedEventArgsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IWebView2WebResourceRequestedEventArgs * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IWebView2WebResourceRequestedEventArgs * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IWebView2WebResourceRequestedEventArgs * This);
        
        HRESULT ( STDMETHODCALLTYPE *get_Request )( 
            IWebView2WebResourceRequestedEventArgs * This,
            /* [retval][out] */ IWebView2WebResourceRequest **request);
        
        HRESULT ( STDMETHODCALLTYPE *get_Response )( 
            IWebView2WebResourceRequestedEventArgs * This,
            /* [retval][out] */ IWebView2WebResourceResponse **response);
        
        HRESULT ( STDMETHODCALLTYPE *put_Response )( 
            IWebView2WebResourceRequestedEventArgs * This,
            /* [in] */ IWebView2WebResourceResponse *response);
        
        HRESULT ( STDMETHODCALLTYPE *GetDeferral )( 
            IWebView2WebResourceRequestedEventArgs * This,
            /* [retval][out] */ IWebView2Deferral **deferral);
        
        END_INTERFACE
    } IWebView2WebResourceRequestedEventArgsVtbl;

    interface IWebView2WebResourceRequestedEventArgs
    {
        CONST_VTBL struct IWebView2WebResourceRequestedEventArgsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWebView2WebResourceRequestedEventArgs_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IWebView2WebResourceRequestedEventArgs_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IWebView2WebResourceRequestedEventArgs_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IWebView2WebResourceRequestedEventArgs_get_Request(This,request)	\
    ( (This)->lpVtbl -> get_Request(This,request) ) 

#define IWebView2WebResourceRequestedEventArgs_get_Response(This,response)	\
    ( (This)->lpVtbl -> get_Response(This,response) ) 

#define IWebView2WebResourceRequestedEventArgs_put_Response(This,response)	\
    ( (This)->lpVtbl -> put_Response(This,response) ) 

#define IWebView2WebResourceRequestedEventArgs_GetDeferral(This,deferral)	\
    ( (This)->lpVtbl -> GetDeferral(This,deferral) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IWebView2WebResourceRequestedEventArgs_INTERFACE_DEFINED__ */


#ifndef __IWebView2WebResourceRequestedEventArgs2_INTERFACE_DEFINED__
#define __IWebView2WebResourceRequestedEventArgs2_INTERFACE_DEFINED__

/* interface IWebView2WebResourceRequestedEventArgs2 */
/* [unique][object][uuid] */ 


EXTERN_C const IID IID_IWebView2WebResourceRequestedEventArgs2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("b38f6f16-9568-4f12-9996-dca7a06299f4")
    IWebView2WebResourceRequestedEventArgs2 : public IWebView2WebResourceRequestedEventArgs
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE get_ResourceContext( 
            WEBVIEW2_WEB_RESOURCE_CONTEXT *context) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IWebView2WebResourceRequestedEventArgs2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IWebView2WebResourceRequestedEventArgs2 * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IWebView2WebResourceRequestedEventArgs2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IWebView2WebResourceRequestedEventArgs2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *get_Request )( 
            IWebView2WebResourceRequestedEventArgs2 * This,
            /* [retval][out] */ IWebView2WebResourceRequest **request);
        
        HRESULT ( STDMETHODCALLTYPE *get_Response )( 
            IWebView2WebResourceRequestedEventArgs2 * This,
            /* [retval][out] */ IWebView2WebResourceResponse **response);
        
        HRESULT ( STDMETHODCALLTYPE *put_Response )( 
            IWebView2WebResourceRequestedEventArgs2 * This,
            /* [in] */ IWebView2WebResourceResponse *response);
        
        HRESULT ( STDMETHODCALLTYPE *GetDeferral )( 
            IWebView2WebResourceRequestedEventArgs2 * This,
            /* [retval][out] */ IWebView2Deferral **deferral);
        
        HRESULT ( STDMETHODCALLTYPE *get_ResourceContext )( 
            IWebView2WebResourceRequestedEventArgs2 * This,
            WEBVIEW2_WEB_RESOURCE_CONTEXT *context);
        
        END_INTERFACE
    } IWebView2WebResourceRequestedEventArgs2Vtbl;

    interface IWebView2WebResourceRequestedEventArgs2
    {
        CONST_VTBL struct IWebView2WebResourceRequestedEventArgs2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWebView2WebResourceRequestedEventArgs2_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IWebView2WebResourceRequestedEventArgs2_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IWebView2WebResourceRequestedEventArgs2_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IWebView2WebResourceRequestedEventArgs2_get_Request(This,request)	\
    ( (This)->lpVtbl -> get_Request(This,request) ) 

#define IWebView2WebResourceRequestedEventArgs2_get_Response(This,response)	\
    ( (This)->lpVtbl -> get_Response(This,response) ) 

#define IWebView2WebResourceRequestedEventArgs2_put_Response(This,response)	\
    ( (This)->lpVtbl -> put_Response(This,response) ) 

#define IWebView2WebResourceRequestedEventArgs2_GetDeferral(This,deferral)	\
    ( (This)->lpVtbl -> GetDeferral(This,deferral) ) 


#define IWebView2WebResourceRequestedEventArgs2_get_ResourceContext(This,context)	\
    ( (This)->lpVtbl -> get_ResourceContext(This,context) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IWebView2WebResourceRequestedEventArgs2_INTERFACE_DEFINED__ */


#ifndef __IWebView2WebResourceRequestedEventHandler_INTERFACE_DEFINED__
#define __IWebView2WebResourceRequestedEventHandler_INTERFACE_DEFINED__

/* interface IWebView2WebResourceRequestedEventHandler */
/* [unique][object][uuid] */ 


EXTERN_C const IID IID_IWebView2WebResourceRequestedEventHandler;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("E2AE08C1-4F67-4348-AE05-C89CB14C2ADD")
    IWebView2WebResourceRequestedEventHandler : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Invoke( 
            /* [in] */ IWebView2WebView *webview,
            /* [in] */ IWebView2WebResourceRequestedEventArgs *args) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IWebView2WebResourceRequestedEventHandlerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IWebView2WebResourceRequestedEventHandler * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IWebView2WebResourceRequestedEventHandler * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IWebView2WebResourceRequestedEventHandler * This);
        
        HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IWebView2WebResourceRequestedEventHandler * This,
            /* [in] */ IWebView2WebView *webview,
            /* [in] */ IWebView2WebResourceRequestedEventArgs *args);
        
        END_INTERFACE
    } IWebView2WebResourceRequestedEventHandlerVtbl;

    interface IWebView2WebResourceRequestedEventHandler
    {
        CONST_VTBL struct IWebView2WebResourceRequestedEventHandlerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWebView2WebResourceRequestedEventHandler_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IWebView2WebResourceRequestedEventHandler_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IWebView2WebResourceRequestedEventHandler_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IWebView2WebResourceRequestedEventHandler_Invoke(This,webview,args)	\
    ( (This)->lpVtbl -> Invoke(This,webview,args) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IWebView2WebResourceRequestedEventHandler_INTERFACE_DEFINED__ */


#ifndef __IWebView2CapturePreviewCompletedHandler_INTERFACE_DEFINED__
#define __IWebView2CapturePreviewCompletedHandler_INTERFACE_DEFINED__

/* interface IWebView2CapturePreviewCompletedHandler */
/* [unique][object][uuid] */ 


EXTERN_C const IID IID_IWebView2CapturePreviewCompletedHandler;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("5755B27A-3FCD-4E01-B368-06834A5AFCDC")
    IWebView2CapturePreviewCompletedHandler : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Invoke( 
            /* [in] */ HRESULT result) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IWebView2CapturePreviewCompletedHandlerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IWebView2CapturePreviewCompletedHandler * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IWebView2CapturePreviewCompletedHandler * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IWebView2CapturePreviewCompletedHandler * This);
        
        HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IWebView2CapturePreviewCompletedHandler * This,
            /* [in] */ HRESULT result);
        
        END_INTERFACE
    } IWebView2CapturePreviewCompletedHandlerVtbl;

    interface IWebView2CapturePreviewCompletedHandler
    {
        CONST_VTBL struct IWebView2CapturePreviewCompletedHandlerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWebView2CapturePreviewCompletedHandler_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IWebView2CapturePreviewCompletedHandler_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IWebView2CapturePreviewCompletedHandler_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IWebView2CapturePreviewCompletedHandler_Invoke(This,result)	\
    ( (This)->lpVtbl -> Invoke(This,result) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IWebView2CapturePreviewCompletedHandler_INTERFACE_DEFINED__ */


#ifndef __IWebView2FocusChangedEventHandler_INTERFACE_DEFINED__
#define __IWebView2FocusChangedEventHandler_INTERFACE_DEFINED__

/* interface IWebView2FocusChangedEventHandler */
/* [unique][object][uuid] */ 


EXTERN_C const IID IID_IWebView2FocusChangedEventHandler;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("76BDBECE-02CC-4E56-AD81-5F808E8572A6")
    IWebView2FocusChangedEventHandler : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Invoke( 
            /* [in] */ IWebView2WebView *webview,
            /* [in] */ IUnknown *args) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IWebView2FocusChangedEventHandlerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IWebView2FocusChangedEventHandler * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IWebView2FocusChangedEventHandler * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IWebView2FocusChangedEventHandler * This);
        
        HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IWebView2FocusChangedEventHandler * This,
            /* [in] */ IWebView2WebView *webview,
            /* [in] */ IUnknown *args);
        
        END_INTERFACE
    } IWebView2FocusChangedEventHandlerVtbl;

    interface IWebView2FocusChangedEventHandler
    {
        CONST_VTBL struct IWebView2FocusChangedEventHandlerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWebView2FocusChangedEventHandler_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IWebView2FocusChangedEventHandler_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IWebView2FocusChangedEventHandler_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IWebView2FocusChangedEventHandler_Invoke(This,webview,args)	\
    ( (This)->lpVtbl -> Invoke(This,webview,args) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IWebView2FocusChangedEventHandler_INTERFACE_DEFINED__ */


#ifndef __IWebView2MoveFocusRequestedEventArgs_INTERFACE_DEFINED__
#define __IWebView2MoveFocusRequestedEventArgs_INTERFACE_DEFINED__

/* interface IWebView2MoveFocusRequestedEventArgs */
/* [unique][object][uuid] */ 


EXTERN_C const IID IID_IWebView2MoveFocusRequestedEventArgs;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("64AF5AE3-27A1-47E0-8901-95119C1BA95B")
    IWebView2MoveFocusRequestedEventArgs : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE get_Reason( 
            /* [retval][out] */ WEBVIEW2_MOVE_FOCUS_REASON *value) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE get_Handled( 
            /* [retval][out] */ BOOL *value) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE put_Handled( 
            /* [in] */ BOOL value) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IWebView2MoveFocusRequestedEventArgsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IWebView2MoveFocusRequestedEventArgs * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IWebView2MoveFocusRequestedEventArgs * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IWebView2MoveFocusRequestedEventArgs * This);
        
        HRESULT ( STDMETHODCALLTYPE *get_Reason )( 
            IWebView2MoveFocusRequestedEventArgs * This,
            /* [retval][out] */ WEBVIEW2_MOVE_FOCUS_REASON *value);
        
        HRESULT ( STDMETHODCALLTYPE *get_Handled )( 
            IWebView2MoveFocusRequestedEventArgs * This,
            /* [retval][out] */ BOOL *value);
        
        HRESULT ( STDMETHODCALLTYPE *put_Handled )( 
            IWebView2MoveFocusRequestedEventArgs * This,
            /* [in] */ BOOL value);
        
        END_INTERFACE
    } IWebView2MoveFocusRequestedEventArgsVtbl;

    interface IWebView2MoveFocusRequestedEventArgs
    {
        CONST_VTBL struct IWebView2MoveFocusRequestedEventArgsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWebView2MoveFocusRequestedEventArgs_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IWebView2MoveFocusRequestedEventArgs_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IWebView2MoveFocusRequestedEventArgs_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IWebView2MoveFocusRequestedEventArgs_get_Reason(This,value)	\
    ( (This)->lpVtbl -> get_Reason(This,value) ) 

#define IWebView2MoveFocusRequestedEventArgs_get_Handled(This,value)	\
    ( (This)->lpVtbl -> get_Handled(This,value) ) 

#define IWebView2MoveFocusRequestedEventArgs_put_Handled(This,value)	\
    ( (This)->lpVtbl -> put_Handled(This,value) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IWebView2MoveFocusRequestedEventArgs_INTERFACE_DEFINED__ */


#ifndef __IWebView2MoveFocusRequestedEventHandler_INTERFACE_DEFINED__
#define __IWebView2MoveFocusRequestedEventHandler_INTERFACE_DEFINED__

/* interface IWebView2MoveFocusRequestedEventHandler */
/* [unique][object][uuid] */ 


EXTERN_C const IID IID_IWebView2MoveFocusRequestedEventHandler;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("F3A49DD0-EA49-469C-8B7A-8CC5E8E4EF27")
    IWebView2MoveFocusRequestedEventHandler : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Invoke( 
            /* [in] */ IWebView2WebView *webview,
            /* [in] */ IWebView2MoveFocusRequestedEventArgs *args) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IWebView2MoveFocusRequestedEventHandlerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IWebView2MoveFocusRequestedEventHandler * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IWebView2MoveFocusRequestedEventHandler * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IWebView2MoveFocusRequestedEventHandler * This);
        
        HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IWebView2MoveFocusRequestedEventHandler * This,
            /* [in] */ IWebView2WebView *webview,
            /* [in] */ IWebView2MoveFocusRequestedEventArgs *args);
        
        END_INTERFACE
    } IWebView2MoveFocusRequestedEventHandlerVtbl;

    interface IWebView2MoveFocusRequestedEventHandler
    {
        CONST_VTBL struct IWebView2MoveFocusRequestedEventHandlerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWebView2MoveFocusRequestedEventHandler_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IWebView2MoveFocusRequestedEventHandler_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IWebView2MoveFocusRequestedEventHandler_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IWebView2MoveFocusRequestedEventHandler_Invoke(This,webview,args)	\
    ( (This)->lpVtbl -> Invoke(This,webview,args) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IWebView2MoveFocusRequestedEventHandler_INTERFACE_DEFINED__ */


#ifndef __IWebView2WebMessageReceivedEventArgs_INTERFACE_DEFINED__
#define __IWebView2WebMessageReceivedEventArgs_INTERFACE_DEFINED__

/* interface IWebView2WebMessageReceivedEventArgs */
/* [unique][object][uuid] */ 


EXTERN_C const IID IID_IWebView2WebMessageReceivedEventArgs;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("E32C6167-14F1-42EA-8743-B014EF6AD27F")
    IWebView2WebMessageReceivedEventArgs : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE get_Source( 
            /* [retval][out] */ LPWSTR *source) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE get_WebMessageAsJson( 
            /* [retval][out] */ LPWSTR *webMessageAsJson) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE get_WebMessageAsString( 
            /* [retval][out] */ LPWSTR *webMessageAsString) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IWebView2WebMessageReceivedEventArgsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IWebView2WebMessageReceivedEventArgs * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IWebView2WebMessageReceivedEventArgs * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IWebView2WebMessageReceivedEventArgs * This);
        
        HRESULT ( STDMETHODCALLTYPE *get_Source )( 
            IWebView2WebMessageReceivedEventArgs * This,
            /* [retval][out] */ LPWSTR *source);
        
        HRESULT ( STDMETHODCALLTYPE *get_WebMessageAsJson )( 
            IWebView2WebMessageReceivedEventArgs * This,
            /* [retval][out] */ LPWSTR *webMessageAsJson);
        
        HRESULT ( STDMETHODCALLTYPE *get_WebMessageAsString )( 
            IWebView2WebMessageReceivedEventArgs * This,
            /* [retval][out] */ LPWSTR *webMessageAsString);
        
        END_INTERFACE
    } IWebView2WebMessageReceivedEventArgsVtbl;

    interface IWebView2WebMessageReceivedEventArgs
    {
        CONST_VTBL struct IWebView2WebMessageReceivedEventArgsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWebView2WebMessageReceivedEventArgs_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IWebView2WebMessageReceivedEventArgs_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IWebView2WebMessageReceivedEventArgs_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IWebView2WebMessageReceivedEventArgs_get_Source(This,source)	\
    ( (This)->lpVtbl -> get_Source(This,source) ) 

#define IWebView2WebMessageReceivedEventArgs_get_WebMessageAsJson(This,webMessageAsJson)	\
    ( (This)->lpVtbl -> get_WebMessageAsJson(This,webMessageAsJson) ) 

#define IWebView2WebMessageReceivedEventArgs_get_WebMessageAsString(This,webMessageAsString)	\
    ( (This)->lpVtbl -> get_WebMessageAsString(This,webMessageAsString) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IWebView2WebMessageReceivedEventArgs_INTERFACE_DEFINED__ */


#ifndef __IWebView2WebMessageReceivedEventHandler_INTERFACE_DEFINED__
#define __IWebView2WebMessageReceivedEventHandler_INTERFACE_DEFINED__

/* interface IWebView2WebMessageReceivedEventHandler */
/* [unique][object][uuid] */ 


EXTERN_C const IID IID_IWebView2WebMessageReceivedEventHandler;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("0E682B9A-B686-4327-9A56-E0305705A3DB")
    IWebView2WebMessageReceivedEventHandler : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Invoke( 
            /* [in] */ IWebView2WebView *webview,
            /* [in] */ IWebView2WebMessageReceivedEventArgs *args) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IWebView2WebMessageReceivedEventHandlerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IWebView2WebMessageReceivedEventHandler * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IWebView2WebMessageReceivedEventHandler * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IWebView2WebMessageReceivedEventHandler * This);
        
        HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IWebView2WebMessageReceivedEventHandler * This,
            /* [in] */ IWebView2WebView *webview,
            /* [in] */ IWebView2WebMessageReceivedEventArgs *args);
        
        END_INTERFACE
    } IWebView2WebMessageReceivedEventHandlerVtbl;

    interface IWebView2WebMessageReceivedEventHandler
    {
        CONST_VTBL struct IWebView2WebMessageReceivedEventHandlerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWebView2WebMessageReceivedEventHandler_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IWebView2WebMessageReceivedEventHandler_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IWebView2WebMessageReceivedEventHandler_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IWebView2WebMessageReceivedEventHandler_Invoke(This,webview,args)	\
    ( (This)->lpVtbl -> Invoke(This,webview,args) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IWebView2WebMessageReceivedEventHandler_INTERFACE_DEFINED__ */


#ifndef __IWebView2DevToolsProtocolEventReceivedEventArgs_INTERFACE_DEFINED__
#define __IWebView2DevToolsProtocolEventReceivedEventArgs_INTERFACE_DEFINED__

/* interface IWebView2DevToolsProtocolEventReceivedEventArgs */
/* [unique][object][uuid] */ 


EXTERN_C const IID IID_IWebView2DevToolsProtocolEventReceivedEventArgs;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("BF0F875F-8EB0-4211-9B80-2892F7276BB9")
    IWebView2DevToolsProtocolEventReceivedEventArgs : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE get_ParameterObjectAsJson( 
            /* [retval][out] */ LPWSTR *parameterObjectAsJson) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IWebView2DevToolsProtocolEventReceivedEventArgsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IWebView2DevToolsProtocolEventReceivedEventArgs * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IWebView2DevToolsProtocolEventReceivedEventArgs * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IWebView2DevToolsProtocolEventReceivedEventArgs * This);
        
        HRESULT ( STDMETHODCALLTYPE *get_ParameterObjectAsJson )( 
            IWebView2DevToolsProtocolEventReceivedEventArgs * This,
            /* [retval][out] */ LPWSTR *parameterObjectAsJson);
        
        END_INTERFACE
    } IWebView2DevToolsProtocolEventReceivedEventArgsVtbl;

    interface IWebView2DevToolsProtocolEventReceivedEventArgs
    {
        CONST_VTBL struct IWebView2DevToolsProtocolEventReceivedEventArgsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWebView2DevToolsProtocolEventReceivedEventArgs_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IWebView2DevToolsProtocolEventReceivedEventArgs_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IWebView2DevToolsProtocolEventReceivedEventArgs_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IWebView2DevToolsProtocolEventReceivedEventArgs_get_ParameterObjectAsJson(This,parameterObjectAsJson)	\
    ( (This)->lpVtbl -> get_ParameterObjectAsJson(This,parameterObjectAsJson) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IWebView2DevToolsProtocolEventReceivedEventArgs_INTERFACE_DEFINED__ */


#ifndef __IWebView2DevToolsProtocolEventReceivedEventHandler_INTERFACE_DEFINED__
#define __IWebView2DevToolsProtocolEventReceivedEventHandler_INTERFACE_DEFINED__

/* interface IWebView2DevToolsProtocolEventReceivedEventHandler */
/* [unique][object][uuid] */ 


EXTERN_C const IID IID_IWebView2DevToolsProtocolEventReceivedEventHandler;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("37D087EA-12F6-4856-81D8-5596C708CA59")
    IWebView2DevToolsProtocolEventReceivedEventHandler : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Invoke( 
            /* [in] */ IWebView2WebView *webview,
            /* [in] */ IWebView2DevToolsProtocolEventReceivedEventArgs *args) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IWebView2DevToolsProtocolEventReceivedEventHandlerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IWebView2DevToolsProtocolEventReceivedEventHandler * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IWebView2DevToolsProtocolEventReceivedEventHandler * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IWebView2DevToolsProtocolEventReceivedEventHandler * This);
        
        HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IWebView2DevToolsProtocolEventReceivedEventHandler * This,
            /* [in] */ IWebView2WebView *webview,
            /* [in] */ IWebView2DevToolsProtocolEventReceivedEventArgs *args);
        
        END_INTERFACE
    } IWebView2DevToolsProtocolEventReceivedEventHandlerVtbl;

    interface IWebView2DevToolsProtocolEventReceivedEventHandler
    {
        CONST_VTBL struct IWebView2DevToolsProtocolEventReceivedEventHandlerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWebView2DevToolsProtocolEventReceivedEventHandler_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IWebView2DevToolsProtocolEventReceivedEventHandler_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IWebView2DevToolsProtocolEventReceivedEventHandler_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IWebView2DevToolsProtocolEventReceivedEventHandler_Invoke(This,webview,args)	\
    ( (This)->lpVtbl -> Invoke(This,webview,args) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IWebView2DevToolsProtocolEventReceivedEventHandler_INTERFACE_DEFINED__ */


#ifndef __IWebView2CallDevToolsProtocolMethodCompletedHandler_INTERFACE_DEFINED__
#define __IWebView2CallDevToolsProtocolMethodCompletedHandler_INTERFACE_DEFINED__

/* interface IWebView2CallDevToolsProtocolMethodCompletedHandler */
/* [unique][object][uuid] */ 


EXTERN_C const IID IID_IWebView2CallDevToolsProtocolMethodCompletedHandler;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("6EA28F62-FEC5-48EA-9669-67979B50579E")
    IWebView2CallDevToolsProtocolMethodCompletedHandler : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Invoke( 
            /* [in] */ HRESULT errorCode,
            /* [in] */ LPCWSTR returnObjectAsJson) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IWebView2CallDevToolsProtocolMethodCompletedHandlerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IWebView2CallDevToolsProtocolMethodCompletedHandler * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IWebView2CallDevToolsProtocolMethodCompletedHandler * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IWebView2CallDevToolsProtocolMethodCompletedHandler * This);
        
        HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IWebView2CallDevToolsProtocolMethodCompletedHandler * This,
            /* [in] */ HRESULT errorCode,
            /* [in] */ LPCWSTR returnObjectAsJson);
        
        END_INTERFACE
    } IWebView2CallDevToolsProtocolMethodCompletedHandlerVtbl;

    interface IWebView2CallDevToolsProtocolMethodCompletedHandler
    {
        CONST_VTBL struct IWebView2CallDevToolsProtocolMethodCompletedHandlerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWebView2CallDevToolsProtocolMethodCompletedHandler_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IWebView2CallDevToolsProtocolMethodCompletedHandler_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IWebView2CallDevToolsProtocolMethodCompletedHandler_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IWebView2CallDevToolsProtocolMethodCompletedHandler_Invoke(This,errorCode,returnObjectAsJson)	\
    ( (This)->lpVtbl -> Invoke(This,errorCode,returnObjectAsJson) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IWebView2CallDevToolsProtocolMethodCompletedHandler_INTERFACE_DEFINED__ */


#ifndef __IWebView2CreateWebViewCompletedHandler_INTERFACE_DEFINED__
#define __IWebView2CreateWebViewCompletedHandler_INTERFACE_DEFINED__

/* interface IWebView2CreateWebViewCompletedHandler */
/* [unique][object][uuid] */ 


EXTERN_C const IID IID_IWebView2CreateWebViewCompletedHandler;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("E0618CDD-4947-4F58-802C-FC1F20BD4274")
    IWebView2CreateWebViewCompletedHandler : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Invoke( 
            HRESULT result,
            IWebView2WebView *webView) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IWebView2CreateWebViewCompletedHandlerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IWebView2CreateWebViewCompletedHandler * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IWebView2CreateWebViewCompletedHandler * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IWebView2CreateWebViewCompletedHandler * This);
        
        HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IWebView2CreateWebViewCompletedHandler * This,
            HRESULT result,
            IWebView2WebView *webView);
        
        END_INTERFACE
    } IWebView2CreateWebViewCompletedHandlerVtbl;

    interface IWebView2CreateWebViewCompletedHandler
    {
        CONST_VTBL struct IWebView2CreateWebViewCompletedHandlerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWebView2CreateWebViewCompletedHandler_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IWebView2CreateWebViewCompletedHandler_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IWebView2CreateWebViewCompletedHandler_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IWebView2CreateWebViewCompletedHandler_Invoke(This,result,webView)	\
    ( (This)->lpVtbl -> Invoke(This,result,webView) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IWebView2CreateWebViewCompletedHandler_INTERFACE_DEFINED__ */


#ifndef __IWebView2NewWindowRequestedEventArgs_INTERFACE_DEFINED__
#define __IWebView2NewWindowRequestedEventArgs_INTERFACE_DEFINED__

/* interface IWebView2NewWindowRequestedEventArgs */
/* [unique][object][uuid] */ 


EXTERN_C const IID IID_IWebView2NewWindowRequestedEventArgs;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("1f6e4074-bc3d-4381-ba8a-cf65feaa036a")
    IWebView2NewWindowRequestedEventArgs : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE get_Uri( 
            /* [retval][out] */ LPWSTR *uri) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE put_NewWindow( 
            /* [in] */ IWebView2WebView *newWindow) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE get_NewWindow( 
            /* [retval][out] */ IWebView2WebView **newWindow) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE put_Handled( 
            /* [in] */ BOOL handled) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE get_Handled( 
            /* [retval][out] */ BOOL *handled) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE get_IsUserInitiated( 
            /* [retval][out] */ BOOL *isUserInitiated) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetDeferral( 
            /* [retval][out] */ IWebView2Deferral **deferral) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IWebView2NewWindowRequestedEventArgsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IWebView2NewWindowRequestedEventArgs * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IWebView2NewWindowRequestedEventArgs * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IWebView2NewWindowRequestedEventArgs * This);
        
        HRESULT ( STDMETHODCALLTYPE *get_Uri )( 
            IWebView2NewWindowRequestedEventArgs * This,
            /* [retval][out] */ LPWSTR *uri);
        
        HRESULT ( STDMETHODCALLTYPE *put_NewWindow )( 
            IWebView2NewWindowRequestedEventArgs * This,
            /* [in] */ IWebView2WebView *newWindow);
        
        HRESULT ( STDMETHODCALLTYPE *get_NewWindow )( 
            IWebView2NewWindowRequestedEventArgs * This,
            /* [retval][out] */ IWebView2WebView **newWindow);
        
        HRESULT ( STDMETHODCALLTYPE *put_Handled )( 
            IWebView2NewWindowRequestedEventArgs * This,
            /* [in] */ BOOL handled);
        
        HRESULT ( STDMETHODCALLTYPE *get_Handled )( 
            IWebView2NewWindowRequestedEventArgs * This,
            /* [retval][out] */ BOOL *handled);
        
        HRESULT ( STDMETHODCALLTYPE *get_IsUserInitiated )( 
            IWebView2NewWindowRequestedEventArgs * This,
            /* [retval][out] */ BOOL *isUserInitiated);
        
        HRESULT ( STDMETHODCALLTYPE *GetDeferral )( 
            IWebView2NewWindowRequestedEventArgs * This,
            /* [retval][out] */ IWebView2Deferral **deferral);
        
        END_INTERFACE
    } IWebView2NewWindowRequestedEventArgsVtbl;

    interface IWebView2NewWindowRequestedEventArgs
    {
        CONST_VTBL struct IWebView2NewWindowRequestedEventArgsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWebView2NewWindowRequestedEventArgs_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IWebView2NewWindowRequestedEventArgs_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IWebView2NewWindowRequestedEventArgs_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IWebView2NewWindowRequestedEventArgs_get_Uri(This,uri)	\
    ( (This)->lpVtbl -> get_Uri(This,uri) ) 

#define IWebView2NewWindowRequestedEventArgs_put_NewWindow(This,newWindow)	\
    ( (This)->lpVtbl -> put_NewWindow(This,newWindow) ) 

#define IWebView2NewWindowRequestedEventArgs_get_NewWindow(This,newWindow)	\
    ( (This)->lpVtbl -> get_NewWindow(This,newWindow) ) 

#define IWebView2NewWindowRequestedEventArgs_put_Handled(This,handled)	\
    ( (This)->lpVtbl -> put_Handled(This,handled) ) 

#define IWebView2NewWindowRequestedEventArgs_get_Handled(This,handled)	\
    ( (This)->lpVtbl -> get_Handled(This,handled) ) 

#define IWebView2NewWindowRequestedEventArgs_get_IsUserInitiated(This,isUserInitiated)	\
    ( (This)->lpVtbl -> get_IsUserInitiated(This,isUserInitiated) ) 

#define IWebView2NewWindowRequestedEventArgs_GetDeferral(This,deferral)	\
    ( (This)->lpVtbl -> GetDeferral(This,deferral) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IWebView2NewWindowRequestedEventArgs_INTERFACE_DEFINED__ */


#ifndef __IWebView2NewWindowRequestedEventHandler_INTERFACE_DEFINED__
#define __IWebView2NewWindowRequestedEventHandler_INTERFACE_DEFINED__

/* interface IWebView2NewWindowRequestedEventHandler */
/* [unique][object][uuid] */ 


EXTERN_C const IID IID_IWebView2NewWindowRequestedEventHandler;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("1daa050a-98de-44ad-b5bb-935c8b9c7c0b")
    IWebView2NewWindowRequestedEventHandler : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Invoke( 
            /* [in] */ IWebView2WebView *webview,
            /* [in] */ IWebView2NewWindowRequestedEventArgs *args) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IWebView2NewWindowRequestedEventHandlerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IWebView2NewWindowRequestedEventHandler * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IWebView2NewWindowRequestedEventHandler * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IWebView2NewWindowRequestedEventHandler * This);
        
        HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IWebView2NewWindowRequestedEventHandler * This,
            /* [in] */ IWebView2WebView *webview,
            /* [in] */ IWebView2NewWindowRequestedEventArgs *args);
        
        END_INTERFACE
    } IWebView2NewWindowRequestedEventHandlerVtbl;

    interface IWebView2NewWindowRequestedEventHandler
    {
        CONST_VTBL struct IWebView2NewWindowRequestedEventHandlerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWebView2NewWindowRequestedEventHandler_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IWebView2NewWindowRequestedEventHandler_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IWebView2NewWindowRequestedEventHandler_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IWebView2NewWindowRequestedEventHandler_Invoke(This,webview,args)	\
    ( (This)->lpVtbl -> Invoke(This,webview,args) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IWebView2NewWindowRequestedEventHandler_INTERFACE_DEFINED__ */


#ifndef __IWebView2DocumentTitleChangedEventHandler_INTERFACE_DEFINED__
#define __IWebView2DocumentTitleChangedEventHandler_INTERFACE_DEFINED__

/* interface IWebView2DocumentTitleChangedEventHandler */
/* [unique][object][uuid] */ 


EXTERN_C const IID IID_IWebView2DocumentTitleChangedEventHandler;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("E190F4F4-7C94-4CB3-BA4D-DDCDA7AC7693")
    IWebView2DocumentTitleChangedEventHandler : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Invoke( 
            /* [in] */ IWebView2WebView3 *webview,
            /* [in] */ IUnknown *args) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IWebView2DocumentTitleChangedEventHandlerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IWebView2DocumentTitleChangedEventHandler * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IWebView2DocumentTitleChangedEventHandler * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IWebView2DocumentTitleChangedEventHandler * This);
        
        HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IWebView2DocumentTitleChangedEventHandler * This,
            /* [in] */ IWebView2WebView3 *webview,
            /* [in] */ IUnknown *args);
        
        END_INTERFACE
    } IWebView2DocumentTitleChangedEventHandlerVtbl;

    interface IWebView2DocumentTitleChangedEventHandler
    {
        CONST_VTBL struct IWebView2DocumentTitleChangedEventHandlerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWebView2DocumentTitleChangedEventHandler_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IWebView2DocumentTitleChangedEventHandler_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IWebView2DocumentTitleChangedEventHandler_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IWebView2DocumentTitleChangedEventHandler_Invoke(This,webview,args)	\
    ( (This)->lpVtbl -> Invoke(This,webview,args) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IWebView2DocumentTitleChangedEventHandler_INTERFACE_DEFINED__ */


#ifndef __IWebView2AcceleratorKeyPressedEventArgs_INTERFACE_DEFINED__
#define __IWebView2AcceleratorKeyPressedEventArgs_INTERFACE_DEFINED__

/* interface IWebView2AcceleratorKeyPressedEventArgs */
/* [unique][object][uuid] */ 


EXTERN_C const IID IID_IWebView2AcceleratorKeyPressedEventArgs;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("64C29E6D-BA57-4EBA-A14F-71697F4F3D86")
    IWebView2AcceleratorKeyPressedEventArgs : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE get_KeyEventType( 
            /* [retval][out] */ WEBVIEW2_KEY_EVENT_TYPE *keyEventType) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE get_VirtualKey( 
            /* [retval][out] */ UINT *virtualKey) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE get_KeyEventLParam( 
            /* [retval][out] */ INT *lParam) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE get_PhysicalKeyStatus( 
            /* [retval][out] */ WEBVIEW2_PHYSICAL_KEY_STATUS *physicalKeyStatus) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Handle( 
            /* [in] */ BOOL handled) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IWebView2AcceleratorKeyPressedEventArgsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IWebView2AcceleratorKeyPressedEventArgs * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IWebView2AcceleratorKeyPressedEventArgs * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IWebView2AcceleratorKeyPressedEventArgs * This);
        
        HRESULT ( STDMETHODCALLTYPE *get_KeyEventType )( 
            IWebView2AcceleratorKeyPressedEventArgs * This,
            /* [retval][out] */ WEBVIEW2_KEY_EVENT_TYPE *keyEventType);
        
        HRESULT ( STDMETHODCALLTYPE *get_VirtualKey )( 
            IWebView2AcceleratorKeyPressedEventArgs * This,
            /* [retval][out] */ UINT *virtualKey);
        
        HRESULT ( STDMETHODCALLTYPE *get_KeyEventLParam )( 
            IWebView2AcceleratorKeyPressedEventArgs * This,
            /* [retval][out] */ INT *lParam);
        
        HRESULT ( STDMETHODCALLTYPE *get_PhysicalKeyStatus )( 
            IWebView2AcceleratorKeyPressedEventArgs * This,
            /* [retval][out] */ WEBVIEW2_PHYSICAL_KEY_STATUS *physicalKeyStatus);
        
        HRESULT ( STDMETHODCALLTYPE *Handle )( 
            IWebView2AcceleratorKeyPressedEventArgs * This,
            /* [in] */ BOOL handled);
        
        END_INTERFACE
    } IWebView2AcceleratorKeyPressedEventArgsVtbl;

    interface IWebView2AcceleratorKeyPressedEventArgs
    {
        CONST_VTBL struct IWebView2AcceleratorKeyPressedEventArgsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWebView2AcceleratorKeyPressedEventArgs_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IWebView2AcceleratorKeyPressedEventArgs_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IWebView2AcceleratorKeyPressedEventArgs_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IWebView2AcceleratorKeyPressedEventArgs_get_KeyEventType(This,keyEventType)	\
    ( (This)->lpVtbl -> get_KeyEventType(This,keyEventType) ) 

#define IWebView2AcceleratorKeyPressedEventArgs_get_VirtualKey(This,virtualKey)	\
    ( (This)->lpVtbl -> get_VirtualKey(This,virtualKey) ) 

#define IWebView2AcceleratorKeyPressedEventArgs_get_KeyEventLParam(This,lParam)	\
    ( (This)->lpVtbl -> get_KeyEventLParam(This,lParam) ) 

#define IWebView2AcceleratorKeyPressedEventArgs_get_PhysicalKeyStatus(This,physicalKeyStatus)	\
    ( (This)->lpVtbl -> get_PhysicalKeyStatus(This,physicalKeyStatus) ) 

#define IWebView2AcceleratorKeyPressedEventArgs_Handle(This,handled)	\
    ( (This)->lpVtbl -> Handle(This,handled) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IWebView2AcceleratorKeyPressedEventArgs_INTERFACE_DEFINED__ */


#ifndef __IWebView2AcceleratorKeyPressedEventHandler_INTERFACE_DEFINED__
#define __IWebView2AcceleratorKeyPressedEventHandler_INTERFACE_DEFINED__

/* interface IWebView2AcceleratorKeyPressedEventHandler */
/* [unique][object][uuid] */ 


EXTERN_C const IID IID_IWebView2AcceleratorKeyPressedEventHandler;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("53E3676B-287C-4967-B7E2-DA0448BEB0F1")
    IWebView2AcceleratorKeyPressedEventHandler : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Invoke( 
            /* [in] */ IWebView2WebView *webview,
            /* [in] */ IWebView2AcceleratorKeyPressedEventArgs *args) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IWebView2AcceleratorKeyPressedEventHandlerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IWebView2AcceleratorKeyPressedEventHandler * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IWebView2AcceleratorKeyPressedEventHandler * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IWebView2AcceleratorKeyPressedEventHandler * This);
        
        HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IWebView2AcceleratorKeyPressedEventHandler * This,
            /* [in] */ IWebView2WebView *webview,
            /* [in] */ IWebView2AcceleratorKeyPressedEventArgs *args);
        
        END_INTERFACE
    } IWebView2AcceleratorKeyPressedEventHandlerVtbl;

    interface IWebView2AcceleratorKeyPressedEventHandler
    {
        CONST_VTBL struct IWebView2AcceleratorKeyPressedEventHandlerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWebView2AcceleratorKeyPressedEventHandler_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IWebView2AcceleratorKeyPressedEventHandler_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IWebView2AcceleratorKeyPressedEventHandler_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IWebView2AcceleratorKeyPressedEventHandler_Invoke(This,webview,args)	\
    ( (This)->lpVtbl -> Invoke(This,webview,args) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IWebView2AcceleratorKeyPressedEventHandler_INTERFACE_DEFINED__ */


#ifndef __IWebView2NewVersionAvailableEventArgs_INTERFACE_DEFINED__
#define __IWebView2NewVersionAvailableEventArgs_INTERFACE_DEFINED__

/* interface IWebView2NewVersionAvailableEventArgs */
/* [unique][object][uuid] */ 


EXTERN_C const IID IID_IWebView2NewVersionAvailableEventArgs;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("0256DA7B-2BF7-4B12-8ECA-EFFCB28C2CD8")
    IWebView2NewVersionAvailableEventArgs : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE get_NewVersion( 
            /* [retval][out] */ LPWSTR *newVersion) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IWebView2NewVersionAvailableEventArgsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IWebView2NewVersionAvailableEventArgs * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IWebView2NewVersionAvailableEventArgs * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IWebView2NewVersionAvailableEventArgs * This);
        
        HRESULT ( STDMETHODCALLTYPE *get_NewVersion )( 
            IWebView2NewVersionAvailableEventArgs * This,
            /* [retval][out] */ LPWSTR *newVersion);
        
        END_INTERFACE
    } IWebView2NewVersionAvailableEventArgsVtbl;

    interface IWebView2NewVersionAvailableEventArgs
    {
        CONST_VTBL struct IWebView2NewVersionAvailableEventArgsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWebView2NewVersionAvailableEventArgs_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IWebView2NewVersionAvailableEventArgs_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IWebView2NewVersionAvailableEventArgs_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IWebView2NewVersionAvailableEventArgs_get_NewVersion(This,newVersion)	\
    ( (This)->lpVtbl -> get_NewVersion(This,newVersion) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IWebView2NewVersionAvailableEventArgs_INTERFACE_DEFINED__ */


#ifndef __IWebView2NewVersionAvailableEventHandler_INTERFACE_DEFINED__
#define __IWebView2NewVersionAvailableEventHandler_INTERFACE_DEFINED__

/* interface IWebView2NewVersionAvailableEventHandler */
/* [unique][object][uuid] */ 


EXTERN_C const IID IID_IWebView2NewVersionAvailableEventHandler;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("9E21312F-6FE7-4118-8CA1-6317C9CD627B")
    IWebView2NewVersionAvailableEventHandler : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Invoke( 
            /* [in] */ IWebView2Environment *webviewEnvironment,
            /* [in] */ IWebView2NewVersionAvailableEventArgs *args) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IWebView2NewVersionAvailableEventHandlerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IWebView2NewVersionAvailableEventHandler * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IWebView2NewVersionAvailableEventHandler * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IWebView2NewVersionAvailableEventHandler * This);
        
        HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IWebView2NewVersionAvailableEventHandler * This,
            /* [in] */ IWebView2Environment *webviewEnvironment,
            /* [in] */ IWebView2NewVersionAvailableEventArgs *args);
        
        END_INTERFACE
    } IWebView2NewVersionAvailableEventHandlerVtbl;

    interface IWebView2NewVersionAvailableEventHandler
    {
        CONST_VTBL struct IWebView2NewVersionAvailableEventHandlerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWebView2NewVersionAvailableEventHandler_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IWebView2NewVersionAvailableEventHandler_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IWebView2NewVersionAvailableEventHandler_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IWebView2NewVersionAvailableEventHandler_Invoke(This,webviewEnvironment,args)	\
    ( (This)->lpVtbl -> Invoke(This,webviewEnvironment,args) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IWebView2NewVersionAvailableEventHandler_INTERFACE_DEFINED__ */


#ifndef __IWebView2ContainsFullScreenElementChangedEventHandler_INTERFACE_DEFINED__
#define __IWebView2ContainsFullScreenElementChangedEventHandler_INTERFACE_DEFINED__

/* interface IWebView2ContainsFullScreenElementChangedEventHandler */
/* [unique][object][uuid] */ 


EXTERN_C const IID IID_IWebView2ContainsFullScreenElementChangedEventHandler;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("37CF6A21-4B0C-41B6-81A6-C85C0D0A7543")
    IWebView2ContainsFullScreenElementChangedEventHandler : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Invoke( 
            /* [in] */ IWebView2WebView5 *webview,
            /* [in] */ IUnknown *args) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IWebView2ContainsFullScreenElementChangedEventHandlerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IWebView2ContainsFullScreenElementChangedEventHandler * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IWebView2ContainsFullScreenElementChangedEventHandler * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IWebView2ContainsFullScreenElementChangedEventHandler * This);
        
        HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IWebView2ContainsFullScreenElementChangedEventHandler * This,
            /* [in] */ IWebView2WebView5 *webview,
            /* [in] */ IUnknown *args);
        
        END_INTERFACE
    } IWebView2ContainsFullScreenElementChangedEventHandlerVtbl;

    interface IWebView2ContainsFullScreenElementChangedEventHandler
    {
        CONST_VTBL struct IWebView2ContainsFullScreenElementChangedEventHandlerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWebView2ContainsFullScreenElementChangedEventHandler_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IWebView2ContainsFullScreenElementChangedEventHandler_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IWebView2ContainsFullScreenElementChangedEventHandler_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IWebView2ContainsFullScreenElementChangedEventHandler_Invoke(This,webview,args)	\
    ( (This)->lpVtbl -> Invoke(This,webview,args) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IWebView2ContainsFullScreenElementChangedEventHandler_INTERFACE_DEFINED__ */


#ifndef __IWebView2Environment_INTERFACE_DEFINED__
#define __IWebView2Environment_INTERFACE_DEFINED__

/* interface IWebView2Environment */
/* [unique][object][uuid] */ 


EXTERN_C const IID IID_IWebView2Environment;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("33D17ECE-82FA-47D9-8978-CD17FF3C3CC6")
    IWebView2Environment : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE CreateWebView( 
            HWND parentWindow,
            IWebView2CreateWebViewCompletedHandler *handler) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CreateWebResourceResponse( 
            IStream *content,
            int statusCode,
            LPCWSTR reasonPhrase,
            LPCWSTR headers,
            IWebView2WebResourceResponse **response) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IWebView2EnvironmentVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IWebView2Environment * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IWebView2Environment * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IWebView2Environment * This);
        
        HRESULT ( STDMETHODCALLTYPE *CreateWebView )( 
            IWebView2Environment * This,
            HWND parentWindow,
            IWebView2CreateWebViewCompletedHandler *handler);
        
        HRESULT ( STDMETHODCALLTYPE *CreateWebResourceResponse )( 
            IWebView2Environment * This,
            IStream *content,
            int statusCode,
            LPCWSTR reasonPhrase,
            LPCWSTR headers,
            IWebView2WebResourceResponse **response);
        
        END_INTERFACE
    } IWebView2EnvironmentVtbl;

    interface IWebView2Environment
    {
        CONST_VTBL struct IWebView2EnvironmentVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWebView2Environment_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IWebView2Environment_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IWebView2Environment_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IWebView2Environment_CreateWebView(This,parentWindow,handler)	\
    ( (This)->lpVtbl -> CreateWebView(This,parentWindow,handler) ) 

#define IWebView2Environment_CreateWebResourceResponse(This,content,statusCode,reasonPhrase,headers,response)	\
    ( (This)->lpVtbl -> CreateWebResourceResponse(This,content,statusCode,reasonPhrase,headers,response) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IWebView2Environment_INTERFACE_DEFINED__ */


#ifndef __IWebView2Environment2_INTERFACE_DEFINED__
#define __IWebView2Environment2_INTERFACE_DEFINED__

/* interface IWebView2Environment2 */
/* [unique][object][uuid] */ 


EXTERN_C const IID IID_IWebView2Environment2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("013124F3-02FD-4DFF-8911-06016AF1E3EE")
    IWebView2Environment2 : public IWebView2Environment
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE get_BrowserVersionInfo( 
            /* [retval][out] */ LPWSTR *versionInfo) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IWebView2Environment2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IWebView2Environment2 * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IWebView2Environment2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IWebView2Environment2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *CreateWebView )( 
            IWebView2Environment2 * This,
            HWND parentWindow,
            IWebView2CreateWebViewCompletedHandler *handler);
        
        HRESULT ( STDMETHODCALLTYPE *CreateWebResourceResponse )( 
            IWebView2Environment2 * This,
            IStream *content,
            int statusCode,
            LPCWSTR reasonPhrase,
            LPCWSTR headers,
            IWebView2WebResourceResponse **response);
        
        HRESULT ( STDMETHODCALLTYPE *get_BrowserVersionInfo )( 
            IWebView2Environment2 * This,
            /* [retval][out] */ LPWSTR *versionInfo);
        
        END_INTERFACE
    } IWebView2Environment2Vtbl;

    interface IWebView2Environment2
    {
        CONST_VTBL struct IWebView2Environment2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWebView2Environment2_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IWebView2Environment2_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IWebView2Environment2_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IWebView2Environment2_CreateWebView(This,parentWindow,handler)	\
    ( (This)->lpVtbl -> CreateWebView(This,parentWindow,handler) ) 

#define IWebView2Environment2_CreateWebResourceResponse(This,content,statusCode,reasonPhrase,headers,response)	\
    ( (This)->lpVtbl -> CreateWebResourceResponse(This,content,statusCode,reasonPhrase,headers,response) ) 


#define IWebView2Environment2_get_BrowserVersionInfo(This,versionInfo)	\
    ( (This)->lpVtbl -> get_BrowserVersionInfo(This,versionInfo) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IWebView2Environment2_INTERFACE_DEFINED__ */


#ifndef __IWebView2Environment3_INTERFACE_DEFINED__
#define __IWebView2Environment3_INTERFACE_DEFINED__

/* interface IWebView2Environment3 */
/* [unique][object][uuid] */ 


EXTERN_C const IID IID_IWebView2Environment3;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("D82C6A26-370F-4084-8149-C08FF1598C9B")
    IWebView2Environment3 : public IWebView2Environment2
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE add_NewVersionAvailable( 
            /* [in] */ IWebView2NewVersionAvailableEventHandler *eventHandler,
            /* [out] */ EventRegistrationToken *token) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE remove_NewVersionAvailable( 
            /* [in] */ EventRegistrationToken token) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IWebView2Environment3Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IWebView2Environment3 * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IWebView2Environment3 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IWebView2Environment3 * This);
        
        HRESULT ( STDMETHODCALLTYPE *CreateWebView )( 
            IWebView2Environment3 * This,
            HWND parentWindow,
            IWebView2CreateWebViewCompletedHandler *handler);
        
        HRESULT ( STDMETHODCALLTYPE *CreateWebResourceResponse )( 
            IWebView2Environment3 * This,
            IStream *content,
            int statusCode,
            LPCWSTR reasonPhrase,
            LPCWSTR headers,
            IWebView2WebResourceResponse **response);
        
        HRESULT ( STDMETHODCALLTYPE *get_BrowserVersionInfo )( 
            IWebView2Environment3 * This,
            /* [retval][out] */ LPWSTR *versionInfo);
        
        HRESULT ( STDMETHODCALLTYPE *add_NewVersionAvailable )( 
            IWebView2Environment3 * This,
            /* [in] */ IWebView2NewVersionAvailableEventHandler *eventHandler,
            /* [out] */ EventRegistrationToken *token);
        
        HRESULT ( STDMETHODCALLTYPE *remove_NewVersionAvailable )( 
            IWebView2Environment3 * This,
            /* [in] */ EventRegistrationToken token);
        
        END_INTERFACE
    } IWebView2Environment3Vtbl;

    interface IWebView2Environment3
    {
        CONST_VTBL struct IWebView2Environment3Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWebView2Environment3_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IWebView2Environment3_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IWebView2Environment3_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IWebView2Environment3_CreateWebView(This,parentWindow,handler)	\
    ( (This)->lpVtbl -> CreateWebView(This,parentWindow,handler) ) 

#define IWebView2Environment3_CreateWebResourceResponse(This,content,statusCode,reasonPhrase,headers,response)	\
    ( (This)->lpVtbl -> CreateWebResourceResponse(This,content,statusCode,reasonPhrase,headers,response) ) 


#define IWebView2Environment3_get_BrowserVersionInfo(This,versionInfo)	\
    ( (This)->lpVtbl -> get_BrowserVersionInfo(This,versionInfo) ) 


#define IWebView2Environment3_add_NewVersionAvailable(This,eventHandler,token)	\
    ( (This)->lpVtbl -> add_NewVersionAvailable(This,eventHandler,token) ) 

#define IWebView2Environment3_remove_NewVersionAvailable(This,token)	\
    ( (This)->lpVtbl -> remove_NewVersionAvailable(This,token) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IWebView2Environment3_INTERFACE_DEFINED__ */


#ifndef __IWebView2CreateWebView2EnvironmentCompletedHandler_INTERFACE_DEFINED__
#define __IWebView2CreateWebView2EnvironmentCompletedHandler_INTERFACE_DEFINED__

/* interface IWebView2CreateWebView2EnvironmentCompletedHandler */
/* [unique][object][uuid] */ 


EXTERN_C const IID IID_IWebView2CreateWebView2EnvironmentCompletedHandler;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("A8346945-51C2-4CE6-8B4C-6F3C4391828B")
    IWebView2CreateWebView2EnvironmentCompletedHandler : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Invoke( 
            HRESULT result,
            IWebView2Environment *webViewEnvironment) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IWebView2CreateWebView2EnvironmentCompletedHandlerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IWebView2CreateWebView2EnvironmentCompletedHandler * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IWebView2CreateWebView2EnvironmentCompletedHandler * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IWebView2CreateWebView2EnvironmentCompletedHandler * This);
        
        HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IWebView2CreateWebView2EnvironmentCompletedHandler * This,
            HRESULT result,
            IWebView2Environment *webViewEnvironment);
        
        END_INTERFACE
    } IWebView2CreateWebView2EnvironmentCompletedHandlerVtbl;

    interface IWebView2CreateWebView2EnvironmentCompletedHandler
    {
        CONST_VTBL struct IWebView2CreateWebView2EnvironmentCompletedHandlerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWebView2CreateWebView2EnvironmentCompletedHandler_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IWebView2CreateWebView2EnvironmentCompletedHandler_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IWebView2CreateWebView2EnvironmentCompletedHandler_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IWebView2CreateWebView2EnvironmentCompletedHandler_Invoke(This,result,webViewEnvironment)	\
    ( (This)->lpVtbl -> Invoke(This,result,webViewEnvironment) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IWebView2CreateWebView2EnvironmentCompletedHandler_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_webview2_0000_0053 */
/* [local] */ 

STDAPI CreateWebView2EnvironmentWithDetails(
    PCWSTR browserExecutableFolder,
    PCWSTR userDataFolder,
    PCWSTR additionalBrowserArguments,
    IWebView2CreateWebView2EnvironmentCompletedHandler* environment_created_handler);
STDAPI CreateWebView2Environment(
    IWebView2CreateWebView2EnvironmentCompletedHandler* environment_created_handler);
STDAPI GetWebView2BrowserVersionInfo(
    PCWSTR browserExecutableFolder,
    LPWSTR* versionInfo);
STDAPI CompareBrowserVersions(
    PCWSTR version1,
    PCWSTR version2,
    int* result);


extern RPC_IF_HANDLE __MIDL_itf_webview2_0000_0053_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_webview2_0000_0053_v0_0_s_ifspec;

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  HWND_UserSize(     unsigned long *, unsigned long            , HWND * ); 
unsigned char * __RPC_USER  HWND_UserMarshal(  unsigned long *, unsigned char *, HWND * ); 
unsigned char * __RPC_USER  HWND_UserUnmarshal(unsigned long *, unsigned char *, HWND * ); 
void                      __RPC_USER  HWND_UserFree(     unsigned long *, HWND * ); 

unsigned long             __RPC_USER  VARIANT_UserSize(     unsigned long *, unsigned long            , VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserMarshal(  unsigned long *, unsigned char *, VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserUnmarshal(unsigned long *, unsigned char *, VARIANT * ); 
void                      __RPC_USER  VARIANT_UserFree(     unsigned long *, VARIANT * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


