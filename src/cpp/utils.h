#pragma once

#define TRY_METHOD(method) \
    try { \
        method; \
    } catch (std::exception &e) { \
        Napi::Error::New(info.Env(), e.what()).ThrowAsJavaScriptException(); \
    } catch (...) { \
        Napi::Error::New(info.Env(), "Unexpected error.").ThrowAsJavaScriptException(); \
    }
