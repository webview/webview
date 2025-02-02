%module webview_napi

%{
    #include <napi.h>
    #include <webview.h>
    #include <JsCallback.h>
%}

//***************************************************** IN MAPS
//casts the `webview_dispatch` cb function parameter to `void *`.
%typemap(in) void (*fn)(webview_t w, void *arg) {
    void *ptr = info[1].As<Napi::External<void>>().Data();
    auto fn = reinterpret_cast<void (*)(webview_t, void *)>(ptr);
    $1 = fn;
}
//casts the `webview_bind` cb function parameter to `void *`.
%typemap(in) void (*fn)(const char *id, const char *req, void *arg) {
    void *ptr = info[2].As<Napi::External<void>>().Data();
    auto fn = reinterpret_cast<void (*)(const char *, const char *, void *)>(ptr);
    $1 = fn;
}
//retrieves `webview_t` as a `void *` pointer from the`BigInt` memory address.
%typemap(in) webview_t w {
    $1 = getPtrFromAddress(info[0]);
}
//retrieves the `webview_create` "window" parameter as a `void *` pointer from the`BigInt` memory address.
%typemap(in) void *window {
    if(!info[1].IsNull()) $1 = getPtrFromAddress(info[1]);     
}
//Handles and casts the `webview_dispatch`/`webview_bind` "arg" parameter to a `void *` pointer.
%typemap(in) void *arg {
    auto jsArg = info[info.Length() -1];
    auto jsObject = jsArg.As<Napi::Object>();
    Value cbUid_maybe;
    Value argId;
    NAPI_CHECK_RESULT(jsObject.Get("cbUid"), cbUid_maybe);
    NAPI_CHECK_RESULT(jsObject.Get("argId"), argId);
    auto cbUid = cbUid_maybe.ToNumber().Uint32Value();
    if(argId.IsNull()){
        //JsCallback instance was destroyed before the callback was called.
        void *voidPtr = nullptr;
        $1 = voidPtr;
    } else {
        auto ids = new std::vector<uint32_t>({cbUid, argId.ToNumber().Uint32Value()});
        void *voidPtr = ids;
        $1 = voidPtr;
    }
}
//casts the `webview_get_native_handle` "kind" parameter to `webview_native_handle_kind_t`
%typemap(in) webview_native_handle_kind_t kind {
    auto isValid = info[1].IsNumber() && info[1].ToNumber().Uint32Value() < 3;
    if(!isValid){
        SWIG_Error(SWIG_ERROR, "Argument 2 for webview_get_native_handle must be of type `webview_native_handle_kind`.");
    }
    auto num = info[1].ToNumber().Uint32Value();
    $1 = static_cast<webview_native_handle_kind_t>(num);
}

//***************************************************** OUT MAPS
// Casts pointers to `uint64_t` aka `Napi::BigInt` addresses so that they can be passed using JS IPC.
%typemap(out) void *webview_create {
    $result = getAddressFromPtr(info.Env(), $1);
}
%typemap(out) void *webview_get_window {
    $result = getAddressFromPtr(info.Env(), $1);
}
%typemap(out) void *webview_get_native_handle{
    $result = getAddressFromPtr(info.Env(), $1);
}
// Formats the Webview version information to N-Api
%typemap(out) const webview_version_info_t *{
    std::string version_number = $1->version_number;
    std::string pre_release = $1->pre_release;
    std::string build_metadata = $1->build_metadata;

    auto wvVersion = Object::New(env);
    wvVersion.Set("major", Number::New(env, $1->version.major));
    wvVersion.Set("minor", Number::New(env, $1->version.minor));
    wvVersion.Set("major", Number::New(env, $1->version.patch));

    auto jsObject = Object::New(env);
    jsObject.Set("version", wvVersion);
    jsObject.Set("version_number", version_number);
    jsObject.Set("pre_release", pre_release);
    jsObject.Set("build_metadata", build_metadata);

    $result = jsObject;
}

//***************************************************** INCLUDE
extern const webview_version_info_t *webview_version(void);
%include "../../../webview.i"











