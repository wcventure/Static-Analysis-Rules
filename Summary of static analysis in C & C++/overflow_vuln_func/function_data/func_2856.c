static int nbd_co_receive_request(NBDRequestData *req, NBDRequest *request,
                                  Error **errp)
{
    NBDClient *client = req->client;
    int valid_flags;

    g_assert(qemu_in_coroutine());
    assert(client->recv_coroutine == qemu_coroutine_self());
    if (nbd_receive_request(client->ioc, request, errp) < 0) {
        return -EIO;
    }

    trace_nbd_co_receive_request_decode_type(request->handle, request->type,
                                             nbd_cmd_lookup(request->type));

    if (request->type != NBD_CMD_WRITE) {
        /
        req->complete = true;
    }

    if (request->type == NBD_CMD_DISC) {
        /
        return -EIO;
    }

    /
    if ((request->from + request->len) < request->from) {
        error_setg(errp,
                   "integer overflow detected, you're probably being attacked");
        return -EINVAL;
    }

    if (request->type == NBD_CMD_READ || request->type == NBD_CMD_WRITE) {
        if (request->len > NBD_MAX_BUFFER_SIZE) {
            error_setg(errp, "len (%" PRIu32" ) is larger than max len (%u)",
                       request->len, NBD_MAX_BUFFER_SIZE);
            return -EINVAL;
        }

        req->data = blk_try_blockalign(client->exp->blk, request->len);
        if (req->data == NULL) {
            error_setg(errp, "No memory");
            return -ENOMEM;
        }
    }
    if (request->type == NBD_CMD_WRITE) {
        if (nbd_read(client->ioc, req->data, request->len, errp) < 0) {
            error_prepend(errp, "reading from socket failed: ");
            return -EIO;
        }
        req->complete = true;

        trace_nbd_co_receive_request_payload_received(request->handle,
                                                      request->len);
    }

    /
    if (request->from + request->len > client->exp->size) {
        error_setg(errp, "operation past EOF; From: %" PRIu64 ", Len: %" PRIu32
                   ", Size: %" PRIu64, request->from, request->len,
                   (uint64_t)client->exp->size);
        return request->type == NBD_CMD_WRITE ? -ENOSPC : -EINVAL;
    }
    valid_flags = NBD_CMD_FLAG_FUA;
    if (request->type == NBD_CMD_READ && client->structured_reply) {
        valid_flags |= NBD_CMD_FLAG_DF;
    } else if (request->type == NBD_CMD_WRITE_ZEROES) {
        valid_flags |= NBD_CMD_FLAG_NO_HOLE;
    }
    if (request->flags & ~valid_flags) {
        error_setg(errp, "unsupported flags for command %s (got 0x%x)",
                   nbd_cmd_lookup(request->type), request->flags);
        return -EINVAL;
    }

    return 0;
}
