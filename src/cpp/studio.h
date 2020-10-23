#pragma once

#include "settings.h"
#include "scene.h"
#include <map>
#include <obs.h>

class Studio {

public:
    Studio(std::string &obsPath, Settings *settings);
    void startup();
    void shutdown();
    void addScene(std::string &sceneId);
    void addSource(std::string &sceneId, std::string &sourceId, SourceType sourceType, std::string &sourceUrl);
    void restartSource(std::string &sceneId, std::string &sourceId);
    void switchToScene(std::string &sceneId, std::string &transitionType, int transitionMs);

private:
    static void loadModule(const std::string &binPath, const std::string &dataPath);
    Scene *findScene(std::string &sceneId);
    std::string getObsBinPath();
    std::string getObsPluginPath();
    std::string getObsPluginDataPath();

    std::string obsPath;
    Settings *settings;
    std::map<std::string, Scene*> scenes;
    std::map<std::string, obs_source_t*> transitions;
    bool started;
    Scene *currentScene;
    obs_encoder_t *video_encoder;
    obs_encoder_t *audio_encoder;
    obs_service_t *output_service;
    obs_output_t *output;
};