GAChannel *ga_channel_new(GAChannelMethod method, const gchar *path,
                          GAChannelCallback cb, gpointer opaque)
{
    GAChannel *c = g_malloc0(sizeof(GAChannel));
    SECURITY_ATTRIBUTES sec_attrs;

    if (!ga_channel_open(c, method, path)) {
        g_critical("error opening channel");
        g_free(c);
        return NULL;
    }

    c->cb = cb;
    c->user_data = opaque;

    sec_attrs.nLength = sizeof(SECURITY_ATTRIBUTES);
    sec_attrs.lpSecurityDescriptor = NULL;
    sec_attrs.bInheritHandle = false;

    c->rstate.buf_size = QGA_READ_COUNT_DEFAULT;
    c->rstate.buf = g_malloc(QGA_READ_COUNT_DEFAULT);
    c->rstate.ov.hEvent = CreateEvent(&sec_attrs, FALSE, FALSE, NULL);

    c->source = ga_channel_create_watch(c);
    g_source_attach(c->source, NULL);
    return c;
}
