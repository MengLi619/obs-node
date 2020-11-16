#pragma once

#include "settings.h"
#include "scene.h"
#include "display.h"
#include <map>
#include <obs.h>

class Studio {

public:
    Studio(std::string &obsPath, Settings *settings);
    void startup();
    void shutdown();
    void addScene(std::string &sceneId);
    void addSource(std::string &sceneId, std::string &sourceId, SourceType sourceType, std::string &sourceUrl);
    void updateSource(std::string &sceneId, std::string &sourceId, std::string &sourceUrl);
    void muteSource(std::string &sceneId, std::string &sourceId, bool mute);
    void restartSource(std::string &sceneId, std::string &sourceId);
    void addDSK(std::string &id, std::string &position, std::string &url, int left, int top, int width, int height);
    void switchToScene(std::string &sceneId, std::string &transitionType, int transitionMs);
    const std::map<std::string, Scene*>& getScenes();
    void createDisplay(std::string &displayName, void *parentHandle, int scaleFactor, std::string &sourceId);
    void destroyDisplay(std::string &displayName);
    void moveDisplay(std::string &displayName, int x, int y, int width, int height);

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
    std::map<std::string, Display*> displays;
    std::map<std::string, Dsk*> dsks;
    Scene *currentScene;
    obs_encoder_t *video_encoder;
    obs_encoder_t *audio_encoder;
    obs_service_t *output_service;
    obs_output_t *output;
};