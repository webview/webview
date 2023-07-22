#ifndef WEBVIEW_MINGW_SUPPORT_H
#define WEBVIEW_MINGW_SUPPORT_H
#ifdef _WIN32

#ifndef __eventtoken_h__
#include <cstdint>

typedef struct EventRegistrationToken {
  int64_t value;
} EventRegistrationToken;
#endif

#ifndef WC_ERR_INVALID_CHARS
#define WC_ERR_INVALID_CHARS 0
#endif

#ifndef DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE
#define DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE ((DPI_AWARENESS_CONTEXT)-3)
#endif

#endif /* _WIN32 */
#endif /* WEBVIEW_MINGW_SUPPORT_H */
