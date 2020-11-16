#include "dsk.h"

Dsk::Dsk(std::string &id, std::string &position, std::string &url, int left, int top, int width, int height) :
    position(position),
    url(url),
    left(left),
    top(top),
    width(width),
    height(height) {
    obs_data_t *obs_data = obs_data_create();
    obs_data_set_string(obs_data, "file", url.c_str());
    obs_data_set_bool(obs_data, "unload", false);
    obs_source = obs_source_create("image_source", id.c_str(), obs_data, nullptr);
    obs_data_release(obs_data);
}

Dsk::~Dsk() {
    if (obs_source) {
        obs_source_release(obs_source);
    }
}