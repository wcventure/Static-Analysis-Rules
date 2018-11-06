static ssize_t nbd_co_receive_request(NBDRequest *req, struct nbd_request *request)
{
    NBDClient *client = req->client;
    uint32_t command;
    ssize_t rc;

    g_assert(qemu_in_coroutine());
    client->recv_coroutine = qemu_coroutine_self();
    nbd_update_can_read(client);

    rc = nbd_receive_request(client->ioc, request);
    if (rc < 0) {
        if (rc != -EAGAIN) {
            rc = -EIO;
        }
        goto out;
    }

    if ((request->from + request->len) < request->from) {
        LOG("integer overflow detected! "
            "you're probably being attacked");
        rc = -EINVAL;
        goto out;
    }

    TRACE("Decoding type");

    command = request->type & NBD_CMD_MASK_COMMAND;
    if (command == NBD_CMD_READ || command == NBD_CMD_WRITE) {
        if (request->len > NBD_MAX_BUFFER_SIZE) {
            LOG("len (%" PRIu32" ) is larger than max len (%u)",
                request->len, NBD_MAX_BUFFER_SIZE);
            rc = -EINVAL;
            goto out;
        }

        req->data = blk_try_blockalign(client->exp->blk, request->len);
        if (req->data == NULL) {
            rc = -ENOMEM;
            goto out;
        }
    }
    if (command == NBD_CMD_WRITE) {
        TRACE("Reading %" PRIu32 " byte(s)", request->len);

        if (read_sync(client->ioc, req->data, request->len) != request->len) {
            LOG("reading from socket failed");
            rc = -EIO;
            goto out;
        }
    }
    rc = 0;

out:
    client->recv_coroutine = NULL;
    nbd_update_can_read(client);

    return rc;
}
