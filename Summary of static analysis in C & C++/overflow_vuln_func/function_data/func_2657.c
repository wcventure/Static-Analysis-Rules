static void v9fs_read(void *opaque)
{
    int32_t fid;
    int64_t off;
    ssize_t err = 0;
    int32_t count = 0;
    size_t offset = 7;
    int32_t max_count;
    V9fsFidState *fidp;
    V9fsPDU *pdu = opaque;
    V9fsState *s = pdu->s;

    pdu_unmarshal(pdu, offset, "dqd", &fid, &off, &max_count);
    trace_v9fs_read(pdu->tag, pdu->id, fid, off, max_count);

    fidp = get_fid(pdu, fid);
    if (fidp == NULL) {
        err = -EINVAL;
        goto out_nofid;
    }
    if (fidp->fid_type == P9_FID_DIR) {

        if (off == 0) {
            v9fs_co_rewinddir(pdu, fidp);
        }
        count = v9fs_do_readdir_with_stat(pdu, fidp, max_count);
        if (count < 0) {
            err = count;
            goto out;
        }
        err = offset;
        err += pdu_marshal(pdu, offset, "d", count);
        err += count;
    } else if (fidp->fid_type == P9_FID_FILE) {
        int32_t cnt;
        int32_t len;
        struct iovec *sg;
        struct iovec iov[128]; /

        sg = iov;
        pdu_marshal(pdu, offset + 4, "v", sg, &cnt);
        sg = cap_sg(sg, max_count, &cnt);
        do {
            if (0) {
                print_sg(sg, cnt);
            }
            /
            do {
                len = v9fs_co_preadv(pdu, fidp, sg, cnt, off);
                if (len >= 0) {
                    off   += len;
                    count += len;
                }
            } while (len == -EINTR && !pdu->cancelled);
            if (len < 0) {
                /
                err = len;
                goto out;
            }
            sg = adjust_sg(sg, len, &cnt);
        } while (count < max_count && len > 0);
        err = offset;
        err += pdu_marshal(pdu, offset, "d", count);
        err += count;
    } else if (fidp->fid_type == P9_FID_XATTR) {
        err = v9fs_xattr_read(s, pdu, fidp, off, max_count);
    } else {
        err = -EINVAL;
    }
    trace_v9fs_read_return(pdu->tag, pdu->id, count, err);
out:
    put_fid(pdu, fidp);
out_nofid:
    complete_pdu(s, pdu, err);
}
