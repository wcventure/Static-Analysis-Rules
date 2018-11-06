static void build_fs_mount_list(FsMountList *mounts, Error **errp)
{
    FsMount *mount;
    char const *mountinfo = "/proc/self/mountinfo";
    FILE *fp;
    char *line = NULL, *dash;
    size_t n;
    char check;
    unsigned int devmajor, devminor;
    int ret, dir_s, dir_e, type_s, type_e, dev_s, dev_e;

    fp = fopen(mountinfo, "r");
    if (!fp) {
        build_fs_mount_list_from_mtab(mounts, errp);
        return;
    }

    while (getline(&line, &n, fp) != -1) {
        ret = sscanf(line, "%*u %*u %u:%u %*s %n%*s%n%c",
                     &devmajor, &devminor, &dir_s, &dir_e, &check);
        if (ret < 3) {
            continue;
        }
        dash = strstr(line + dir_e, " - ");
        if (!dash) {
            continue;
        }
        ret = sscanf(dash, " - %n%*s%n %n%*s%n%c",
                     &type_s, &type_e, &dev_s, &dev_e, &check);
        if (ret < 1) {
            continue;
        }
        line[dir_e] = 0;
        dash[type_e] = 0;
        dash[dev_e] = 0;
        decode_mntname(line + dir_s, dir_e - dir_s);
        decode_mntname(dash + dev_s, dev_e - dev_s);
        if (devmajor == 0) {
            /
            if (strcmp("btrfs", dash + type_s) != 0 ||
                dev_major_minor(dash + dev_s, &devmajor, &devminor) < 0) {
                continue;
            }
        }

        mount = g_malloc0(sizeof(FsMount));
        mount->dirname = g_strdup(line + dir_s);
        mount->devtype = g_strdup(dash + type_s);
        mount->devmajor = devmajor;
        mount->devminor = devminor;

        QTAILQ_INSERT_TAIL(mounts, mount, next);
    }
    free(line);

    fclose(fp);
}
