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
    bool isStarted() const { return started; }
    static SourceType getSourceType(const std::string &sourceType);

private:
    std::string id;
    SourceType type;
    std::string url;
    obs_scene_t *obs_scene;
    Settings *settings;
    obs_source_t *obs_source;
    bool started;
};
