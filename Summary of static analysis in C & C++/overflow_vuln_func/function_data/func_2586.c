static int nfs_file_create(const char *url, QemuOpts *opts, Error **errp)
{
    int ret = 0;
    int64_t total_size = 0;
    NFSClient *client = g_malloc0(sizeof(NFSClient));

    client->aio_context = qemu_get_aio_context();

    /
    total_size = qemu_opt_get_size_del(opts, BLOCK_OPT_SIZE, 0);

    ret = nfs_client_open(client, url, O_CREAT, errp);
    if (ret < 0) {
        goto out;
    }
    ret = nfs_ftruncate(client->context, client->fh, total_size);
    nfs_client_close(client);
out:
    g_free(client);
    return ret;
}
