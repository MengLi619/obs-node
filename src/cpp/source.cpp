#include "source.h"
#include "trace.h"

SourceType Source::getSourceType(const std::string &sourceType) {
    if (sourceType == "Image") {
        return Image;
    } else if (sourceType == "MediaSource") {
        return MediaSource;
    } else {
        throw std::invalid_argument("Invalid sourceType: " + sourceType);
    }
}

Source::Source(std::string &id, SourceType type, std::string &url, obs_scene_t *obs_scene, Settings *settings)
        : id(id),
          type(type),
          url(url),
          obs_scene(obs_scene),
          settings(settings),
          obs_source(nullptr),
          started(false) {
}

void Source::start() {
    obs_data_t *obs_data = obs_data_create();
    if (type == Image) {
        obs_data_set_string(obs_data, "file", url.c_str());
        obs_data_set_bool(obs_data, "unload", false);
        obs_source = obs_source_create("image_source", "obs_image_source", obs_data, nullptr);
    } else if (type == MediaSource) {
        obs_data_set_string(obs_data, "input", url.c_str());
        obs_data_set_bool(obs_data, "is_local_file", false);
        obs_data_set_bool(obs_data, "looping", false);
        obs_data_set_bool(obs_data, "hw_decode", settings->video_hw_decode);
        obs_data_set_bool(obs_data, "close_when_inactive", false);
        obs_data_set_bool(obs_data, "restart_on_activate", false);
        obs_source = obs_source_create("ffmpeg_source", this->id.c_str(), obs_data, nullptr);
    }

    obs_data_release(obs_data);

    if (!obs_source) {
        throw std::runtime_error("Failed to create obs_source");
    }

    // Add the source to the scene
    obs_sceneitem_t *obs_scene_item = obs_scene_add(obs_scene, obs_source);
    if (!obs_scene_item) {
        throw std::runtime_error("Failed to add scene item.");
    }

    // Scale source to output size by setting bounds
    struct vec2 bounds = {};
    bounds.x = settings->video_width;
    bounds.y = settings->video_height;
    uint32_t align = OBS_ALIGN_TOP + OBS_ALIGN_LEFT;
    obs_sceneitem_set_bounds_type(obs_scene_item, OBS_BOUNDS_SCALE_INNER);
    obs_sceneitem_set_bounds(obs_scene_item, &bounds);
    obs_sceneitem_set_bounds_alignment(obs_scene_item, align);
    this->started = true;
}

void Source::stop() {
    obs_source_remove(obs_source);
    obs_source_release(obs_source);
    this->started = false;
}
