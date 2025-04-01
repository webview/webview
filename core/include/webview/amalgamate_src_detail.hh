#pragma once
#if defined(__cplusplus) && !defined(WEBVIEW_HEADER)

#include "../../src/detail/engine_base.cc"

/** TODO
 *
 * #include "../../src/detail/basic_result.cc"
 * #include "../../src/detail/exceptions.cc"
 * #include "../../src/detail/json.cc"
 * #include "../../src/detail/native_library.cc"
 * #include "../../src/detail/optional.cc"
 * #include "../../src/detail/user_script.cc"
 * 
 * #include "../../src/detail/backends/cocoa_webkit.cc"
 * #include "../../src/detail/backends/gtk_webkitgtk.cc"
 * #include "../../src/detail/backends/win32_edge.cc"
 * 
 * #include "../../src/detail/platform/darwin/cocoa.cc"
 * #include "../../src/detail/platform/darwin/objc.cc"
 * #include "../../src/detail/platform/darwin/webkit.cc"
 * 
 * #include "../../src/detail/platform/linux/gtk/compat.cc"
 * #include "../../src/detail/platform/linux/webkitgtk/compat.cc"
 * #include "../../src/detail/platform/linux/webkitgtk/dmabuf.cc"
 * 
 * #include "../../src/detail/platform/windows/com_init_wrapper.cc
 * #include "../../src/detail/platform/windows/dpi.cc
 * #include "../../src/detail/platform/windows/dwmapi.cc
 * #include "../../src/detail/platform/windows/iid.cc
 * #include "../../src/detail/platform/windows/ntdll.cc
 * #include "../../src/detail/platform/windows/reg_key.cc
 * #include "../../src/detail/platform/windows/shcore.cc
 * #include "../../src/detail/platform/windows/theme.cc
 * #include "../../src/detail/platform/windows/user32.cc
 * #include "../../src/detail/platform/windows/version.cc
 * #include "../../src/detail/platform/windows/webview2/loader.cc
*/

/** For consideration:
 *
 * Maybe move "core/include/webview/detail/utility/string.hh" to a more generic
 * "core/include/webview/utility" location with a webview::utility namespace.
 * 
 * Using the almalgamation pattern of this file, maybe refactor the root of
 * the "include" dir  to contain only the following files:
 * - almalgamate_src_api.hh
 * - almalgamate_src_deprecated.hh
 * - almalgamate_src_detail.hh
 * - almalgamate_src_utility.hh
 * - types.h
 * - webview.h
 * 
 * and re-locate the remaining files into the following new "include" root folders:
 * - "/api"
 * - "/utility"
 * - "/deprecated"
 * 
 * with refactored webview::?? namespaces appropriately
 * 
 */

#endif
