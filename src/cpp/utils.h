#pragma once

#define TRY_METHOD(method) \
    try { \
        method; \
    } catch (std::exception &e) { \
        Napi::Error::New(info.Env(), e.what()).ThrowAsJavaScriptException(); \
    } catch (...) { \
        Napi::Error::New(info.Env(), "Unexpected error.").ThrowAsJavaScriptException(); \
    }

inline int getNapiInt(Napi::Object object, const std::string &property) {
    auto value = object.Get(property);
    if (value.IsUndefined()) {
        throw std::invalid_argument(property + " should not be undefined");
    }
    return value.As<Napi::Number>();
}

inline std::string getNapiString(Napi::Object object, const std::string &property) {
    auto value = object.Get(property);
    if (value.IsUndefined()) {
        throw std::invalid_argument(property + " should not be undefined");
    }
    return value.As<Napi::String>();
}

inline std::string getNapiStringOrDefault(Napi::Object object, const std::string &property, const std::string &defaultValue) {
    auto value = object.Get(property);
    return value.IsUndefined() ? defaultValue : value.As<Napi::String>();
}

inline bool getNapiBoolean(Napi::Object object, const std::string &property) {
    auto value = object.Get(property);
    if (value.IsUndefined()) {
        throw std::invalid_argument(property + " should not be undefined");
    }
    return value.As<Napi::Boolean>();
}