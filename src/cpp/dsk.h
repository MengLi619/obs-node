#pragma once
#include <string>
#include <obs.h>

class Dsk {

public:
    Dsk(std::string &id, std::string &position, std::string &url, int left, int top, int width, int height);
    ~Dsk();

    std::string &getPosition() { return position; }

    int getLeft() const { return left; }

    int getTop() const { return top; }

    int getWidth() const { return width; }

    int getHeight() const { return height; }

    obs_source_t *getObsSource() { return obs_source; }

private:
    obs_source_t *obs_source;
    std::string position;
    std::string url;
    int left;
    int top;
    int width;
    int height;
};