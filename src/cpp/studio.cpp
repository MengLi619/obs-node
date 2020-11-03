#include "studio.h"
#include <obs.h>
#include <unistd.h>

Studio::Studio(std::string &obsPath, Settings *settings)
        : obsPath(obsPath),
          settings(settings),
          currentScene(nullptr),
          video_encoder(nullptr),
          audio_encoder(nullptr),
          output_service(nullptr),
          output(nullptr) {
}

void Studio::startup() {
    std::string currentWorkDir = getcwd(nullptr, 0);
    obs_data_t *audio_encoder_settings = nullptr;
    obs_data_t *video_encoder_settings = nullptr;
    obs_data_t *output_service_settings = nullptr;

    // Change work directory to obs bin path to setup obs properly.
    std::string obsBinPath = getObsBinPath();
    blog(LOG_INFO, "Set working directory to %s for loading obs data", obsBinPath.c_str());
    chdir(obsBinPath.c_str());

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
        chdir(currentWorkDir.c_str());
    };

    try {
        obs_startup("en-US", nullptr, nullptr);
        if (!obs_initialized()) {
            throw std::runtime_error("Failed to startup obs studio.");
        }

        // reset video
        if (settings->video) {
            obs_video_info ovi = {};
            memset(&ovi, 0, sizeof(ovi));
            ovi.adapter = 0;
#ifdef _WIN32
            ovi.graphics_module = "libobs-opengl.dll";
#else
            ovi.graphics_module = "libobs-opengl.so";
#endif
            ovi.output_format = VIDEO_FORMAT_NV12;
            ovi.fps_num = settings->video->fpsNum;
            ovi.fps_den = settings->video->fpsDen;
            ovi.base_width = settings->video->baseWidth;
            ovi.base_height = settings->video->baseHeight;
            ovi.output_width = settings->video->outputWidth;
            ovi.output_height = settings->video->outputHeight;
            ovi.gpu_conversion = true; // always be true for the OBS issue

            int result = obs_reset_video(&ovi);
            if (result != OBS_VIDEO_SUCCESS) {
                throw std::runtime_error("Failed to reset video");
            }
        }

        // reset audio
        if (settings->audio) {
            obs_audio_info oai = {};
            memset(&oai, 0, sizeof(oai));
            oai.samples_per_sec = settings->audio->sampleRate;
            oai.speakers = SPEAKERS_STEREO;
            if (!obs_reset_audio(&oai)) {
                throw std::runtime_error("Failed to reset audio");
            }
        }

        // load modules
#ifdef _WIN32
        loadModule(getObsPluginPath() + "\\image-source.dll", getObsPluginDataPath() + "\\image-source");
        loadModule(getObsPluginPath() + "\\obs-ffmpeg.dll", getObsPluginDataPath() + "\\obs-ffmpeg");
        loadModule(getObsPluginPath() + "\\obs-transitions.dll", getObsPluginDataPath() + "\\obs-transitions");
        loadModule(getObsPluginPath() + "\\rtmp-services.dll", getObsPluginDataPath() + "\\rtmp-services");
        loadModule(getObsPluginPath() + "\\obs-x264.dll", getObsPluginDataPath() + "\\obs-x264");
        loadModule(getObsPluginPath() + "\\obs-outputs.dll", getObsPluginDataPath() + "\\obs-outputs");
#else
        loadModule(getObsPluginPath() + "/image-source.so", getObsPluginDataPath() + "/image-source");
        loadModule(getObsPluginPath() + "/obs-ffmpeg.so", getObsPluginDataPath() + "/obs-ffmpeg");
        loadModule(getObsPluginPath() + "/obs-transitions.so", getObsPluginDataPath() + "/obs-transitions");
        loadModule(getObsPluginPath() + "/rtmp-services.so", getObsPluginDataPath() + "/rtmp-services");
        loadModule(getObsPluginPath() + "/obs-x264.so", getObsPluginDataPath() + "/obs-x264");
        loadModule(getObsPluginPath() + "/obs-outputs.so", getObsPluginDataPath() + "/obs-outputs");
#endif

        obs_post_load_modules();

        // audio encoder
        if (settings->audioEncoder) {
            audio_encoder = obs_audio_encoder_create("ffmpeg_aac", "aac enc", nullptr, 0, nullptr);
            if (!audio_encoder) {
                throw std::runtime_error("Failed to create audio encoder.");
            }

            audio_encoder_settings = obs_encoder_get_settings(audio_encoder);
            if (!audio_encoder_settings) {
                throw std::runtime_error("Failed to get audio encoder settings.");
            }

            obs_data_set_int(audio_encoder_settings, "bitrate", settings->audioEncoder->bitrateKbps);
            obs_encoder_update(audio_encoder, audio_encoder_settings);
            obs_encoder_set_audio(audio_encoder, obs_get_audio());
        }

        // video encoder
        if (settings->videoEncoder) {
            std::string encoder = settings->videoEncoder->hardwareEnable ? "ffmpeg_nvenc" : "obs_x264";
            video_encoder = obs_video_encoder_create(encoder.c_str(), "h264 enc", nullptr, nullptr);
            if (!video_encoder) {
                throw std::runtime_error("Failed to create video encoder.");
            }

            video_encoder_settings = obs_encoder_get_settings(video_encoder);
            if (!video_encoder_settings) {
                throw std::runtime_error("Failed to get video encoder settings.");
            }

            obs_data_set_int(video_encoder_settings, "bitrate", settings->videoEncoder->bitrateKbps);
            obs_data_set_int(video_encoder_settings, "keyint_sec", settings->videoEncoder->keyintSec);
            obs_data_set_string(video_encoder_settings, "rate_control", settings->videoEncoder->rateControl.c_str());
            obs_data_set_int(video_encoder_settings, "width", settings->videoEncoder->width);
            obs_data_set_int(video_encoder_settings, "height", settings->videoEncoder->height);
            obs_data_set_string(video_encoder_settings, "preset", settings->videoEncoder->preset.c_str());
            obs_data_set_string(video_encoder_settings, "profile", settings->videoEncoder->profile.c_str());
            obs_data_set_string(video_encoder_settings, "tune", settings->videoEncoder->tune.c_str());
            obs_data_set_string(video_encoder_settings, "x264opts", settings->videoEncoder->x264opts.c_str());
        }

        // video decoder
        if (settings->videoDecoder) {
            obs_encoder_update(video_encoder, video_encoder_settings);
            obs_encoder_set_video(video_encoder, obs_get_video());
        }

        // output
        if (settings->output) {
            bool is_rtmp = settings->output->server.rfind("rtmp", 0) == 0;
            if (is_rtmp) {
                output_service = obs_service_create("rtmp_common", "rtmp service", nullptr, nullptr);
            } else {
                output_service = obs_service_create("rtmp_custom", "custom service", nullptr, nullptr);
            }

            if (!output_service) {
                throw std::runtime_error("Failed to create output service.");
            }

            output_service_settings = obs_data_create();
            if (!output_service_settings) {
                throw std::runtime_error("Failed to create output settings.");
            }

            obs_data_set_string(output_service_settings, "server", settings->output->server.c_str());
            obs_data_set_string(output_service_settings, "key", settings->output->key.c_str());
            obs_service_update(output_service, output_service_settings);
            obs_service_apply_encoder_settings(output_service, video_encoder_settings, audio_encoder_settings);

            if (is_rtmp) {
                output = obs_output_create("rtmp_output", "rtmp output", nullptr, nullptr);
            } else {
                output = obs_output_create("ffmpeg_output", "ffmpeg output", nullptr, nullptr);
            }

            if (!output) {
                throw std::runtime_error("Failed to create output.");
            }

            if (video_encoder) {
                obs_output_set_video_encoder(output, video_encoder);
            }

            if (audio_encoder) {
                obs_output_set_audio_encoder(output, audio_encoder, 0);
            }

            obs_output_set_service(output, output_service);

            if (!obs_output_start(output)) {
                throw std::runtime_error("Failed to start output.");
            }
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

void Studio::updateSource(std::string &sceneId, std::string &sourceId, std::string &sourceUrl) {
    auto it = scenes.find(sceneId);
    if (it == scenes.end()) {
        throw std::invalid_argument("Can't find scene " + sceneId);
    }
    it->second->updateSource(sourceId, sourceUrl);
}

void Studio::muteSource(std::string &sceneId, std::string &sourceId, bool mute) {
    auto it = scenes.find(sceneId);
    if (it == scenes.end()) {
        throw std::invalid_argument("Can't find scene " + sceneId);
    }
    it->second->muteSource(sourceId, mute);
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
    if (next == nullptr) {
        throw std::invalid_argument("Can't find scene " + sceneId);
    }

    if (next == currentScene) {
        blog(LOG_INFO, "Same with current scene, no need to switch, skip.");
        return;
    }

    blog(LOG_INFO, "Start transition: %s -> %s", (currentScene ? currentScene->getId().c_str() : ""), next->getId().c_str());

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
    obs_module_t *module = nullptr;
    int code = obs_open_module(&module, binPath.c_str(), dataPath.c_str());
    if (code != MODULE_SUCCESS) {
        throw std::runtime_error("Failed to load module '" + binPath + "'");
    }
    if (!obs_init_module(module)) {
        throw std::runtime_error("Failed to load module '" + binPath + "'");
    }
}

const std::map<std::string, Scene*>& Studio::getScenes() {
    return scenes;
}

void Studio::createDisplay(std::string &displayName, void *parentHandle, int scaleFactor, std::string &sourceId) {
    auto found = displays.find(displayName);
    if (found != displays.end()) {
        throw std::logic_error("Display " + displayName + " already existed");
    }
    auto *display = new Display(parentHandle, scaleFactor, sourceId);
    displays[displayName] = display;
}

void Studio::destroyDisplay(std::string &displayName) {
    auto found = displays.find(displayName);
    if (found == displays.end()) {
        throw std::logic_error("Can't find display: " + displayName);
    }
    Display *display = found->second;
    displays.erase(displayName);
    delete display;
}

void Studio::moveDisplay(std::string &displayName, int x, int y, int width, int height) {
    auto found = displays.find(displayName);
    if (found == displays.end()) {
        throw std::logic_error("Can't find display: " + displayName);
    }
    found->second->move(x, y, width, height);
}

Scene *Studio::findScene(std::string &sceneId) {
    auto it = scenes.find(sceneId);
    if (it == scenes.end()) {
        return nullptr;
    }
    return it->second;
}

std::string Studio::getObsBinPath() {
#ifdef _WIN32
    return obsPath + "\\bin\\64bit";
#elif __linux__
    return obsPath + "/bin/64bit";
#else
    return obsPath + "/bin";
#endif
}

std::string Studio::getObsPluginPath() {
#ifdef _WIN32
    // Obs plugin path is same with bin path, due to SetDllDirectoryW called in obs-studio/libobs/util/platform-windows.c.
    return obsPath + "\\bin\\64bit";
#elif __linux__
    return obsPath + "/obs-plugins/64bit";
#else
    return obsPath + "/obs-plugins";
#endif
}

std::string Studio::getObsPluginDataPath() {
#ifdef _WIN32
    return obsPath + "\\data\\obs-plugins";
#else
    return obsPath + "/data/obs-plugins";
#endif
}