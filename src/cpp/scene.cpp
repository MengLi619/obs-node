#include "scene.h"

Scene::Scene(std::string &id, Settings *settings) :
        id(id),
        settings(settings),
        obs_scene(createObsScene(id)) {
}

void Scene::addSource(std::string &sourceId, SourceType sourceType, std::string &sourceUrl) {
    auto source = new Source(sourceId, sourceType, sourceUrl, obs_scene, settings);
    sources[sourceId] = source;

    // Start the source as soon as it's added.
    source->start();
}

void Scene::updateSource(std::string &sourceId, std::string &sourceUrl) {
    auto it = sources.find(sourceId);
    if (it == sources.end()) {
        throw std::invalid_argument("Can't find source " + sourceId);
    }
    it->second->updateUrl(sourceUrl);
}

void Scene::muteSource(std::string &sourceId, bool mute) {
    auto it = sources.find(sourceId);
    if (it == sources.end()) {
        throw std::invalid_argument("Can't find source " + sourceId);
    }
    it->second->mute(mute);
}

void Scene::restartSource(std::string &sourceId) {
    auto it = sources.find(sourceId);
    if (it == sources.end()) {
        throw std::invalid_argument("Can't find source " + sourceId);
    }
    it->second->stop();
    it->second->start();
}

obs_scene_t *Scene::createObsScene(std::string &sceneId) {
    obs_scene_t *scene = obs_scene_create(sceneId.c_str());
    if (scene == nullptr) {
        throw std::runtime_error("Failed to create obs scene " + sceneId);
    }
    return scene;
}

Napi::Object Scene::getNapiScene(const Napi::Env &env) {
    auto napiScene = Napi::Object::New(env);
    auto napiSources = Napi::Array::New(env, sources.size());
    int i = 0;
    for (auto &source : sources) {
        napiSources[i++] = source.second->getNapiSource(env);
    }
    napiScene.Set("id", id);
    napiScene.Set("sources", napiSources);
    return napiScene;
}
