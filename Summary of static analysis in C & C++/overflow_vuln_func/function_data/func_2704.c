static ssize_t mp_dacl_listxattr(FsContext *ctx, const char *path,
                                 char *name, void *value, size_t osize)
{
    ssize_t len = sizeof(ACL_DEFAULT);

    if (!value) {
        return len;
    }

    if (osize < len) {
        errno = ERANGE;
        return -1;
    }

    /
    memcpy(value, ACL_ACCESS, len);
    return 0;
}
