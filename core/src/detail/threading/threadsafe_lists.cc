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

#ifndef WEBVIEW_DETAIL_THREADSAFE_LISTS_CC
#define WEBVIEW_DETAIL_THREADSAFE_LISTS_CC

#if defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#include "detail/threading/threadsafe_lists.hh"
#include "detail/engine_base.hh"
#include <algorithm>
#include <iterator>

using namespace webview::detail::threading;
using namespace webview::_lib::_detail::_threading;

/* Nested API _lib for thread-safe functions
 * ∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇∇ */

/* Thread-safe operations on a name map of active bindings
 * ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ */

size_t bindings_t::size() const {
  std::lock_guard<std::mutex> lock(mtx);
  return bindings_map.size();
}
void bindings_t::get_names(std::vector<std::string> &bound_names) {
  std::lock_guard<std::mutex> lock(mtx);
  bound_names.reserve(bindings_map.size());
  std::transform(bindings_map.begin(), bindings_map.end(),
                 std::back_inserter(bound_names),
                 [](const std::pair<std::string, binding_ctx_t> &pair) {
                   return pair.first;
                 });
}
bool bindings_t::has_name(const std::string &name) const {
  std::lock_guard<std::mutex> lock(mtx);
  bool is_found{true};
  auto found = bindings_map.find(name);
  if (found == bindings_map.end()) {
    is_found = false;
  }
  found = bindings_map.end();
  return is_found;
}
void bindings_t::emplace(const std::string &name, binding_t fn, void *arg) {
  std::lock_guard<std::mutex> lock(mtx);
  bindings_map.emplace(name, binding_ctx_t(fn, arg));
}
void bindings_t::erase(const std::string &name) {
  std::lock_guard<std::mutex> lock(mtx);
  bindings_map.erase(name);
}
size_t bindings_t::count(const std::string &name) const {
  std::lock_guard<std::mutex> lock(mtx);
  return bindings_map.count(name);
}
binding_ctx_t bindings_t::at(const std::string &name) const {
  std::lock_guard<std::mutex> lock(mtx);
  return bindings_map.at(name);
}

/* Thread-safe operations for adding and replacing user functions and scripts
 * ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ */

user_script *user_scripts_t::add(const std::string &js, engine_base *base) {
  std::lock_guard<std::mutex> lock(mtx);

  return std::addressof(*m_user_scripts.emplace(
      m_user_scripts.end(), base->add_user_script_impl(js)));
}
user_script *user_scripts_t::replace(const user_script &old_script,
                                     const std::string &new_script_code,
                                     engine_base *base) {
  std::lock_guard<std::mutex> lock(mtx);

  base->remove_all_user_scripts_impl(m_user_scripts);
  user_script *old_script_ptr{};
  for (auto &script : m_user_scripts) {
    auto is_old_script = base->are_user_scripts_equal_impl(script, old_script);
    script = base->add_user_script_impl(is_old_script ? new_script_code
                                                      : script.get_code());
    if (is_old_script) {
      old_script_ptr = std::addressof(script);
    }
  }
  return old_script_ptr;
}

/* Thread-safe operations for the user actions queue
 * ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ */

action_t queue_t::front() {
  std::lock_guard<std::mutex> lock(mtx);
  return queue.front();
}
size_t queue_t::size() const {
  std::lock_guard<std::mutex> lock(mtx);
  return queue.size();
}
void queue_t::pop_front() {
  std::lock_guard<std::mutex> lock(mtx);
  queue.pop_front();
}
void queue_t::push_back(context_t ctx, dispatch_fn_t work_fn,
                        std::string name_or_js) {
  std::lock_guard<std::mutex> lock(mtx);
  queue.push_back({ctx, work_fn, name_or_js});
}
void queue_t::clear() {
  std::lock_guard<std::mutex> lock(mtx);
  queue.clear();
}
bool queue_t::empty() const {
  std::lock_guard<std::mutex> lock(mtx);
  return queue.empty();
}

/* Thread-safe operations on a map of unresolved promises
 * ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ */

void unres_promises_t::set(const std::string &name,
                           std::list<std::string> ids) {
  std::lock_guard<std::mutex> lock(mtx);
  unres_promises[name] = std::move(ids);
}
std::list<std::string>
unres_promises_t::get_copy(const std::string &name) const {
  std::lock_guard<std::mutex> lock(mtx);
  auto found = unres_promises.find(name);
  if (found == unres_promises.end()) {
    return {};
  }
  std::list<std::string> list_copy = found->second;
  found = unres_promises.end();
  return list_copy;
}
void unres_promises_t::remove_id(const std::string &name,
                                 const std::string &id) {
  std::lock_guard<std::mutex> lock(mtx);
  auto found = unres_promises.find(name);
  if (found == unres_promises.end()) {
    return;
  }
  found->second.remove(id);
  found = unres_promises.end();
}
void unres_promises_t::add_id(const std::string &name, const std::string &id) {
  std::lock_guard<std::mutex> lock(mtx);
  auto found = unres_promises.find(name);
  if (found == unres_promises.end()) {
    unres_promises.emplace(name, std::list<std::string>{id});
  } else {
    found->second.push_back(id);
  }
  found = unres_promises.end();
}
void unres_promises_t::erase(const std::string &name) {
  std::lock_guard<std::mutex> lock(mtx);
  unres_promises.erase(name);
}
bool unres_promises_t::empty(const std::string &name) const {
  std::lock_guard<std::mutex> lock(mtx);
  auto found = unres_promises.find(name);
  if (found == unres_promises.end()) {
    return true;
  }
  auto empty = found->second.empty();
  found = unres_promises.end();
  return empty;
}

/* Thread-safe operations on a map of promise id's to binding name.
 * ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ */

std::string id_name_map_t::get(const std::string &id) const {
  std::lock_guard<std::mutex> lock(mtx);
  auto found = id_name.find(id);
  if (found == id_name.end()) {
    return "";
  }
  auto name = found->second;
  found = id_name.end();
  return name;
}
void id_name_map_t::set(const std::string &id, const std::string &name) {
  std::lock_guard<std::mutex> lock(mtx);
  id_name[id] = name;
}
void id_name_map_t::erase(const std::string &id) {
  std::lock_guard<std::mutex> lock(mtx);
  id_name.erase(id);
}

/* Thread-safe operations on an ordered list of pending bind / unbind operations in the queue.
 * ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓ */

void pending_t::pop_front() {
  std::lock_guard<std::mutex> lock(mtx);
  pending_bind_unbind.pop_front();
}
void pending_t::push_back(const std::string &name) {
  std::lock_guard<std::mutex> lock(mtx);
  pending_bind_unbind.push_back(name);
}
indices_t pending_t::indices(const std::string &name) const {
  std::lock_guard<std::mutex> lock(mtx);
  auto len = int(pending_bind_unbind.size());
  int bind_i = -1;
  int unbind_i = -1;
  for (int i = 0; i < len; i++) {
    auto val = pending_bind_unbind.at(i);
    if (val == "bind-" + name) {
      bind_i = i;
    }
    if (val == "unbind-" + name) {
      unbind_i = i;
    }
  }
  return {bind_i, unbind_i};
}

/* ∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆∆  
 * Nested API _lib for thread-safe functions */

#endif // defined(__cplusplus) && !defined(WEBVIEW_HEADER)
#endif // WEBVIEW_DETAIL_THREADSAFE_LISTS_CC
