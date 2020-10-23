#pragma once

#include "settings.h"
#include "source.h"
#include <string>
#include <map>
#include <obs.h>

class Scene {
public:
    Scene(std::string &id, Settings *settings);
    std::string getId() { return id; }
    obs_scene_t *getObsScene() { return obs_scene; }

    void addSource(std::string &sourceId, SourceType sourceType, std::string &sourceUrl);
    void restartSource(std::string &sourceId);

private:
    static obs_scene_t *createObsScene(std::string &sceneId);
    std::string id;
    Settings *settings;
    obs_scene_t *obs_scene;
    std::map<std::string, Source*> sources;
};
