static coroutine_fn void nbd_trip(void *opaque)
{
    NBDClient *client = opaque;
    NBDExport *exp = client->exp;
    NBDRequestData *req;
    NBDRequest request = { 0 };    /
    int ret;
    int flags;
    int reply_data_len = 0;
    Error *local_err = NULL;
    char *msg = NULL;

    trace_nbd_trip();
    if (client->closing) {
        nbd_client_put(client);
        return;
    }

    req = nbd_request_get(client);
    ret = nbd_co_receive_request(req, &request, &local_err);
    client->recv_coroutine = NULL;
    nbd_client_receive_next_request(client);
    if (ret == -EIO) {
        goto disconnect;
    }

    if (ret < 0) {
        goto reply;
    }

    if (client->closing) {
        /
        goto done;
    }

    switch (request.type) {
    case NBD_CMD_READ:
        /
        if (request.flags & NBD_CMD_FLAG_FUA) {
            ret = blk_co_flush(exp->blk);
            if (ret < 0) {
                error_setg_errno(&local_err, -ret, "flush failed");
                break;
            }
        }

        ret = blk_pread(exp->blk, request.from + exp->dev_offset,
                        req->data, request.len);
        if (ret < 0) {
            error_setg_errno(&local_err, -ret, "reading from file failed");
            break;
        }

        reply_data_len = request.len;

        break;
    case NBD_CMD_WRITE:
        if (exp->nbdflags & NBD_FLAG_READ_ONLY) {
            error_setg(&local_err, "Export is read-only");
            ret = -EROFS;
            break;
        }

        flags = 0;
        if (request.flags & NBD_CMD_FLAG_FUA) {
            flags |= BDRV_REQ_FUA;
        }
        ret = blk_pwrite(exp->blk, request.from + exp->dev_offset,
                         req->data, request.len, flags);
        if (ret < 0) {
            error_setg_errno(&local_err, -ret, "writing to file failed");
        }

        break;
    case NBD_CMD_WRITE_ZEROES:
        if (exp->nbdflags & NBD_FLAG_READ_ONLY) {
            error_setg(&local_err, "Export is read-only");
            ret = -EROFS;
            break;
        }

        flags = 0;
        if (request.flags & NBD_CMD_FLAG_FUA) {
            flags |= BDRV_REQ_FUA;
        }
        if (!(request.flags & NBD_CMD_FLAG_NO_HOLE)) {
            flags |= BDRV_REQ_MAY_UNMAP;
        }
        ret = blk_pwrite_zeroes(exp->blk, request.from + exp->dev_offset,
                                request.len, flags);
        if (ret < 0) {
            error_setg_errno(&local_err, -ret, "writing to file failed");
        }

        break;
    case NBD_CMD_DISC:
        /
        abort();

    case NBD_CMD_FLUSH:
        ret = blk_co_flush(exp->blk);
        if (ret < 0) {
            error_setg_errno(&local_err, -ret, "flush failed");
        }

        break;
    case NBD_CMD_TRIM:
        ret = blk_co_pdiscard(exp->blk, request.from + exp->dev_offset,
                              request.len);
        if (ret < 0) {
            error_setg_errno(&local_err, -ret, "discard failed");
        }

        break;
    default:
        error_setg(&local_err, "invalid request type (%" PRIu32 ") received",
                   request.type);
        ret = -EINVAL;
    }

reply:
    if (local_err) {
        /
        assert(ret < 0);
        msg = g_strdup(error_get_pretty(local_err));
        error_report_err(local_err);
        local_err = NULL;
    }

    if (client->structured_reply &&
        (ret < 0 || request.type == NBD_CMD_READ)) {
        if (ret < 0) {
            ret = nbd_co_send_structured_error(req->client, request.handle,
                                               -ret, msg, &local_err);
        } else if (reply_data_len) {
            ret = nbd_co_send_structured_read(req->client, request.handle,
                                              request.from, req->data,
                                              reply_data_len, &local_err);
        } else {
            ret = nbd_co_send_structured_done(req->client, request.handle,
                                              &local_err);
        }
    } else {
        ret = nbd_co_send_simple_reply(req->client, request.handle,
                                       ret < 0 ? -ret : 0,
                                       req->data, reply_data_len, &local_err);
    }
    g_free(msg);
    if (ret < 0) {
        error_prepend(&local_err, "Failed to send reply: ");
        goto disconnect;
    }

    /
    if (!req->complete) {
        error_setg(&local_err, "Request handling failed in intermediate state");
        goto disconnect;
    }

done:
    nbd_request_put(req);
    nbd_client_put(client);
    return;

disconnect:
    if (local_err) {
        error_reportf_err(local_err, "Disconnect client, due to: ");
    }
    nbd_request_put(req);
    client_close(client, true);
    nbd_client_put(client);
}
