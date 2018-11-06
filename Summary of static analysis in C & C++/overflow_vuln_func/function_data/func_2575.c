void bdrv_iterate_format(void (*it)(void *opaque, const char *name),
                         void *opaque)
{
    BlockDriver *drv;
    int count = 0;
    const char **formats = NULL;

    QLIST_FOREACH(drv, &bdrv_drivers, list) {
        if (drv->format_name) {
            bool found = false;
            int i = count;
            while (formats && i && !found) {
                found = !strcmp(formats[--i], drv->format_name);
            }

            if (!found) {
                formats = g_realloc(formats, (count + 1) * sizeof(char *));
                formats[count++] = drv->format_name;
                it(opaque, drv->format_name);
            }
        }
    }
    g_free(formats);
}
