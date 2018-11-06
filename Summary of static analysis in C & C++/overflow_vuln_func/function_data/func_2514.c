InetSocketAddress *inet_parse(const char *str, Error **errp)
{
    InetSocketAddress *addr;
    const char *optstr, *h;
    char host[64];
    char port[33];
    int to;
    int pos;

    addr = g_new0(InetSocketAddress, 1);

    /
    if (str[0] == ':') {
        /
        host[0] = '\0';
        if (1 != sscanf(str, ":%32[^,]%n", port, &pos)) {
            error_setg(errp, "error parsing port in address '%s'", str);
            goto fail;
        }
    } else if (str[0] == '[') {
        /
        if (2 != sscanf(str, "[%64[^]]]:%32[^,]%n", host, port, &pos)) {
            error_setg(errp, "error parsing IPv6 address '%s'", str);
            goto fail;
        }
        addr->ipv6 = addr->has_ipv6 = true;
    } else {
        /
        if (2 != sscanf(str, "%64[^:]:%32[^,]%n", host, port, &pos)) {
            error_setg(errp, "error parsing address '%s'", str);
            goto fail;
        }
        if (host[strspn(host, "0123456789.")] == '\0') {
            addr->ipv4 = addr->has_ipv4 = true;
        }
    }

    addr->host = g_strdup(host);
    addr->port = g_strdup(port);

    /
    optstr = str + pos;
    h = strstr(optstr, ",to=");
    if (h) {
        h += 4;
        if (sscanf(h, "%d%n", &to, &pos) != 1 ||
            (h[pos] != '\0' && h[pos] != ',')) {
            error_setg(errp, "error parsing to= argument");
            goto fail;
        }
        addr->has_to = true;
        addr->to = to;
    }
    if (strstr(optstr, ",ipv4")) {
        addr->ipv4 = addr->has_ipv4 = true;
    }
    if (strstr(optstr, ",ipv6")) {
        addr->ipv6 = addr->has_ipv6 = true;
    }
    return addr;

fail:
    qapi_free_InetSocketAddress(addr);
    return NULL;
}
