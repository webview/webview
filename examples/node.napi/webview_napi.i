%module webview_napi

%{
#include "webview_napi.h"
#include <node_api.h>

static napi_env __swig_env = nullptr;
%}

// Enable directors
%feature("director") JSCallback;

%init %{
    __swig_env = env;
%}

// Typemap for converting JavaScript function to napi_value
%typemap(in) napi_value {
    $1 = $input;
}

// Ignore the default constructor
%ignore JSCallback::JSCallback();

// Add the constructor we want
%extend JSCallback {
    JSCallback(napi_value jsCallback) {
        auto result = new JSCallback();
        result->init(__swig_env, jsCallback);
        return result;
    }
}

%include "stdint.i" // Ensure uint64_t is recognized in SWIG
%include "webview_napi.h"
%include "../../webview.i" // The full webview interface