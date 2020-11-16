#pragma once

#include "settings.h"
#include "source.h"
#include "dsk.h"
#include <string>
#include <map>
#include <obs.h>

class Scene {
public:
    Scene(std::string &id, Settings *settings);
    ~Scene();

    std::string getId() { return id; }

    obs_scene_t *getObsScene() { return obs_scene; }

    void addSource(std::string &sourceId, SourceType sourceType, std::string &sourceUrl);

    void updateSource(std::string &sourceId, std::string &sourceUrl);

    void muteSource(std::string &sourceId, bool mute);

    void restartSource(std::string &sourceId);

    obs_scene_t *getObsOutputScene(std::map<std::string, Dsk*> &dsks);

    Napi::Object getNapiScene(const Napi::Env &env);

private:
    static obs_scene_t *createObsScene(std::string &sceneId);

    std::string id;
    Settings *settings;
    obs_scene_t *obs_scene;
    obs_scene_t *obs_output_scene;
    std::map<std::string, Source *> sources;
};
