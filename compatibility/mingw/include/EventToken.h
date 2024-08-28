#ifndef WEBVIEW_COMPAT_EVENTTOKEN_H
#define WEBVIEW_COMPAT_EVENTTOKEN_H
#ifdef _WIN32

// This compatibility header is useful when cross-compiling because and when
// the "EventToken.h" header used by WebView2 is unavailable.

#ifndef __eventtoken_h__
#include <cstdint>
typedef struct EventRegistrationToken {
  int64_t value;
} EventRegistrationToken;
#endif /* __eventtoken_h__ */

#endif /* _WIN32 */
#endif /* WEBVIEW_COMPAT_EVENTTOKEN_H */
