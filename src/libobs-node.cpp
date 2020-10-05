#include <napi.h>
#include <obs.h>

static Napi::Boolean startup(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    bool result = obs_startup("en-US", nullptr, nullptr);
    return Napi::Boolean::New(env, result);
}

static Napi::Object Init(Napi::Env env, Napi::Object exports) {
    exports.Set(Napi::String::New(env, "startup"), Napi::Function::New(env, startup));
    return exports;
}

NODE_API_MODULE(libobs_node, Init)