static int nfs_file_create(const char *url, QemuOpts *opts, Error **errp)
{
    int ret = 0;
    int64_t total_size = 0;
    NFSClient *client = g_new0(NFSClient, 1);
    QDict *options = NULL;

    client->aio_context = qemu_get_aio_context();

    /
    total_size = ROUND_UP(qemu_opt_get_size_del(opts, BLOCK_OPT_SIZE, 0),
                          BDRV_SECTOR_SIZE);

    options = qdict_new();
    ret = nfs_parse_uri(url, options, errp);
    if (ret < 0) {
        goto out;
    }

    ret = nfs_client_open(client, options, O_CREAT, 0, errp);
    if (ret < 0) {
        goto out;
    }
    ret = nfs_ftruncate(client->context, client->fh, total_size);
    nfs_client_close(client);
out:
    QDECREF(options);
    g_free(client);
    return ret;
}
