#pragma once

#include <string>
#include <napi.h>

class Settings {

public:
    explicit Settings(const Napi::Object& settings);

    std::string show_name;
    std::string server;
    std::string key;
    bool video_hw_decode;
    bool video_hw_encode;
    bool video_gpu_conversion;
    int video_bitrate_kbps;
    int video_keyint_sec;
    std::string video_rate_control;
    uint32_t video_width;
    uint32_t video_height;
    int video_fps_num;
    int video_fps_den;
    int audio_sample_rate;
    int audio_bitrate_kbps;
    std::string preset;
    std::string profile;
    std::string tune;
    std::string x264opts;
};
