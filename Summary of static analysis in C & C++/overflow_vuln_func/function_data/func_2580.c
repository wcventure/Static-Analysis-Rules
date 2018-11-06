static int archipelago_aio_segmented_rw(BDRVArchipelagoState *s,
                                        size_t count,
                                        off_t offset,
                                        ArchipelagoAIOCB *aio_cb,
                                        int op)
{
    int i, ret, segments_nr, last_segment_size;
    ArchipelagoSegmentedRequest *segreq;

    segreq = g_malloc(sizeof(ArchipelagoSegmentedRequest));

    if (op == ARCHIP_OP_FLUSH) {
        segments_nr = 1;
        segreq->ref = segments_nr;
        segreq->total = count;
        segreq->count = 0;
        segreq->failed = 0;
        ret = archipelago_submit_request(s, 0, count, offset, aio_cb,
                                           segreq, ARCHIP_OP_FLUSH);
        if (ret < 0) {
            goto err_exit;
        }
        return 0;
    }

    segments_nr = (int)(count / MAX_REQUEST_SIZE) + \
                  ((count % MAX_REQUEST_SIZE) ? 1 : 0);
    last_segment_size = (int)(count % MAX_REQUEST_SIZE);

    segreq->ref = segments_nr;
    segreq->total = count;
    segreq->count = 0;
    segreq->failed = 0;

    for (i = 0; i < segments_nr - 1; i++) {
        ret = archipelago_submit_request(s, i * MAX_REQUEST_SIZE,
                                           MAX_REQUEST_SIZE,
                                           offset + i * MAX_REQUEST_SIZE,
                                           aio_cb, segreq, op);

        if (ret < 0) {
            goto err_exit;
        }
    }

    if ((segments_nr > 1) && last_segment_size) {
        ret = archipelago_submit_request(s, i * MAX_REQUEST_SIZE,
                                           last_segment_size,
                                           offset + i * MAX_REQUEST_SIZE,
                                           aio_cb, segreq, op);
    } else if ((segments_nr > 1) && !last_segment_size) {
        ret = archipelago_submit_request(s, i * MAX_REQUEST_SIZE,
                                           MAX_REQUEST_SIZE,
                                           offset + i * MAX_REQUEST_SIZE,
                                           aio_cb, segreq, op);
    } else if (segments_nr == 1) {
            ret = archipelago_submit_request(s, 0, count, offset, aio_cb,
                                               segreq, op);
    }

    if (ret < 0) {
        goto err_exit;
    }

    return 0;

err_exit:
    __sync_add_and_fetch(&segreq->failed, 1);
    if (segments_nr == 1) {
        if (__sync_add_and_fetch(&segreq->ref, -1) == 0) {
            g_free(segreq);
        }
    } else {
        if ((__sync_add_and_fetch(&segreq->ref, -segments_nr + i)) == 0) {
            g_free(segreq);
        }
    }

    return ret;
}
