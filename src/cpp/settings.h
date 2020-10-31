#pragma once

#include <string>
#include <napi.h>

struct VideoSettings {
    int baseWidth;
    int baseHeight;
    int outputWidth;
    int outputHeight;
    int fpsNum;
    int fpsDen;
};

struct AudioSettings {
    int sampleRate;
};

struct VideoDecoderSettings {
    bool hardwareEnable;
};

struct VideoEncoderSettings {
    bool hardwareEnable;
    int width;
    int height;
    int bitrateKbps;
    int keyintSec;
    std:: string rateControl;
    std:: string preset;
    std:: string profile;
    std:: string tune;
    std:: string x264opts;
};

struct AudioEncoderSettings {
    int bitrateKbps;
};

struct OutputSettings {
    std::string server;
    std::string key;
};

class Settings {

public:
    explicit Settings(const Napi::Object& settings);
    ~Settings();
    VideoSettings *video;
    AudioSettings *audio;
    OutputSettings *output;
    VideoDecoderSettings *videoDecoder;
    VideoEncoderSettings *videoEncoder;
    AudioEncoderSettings *audioEncoder;
};
