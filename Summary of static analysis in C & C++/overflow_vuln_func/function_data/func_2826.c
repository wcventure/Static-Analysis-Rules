static void build_guest_fsinfo_for_virtual_device(char const *syspath,
                                                  GuestFilesystemInfo *fs,
                                                  Error **errp)
{
    DIR *dir;
    char *dirpath;
    struct dirent entry, *result;

    dirpath = g_strdup_printf("%s/slaves", syspath);
    dir = opendir(dirpath);
    if (!dir) {
        error_setg_errno(errp, errno, "opendir(\"%s\")", dirpath);
        g_free(dirpath);
        return;
    }
    g_free(dirpath);

    for (;;) {
        if (readdir_r(dir, &entry, &result) != 0) {
            error_setg_errno(errp, errno, "readdir_r(\"%s\")", dirpath);
            break;
        }
        if (!result) {
            break;
        }

        if (entry.d_type == DT_LNK) {
            g_debug(" slave device '%s'", entry.d_name);
            dirpath = g_strdup_printf("%s/slaves/%s", syspath, entry.d_name);
            build_guest_fsinfo_for_device(dirpath, fs, errp);
            g_free(dirpath);

            if (*errp) {
                break;
            }
        }
    }

    closedir(dir);
}
