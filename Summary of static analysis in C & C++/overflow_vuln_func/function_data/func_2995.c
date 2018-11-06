static int64_t guest_file_handle_add(FILE *fh, Error **errp)
{
    GuestFileHandle *gfh;
    int64_t handle;

    handle = ga_get_fd_handle(ga_state, errp);
    if (handle < 0) {
        return -1;
    }

    gfh = g_malloc0(sizeof(GuestFileHandle));
    gfh->id = handle;
    gfh->fh = fh;
    QTAILQ_INSERT_TAIL(&guest_file_state.filehandles, gfh, next);

    return handle;
}
