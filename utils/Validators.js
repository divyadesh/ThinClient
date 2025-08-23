// input validation helpers

// Validate IPv4 address
function isValidIP(ip) {
    var regex = /^(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)(\.(?!$)|$){4}$/;
    return regex.test(ip);
}

// Validate subnet mask (basic check)
function isValidSubnet(mask) {
    var regex = /^(255|254|252|248|240|224|192|128|0+)\.(255|254|252|248|240|224|192|128|0+)\.(255|254|252|248|240|224|192|128|0+)\.(255|254|252|248|240|224|192|128|0+)$/;
    return regex.test(mask);
}

// Validate gateway (just IP check for now)
function isValidGateway(ip) {
    return isValidIP(ip);
}

// Validate MAC address
function isValidMAC(mac) {
    var regex = /^([0-9A-Fa-f]{2}[:-]){5}([0-9A-Fa-f]{2})$/;
    return regex.test(mac);
}

// Validate hostname
function isValidHostname(name) {
    var regex = /^(?=.{1,253}$)(?!-)[a-zA-Z0-9-]+(\.[a-zA-Z0-9-]+)*$/;
    return regex.test(name);
}
