#ifndef WEBVIEW_COMPAT_EVENTTOKEN_H
#define WEBVIEW_COMPAT_EVENTTOKEN_H
#ifdef _WIN32

// This compatibility header provides types used by MS WebView2. This header can
// be used as an alternative to the "EventToken.h" header normally provided by
// the Windows SDK. Depending on the MinGW distribution, this header may not be
// present, or it may be present with the name "eventtoken.h". The letter casing
// matters when cross-compiling on a system with case-sensitive file names.

#ifndef __eventtoken_h__

#ifdef __cplusplus
#include <cstdint>
#else
#include <stdint.h>
#endif

typedef struct EventRegistrationToken {
  int64_t value;
} EventRegistrationToken;
#endif // __eventtoken_h__

#endif // _WIN32
#endif // WEBVIEW_COMPAT_EVENTTOKEN_H
