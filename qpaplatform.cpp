#include "qpaplatform.h"

QByteArray QpaPlatform::toEnvValue(Platform platform)
{
    switch (platform) {
    case Eglfs:
        return "eglfs";
    case Wayland:
        return "wayland";
    case WaylandEgl:
        return "wayland-egl";
    case Xcb:
        return "xcb";
    case LinuxFb:
        return "linuxfb";
    case Minimal:
        return "minimal";
    case Offscreen:
        return "offscreen";
    default:
        return "eglfs"; // safe default for embedded
    }
}
