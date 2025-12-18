#pragma once

#include <QString>
#include <QByteArray>

class QpaPlatform
{
public:
    enum Platform {
        Eglfs,
        Wayland,
        WaylandEgl,
        Xcb,
        LinuxFb,
        Minimal,
        Offscreen
    };

    static QByteArray toEnvValue(Platform platform);
};
