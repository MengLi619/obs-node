#include "studio.h"
#include "trace.h"
#include "scene.h"
#include <filesystem>
#include <obs.h>

Studio::Studio(std::string &obsPath, Settings *settings)
        : obsPath(obsPath),
          settings(settings),
          started(false),
          currentScene(nullptr),
          video_encoder(nullptr),
          audio_encoder(nullptr),
          output_service(nullptr),
          output(nullptr) {
}

void Studio::startup() {
    auto currentWorkDir = std::filesystem::current_path();
    obs_data_t *audio_encoder_settings = nullptr;
    obs_data_t *video_encoder_settings = nullptr;
    obs_data_t *output_service_settings = nullptr;

    // Change work directory to obs bin path to setup obs properly.
    std::filesystem::current_path(getObsBinPath());

    auto restore = [&] {
        if (audio_encoder_settings) {
            obs_data_release(audio_encoder_settings);
        }
        if (video_encoder_settings) {
            obs_data_release(video_encoder_settings);
        }
        if (output_service_settings) {
            obs_data_release(output_service_settings);
        }
        std::filesystem::current_path(currentWorkDir);
    };

    try {
        obs_startup("en-US", nullptr, nullptr);
        if (!obs_initialized()) {
            throw std::runtime_error("Failed to startup obs studio.");
        }

        // reset video
        obs_video_info ovi = {};
        memset(&ovi, 0, sizeof(ovi));
        ovi.adapter = 0;
        ovi.graphics_module = "libobs-opengl.so";
        ovi.output_format = VIDEO_FORMAT_NV12;
        ovi.fps_num = settings->video_fps_num;
        ovi.fps_den = settings->video_fps_den;
        ovi.base_width = settings->video_width;
        ovi.base_height = settings->video_height;
        ovi.output_width = settings->video_width;
        ovi.output_height = settings->video_height;
        ovi.gpu_conversion = settings->video_gpu_conversion;

        int result = obs_reset_video(&ovi);
        if (result != OBS_VIDEO_SUCCESS) {
            throw std::runtime_error("Failed to reset video");
        }

        // reset audio
        obs_audio_info oai = {};
        memset(&oai, 0, sizeof(oai));

        oai.samples_per_sec = settings->audio_sample_rate;
        oai.speakers = SPEAKERS_STEREO;

        if (!obs_reset_audio(&oai)) {
            throw std::runtime_error("Failed to reset audio");
        }

        // load modules
        loadModule(getObsPluginPath() + "/image-source.so", getObsPluginDataPath() + "/image-source");
        loadModule(getObsPluginPath() + "/obs-ffmpeg.so", getObsPluginDataPath() + "/obs-ffmpeg");
        loadModule(getObsPluginPath() + "/obs-transitions.so", getObsPluginDataPath() + "/obs-transitions");
        loadModule(getObsPluginPath() + "/rtmp-services.so", getObsPluginDataPath() + "/rtmp-services");
        loadModule(getObsPluginPath() + "/obs-x264.so", getObsPluginDataPath() + "/obs-x264");
        loadModule(getObsPluginPath() + "/obs-libfdk.so", getObsPluginDataPath() + "/obs-libfdk");
        loadModule(getObsPluginPath() + "/obs-outputs.so", getObsPluginDataPath() + "/obs-outputs");

        obs_post_load_modules();

        // output
        // audio encoder
        audio_encoder = obs_audio_encoder_create("libfdk_aac", "aac enc", nullptr, 0, nullptr);
        if (!audio_encoder) {
            throw std::runtime_error("Failed to create audio encoder.");
        }

        audio_encoder_settings = obs_encoder_get_settings(audio_encoder);
        if (!audio_encoder_settings) {
            throw std::runtime_error("Failed to get audio encoder settings.");
        }

        obs_data_set_int(audio_encoder_settings, "bitrate", settings->audio_bitrate_kbps);
        obs_data_set_bool(audio_encoder_settings, "afterburner", true);
        obs_encoder_update(audio_encoder, audio_encoder_settings);
        obs_encoder_set_audio(audio_encoder, obs_get_audio());

        // video encoder
        std::string encoder = settings->video_hw_encode ? "ffmpeg_nvenc" : "obs_x264";
        video_encoder = obs_video_encoder_create(encoder.c_str(), "h264 enc", nullptr, nullptr);
        if (!video_encoder) {
            throw std::runtime_error("Failed to create video encoder.");
        }

        video_encoder_settings = obs_encoder_get_settings(video_encoder);
        if (!video_encoder_settings) {
            throw std::runtime_error("Failed to get video encoder settings.");
        }

        obs_data_set_int(video_encoder_settings, "bitrate", settings->video_bitrate_kbps);
        obs_data_set_int(video_encoder_settings, "keyint_sec", settings->video_keyint_sec);
        obs_data_set_string(video_encoder_settings, "rate_control", settings->video_rate_control.c_str());
        if (settings->video_hw_encode) {
            obs_data_set_string(video_encoder_settings, "preset", "default");
            obs_data_set_string(video_encoder_settings, "profile", "main");
            obs_data_set_int(video_encoder_settings, "bf", 2);
            obs_data_set_bool(video_encoder_settings, "psycho_aq", false);
            obs_data_set_bool(video_encoder_settings, "lookahead", false);
        } else {
            obs_data_set_int(video_encoder_settings, "width", settings->video_width);
            obs_data_set_int(video_encoder_settings, "height", settings->video_height);
            obs_data_set_int(video_encoder_settings, "fps_num", settings->video_fps_num);
            obs_data_set_int(video_encoder_settings, "fps_den", settings->video_fps_den);
            obs_data_set_string(video_encoder_settings, "preset", settings->preset.c_str());
            obs_data_set_string(video_encoder_settings, "profile", settings->profile.c_str());
            obs_data_set_string(video_encoder_settings, "tune", settings->tune.c_str());
            obs_data_set_string(video_encoder_settings, "x264opts", settings->x264opts.c_str());
        }

        obs_encoder_update(video_encoder, video_encoder_settings);
        obs_encoder_set_video(video_encoder, obs_get_video());

        // output service
        output_service = obs_service_create("rtmp_common", "rtmp service", nullptr, nullptr);
        if (!output_service) {
            throw std::runtime_error("Failed to create output service.");
        }

        output_service_settings = obs_data_create();
        if (!output_service_settings) {
            throw std::runtime_error("Failed to create output settings.");
        }

        obs_data_set_string(output_service_settings, "server", settings->server.c_str());
        obs_data_set_string(output_service_settings, "key", settings->key.c_str());
        obs_service_update(output_service, output_service_settings);
        obs_service_apply_encoder_settings(output_service, video_encoder_settings, audio_encoder_settings);

        // output
        output = obs_output_create("rtmp_output", "RTMP output", nullptr, nullptr);
        if (!output) {
            throw std::runtime_error("Failed to create output.");
        }

        obs_output_set_video_encoder(output, video_encoder);
        obs_output_set_audio_encoder(output, audio_encoder, 0);
        obs_output_set_service(output, output_service);

        if (!obs_output_start(output)) {
            throw std::runtime_error("Failed to start output.");
        }

        restore();

    } catch (...) {
        restore();
        throw;
    }
}

void Studio::shutdown() {
    obs_encoder_release(video_encoder);
    obs_encoder_release(audio_encoder);
    obs_output_release(output);
    obs_service_release(output_service);
    obs_shutdown();
    if (obs_initialized()) {
        throw std::runtime_error("Failed to shutdown obs studio.");
    }
    this->started = false;
}

void Studio::addScene(std::string &sceneId) {
    auto scene = new Scene(sceneId, settings);
    scenes[sceneId] = scene;
}

void Studio::addSource(std::string &sceneId, std::string &sourceId, SourceType sourceType, std::string &sourceUrl) {
    auto it = scenes.find(sceneId);
    if (it == scenes.end()) {
        throw std::invalid_argument("Can't find scene " + sceneId);
    }
    it->second->addSource(sourceId, sourceType, sourceUrl);
}

void Studio::restartSource(std::string &sceneId, std::string &sourceId) {
    auto scene = findScene(sceneId);
    if (!scene) {
        throw std::invalid_argument("Can't find scene " + sceneId);
    }
    scene->restartSource(sourceId);
}

void Studio::switchToScene(std::string &sceneId, std::string &transitionType, int transitionMs) {
    Scene *next = findScene(sceneId);
    if (!next) {
        throw std::invalid_argument("Can't find scene " + sceneId);
    }

    if (next == currentScene) {
        trace_info("Same with current scene, no need to switch, skip.")
        return;
    }

    trace_info("Start transition: " + (currentScene ? currentScene->getId() : "") + " -> " + next->getId())

    // Find or create transition
    auto it = transitions.find(transitionType);
    if (it == transitions.end()) {
        transitions[transitionType] = obs_source_create(transitionType.c_str(), transitionType.c_str(), nullptr,
                                                        nullptr);
    }

    obs_source_t *transition = transitions[transitionType];
    if (currentScene) {
        obs_transition_set(transition, obs_scene_get_source(currentScene->getObsScene()));
    }

    obs_set_output_source(0, transition);

    bool ret = obs_transition_start(
            transition,
            OBS_TRANSITION_MODE_AUTO,
            transitionMs,
            obs_scene_get_source(next->getObsScene())
    );

    if (!ret) {
        throw std::runtime_error("Failed to start transition.");
    }

    currentScene = next;
}

void Studio::loadModule(const std::string &binPath, const std::string &dataPath) {
    obs_module_t *module;
    int code = obs_open_module(&module, binPath.c_str(), dataPath.c_str());
    if (code != MODULE_SUCCESS) {
        throw std::runtime_error("Failed to load module '" + binPath + "'");
    }
    if (!obs_init_module(module)) {
        throw std::runtime_error("Failed to load module '" + binPath + "'");
    }
}

Scene *Studio::findScene(std::string &sceneId) {
    auto it = scenes.find(sceneId);
    if (it == scenes.end()) {
        return nullptr;
    }
    return it->second;
}

std::string Studio::getObsBinPath() {
#ifdef __linux__
    return obsPath + "/bin/64bit";
#else
    return obsPath + "/bin";
#endif
}

std::string Studio::getObsPluginPath() {
#ifdef __linux__
    return obsPath + "/obs-plugins/64bit";
#else
    return obsPath + "/obs-plugins";
#endif
}

std::string Studio::getObsPluginDataPath() {
    return obsPath + "/data/obs-plugins";
}
