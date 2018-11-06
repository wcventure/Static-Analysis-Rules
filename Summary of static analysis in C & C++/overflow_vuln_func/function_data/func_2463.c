static int do_readlink(struct iovec *iovec, struct iovec *out_iovec)
{
    char *buffer;
    int size, retval;
    V9fsString target, path;

    v9fs_string_init(&path);
    retval = proxy_unmarshal(iovec, PROXY_HDR_SZ, "sd", &path, &size);
    if (retval < 0) {
        v9fs_string_free(&path);
        return retval;
    }
    buffer = g_malloc(size);
    v9fs_string_init(&target);
    retval = readlink(path.data, buffer, size);
    if (retval > 0) {
        buffer[retval] = '\0';
        v9fs_string_sprintf(&target, "%s", buffer);
        retval = proxy_marshal(out_iovec, PROXY_HDR_SZ, "s", &target);
    } else {
        retval = -errno;
    }
    g_free(buffer);
    v9fs_string_free(&target);
    v9fs_string_free(&path);
    return retval;
}
