#include "DisplaySettings.h"

#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QProcess>
#include <QRegularExpression>
#include <QMap>
#include <QString>

static const QString WESTON_CONFIG = "/etc/xdg/weston/weston.ini";

QString removeExtraBlankLines(const QString &input)
{
    QString output;
    bool lastWasEmpty = false;

    const QStringList lines = input.split('\n');

    for (const QString &line : lines) {
        bool isEmpty = line.trimmed().isEmpty();

        if (isEmpty) {
            // If previous line was also empty, skip this one
            if (lastWasEmpty)
                continue;

            // Keep a single blank line
            output += "\n";
            lastWasEmpty = true;
        } else {
            // Normal line: always keep
            output += line + "\n";
            lastWasEmpty = false;
        }
    }

    return output.trimmed() + "\n";  // ensure clean ending
}

// -----------------------------
// Helpers
// -----------------------------

// Try to detect active Weston output name from `weston-info`.
// Falls back to "HDMI-A-1" if detection fails.
static QString detectOutput()
{
    QProcess p;
    p.start("weston-info");
    p.waitForFinished(2000); // 2s timeout

    QString out = p.readAllStandardOutput();
    QRegularExpression rx("name:\\s+'([A-Za-z0-9\\-]+)'");
    auto m = rx.match(out);

    if (m.hasMatch())
        return m.captured(1);

    qWarning() << "Could not detect output name via weston-info, using HDMI-A-1";
    return "HDMI-A-1";
}

// Insert or replace whole section.
static void setBlock(QMap<QString, QStringList> &ini,
                     const QString &section,
                     const QStringList &lines)
{
    ini[section] = lines;
}

// Parse weston.ini to: sectionName -> list of lines (no [section] header).
static QMap<QString, QStringList> loadWestonIniRaw()
{
    QMap<QString, QStringList> map;
    QFile f(WESTON_CONFIG);

    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Failed to open" << WESTON_CONFIG << "for reading";
        return map;
    }

    QTextStream ts(&f);
    QString current;

    while (!ts.atEnd()) {
        const QString rawLine = ts.readLine();
        const QString lineTrim = rawLine.trimmed();

        if (lineTrim.startsWith('[') && lineTrim.endsWith(']')) {
            current = lineTrim.mid(1, lineTrim.length() - 2).trimmed();
            if (!map.contains(current))
                map.insert(current, QStringList());
        } else if (!current.isEmpty()) {
            map[current].append(rawLine); // keep original spacing
        }
    }
    return map;
}

// Write the map back to weston.ini.
static bool saveWestonIniRaw(const QMap<QString, QStringList> &ini)
{
    QFile f(WESTON_CONFIG);

    if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
        qWarning() << "Failed to open" << WESTON_CONFIG << "for writing";
        return false;
    }

    QString buffer;
    QTextStream ts(&f);

    for (auto it = ini.constBegin(); it != ini.constEnd(); ++it) {
        ts << "[" << it.key() << "]\n";
        for (const QString &l : it.value())
            ts << l << "\n";
        ts << "\n";
    }

    // 🧹 Clean excessive blank lines
    buffer = removeExtraBlankLines(buffer);

    // Write cleaned content to file
    QTextStream tsFile(&f);
    tsFile << buffer;

    return true;
}

// -----------------------------
// DisplaySettings implementation
// -----------------------------

DisplaySettings::DisplaySettings(QObject *parent)
    : QObject(parent)
{
    normalizeWestonIniSpacing();
    // Load current settings from weston.ini on startup
    if (!loadFromWestonIni()) {
        // Apply defaults if ini missing/broken
        m_resolution = detectCurrentResolution();
        m_orientation = "normal";
        m_idleTimeSeconds = 0;
        m_touchEnabled = false;
    }
}

// ----- Q_PROPERTY setters -----

void DisplaySettings::setResolution(const QString &res)
{
    if (m_resolution == res)
        return;
    m_resolution = res;
    emit resolutionChanged();
}

void DisplaySettings::setOrientation(const QString &ori)
{
    if (m_orientation == ori)
        return;
    m_orientation = ori;
    emit orientationChanged();
}

void DisplaySettings::setIdleTimeSeconds(int seconds)
{
    if (seconds < 0)
        seconds = 0;
    if (m_idleTimeSeconds == seconds)
        return;
    m_idleTimeSeconds = seconds;
    emit idleTimeSecondsChanged();
}

void DisplaySettings::setTouchEnabled(bool enabled)
{
    if (m_touchEnabled == enabled)
        return;
    m_touchEnabled = enabled;
    emit touchEnabledChanged();
}

// ----- Public helper methods -----

QStringList DisplaySettings::availableResolutions() const
{
    return {
        "1920x1080",
        "1600x900",
        "1366x768",
        "1280x720",
        "1024x768",
        "800x600"
    };
}

QStringList DisplaySettings::availableOrientations() const
{
    // Valid Weston transform values (non-flipped ones)
    return {
        "normal",
        "rotate-90",
        "rotate-180",
        "rotate-270"
    };
}

QString DisplaySettings::makeModeString(int width, int height) const
{
    if (width <= 0 || height <= 0) {
        qWarning() << "Invalid custom resolution:" << width << "x" << height;
        return {};
    }
    return QString("%1x%2").arg(width).arg(height);
}

bool DisplaySettings::applyCurrentSettings()
{
    return applyDisplaySettings(m_resolution,
                                m_orientation,
                                m_idleTimeSeconds,
                                m_touchEnabled);
}

bool DisplaySettings::applyDisplaySettings(const QString &resolution,
                                           const QString &orientation,
                                           int idleSeconds,
                                           bool enableTouch)
{
    if (resolution.isEmpty()) {
        qWarning() << "applyDisplaySettings: empty resolution string";
        return false;
    }

    QString finalResolution = resolution;

    if (resolution == "auto" || resolution.isEmpty()) {
        finalResolution = detectCurrentResolution();
        qInfo() << "Auto resolution selected → detected" << finalResolution;
    }

    if (!availableOrientations().contains(orientation)) {
        qWarning() << "applyDisplaySettings: invalid orientation:" << orientation;
        return false;
    }

    // Update config sections
    if (!applyOnlyOutput(finalResolution, orientation)) {
        qWarning() << "Failed to update [output] section";
        return false;
    }

    if (!applyIdleTimeout(idleSeconds)) {
        qWarning() << "Failed to update [core].idle-time";
        return false;
    }

    if (!enableTouchpad(enableTouch)) {
        qWarning() << "Failed to update [touchpad] section";
        return false;
    }

    // Update properties and signals
    setResolution(resolution);
    setOrientation(orientation);
    setIdleTimeSeconds(idleSeconds);
    setTouchEnabled(enableTouch);

    return restartWeston();
}

bool DisplaySettings::reloadFromConfig()
{
    bool ok = loadFromWestonIni();
    if (!ok)
        qWarning() << "reloadFromConfig: failed to load from weston.ini";
    return ok;
}

// ----- Private helpers to modify config -----

bool DisplaySettings::applyOnlyOutput(const QString &resolution,
                                      const QString &orientation)
{
    QString output = detectOutput();
    QMap<QString, QStringList> ini = loadWestonIniRaw();

    QStringList block{
        "name=" + output,
        "mode=" + resolution,
        "transform=" + orientation
    };

    setBlock(ini, "output", block);

    if (!saveWestonIniRaw(ini)) {
        qWarning() << "Failed to save weston.ini for output update";
        return false;
    }

    qInfo() << "Updated [output] section:" << resolution << orientation;
    return true;
}

bool DisplaySettings::applyIdleTimeout(int seconds)
{
    if (seconds < 0)
        seconds = 0;

    QMap<QString, QStringList> ini = loadWestonIniRaw();
    QStringList core = ini.value("core");

    // Remove any existing idle-time lines
    for (int i = core.size() - 1; i >= 0; --i) {
        const QString lineTrim = core.at(i).trimmed();
        if (lineTrim.startsWith("idle-time="))
            core.removeAt(i);
    }

    core.append(QString("idle-time=%1").arg(seconds));
    ini["core"] = core;

    if (!saveWestonIniRaw(ini)) {
        qWarning() << "Failed to save weston.ini for idle-time update";
        return false;
    }

    qInfo() << "Updated [core].idle-time to" << seconds;
    return true;
}

bool DisplaySettings::enableTouchpad(bool enable)
{
    QMap<QString, QStringList> ini = loadWestonIniRaw();

    if (enable) {
        setBlock(ini, "touchpad", {
                                      "constant_accel_factor = 50",
                                      "min_accel_factor = 0.16",
                                      "max_accel_factor = 1.0"
                                  });
        qInfo() << "Enabled [touchpad] section";
    } else {
        ini.remove("touchpad");
        qInfo() << "Removed [touchpad] section";
    }

    if (!saveWestonIniRaw(ini)) {
        qWarning() << "Failed to save weston.ini for touchpad update";
        return false;
    }

    return true;
}

// ----- Load current settings from weston.ini -----

bool DisplaySettings::loadFromWestonIni()
{
    QMap<QString, QStringList> ini = loadWestonIniRaw();
    if (ini.isEmpty())
        return false;

    // Defaults
    QString res = detectCurrentResolution();
    QString ori = "normal";
    int idle = 0;
    bool touch = false;

    // [output]
    if (ini.contains("output")) {
        const QStringList outLines = ini.value("output");
        for (const QString &lineRaw : outLines) {
            QString line = lineRaw.trimmed();
            if (line.startsWith("mode="))
                res = line.mid(QString("mode=").length()).trimmed();
            else if (line.startsWith("transform="))
                ori = line.mid(QString("transform=").length()).trimmed();
        }
    }

    // [core]
    if (ini.contains("core")) {
        const QStringList coreLines = ini.value("core");
        for (const QString &lineRaw : coreLines) {
            QString line = lineRaw.trimmed();
            if (line.startsWith("idle-time=")) {
                bool ok = false;
                int val = line.mid(QString("idle-time=").length()).trimmed().toInt(&ok);
                if (ok)
                    idle = val;
            }
        }
    }

    // [touchpad]
    touch = ini.contains("touchpad");

    // Assign & emit
    setResolution(res);
    setOrientation(ori);
    setIdleTimeSeconds(idle);
    setTouchEnabled(touch);

    qInfo() << "Loaded from weston.ini:"
            << "resolution=" << m_resolution
            << "orientation=" << m_orientation
            << "idleTime=" << m_idleTimeSeconds
            << "touchEnabled=" << m_touchEnabled;

    return true;
}

QString DisplaySettings::detectCurrentResolution() const
{
    QProcess p;
    p.start("weston-info");
    p.waitForFinished(2000);

    QString out = p.readAllStandardOutput();

    QRegularExpression rx("logical_width:\\s*(\\d+)\\s*,\\s*logical_height:\\s*(\\d+)");
    QRegularExpressionMatch m = rx.match(out);

    if (m.hasMatch()) {
        QString width = m.captured(1);
        QString height = m.captured(2);
        return width + "x" + height;
    }

    // Fallback to EDID preferred (try to read modes)
    QRegularExpression rx2("width:\\s*(\\d+) px, height:\\s*(\\d+) px, refresh:");
    auto m2 = rx2.match(out);
    if (m2.hasMatch()) {
        return m2.captured(1) + "x" + m2.captured(2);
    }

    qWarning() << "Could not detect resolution.";
    return "1920x1080";
}

// ----- Service & power control -----

bool DisplaySettings::restartWeston()
{
    qInfo() << "Restarting weston.service...";
    int code = QProcess::execute("systemctl", {"restart", "weston.service"});
    if (code != 0) {
        qWarning() << "systemctl restart weston.service failed, code:" << code;
        return false;
    }
    return true;
}

bool DisplaySettings::shutdownDevice()
{
    qInfo() << "Requesting system poweroff...";
    bool ok = QProcess::startDetached("systemctl", {"poweroff"});
    if (!ok)
        qWarning() << "Failed to start 'systemctl poweroff'";
    return ok;
}

int DisplaySettings::displayIdleMinutes() const
{
    return m_displayIdleMinutes;
}

void DisplaySettings::setDisplayIdleMinutes(int newDisplayIdleMinutes)
{
    if (m_displayIdleMinutes == newDisplayIdleMinutes)
        return;
    m_displayIdleMinutes = newDisplayIdleMinutes;
    emit displayIdleMinutesChanged();
}

int DisplaySettings::deviceAutoPowerOffHours() const
{
    return m_deviceAutoPowerOffHours;
}

void DisplaySettings::setDeviceAutoPowerOffHours(int newDeviceAutoPowerOffHours)
{
    if (m_deviceAutoPowerOffHours == newDeviceAutoPowerOffHours)
        return;
    m_deviceAutoPowerOffHours = newDeviceAutoPowerOffHours;
    emit deviceAutoPowerOffHoursChanged();
}


void DisplaySettings::normalizeWestonIniSpacing()
{
    QFile f(WESTON_CONFIG);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Cannot open weston.ini for normalization";
        return;
    }

    QString content = QString::fromUtf8(f.readAll());
    f.close();

    // Remove excessive blank lines
    QString cleaned = removeExtraBlankLines(content);

    // Write back only if changed
    if (cleaned != content) {
        if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
            qWarning() << "Cannot write cleaned weston.ini";
            return;
        }
        f.write(cleaned.toUtf8());
        f.close();
        qInfo() << "Normalized weston.ini (removed excessive blank lines)";
    }
}
