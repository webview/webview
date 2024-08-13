#ifndef WEBVIEW_MINGW_SUPPORT_H
#define WEBVIEW_MINGW_SUPPORT_H
#ifdef _WIN32

// Some MinGW distributions do not have the EventToken.h header (used by WebView2).
// Define the things used by WebView2 from the EventToken.h header if needed.
#if defined(__has_include)
#if !defined(WEBVIEW_HAVE_EVENTTOKEN_H) && __has_include(<EventToken.h>)
#include <EventToken.h>
#define WEBVIEW_HAVE_EVENTTOKEN_H
#endif
#if !defined(WEBVIEW_HAVE_EVENTTOKEN_H) && defined(__eventtoken_h__)
#define WEBVIEW_HAVE_EVENTTOKEN_H
#endif
#if !defined(WEBVIEW_HAVE_EVENTTOKEN_H)
#include <cstdint>
typedef struct EventRegistrationToken {
  int64_t value;
} EventRegistrationToken;
#endif
#endif

#endif /* _WIN32 */
#endif /* WEBVIEW_MINGW_SUPPORT_H */
