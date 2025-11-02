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
    QString orientation;
    QString deviceOff;
    QString displayOff;
};
