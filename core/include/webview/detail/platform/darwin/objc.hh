/*
 * MIT License
 *
 * Copyright (c) 2017 Serge Zaitsev
 * Copyright (c) 2022 Steffen André Langnes
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#if !defined(WEBVIEW_PLATFORM_DARWIN_OBJC_HH) &&                               \
    defined(WEBVIEW_PLATFORM_DARWIN)
#define WEBVIEW_PLATFORM_DARWIN_OBJC_HH

#include <cstddef>

#include <objc/NSObjCRuntime.h>
#include <objc/objc-runtime.h>

namespace webview {
namespace detail {
namespace objc {

// A convenient template function for unconditionally casting the specified
// C-like function into a function that can be called with the given return
// type and arguments. Caller takes full responsibility for ensuring that
// the function call is valid. It is assumed that the function will not
// throw exceptions.
template <typename Result, typename Callable, typename... Args>
Result invoke(Callable callable, Args... args) noexcept {
  return reinterpret_cast<Result (*)(Args...)>(callable)(args...);
}

// Calls objc_msgSend.
template <typename Result, typename... Args>
Result msg_send(Args... args) noexcept {
  return invoke<Result>(objc_msgSend, args...);
}

// Calls objc_msgSend_stret or objc_msgSend depending on architecture.
template <typename Result, typename... Args>
Result msg_send_stret(Args... args) noexcept {
#if defined(__arm64__)
  return invoke<Result>(objc_msgSend, args...);
#else
  return invoke<Result>(objc_msgSend_stret, args...);
#endif
}

// Wrapper around NSAutoreleasePool that drains the pool on destruction.
class autoreleasepool {
public:
  autoreleasepool()
      : m_pool(msg_send<id>(objc_getClass("NSAutoreleasePool"),
                            sel_registerName("new"))) {}

  ~autoreleasepool() {
    if (m_pool) {
      msg_send<void>(m_pool, sel_registerName("drain"));
    }
  }

  autoreleasepool(const autoreleasepool &) = delete;
  autoreleasepool &operator=(const autoreleasepool &) = delete;
  autoreleasepool(autoreleasepool &&) = delete;
  autoreleasepool &operator=(autoreleasepool &&) = delete;

private:
  id m_pool{};
};

inline id autoreleased(id object) {
  msg_send<void>(object, sel_registerName("autorelease"));
  return object;
}

namespace literals {

// Convenient conversion of string literals.
inline id operator"" _cls(const char *s, std::size_t) {
  return (id)objc_getClass(s);
}

inline SEL operator"" _sel(const char *s, std::size_t) {
  return sel_registerName(s);
}

inline id operator"" _str(const char *s, std::size_t) {
  return msg_send<id>("NSString"_cls, "stringWithUTF8String:"_sel, s);
}

} // namespace literals
} // namespace objc
} // namespace detail
} // namespace webview

#endif // WEBVIEW_PLATFORM_DARWIN_OBJC_HH
