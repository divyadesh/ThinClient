#pragma once
#include <QString>

struct SystemSettings {
    QString audio;
    QString timeZone;
    bool enableOnScreenKeyboard = false;
    bool enableTouchScreen = false;
    QString resolution;
    QString ethernet;
    QString network;
    int orientation = 0;
    int deviceOff = 0;
    int displayOff = 0;
};
