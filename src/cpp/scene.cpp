#include "scene.h"
#include "dsk.h"
#include <map>

Scene::Scene(std::string &id, Settings *settings) :
        id(id),
        settings(settings),
        obs_scene(createObsScene(id)),
        obs_output_scene(nullptr) {
}

Scene::~Scene() {
    if (obs_scene) {
        obs_scene_release(obs_scene);
    }
    if (obs_output_scene) {
        obs_scene_release(obs_scene);
    }
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

obs_scene_t *Scene::getObsOutputScene(std::map<std::string, Dsk*> &dsks) {
    if (obs_output_scene) {
        obs_scene_release(obs_output_scene);
        obs_output_scene = nullptr;
    }

    // create duplicate scene
    obs_output_scene = obs_scene_duplicate(obs_scene, (id + "_output").c_str(), OBS_SCENE_DUP_REFS);

    // add dsks
    for (auto &dsk : dsks) {
        // Add the source to the scene
        obs_scene_item *obs_scene_item = obs_scene_add(obs_output_scene, dsk.second->getObsSource());
        if (!obs_scene_item) {
            throw std::runtime_error("Failed to add scene item.");
        }

        // set position
        struct vec2 pos = {};
        pos.x = (float)dsk.second->getLeft();
        pos.y = (float)dsk.second->getTop();
        obs_sceneitem_set_pos(obs_scene_item, &pos);

        // set align
        std::string position = dsk.second->getPosition();
        uint32_t align = 0;
        if (position == "top") {
            align = OBS_ALIGN_TOP;
        } else if (position == "top-right") {
            align = OBS_ALIGN_TOP + OBS_ALIGN_RIGHT;
        } else if (position == "right") {
            align = OBS_ALIGN_RIGHT;
        } else if (position == "bottom-right") {
            align = OBS_ALIGN_BOTTOM + OBS_ALIGN_RIGHT;
        } else if (position == "bottom") {
            align = OBS_ALIGN_BOTTOM;
        } else if (position == "bottom-left") {
            align = OBS_ALIGN_BOTTOM + OBS_ALIGN_LEFT;
        } else if (position == "left") {
            align = OBS_ALIGN_LEFT;
        } else if (position == "top-left") {
            align = OBS_ALIGN_TOP + OBS_ALIGN_LEFT;
        }
        obs_sceneitem_set_bounds_alignment(obs_scene_item, align);

        // set size
        struct vec2 bounds = {};
        bounds.x = (float)dsk.second->getWidth();
        bounds.y = (float)dsk.second->getHeight();
        obs_sceneitem_set_bounds_type(obs_scene_item, OBS_BOUNDS_SCALE_INNER);
        obs_sceneitem_set_bounds(obs_scene_item, &bounds);

        // set top most
        obs_sceneitem_set_order(obs_scene_item, OBS_ORDER_MOVE_TOP);
    }

    return obs_output_scene;
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
