#include "settings.h"
#include <obs.h>

Settings::Settings(const Napi::Object& settings) {
    server = settings.Get("server").As<Napi::String>();
    key = settings.Get("key").As<Napi::String>();
    video_hw_decode = settings.Get("videoHWDecode").As<Napi::Boolean>();
    video_hw_encode = settings.Get("videoHWEncode").As<Napi::Boolean>();
    video_gpu_conversion = settings.Get("videoGpuConversion").As<Napi::Boolean>();
    video_bitrate_kbps = settings.Get("videoBitrateKbps").As<Napi::Number>();
    video_keyint_sec = settings.Get("videoKeyintSec").As<Napi::Number>();
    video_rate_control = settings.Get("videoRateControl").As<Napi::String>();
    video_width = settings.Get("videoWidth").As<Napi::Number>();
    video_height = settings.Get("videoHeight").As<Napi::Number>();
    video_fps_num = settings.Get("videoFpsNum").As<Napi::Number>();
    video_fps_den = settings.Get("videoFpsDen").As<Napi::Number>();
    audio_sample_rate = settings.Get("audioSampleRate").As<Napi::Number>();
    audio_bitrate_kbps = settings.Get("audioBitrateKbps").As<Napi::Number>();
    preset = settings.Get("preset").As<Napi::String>();
    profile = settings.Get("profile").As<Napi::String>();
    tune = settings.Get("tune").As<Napi::String>();
    x264opts = settings.Get("x264opts").As<Napi::String>();

    blog(LOG_INFO, "server = %s", server.c_str());
    blog(LOG_INFO, "key = %s", key.c_str());
    blog(LOG_INFO, "video_hw_decode = %s", video_hw_decode ? "true" : "false");
    blog(LOG_INFO, "video_hw_encode = %s", video_hw_encode ? "true" : "false");
    blog(LOG_INFO, "video_gpu_conversion = %s", video_gpu_conversion ? "true" : "false");
    blog(LOG_INFO, "video_bitrate_kbps = %d", video_bitrate_kbps);
    blog(LOG_INFO, "video_keyint_sec = %d", video_keyint_sec);
    blog(LOG_INFO, "video_rate_control = %s", video_rate_control.c_str());
    blog(LOG_INFO, "video_width = %d", video_width);
    blog(LOG_INFO, "video_height = %d", video_height);
    blog(LOG_INFO, "video_fps_num = %d", video_fps_num);
    blog(LOG_INFO, "video_fps_den = %d", video_fps_den);
    blog(LOG_INFO, "audio_sample_rate = %d", audio_sample_rate);
    blog(LOG_INFO, "audio_bitrate_kbps = %d", audio_bitrate_kbps);
    blog(LOG_INFO, "preset = %s", preset.c_str());
    blog(LOG_INFO, "profile = %s", profile.c_str());
    blog(LOG_INFO, "tune = %s", tune.c_str());
    blog(LOG_INFO, "x264opts = %s", x264opts.c_str());
}