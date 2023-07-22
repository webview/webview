#ifndef WEBVIEW_MINGW_SUPPORT_H
#define WEBVIEW_MINGW_SUPPORT_H
#ifdef _WIN32

#ifndef __eventtoken_h__
#include <cstdint>

typedef struct EventRegistrationToken {
  int64_t value;
} EventRegistrationToken;
#endif

#endif /* _WIN32 */
#endif /* WEBVIEW_MINGW_SUPPORT_H */
