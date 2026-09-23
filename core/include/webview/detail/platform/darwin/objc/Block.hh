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

#ifndef WEBVIEW_PLATFORM_DARWIN_OBJC_BLOCK_HH
#define WEBVIEW_PLATFORM_DARWIN_OBJC_BLOCK_HH

#if defined(__cplusplus) && !defined(WEBVIEW_HEADER)

#include "../../../../macros.h"

#if defined(WEBVIEW_PLATFORM_DARWIN)

#include <Block.h>
#include <functional>
#include <objc/objc-runtime.h>
#include <type_traits>
#include <utility>

namespace webview {
namespace detail {
namespace objc {

template <typename Signature> struct Block;

// Block<Ret(Args...)>
//
// Ret      – return type of the completion handler
// Args...  – parameters the ObjC runtime passes to the block
template <typename Ret, typename... Args> struct Block<Ret(Args...)> {
  using function_type = std::function<Ret(Args...)>;

  struct State {
    std::function<Ret(Args...)> fn;
  };

  // Static descriptor shared by all instances of one block type
  struct Descriptor {
    unsigned long reserved;
    unsigned long size;
    void (*copy_helper)(void *dst, const void *src);
    void (*dispose_helper)(void *src);
  };

  // ABI header (must be exactly this layout): https://clang.llvm.org/docs/Block-ABI-Apple.html
  void *isa = &_NSConcreteStackBlock;
  int flags = 0;
  int reserved = 0;
  Ret (*invoke)(Block *, Args...) = nullptr;
  Descriptor *descriptor = nullptr;
  State *state = nullptr;

  static Descriptor &shared_descriptor() {
    static Descriptor d = {0, sizeof(Block), &copy_helper, &dispose_helper};
    return d;
  }

  // store the std::function inside the state
  static Block *make(std::function<Ret(Args...)> fn) {
    auto *b = new Block();
    b->descriptor = &shared_descriptor();
    b->invoke = &invoke_trampoline;
    b->state = new State{std::move(fn)};
    b->flags |= (1 << 25); // BLOCK_HAS_COPY_DISPOSE
    return b;
  }

  static void release(Block *b) { delete b; }

  static void copy_helper(void *dst, const void *src) {
    auto *d = static_cast<Block *>(dst);
    const auto *s = static_cast<const Block *>(src);
    d->state = new State(*s->state);
  }

  static void dispose_helper(void *src) {
    auto *b = static_cast<Block *>(src);
    delete b->state;
    b->state = nullptr;
  }

  static Ret invoke_trampoline(Block *self, Args... args) {
    // Works for both void and non-void Ret without `if constexpr`:
    //  - `return <void expr>;` is valid in a function returning void
    //  - `Ret()` is a valid (value-initialized) expression even when Ret is void
    if (self->state && self->state->fn) {
      return self->state->fn(std::forward<Args>(args)...);
    }
    return Ret();
  }
};

template <typename Signature>
Block<Signature> *make_block(std::function<Signature> fn) {
  return Block<Signature>::make(std::move(fn));
}

} // namespace objc
} // namespace detail
} // namespace webview

#endif // defined(WEBVIEW_PLATFORM_DARWIN) && defined(WEBVIEW_COCOA)
#endif // defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#endif // WEBVIEW_PLATFORM_DARWIN_OBJC_BLOCK_HH
