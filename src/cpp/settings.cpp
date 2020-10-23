#include "settings.h"
#include "trace.h"

Settings::Settings(const Napi::Object& settings) {
    show_name = settings.Get("showName").As<Napi::String>();
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

    trace_debug("", field_s(show_name))
    trace_debug("", field_s(server))
    trace_debug("", field_s(key))
    trace_debug("", field(video_hw_decode))
    trace_debug("", field(video_hw_encode))
    trace_debug("", field(video_gpu_conversion))
    trace_debug("", field(video_bitrate_kbps))
    trace_debug("", field(video_keyint_sec))
    trace_debug("", field_s(video_rate_control))
    trace_debug("", field(video_width))
    trace_debug("", field(video_height))
    trace_debug("", field(video_fps_num))
    trace_debug("", field(video_fps_den))
    trace_debug("", field(audio_sample_rate))
    trace_debug("", field(audio_bitrate_kbps))
    trace_debug("", field_s(preset))
    trace_debug("", field_s(profile))
    trace_debug("", field_s(tune))
    trace_debug("", field_s(x264opts))
}