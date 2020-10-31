#pragma once

#include "settings.h"
#include <string>
#include <obs.h>
#include <graphics/graphics.h>

class Display {

public:
    Display(void *parentHandle, int scaleFactor, std::string &sourceName);
    ~Display();
    void move(int x, int y, int width, int height);

private:
    static void displayCallback(void* displayPtr, uint32_t cx, uint32_t cy);
    void *parentHandle; // For MacOS is NSView**, For Windows is HWND*
    int scaleFactor;
    void *windowHandle;
    obs_display_t* obs_display;
    obs_source_t* obs_source;
    int x = 0;
    int y = 0;
    int width = 0;
    int height = 0;
};