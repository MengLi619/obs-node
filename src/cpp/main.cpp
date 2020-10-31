#include "studio.h"
#include "utils.h"
#include <napi.h>

#ifdef __linux__
// Need QT for linux to setup OpenGL properly.
#include <QApplication>
#include <QPushButton>
QApplication *qApplication;
#endif

std::string obsPath;
Studio *studio = nullptr;
Settings *settings = nullptr;

Napi::Value setObsPath(const Napi::CallbackInfo &info) {
    obsPath = info[0].As<Napi::String>();
    return info.Env().Undefined();
}

Napi::Value startup(const Napi::CallbackInfo &info) {
#ifdef __linux__
    int argc = 0;
    char **argv = nullptr;
    qApplication = new QApplication(argc, argv);
#endif
    settings = new Settings(info[0].As<Napi::Object>());
    studio = new Studio(obsPath, settings);
    TRY_METHOD(studio->startup())
    return info.Env().Undefined();
}

Napi::Value shutdown(const Napi::CallbackInfo &info) {
    TRY_METHOD(studio->shutdown())
#ifdef __linux__
    delete qApplication;
#endif
    delete studio;
    delete settings;
    return info.Env().Undefined();
}

Napi::Value addScene(const Napi::CallbackInfo &info) {
    std::string sceneId = info[0].As<Napi::String>();
    TRY_METHOD(studio->addScene(sceneId))
    return info.Env().Undefined();
}

Napi::Value addSource(const Napi::CallbackInfo &info) {
    std::string sceneId = info[0].As<Napi::String>();
    std::string sourceId = info[1].As<Napi::String>();
    SourceType sourceType = Source::getSourceType(info[2].As<Napi::String>());
    std::string sourceUrl = info[3].As<Napi::String>();
    TRY_METHOD(studio->addSource(sceneId, sourceId, sourceType, sourceUrl))
    return info.Env().Undefined();
}

Napi::Value updateSource(const Napi::CallbackInfo &info) {
    std::string sceneId = info[0].As<Napi::String>();
    std::string sourceId = info[1].As<Napi::String>();
    std::string sourceUrl = info[2].As<Napi::String>();
    TRY_METHOD(studio->updateSource(sceneId, sourceId, sourceUrl))
    return info.Env().Undefined();
}

Napi::Value muteSource(const Napi::CallbackInfo &info) {
    std::string sceneId = info[0].As<Napi::String>();
    std::string sourceId = info[1].As<Napi::String>();
    bool mute = info[2].As<Napi::Boolean>();
    TRY_METHOD(studio->muteSource(sceneId, sourceId, mute))
    return info.Env().Undefined();
}

Napi::Value restartSource(const Napi::CallbackInfo &info) {
    std::string sceneId = info[0].As<Napi::String>();
    std::string sourceId = info[1].As<Napi::String>();
    TRY_METHOD(studio->restartSource(sceneId, sourceId))
    return info.Env().Undefined();
}

Napi::Value switchToScene(const Napi::CallbackInfo &info) {
    std::string sceneId = info[0].As<Napi::String>();
    std::string transitionType = info[1].As<Napi::String>();
    int transitionMs = info[2].As<Napi::Number>();
    TRY_METHOD(studio->switchToScene(sceneId, transitionType, transitionMs))
    return info.Env().Undefined();
}

Napi::Array getScenes(const Napi::CallbackInfo &info) {
    auto scenes = studio->getScenes();
    auto result = Napi::Array::New(info.Env(), scenes.size());
    int i = 0;
    for (auto &scene : scenes) {
        result[(uint32_t)i++] = scene.second->getNapiScene(info.Env());
    }
    return result;
}

Napi::Value createDisplay(const Napi::CallbackInfo &info) {
    std::string displayName = info[0].As<Napi::String>();
    void* parentHandle = info[1].As<Napi::Buffer<void*>>().Data();
    int scaleFactor = info[2].As<Napi::Number>();
    std::string sourceId = info[3].As<Napi::String>();
    TRY_METHOD(studio->createDisplay(displayName, parentHandle, scaleFactor, sourceId))
    return info.Env().Undefined();
}

Napi::Value destroyDisplay(const Napi::CallbackInfo &info) {
    blog(LOG_INFO, "destroyDisplay - 1");
    std::string displayName = info[0].As<Napi::String>();
    TRY_METHOD(studio->destroyDisplay(displayName))
    blog(LOG_INFO, "destroyDisplay - 2");
    return info.Env().Undefined();
}

Napi::Value moveDisplay(const Napi::CallbackInfo &info) {
    std::string displayName = info[0].As<Napi::String>();
    int x = info[1].As<Napi::Number>();
    int y = info[2].As<Napi::Number>();
    int width = info[3].As<Napi::Number>();
    int height = info[4].As<Napi::Number>();
    TRY_METHOD(studio->moveDisplay(displayName, x, y, width, height))
    return info.Env().Undefined();
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
    exports.Set(Napi::String::New(env, "setObsPath"), Napi::Function::New(env, setObsPath));
    exports.Set(Napi::String::New(env, "startup"), Napi::Function::New(env, startup));
    exports.Set(Napi::String::New(env, "shutdown"), Napi::Function::New(env, shutdown));
    exports.Set(Napi::String::New(env, "addScene"), Napi::Function::New(env, addScene));
    exports.Set(Napi::String::New(env, "addSource"), Napi::Function::New(env, addSource));
    exports.Set(Napi::String::New(env, "updateSource"), Napi::Function::New(env, updateSource));
    exports.Set(Napi::String::New(env, "muteSource"), Napi::Function::New(env, muteSource));
    exports.Set(Napi::String::New(env, "restartSource"), Napi::Function::New(env, restartSource));
    exports.Set(Napi::String::New(env, "switchToScene"), Napi::Function::New(env, switchToScene));
    exports.Set(Napi::String::New(env, "getScenes"), Napi::Function::New(env, getScenes));
    exports.Set(Napi::String::New(env, "createDisplay"), Napi::Function::New(env, createDisplay));
    exports.Set(Napi::String::New(env, "destroyDisplay"), Napi::Function::New(env, destroyDisplay));
    exports.Set(Napi::String::New(env, "moveDisplay"), Napi::Function::New(env, moveDisplay));
    return exports;
}

NODE_API_MODULE(obs_node, Init)