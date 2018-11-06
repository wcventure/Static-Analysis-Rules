static void v9fs_write(void *opaque)
{
    int cnt;
    ssize_t err;
    int32_t fid;
    int64_t off;
    int32_t count;
    int32_t len = 0;
    int32_t total = 0;
    size_t offset = 7;
    V9fsFidState *fidp;
    struct iovec iov[128]; /
    struct iovec *sg = iov;
    V9fsPDU *pdu = opaque;
    V9fsState *s = pdu->s;

    pdu_unmarshal(pdu, offset, "dqdv", &fid, &off, &count, sg, &cnt);
    trace_v9fs_write(pdu->tag, pdu->id, fid, off, count, cnt);

    fidp = get_fid(pdu, fid);
    if (fidp == NULL) {
        err = -EINVAL;
        goto out_nofid;
    }
    if (fidp->fid_type == P9_FID_FILE) {
        if (fidp->fs.fd == -1) {
            err = -EINVAL;
            goto out;
        }
    } else if (fidp->fid_type == P9_FID_XATTR) {
        /
        err = v9fs_xattr_write(s, pdu, fidp, off, count, sg, cnt);
        goto out;
    } else {
        err = -EINVAL;
        goto out;
    }
    sg = cap_sg(sg, count, &cnt);
    do {
        if (0) {
            print_sg(sg, cnt);
        }
        /
        do {
            len = v9fs_co_pwritev(pdu, fidp, sg, cnt, off);
            if (len >= 0) {
                off   += len;
                total += len;
            }
        } while (len == -EINTR && !pdu->cancelled);
        if (len < 0) {
            /
            err = len;
            goto out;
        }
        sg = adjust_sg(sg, len, &cnt);
    } while (total < count && len > 0);
    offset += pdu_marshal(pdu, offset, "d", total);
    err = offset;
    trace_v9fs_write_return(pdu->tag, pdu->id, total, err);
out:
    put_fid(pdu, fidp);
out_nofid:
    complete_pdu(s, pdu, err);
}
