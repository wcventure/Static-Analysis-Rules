static int v9fs_xattr_write(V9fsState *s, V9fsPDU *pdu, V9fsFidState *fidp,
                            uint64_t off, uint32_t count,
                            struct iovec *sg, int cnt)
{
    int i, to_copy;
    ssize_t err = 0;
    int write_count;
    int64_t xattr_len;
    size_t offset = 7;


    xattr_len = fidp->fs.xattr.len;
    write_count = xattr_len - off;
    if (write_count > count) {
        write_count = count;
    } else if (write_count < 0) {
        /
        err = -ENOSPC;
        goto out;
    }
    err = pdu_marshal(pdu, offset, "d", write_count);
    if (err < 0) {
        return err;
    }
    err += offset;
    fidp->fs.xattr.copied_len += write_count;
    /
    for (i = 0; i < cnt; i++) {
        if (write_count > sg[i].iov_len) {
            to_copy = sg[i].iov_len;
        } else {
            to_copy = write_count;
        }
        memcpy((char *)fidp->fs.xattr.value + off, sg[i].iov_base, to_copy);
        /
        off += to_copy;
        write_count -= to_copy;
    }
out:
    return err;
}
