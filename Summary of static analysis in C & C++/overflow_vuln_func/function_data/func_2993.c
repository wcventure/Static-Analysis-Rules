GAChannel *ga_channel_new(GAChannelMethod method, const gchar *path,
                          GAChannelCallback cb, gpointer opaque)
{
    GAChannel *c = g_malloc0(sizeof(GAChannel));
    c->event_cb = cb;
    c->user_data = opaque;

    if (!ga_channel_open(c, path, method)) {
        g_critical("error opening channel");
        ga_channel_free(c);
        return NULL;
    }

    return c;
}
