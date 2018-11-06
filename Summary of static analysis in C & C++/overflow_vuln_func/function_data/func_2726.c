static char *read_splashfile(char *filename, int *file_sizep, int *file_typep)
{
    GError *err = NULL;
    gboolean res;
    gchar *content;
    int file_type = -1;
    unsigned int filehead = 0;
    int bmp_bpp;

    res = g_file_get_contents(filename, &content, (gsize *)file_sizep, &err);
    if (res == FALSE) {
        error_report("failed to read splash file '%s'", filename);
        g_error_free(err);
        return NULL;
    }

    /
    if (*file_sizep < 30) {
        goto error;
    }

    /
    filehead = ((content[0] & 0xff) + (content[1] << 8)) & 0xffff;
    if (filehead == 0xd8ff) {
        file_type = JPG_FILE;
    } else if (filehead == 0x4d42) {
        file_type = BMP_FILE;
    } else {
        goto error;
    }

    /
    if (file_type == BMP_FILE) {
        bmp_bpp = (content[28] + (content[29] << 8)) & 0xffff;
        if (bmp_bpp != 24) {
            goto error;
        }
    }

    /
    *file_typep = file_type;

    return content;

error:
    error_report("splash file '%s' format not recognized; must be JPEG "
                 "or 24 bit BMP", filename);
    g_free(content);
    return NULL;
}
