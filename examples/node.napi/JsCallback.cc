
#include "JsCallback.h"
#include <string>

using namespace Napi;

// JsCallback
// ********************************************************************************************************
JsCallback::JsCallback(const CallbackInfo &info)
    : ObjectWrap<JsCallback>(info),
      cbUid(cbCount) {

    std::lock_guard<std::mutex> lock(mtx);
    cbCount++;

    //auto self = new std::unique_ptr<JsCallback>(this);
    auto *context = new Reference<Napi::Value>(Persistent(info.This()));
    tsfn = static_cast<Trampoline::tsfn_t>(Trampoline::tsfn_t::New(
        info.Env(),
        info[0].As<Function>(),
        "tsfn",
        0,
        1,
        context
    ));
    cbMap.emplace(cbUid, this);

    cleanup_tsf = [this, &info]() {
        this->tsCleanup = true;
        this->cleanup(info);
    };
    cleanup_f = [this, &info]() {
        this->tsCleanup = false;
        this->cleanup(info);
    };
}

Napi::Value JsCallback::getDispatchPtr(const CallbackInfo &info) {
    cbKind = dispatch;
    auto ptr = reinterpret_cast<void *>(&Trampoline().dispatchCb);
    return External<void>::New(info.Env(), ptr);
}

Napi::Value JsCallback::getBindPtr(const CallbackInfo &info) {
    cbKind = bind;
    auto ptr = reinterpret_cast<void *>(&Trampoline().bindCb);
    return External<void>::New(info.Env(), ptr);
}

Napi::Value JsCallback::getCbUid(const CallbackInfo &info) {
    return Napi::Number::New(info.Env(), cbUid).As<Napi::Value>();
}

Napi::Value JsCallback::getArgId(const CallbackInfo &info) {
    if (cbStatus != opened)
        return info.Env().Null();
    auto argId = argCount;
    argCount++;
    return Napi::Number::New(info.Env(), argId).As<Napi::Value>();
}

void JsCallback::setArg(const CallbackInfo &info) {
    std::lock_guard<std::mutex> lock(mtx);
    //auto argObj = info[0].ToObject();
    auto argId = info[1].ToNumber().Int32Value();
    auto argRef = new Reference<Napi::Value>(Persistent(info[0]));
    argMap.emplace(argId, argRef);
    if (cbKind == dispatch)
        cbTally++;
}
Napi::Value JsCallback::getArg(uint32_t argId) {
    auto jsArgObj = argMap.at(argId)->Value().As<Napi::Object>();
    auto jsArg = jsArgObj.Get("arg");
    return jsArg;
};

void JsCallback::cleanup(const CallbackInfo &info) {
    if (cbStatus == closed)
        return;
    auto doCleanup = [this]() {
        cbStatus = closed;
        tsfn.Abort();
        for (auto &pair : argMap) {
            auto argRef = pair.second;
            argRef->Reset();
        }
        argMap.clear();
        cbMap.erase(cbUid);
    };
    if (tsCleanup) {
        std::lock_guard<std::mutex> lock(mtx);
        if (cbTally > 0) {
            cbStatus = deferred;
            return;
        }
        doCleanup();
        delete this;
    } else {
        doCleanup();
    }
};

uint32_t JsCallback::cbCount = 0;

// Trampoline
// ********************************************************************************************************

void Trampoline::dispatchCb(webview_t w, void *arg) {
    std::lock_guard<std::mutex> lock(mtx);
    if (reject(arg))
        return;
    auto ids = getIds(arg);
    auto tsfn = getTsfn(ids.cbUid);
    auto tsfnData = makeTsfnData("", "", w, ids);
    tsfn->BlockingCall(tsfnData);
};

void Trampoline::bindCb(const char *id, const char *req, void *arg) {
    std::lock_guard<std::mutex> lock(mtx);
    if (reject(arg))
        return;
    auto ids = getIds(arg);
    auto tsfn = getTsfn(ids.cbUid);
    auto tsfnData = makeTsfnData(id, req, nullptr, ids);
    tsfn->BlockingCall(tsfnData);
};

void Trampoline::callTsfn(
    Env env,
    Function callback,
    tsfnContext *context,
    tsfnData_t *data
) {

    auto self = getSelf(data->cbUid);
    auto jsArg = self->getArg(data->argId);
    if (self->cbKind == dispatch) {
        auto jsW = getAddressFromPtr(env, data->w);
        try {
            callback.Call(context->Value(), {jsW, jsArg});
        } catch (const Napi::Error &e) {
            e.ThrowAsJavaScriptException();
        }
        self->argMap.erase(data->argId);
        self->cbTally--;
        if (self->cbStatus == deferred && self->cbTally == 0) {
            self->cleanup_tsf();
        }
    }
    if (self->cbKind == bind) {
        auto jsId = String::New(env, data->id);
        auto jsReq = String::New(env, data->req);
        try {
            callback.Call(context->Value(), {jsId, jsReq, jsArg});
        } catch (const Napi::Error &e) {
            e.ThrowAsJavaScriptException();
        }
    }
};

ids_t Trampoline::getIds(void *arg) {
    auto vctr = reinterpret_cast<std::vector<uint32_t> *>(arg);
    ids_t ids = {};
    ids.cbUid = (*vctr)[0];
    ids.argId = (*vctr)[1];
    return ids;
};
tsfnData *Trampoline::makeTsfnData(
    std::string id,
    std::string req,
    webview_t w,
    ids_t ids
) {
    tsfnData_t data = {};
    data.id = id;
    data.req = req;
    data.w = w;
    data.cbUid = ids.cbUid;
    data.argId = ids.argId;
    return new tsfnData_t(data);
};
Trampoline::tsfn_t *Trampoline::getTsfn(uint32_t cbUid) {
    auto self = getSelf(cbUid);
    if (self == nullptr)
        return nullptr;
    return &self->tsfn;
};
JsCallback *Trampoline::getSelf(uint32_t cbUid) {
    int cbCount = cbMap.count(cbUid);
    if (cbCount == 0) {
        //The callback was cleaned up before being executed.
        return nullptr;
    }
    auto self = cbMap.at(cbUid);
    return self;
};
bool Trampoline::reject(void *arg) {
    if (arg == nullptr)
        return true;
    auto ids = getIds(arg);
    auto self = getSelf(ids.cbUid);
    if (self == nullptr)
        return true;
    if (self->cbStatus == closed)
        return true;
    return false;
}

// N-Api module initialiser
// ********************************************************************************************************
Object JsCallback::Init(Napi::Env env, Object exports) {
    Function func = DefineClass(
        env,
        "JsCallback",
        {
            InstanceMethod("cleanup", &JsCallback::cleanup),
            InstanceMethod("setArg", &JsCallback::setArg),
            InstanceAccessor<&JsCallback::getDispatchPtr>("dispatchPtr"),
            InstanceAccessor<&JsCallback::getBindPtr>("bindPtr"),
            InstanceAccessor<&JsCallback::getArgId>("argId"),
            InstanceAccessor<&JsCallback::getCbUid>("cbUid"),
        }
    );
    FunctionReference *constructor = new Napi::FunctionReference();
    *constructor = Persistent(func);
    //constructor->SuppressDestruct();
    env.SetInstanceData(constructor);
    exports.Set("JsCallback", func);
    return exports;
}

Object Init(Napi::Env env, Napi::Object exports) {
    return JsCallback::Init(env, exports);
};

NODE_API_MODULE(addon, Init);
