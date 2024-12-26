#ifndef EXAMPLE_H
#define EXAMPLE_H

#include <functional>
#include <memory>
#include <node_api.h>
#include <stdint.h>

class JSCallback {
private:
  napi_env env;
  napi_ref functionRef;
  std::function<void(void *)> callback;

public:
  inline JSCallback() : env(nullptr), functionRef(nullptr), callback(nullptr) {}

  inline virtual ~JSCallback() {
    if (functionRef != nullptr) {
      free();
    }
  }

  inline void init(napi_env env, napi_value jsCallback) {
    this->env = env;

    napi_status status =
        napi_create_reference(env, jsCallback, 1, &functionRef);
    if (status != napi_ok) {
      napi_throw_error(env, nullptr, "Failed to create reference to callback");
      return;
    }

    callback = [this](void *data) {
      napi_value global, callback, result;
      napi_value args[1];

      napi_get_global(this->env, &global);
      napi_get_reference_value(this->env, this->functionRef, &callback);
      napi_create_bigint_uint64(this->env, reinterpret_cast<uint64_t>(data),
                                &args[0]);
      napi_call_function(this->env, global, callback, 1, args, &result);
    };
  }

  inline const void *getPointer() const { return this; }

  inline void free() {
    if (functionRef != nullptr && env != nullptr) {
      napi_delete_reference(env, functionRef);
      functionRef = nullptr;
    }
  }

  inline void invoke(void *data) {
    if (callback) {
      callback(data);
    }
  }

protected:
  JSCallback(const JSCallback &) = delete;
  JSCallback &operator=(const JSCallback &) = delete;
};

// Function to convert a pointer to a BigInt representation
static inline uint64_t swig_value(void *pointer) { return (uint64_t)pointer; }

// Function to convert a BigInt representation back to a pointer
static inline void *swig_create(uint64_t number) { return (void *)number; }

#endif