#ifndef WEBVIEW_COMPAT_EVENTTOKEN_H
#define WEBVIEW_COMPAT_EVENTTOKEN_H
#ifdef _WIN32

// Some MinGW distributions do not have the EventToken.h header (used by WebView2).
// Define the things used by WebView2 from the EventToken.h header if needed.

#include <cstdint>
typedef struct EventRegistrationToken {
  int64_t value;
} EventRegistrationToken;

#endif /* _WIN32 */
#endif /* WEBVIEW_COMPAT_EVENTTOKEN_H */
