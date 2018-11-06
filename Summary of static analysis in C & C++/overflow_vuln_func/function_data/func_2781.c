QemuOpts *qemu_chr_parse_compat(const char *label, const char *filename)
{
    char host[65], port[33], width[8], height[8];
    int pos;
    const char *p;
    QemuOpts *opts;
    Error *local_err = NULL;

    opts = qemu_opts_create(qemu_find_opts("chardev"), label, 1, &local_err);
    if (error_is_set(&local_err)) {
        qerror_report_err(local_err);
        error_free(local_err);
        return NULL;
    }

    if (strstart(filename, "mon:", &p)) {
        filename = p;
        qemu_opt_set(opts, "mux", "on");
        if (strcmp(filename, "stdio") == 0) {
            /
            qemu_opt_set(opts, "signal", "off");
        }
    }

    if (strcmp(filename, "null")    == 0 ||
        strcmp(filename, "pty")     == 0 ||
        strcmp(filename, "msmouse") == 0 ||
        strcmp(filename, "braille") == 0 ||
        strcmp(filename, "stdio")   == 0) {
        qemu_opt_set(opts, "backend", filename);
        return opts;
    }
    if (strstart(filename, "vc", &p)) {
        qemu_opt_set(opts, "backend", "vc");
        if (*p == ':') {
            if (sscanf(p+1, "%8[0-9]x%8[0-9]", width, height) == 2) {
                /
                qemu_opt_set(opts, "width", width);
                qemu_opt_set(opts, "height", height);
            } else if (sscanf(p+1, "%8[0-9]Cx%8[0-9]C", width, height) == 2) {
                /
                qemu_opt_set(opts, "cols", width);
                qemu_opt_set(opts, "rows", height);
            } else {
                goto fail;
            }
        }
        return opts;
    }
    if (strcmp(filename, "con:") == 0) {
        qemu_opt_set(opts, "backend", "console");
        return opts;
    }
    if (strstart(filename, "COM", NULL)) {
        qemu_opt_set(opts, "backend", "serial");
        qemu_opt_set(opts, "path", filename);
        return opts;
    }
    if (strstart(filename, "file:", &p)) {
        qemu_opt_set(opts, "backend", "file");
        qemu_opt_set(opts, "path", p);
        return opts;
    }
    if (strstart(filename, "pipe:", &p)) {
        qemu_opt_set(opts, "backend", "pipe");
        qemu_opt_set(opts, "path", p);
        return opts;
    }
    if (strstart(filename, "tcp:", &p) ||
        strstart(filename, "telnet:", &p)) {
        if (sscanf(p, "%64[^:]:%32[^,]%n", host, port, &pos) < 2) {
            host[0] = 0;
            if (sscanf(p, ":%32[^,]%n", port, &pos) < 1)
                goto fail;
        }
        qemu_opt_set(opts, "backend", "socket");
        qemu_opt_set(opts, "host", host);
        qemu_opt_set(opts, "port", port);
        if (p[pos] == ',') {
            if (qemu_opts_do_parse(opts, p+pos+1, NULL) != 0)
                goto fail;
        }
        if (strstart(filename, "telnet:", &p))
            qemu_opt_set(opts, "telnet", "on");
        return opts;
    }
    if (strstart(filename, "udp:", &p)) {
        qemu_opt_set(opts, "backend", "udp");
        if (sscanf(p, "%64[^:]:%32[^@,]%n", host, port, &pos) < 2) {
            host[0] = 0;
            if (sscanf(p, ":%32[^@,]%n", port, &pos) < 1) {
                goto fail;
            }
        }
        qemu_opt_set(opts, "host", host);
        qemu_opt_set(opts, "port", port);
        if (p[pos] == '@') {
            p += pos + 1;
            if (sscanf(p, "%64[^:]:%32[^,]%n", host, port, &pos) < 2) {
                host[0] = 0;
                if (sscanf(p, ":%32[^,]%n", port, &pos) < 1) {
                    goto fail;
                }
            }
            qemu_opt_set(opts, "localaddr", host);
            qemu_opt_set(opts, "localport", port);
        }
        return opts;
    }
    if (strstart(filename, "unix:", &p)) {
        qemu_opt_set(opts, "backend", "socket");
        if (qemu_opts_do_parse(opts, p, "path") != 0)
            goto fail;
        return opts;
    }
    if (strstart(filename, "/dev/parport", NULL) ||
        strstart(filename, "/dev/ppi", NULL)) {
        qemu_opt_set(opts, "backend", "parport");
        qemu_opt_set(opts, "path", filename);
        return opts;
    }
    if (strstart(filename, "/dev/", NULL)) {
        qemu_opt_set(opts, "backend", "tty");
        qemu_opt_set(opts, "path", filename);
        return opts;
    }

fail:
    qemu_opts_del(opts);
    return NULL;
}
