int net_init_l2tpv3(const NetClientOptions *opts,
                    const char *name,
                    NetClientState *peer)
{


    const NetdevL2TPv3Options *l2tpv3;
    NetL2TPV3State *s;
    NetClientState *nc;
    int fd = -1, gairet;
    struct addrinfo hints;
    struct addrinfo *result = NULL;
    char *srcport, *dstport;

    nc = qemu_new_net_client(&net_l2tpv3_info, peer, "l2tpv3", name);

    s = DO_UPCAST(NetL2TPV3State, nc, nc);

    s->queue_head = 0;
    s->queue_tail = 0;
    s->header_mismatch = false;

    assert(opts->kind == NET_CLIENT_OPTIONS_KIND_L2TPV3);
    l2tpv3 = opts->l2tpv3;

    if (l2tpv3->has_ipv6 && l2tpv3->ipv6) {
        s->ipv6 = l2tpv3->ipv6;
    } else {
        s->ipv6 = false;
    }

    if ((l2tpv3->has_offset) && (l2tpv3->offset > 256)) {
        error_report("l2tpv3_open : offset must be less than 256 bytes");
        goto outerr;
    }

    if (l2tpv3->has_rxcookie || l2tpv3->has_txcookie) {
        if (l2tpv3->has_rxcookie && l2tpv3->has_txcookie) {
            s->cookie = true;
        } else {
            goto outerr;
        }
    } else {
        s->cookie = false;
    }

    if (l2tpv3->has_cookie64 || l2tpv3->cookie64) {
        s->cookie_is_64  = true;
    } else {
        s->cookie_is_64  = false;
    }

    if (l2tpv3->has_udp && l2tpv3->udp) {
        s->udp = true;
        if (!(l2tpv3->has_srcport && l2tpv3->has_dstport)) {
            error_report("l2tpv3_open : need both src and dst port for udp");
            goto outerr;
        } else {
            srcport = l2tpv3->srcport;
            dstport = l2tpv3->dstport;
        }
    } else {
        s->udp = false;
        srcport = NULL;
        dstport = NULL;
    }


    s->offset = 4;
    s->session_offset = 0;
    s->cookie_offset = 4;
    s->counter_offset = 4;

    s->tx_session = l2tpv3->txsession;
    if (l2tpv3->has_rxsession) {
        s->rx_session = l2tpv3->rxsession;
    } else {
        s->rx_session = s->tx_session;
    }

    if (s->cookie) {
        s->rx_cookie = l2tpv3->rxcookie;
        s->tx_cookie = l2tpv3->txcookie;
        if (s->cookie_is_64 == true) {
            /
            s->offset += 8;
            s->counter_offset += 8;
        } else {
            /
            s->offset += 4;
            s->counter_offset += 4;
        }
    }

    memset(&hints, 0, sizeof(hints));

    if (s->ipv6) {
        hints.ai_family = AF_INET6;
    } else {
        hints.ai_family = AF_INET;
    }
    if (s->udp) {
        hints.ai_socktype = SOCK_DGRAM;
        hints.ai_protocol = 0;
        s->offset += 4;
        s->counter_offset += 4;
        s->session_offset += 4;
        s->cookie_offset += 4;
    } else {
        hints.ai_socktype = SOCK_RAW;
        hints.ai_protocol = IPPROTO_L2TP;
    }

    gairet = getaddrinfo(l2tpv3->src, srcport, &hints, &result);

    if ((gairet != 0) || (result == NULL)) {
        error_report(
            "l2tpv3_open : could not resolve src, errno = %s",
            gai_strerror(gairet)
        );
        goto outerr;
    }
    fd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (fd == -1) {
        fd = -errno;
        error_report("l2tpv3_open : socket creation failed, errno = %d", -fd);
        goto outerr;
    }
    if (bind(fd, (struct sockaddr *) result->ai_addr, result->ai_addrlen)) {
        error_report("l2tpv3_open :  could not bind socket err=%i", errno);
        goto outerr;
    }
    if (result) {
        freeaddrinfo(result);
    }

    memset(&hints, 0, sizeof(hints));

    if (s->ipv6) {
        hints.ai_family = AF_INET6;
    } else {
        hints.ai_family = AF_INET;
    }
    if (s->udp) {
        hints.ai_socktype = SOCK_DGRAM;
        hints.ai_protocol = 0;
    } else {
        hints.ai_socktype = SOCK_RAW;
        hints.ai_protocol = IPPROTO_L2TP;
    }

    result = NULL;
    gairet = getaddrinfo(l2tpv3->dst, dstport, &hints, &result);
    if ((gairet != 0) || (result == NULL)) {
        error_report(
            "l2tpv3_open : could not resolve dst, error = %s",
            gai_strerror(gairet)
        );
        goto outerr;
    }

    s->dgram_dst = g_new0(struct sockaddr_storage, 1);
    memcpy(s->dgram_dst, result->ai_addr, result->ai_addrlen);
    s->dst_size = result->ai_addrlen;

    if (result) {
        freeaddrinfo(result);
    }

    if (l2tpv3->has_counter && l2tpv3->counter) {
        s->has_counter = true;
        s->offset += 4;
    } else {
        s->has_counter = false;
    }

    if (l2tpv3->has_pincounter && l2tpv3->pincounter) {
        s->has_counter = true;  /
        s->pin_counter = true;
    } else {
        s->pin_counter = false;
    }

    if (l2tpv3->has_offset) {
        /
        s->offset += l2tpv3->offset;
    }

    if ((s->ipv6) || (s->udp)) {
        s->header_size = s->offset;
    } else {
        s->header_size = s->offset + sizeof(struct iphdr);
    }

    s->msgvec = build_l2tpv3_vector(s, MAX_L2TPV3_MSGCNT);
    s->vec = g_malloc(sizeof(struct iovec) * MAX_L2TPV3_IOVCNT);
    s->header_buf = g_malloc(s->header_size);

    qemu_set_nonblock(fd);

    s->fd = fd;
    s->counter = 0;

    l2tpv3_read_poll(s, true);

    snprintf(s->nc.info_str, sizeof(s->nc.info_str),
             "l2tpv3: connected");
    return 0;
outerr:
    qemu_del_net_client(nc);
    if (fd >= 0) {
        close(fd);
    }
    if (result) {
        freeaddrinfo(result);
    }
    return -1;
}
