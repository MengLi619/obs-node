#pragma once

#include "settings.h"
#include <string>
#include <obs.h>

enum SourceType {
    Image = 0,
    MediaSource = 1,
};

class Source {

public:
    Source(std::string &id, SourceType type, std::string &url, obs_scene_t *obs_scene, Settings *settings);

    void start();

    void stop();

    static SourceType getSourceType(const std::string &sourceType);

    Napi::Object getNapiSource(const Napi::Env &env);

private:
    static std::string getSourceTypeString(SourceType sourceType);

    std::string id;
    SourceType type;
    std::string url;
    obs_scene_t *obs_scene;
    Settings *settings;
    obs_source_t *obs_source;
    bool started;
};
