#pragma once

void *createDisplayWindow(void *parentHandle);
void destroyWindow(void *windowHandle);
void moveWindow(void *windowHandle, int x, int y, int width, int height);