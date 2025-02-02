#ifndef JSCALLBACK_H
#define JSCALLBACK_H

#include <iostream>
#include <map>
#include <mutex>
#include <napi.h>
#include <string>

/** @see https://github.com/nodejs/node-addon-api/blob/main/doc/README.md for N-Api documentation. */
using namespace Napi;

typedef void *webview_t;
/** Tracks an `JsCallback` instance lifecycle. */
enum status_t { opened, deferred, closing, closed };
/** Identifies the kind of Webview cb function of the instance. */
enum kind_t { dispatch, bind, unknown };
/** Holds the `arg` ID and `JsCallback` instance UID. */
struct ids_t {
  uint32_t cbUid;
  uint32_t argId;
};
/** Holds the values being passed into the N-Api thread-safe function. */
struct tsfnData_t {
  std::string id;
  std::string req;
  webview_t w;
  uint32_t cbUid;
  uint32_t argId;
};
using tsfnContext = Reference<Value>;
using tsfnData = tsfnData_t;

// Trampoline
// ********************************************************************************************************
class JsCallback;
extern "C" {
/**
  * An utility class providing native static callback functions to Webview.
  * It bounces back functions with correctly shaped signatures for `webview_dispatch` and `webview_bind`.
  * Webview `arg` parameters are used to piggy-back the `JsCallback` instance UID along with the `arg` ID.
  */
class Trampoline {
public:
private:
protected:
  static std::mutex mtx;

  /** A `map` to reference `JsCallback` instances by UID. */
  static std::map<uint32_t, JsCallback *> cbMap;

  /** A native static `webview_dispatch` cb function */
  static void dispatchCb(webview_t w, void *arg);

  /** A native static `webview_bind` cb function */
  static void bindCb(const char *id, const char *req, void *arg);

  /** N-API boilerplate shaped function to call a threadsafe function */
  static void callTsfn(Env env, Function callback, tsfnContext *context,
                       tsfnData_t *data);

  /** N-API boilerplate type alias for a threadsafe function */
  using tsfn_t = TypedThreadSafeFunction<tsfnContext, tsfnData_t, callTsfn>;

  /**
    * It gets `this` from the callback instance map.
    * @param cbUid - The UID of the callback instance.
    */
  static JsCallback *getSelf(uint32_t cbUid);

  /** It converts the void `arg` pointer received from Webview into a usable data format. */
  static ids_t getIds(void *arg);

  /** It formats data to an efficient shape for use in the thread-safe function call. */
  static tsfnData_t *makeTsfnData(std::string id, std::string req, webview_t w,
                                  ids_t ids);

  /** 
    * It gets the thread-safe function from the callback instance map.
    * @param cbUid - The UID of the callback instance.
    */
  static tsfn_t *getTsfn(uint32_t cbUid);

  /** It determines if the the cb instance is still available for execution */
  static bool reject(void *arg);
};
}

// JsCallback
// ********************************************************************************************************

/**
 * A N-Api native module class exposed to JS.
 * It has methods to generate native cb function pointers and manage their memory lifecycle to/from javascript/Webview.
 * @param {Napi::Function} callback - The wrapped user provided JS cb function.
 */
class JsCallback : Trampoline, public ObjectWrap<JsCallback> {
public:
  JsCallback(const CallbackInfo &info);

  /** A `map` of the user arguments passed to the cb function instance */
  std::map<uint32_t, Reference<Napi::Value> *> argMap;

  /** The N-Api thread-safe cb function for the instance. */
  tsfn_t tsfn;

  /**
    * Makes a `Napi::External` pointer for a `webview_dispatch` native cb function.
    * @return {Napi::Bigint} - A Bigint memory address for the cb function.
    */
  Napi::Value getDispatchPtr(const CallbackInfo &info);

  /**
    * Makes a `Napi::External` pointer for a `webview_bind` native cb function.
    * @return {Napi::Bigint} - A Bigint memory address for the cb function.
    */
  Napi::Value getBindPtr(const CallbackInfo &info);

  /** Retrieves a user provided `arg` from an instance `map` by ID. */
  Napi::Value getArg(uint32_t argId);
  /** Destroys a user provided `arg` from an instance `map` by ID. */
  void destroyArg(uint32_t argId);

  /**
    * Frees up associated memory when a cb function instance is no longer needed.
    * This method must explicitly be called by the user.
    */
  void destroy(bool isShutdown);

  /** Is the cb function kind for `webview_bind` or `webview_dispatch`? */
  kind_t cbKind;

  /** A tally of the cb queue size*/
  int cbQTally = 0;

  /**Tracks the lifecycle of the cb function instance */
  status_t cbStatus = opened;

  /** The boilerplate N-Api class initialiser */
  static Object Init(Napi::Env env, Object exports);

private:
  /** A unique ID for the `JsCallback` class instance */
  const uint32_t cbUid;

  /** Proxy for JS access to `destroy` */
  void destroy(const CallbackInfo &info);

  /** Accessor to get the cbUid in JS */
  Napi::Value getCbUid(const CallbackInfo &info);

  /** Stores a user provided `arg` to an instance `map` by ID. */
  void setArg(const CallbackInfo &info);

  /** Counter for generating a `JsCallback` instance `arg` ID */
  uint32_t argCount = 0;

  /** Counter for generating an unique `JsCallback` instance UID */
  static uint32_t cbCount;
};

// Shared utilities with the SWIG interface
// ********************************************************************************************************
#ifndef SWIG_VERSION
#define SWIG_ERROR (-1)
#define SWIG_Error(code, msg)
#define SWIG_VERSION false
#endif

/** Global logger function for debugging purposes. */
inline void debug(std::string message) {
  std::string prefix = (!!SWIG_VERSION) ? "[Swig] " : "[JsCallback] ";
  std::cout << prefix << message << std::endl;
}

/**
 * It converts a void * pointer to uint64_t sized memory address.
 * This allows the `webview_t` pointer to be passed between threads in JS using IPC.
 * @param env {Napi::Env} - The N-Api JS environment.
 * @param ptr {void *} - A pointer to the memory location you want to convert.
 * @return {Napi::Value} aka {Napi::BigInt} - The memory address.
 */
inline Value getAddressFromPtr(Env env, void *ptr) {
  auto address = (uint64_t)ptr;
  auto jsAddress = BigInt::New(env, address);

  return Value(env, jsAddress);
};

/**
 * The counterpart to `getAddressFromPtr`.
 * It converts a uint64_t sized memory address to a void * pointer.
 * @param jsAddress {Napi::Value} aka {Napi::BigInt}
 * @return {void *}
 */
inline void *getPtrFromAddress(Value jsAddress) {
  auto env = jsAddress.Env();
  HandleScope scope(env);

  if (!jsAddress.IsBigInt()) {
    return SWIG_Error(SWIG_ERROR,
                      "pointer reference must be a uint64_t memory address");
  }
  bool lossless;
  uint64_t address = jsAddress.As<BigInt>().Uint64Value(&lossless);
  if (!lossless) {
    return SWIG_Error(SWIG_ERROR,
                      "Information lost retrieving a pointer from an address.");
  }
  return reinterpret_cast<void *>(address);
};

#endif
