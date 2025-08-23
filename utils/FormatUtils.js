// e.g., format time, numbers, units

function formatTime(ms) {
    var date = new Date(ms);
    return Qt.formatTime(date, "hh:mm:ss");
}

function formatNumber(n, decimals) {
    return n.toFixed(decimals);
}

// Format bytes into KB/MB/GB
function formatBytes(bytes) {
    if (bytes < 1024) return bytes + " B";
    else if (bytes < 1048576) return (bytes / 1024).toFixed(1) + " KB";
    else if (bytes < 1073741824) return (bytes / 1048576).toFixed(1) + " MB";
    return (bytes / 1073741824).toFixed(1) + " GB";
}

// Format uptime in hh:mm:ss
function formatUptime(seconds) {
    var h = Math.floor(seconds / 3600);
    var m = Math.floor((seconds % 3600) / 60);
    var s = seconds % 60;
    return (h < 10 ? "0" : "") + h + ":" +
           (m < 10 ? "0" : "") + m + ":" +
           (s < 10 ? "0" : "") + s;
}

// Format number with commas
function formatNumber(num) {
    return num.toString().replace(/\B(?=(\d{3})+(?!\d))/g, ",");
}

// Format percentage
function formatPercent(value, decimals) {
    return (value * 100).toFixed(decimals || 1) + "%";
}

// Format IP address (pad zeros if needed)
function normalizeIP(ip) {
    return ip.split(".").map(function (octet) {
        return parseInt(octet, 10);
    }).join(".");
}
