#include "settings.h"
#include "utils.h"
#include <obs.h>

Settings::Settings(const Napi::Object& settings) :
        video(nullptr),
        audio(nullptr),
        output(nullptr),
        videoDecoder(nullptr),
        videoEncoder(nullptr),
        audioEncoder(nullptr) {

    if (!settings.Get("video").IsUndefined()) {
        auto videoObject = settings.Get("video").As<Napi::Object>();
        video = new VideoSettings();
        video->baseWidth = getNapiInt(videoObject, "baseWidth");
        video->baseHeight = getNapiInt(videoObject, "baseHeight");
        video->outputWidth = getNapiInt(videoObject, "outputWidth");
        video->outputHeight = getNapiInt(videoObject, "outputHeight");
        video->fpsNum = getNapiInt(videoObject, "fpsNum");
        video->fpsDen = getNapiInt(videoObject, "fpsDen");

        blog(LOG_INFO, "Video Settings");
        blog(LOG_INFO, "=====================");
        blog(LOG_INFO, "baseWidth = %d", video->baseWidth);
        blog(LOG_INFO, "baseHeight = %d", video->baseHeight);
        blog(LOG_INFO, "outputWidth = %d", video->outputWidth);
        blog(LOG_INFO, "outputHeight = %d", video->outputHeight);
        blog(LOG_INFO, "fpsNum = %d", video->fpsNum);
        blog(LOG_INFO, "fpsDen = %d", video->fpsDen);
    }

    if (!settings.Get("audio").IsUndefined()) {
        auto audioObject = settings.Get("audio").As<Napi::Object>();
        audio = new AudioSettings();
        audio->sampleRate = getNapiInt(audioObject, "sampleRate");

        blog(LOG_INFO, "Audio Settings");
        blog(LOG_INFO, "=====================");
        blog(LOG_INFO, "sampleRate = %d", audio->sampleRate);
    }

    if (!settings.Get("videoDecoder").IsUndefined()) {
        auto videoDecoderObject = settings.Get("videoDecoder").As<Napi::Object>();
        videoDecoder = new VideoDecoderSettings();
        videoDecoder->hardwareEnable = getNapiBoolean(videoDecoderObject, "hardwareEnable");

        blog(LOG_INFO, "Video Decoder Settings");
        blog(LOG_INFO, "=====================");
        blog(LOG_INFO, "hardwareEnable = %s", videoDecoder->hardwareEnable ? "true" : "false");
    }

    if (!settings.Get("videoEncoder").IsUndefined()) {
        auto videoEncoderObject = settings.Get("videoEncoder").As<Napi::Object>();
        videoEncoder = new VideoEncoderSettings();
        videoEncoder->hardwareEnable = getNapiBoolean(videoEncoderObject, "hardwareEnable");
        videoEncoder->width = getNapiInt(videoEncoderObject, "width");
        videoEncoder->height = getNapiInt(videoEncoderObject, "height");
        videoEncoder->bitrateKbps = getNapiInt(videoEncoderObject, "bitrateKbps");
        videoEncoder->keyintSec = getNapiInt(videoEncoderObject, "keyintSec");
        videoEncoder->rateControl = getNapiString(videoEncoderObject, "rateControl");
        videoEncoder->preset = getNapiString(videoEncoderObject, "preset");
        videoEncoder->profile = getNapiString(videoEncoderObject, "profile");
        videoEncoder->tune = getNapiString(videoEncoderObject, "tune");
        videoEncoder->x264opts = getNapiStringOrDefault(videoEncoderObject, "x264opts", "");

        blog(LOG_INFO, "Video Encoder Settings");
        blog(LOG_INFO, "=====================");
        blog(LOG_INFO, "hardwareEnable = %s", videoEncoder->hardwareEnable ? "true" : "false");
        blog(LOG_INFO, "bitrateKbps = %d", videoEncoder->bitrateKbps);
        blog(LOG_INFO, "keyintSec = %d", videoEncoder->keyintSec);
        blog(LOG_INFO, "rateControl = %s", videoEncoder->rateControl.c_str());
        blog(LOG_INFO, "preset = %s", videoEncoder->preset.c_str());
        blog(LOG_INFO, "profile = %s", videoEncoder->profile.c_str());
        blog(LOG_INFO, "tune = %s", videoEncoder->tune.c_str());
        blog(LOG_INFO, "x264opts = %s", videoEncoder->x264opts.c_str());
    }

    if (!settings.Get("audioEncoder").IsUndefined()) {
        auto autoEncoderObject = settings.Get("audioEncoder").As<Napi::Object>();
        audioEncoder = new AudioEncoderSettings();
        audioEncoder->bitrateKbps = getNapiInt(autoEncoderObject, "bitrateKbps");

        blog(LOG_INFO, "Audio Encoder Settings");
        blog(LOG_INFO, "=====================");
        blog(LOG_INFO, "bitrateKbps = %d", audioEncoder->bitrateKbps);
    }

    if (!settings.Get("output").IsUndefined()) {
        auto outputObject = settings.Get("output").As<Napi::Object>();
        output = new OutputSettings();
        output->server = getNapiString(outputObject, "server");
        output->key = getNapiString(outputObject, "key");

        blog(LOG_INFO, "Output Settings");
        blog(LOG_INFO, "=====================");
        blog(LOG_INFO, "server = %s", output->server.c_str());
        blog(LOG_INFO, "key = %s", output->key.c_str());
    }
}

Settings::~Settings() {
    delete video;
    delete audio;
    delete output;
    delete videoDecoder;
    delete videoEncoder;
    delete audioEncoder;
}
