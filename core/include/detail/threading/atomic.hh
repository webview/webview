/*
 * MIT License
 *
 * Copyright (c) 2025 Michael Jonker
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

#ifndef WEBVIEW_DETAIL_THREADING_ATOMIC_HH
#define WEBVIEW_DETAIL_THREADING_ATOMIC_HH

#if defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#include "types/types.hh"
#include <atomic>
#include <condition_variable>

using namespace webview::types;
namespace webview {
namespace detail {
namespace threading {
// forward declaration
class atomic_api_t;
} // namespace threading
} // namespace detail
using namespace detail::threading;
namespace _lib {
namespace _detail {
namespace _threading {

/// Nested API to get and set if the window DOM is ready
struct atomic_dom_ready_t : nested_api_t<atomic_api_t> {
  atomic_dom_ready_t(atomic_api_t *self) : nested_api_t(self) {}

  /// Query if the Webview JS is ready to take commands.
  bool webview_ready() const;
  /// Set if the Webview JS is ready to take commands.
  void webview_ready(bool flag);
  /// Query if the threading is ready to do work.
  bool ready() const;
  /// Notify the queue that the threading is ready to receive work.
  void ready(bool flag);
};

/// Nested API to get and set if various queue operations have completed
struct atomic_done_t : nested_api_t<atomic_api_t> {
  atomic_done_t(atomic_api_t *self) : nested_api_t(self) {}
  /// Gets the bind flag state
  bool bind() const;
  /// Sets the bind flag state
  void bind(bool val);
  /// Gets the unbind flag state
  bool unbind() const;
  /// Sets the unbind flag state
  void unbind(bool val);
  /// Gets the eval flag state
  bool eval() const;
  /// Sets the eval flag state
  void eval(bool val);
};
} // namespace _threading
} // namespace _detail
} // namespace _lib

using namespace _lib::_detail::_threading;
namespace detail {
namespace threading {

/// Root API to work with atomic flags
class atomic_api_t {

  /// The nested root API
  struct api_root_t : nested_api_t<atomic_api_t> {
    api_root_t(atomic_api_t *self) : nested_api_t(self){};

    /// Get and set if the Webview JS and window DOM is ready
    atomic_dom_ready_t dom{self};
    /// Get and set if various queue operations have completed
    atomic_done_t done{self};
    /// Query if Webview is busy shutting down
    bool terminating() const;
    /// Evaluate mutiple conditions with automatic `terminating` guard
    bool AND(std::initializer_list<bool> flags) const;
  };

  /// Condition variables for various stages in the event loop lifecycle
  struct cv_api_t {
    cv_api_t();

    /// Controls the main queue loop
    std::condition_variable queue;
    /// Controls the bind section of the event loop
    std::condition_variable bind;
    /// Controls the eval section of the event loop
    std::condition_variable eval;
    /// Controls the unbind section of the event loop
    std::condition_variable unbind;
    /// Controls the unbind timeout section of the event loop
    std::condition_variable unbind_timeout;
    /// An array of all the event loop condition variables
    std::condition_variable *all[5];
    /// Calls `notify_all` on all event loop condition variables
    void notify_all();
  };

protected:
  std::atomic_bool is_webview_ready{};
  std::atomic_bool is_dom_ready{};
  std::atomic_bool unbind_done{};
  std::atomic_bool bind_done{};
  std::atomic_bool eval_done{};
  std::atomic_bool is_terminating{};

  cv_api_t cv{};
  api_root_t atomic{this};

  friend struct _lib::_detail::_threading::atomic_done_t;
  friend struct _lib::_detail::_threading::atomic_dom_ready_t;
};

} // namespace threading
} // namespace detail
} // namespace webview

#endif // defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#endif // WEBVIEW_DETAIL_THREADING_ATOMIC_HH
