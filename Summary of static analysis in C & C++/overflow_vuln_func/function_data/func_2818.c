static void usb_mtp_object_readdir(MTPState *s, MTPObject *o)
{
    struct dirent *entry;
    DIR *dir;

    if (o->have_children) {
        return;
    }
    o->have_children = true;

    dir = opendir(o->path);
    if (!dir) {
        return;
    }
    while ((entry = readdir(dir)) != NULL) {
        if ((o->nchildren % 32) == 0) {
            o->children = g_realloc(o->children,
                                    (o->nchildren + 32) * sizeof(MTPObject *));
        }
        o->children[o->nchildren] =
            usb_mtp_object_alloc(s, s->next_handle++, o, entry->d_name);
        if (o->children[o->nchildren] != NULL) {
            o->nchildren++;
        }
    }
    closedir(dir);
}
