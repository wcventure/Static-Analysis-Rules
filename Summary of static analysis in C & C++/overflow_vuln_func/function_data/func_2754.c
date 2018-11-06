static void vnc_connect(VncDisplay *vd, int csock,
                        bool skipauth, bool websocket)
{
    VncState *vs = g_malloc0(sizeof(VncState));
    int i;

    vs->csock = csock;
    vs->vd = vd;

    if (skipauth) {
	vs->auth = VNC_AUTH_NONE;
	vs->subauth = VNC_AUTH_INVALID;
    } else {
        if (websocket) {
            vs->auth = vd->ws_auth;
            vs->subauth = VNC_AUTH_INVALID;
        } else {
            vs->auth = vd->auth;
            vs->subauth = vd->subauth;
        }
    }
    VNC_DEBUG("Client sock=%d ws=%d auth=%d subauth=%d\n",
              csock, websocket, vs->auth, vs->subauth);

    vs->lossy_rect = g_malloc0(VNC_STAT_ROWS * sizeof (*vs->lossy_rect));
    for (i = 0; i < VNC_STAT_ROWS; ++i) {
        vs->lossy_rect[i] = g_malloc0(VNC_STAT_COLS * sizeof (uint8_t));
    }

    VNC_DEBUG("New client on socket %d\n", csock);
    update_displaychangelistener(&vd->dcl, VNC_REFRESH_INTERVAL_BASE);
    qemu_set_nonblock(vs->csock);
    if (websocket) {
        vs->websocket = 1;
        if (vd->ws_tls) {
            qemu_set_fd_handler(vs->csock, vncws_tls_handshake_io, NULL, vs);
        } else {
            qemu_set_fd_handler(vs->csock, vncws_handshake_read, NULL, vs);
        }
    } else
    {
        qemu_set_fd_handler(vs->csock, vnc_client_read, NULL, vs);
    }

    vnc_client_cache_addr(vs);
    vnc_qmp_event(vs, QAPI_EVENT_VNC_CONNECTED);
    vnc_set_share_mode(vs, VNC_SHARE_MODE_CONNECTING);

    if (!vs->websocket) {
        vnc_init_state(vs);
    }

    if (vd->num_connecting > vd->connections_limit) {
        QTAILQ_FOREACH(vs, &vd->clients, next) {
            if (vs->share_mode == VNC_SHARE_MODE_CONNECTING) {
                vnc_disconnect_start(vs);
                return;
            }
        }
    }
}
