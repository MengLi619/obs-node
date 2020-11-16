#include "source.h"

SourceType Source::getSourceType(const std::string &sourceType) {
    if (sourceType == "Image") {
        return Image;
    } else if (sourceType == "MediaSource") {
        return MediaSource;
    } else {
        throw std::invalid_argument("Invalid sourceType: " + sourceType);
    }
}

std::string Source::getSourceTypeString(SourceType sourceType) {
    switch (sourceType) {
        case Image:
            return "Image";
        case MediaSource:
            return "MediaSource";
        default:
            throw std::invalid_argument("Invalid sourceType: " + std::to_string(sourceType));
    }
}

Source::Source(std::string &id, SourceType type, std::string &url, obs_scene_t *obs_scene, Settings *settings)
        : id(id),
          type(type),
          url(url),
          obs_scene(obs_scene),
          settings(settings),
          obs_source(nullptr),
          obs_scene_item(nullptr),
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
        if (settings->videoDecoder) {
            obs_data_set_bool(obs_data, "hw_decode", settings->videoDecoder->hardwareEnable);
        }
        obs_data_set_bool(obs_data, "close_when_inactive", false);  // make source always read
        obs_data_set_bool(obs_data, "restart_on_activate", false);  // make source always read
        obs_source = obs_source_create("ffmpeg_source", this->id.c_str(), obs_data, nullptr);
    }

    obs_data_release(obs_data);

    if (!obs_source) {
        throw std::runtime_error("Failed to create obs_source");
    }

    // Add the source to the scene
    obs_scene_item = obs_scene_add(obs_scene, obs_source);
    if (!obs_scene_item) {
        throw std::runtime_error("Failed to add scene item.");
    }

    // Scale source to output size by setting bounds
    if (settings->video && settings->video->baseWidth > 0 && settings->video->baseHeight > 0) {
        struct vec2 bounds = {};
        bounds.x = (float)settings->video->baseWidth;
        bounds.y = (float)settings->video->baseHeight;
        uint32_t align = OBS_ALIGN_TOP + OBS_ALIGN_LEFT;
        obs_sceneitem_set_bounds_type(obs_scene_item, OBS_BOUNDS_SCALE_INNER);
        obs_sceneitem_set_bounds(obs_scene_item, &bounds);
        obs_sceneitem_set_bounds_alignment(obs_scene_item, align);
        started = true;
    }
}

void Source::stop() {
    // obs_sceneitem_remove will call obs_sceneitem_release internally,
    // so it's no need to call obs_sceneitem_release.
    obs_sceneitem_remove(obs_scene_item);
    obs_source_remove(obs_source);
    obs_source_release(obs_source);
    obs_source = nullptr;
    obs_scene_item = nullptr;
    started = false;
}

void Source::updateUrl(std::string &sourceUrl) {
    stop();
    url = sourceUrl;
    start();
}

void Source::mute(bool mute) {
    if (obs_source) {
        obs_source_set_muted(obs_source, mute);
        if (!mute && obs_source_get_monitoring_type(obs_source) == OBS_MONITORING_TYPE_NONE) {
            obs_source_set_monitoring_type(obs_source, OBS_MONITORING_TYPE_MONITOR_ONLY);
        }
    }
}

Napi::Object Source::getNapiSource(const Napi::Env &env) {
    auto source = Napi::Object::New(env);
    source.Set("id", id);
    source.Set("type", getSourceTypeString(type));
    source.Set("url", url);
    return source;
}