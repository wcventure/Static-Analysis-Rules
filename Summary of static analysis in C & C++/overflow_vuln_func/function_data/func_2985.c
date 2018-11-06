static int v9fs_xattr_read(V9fsState *s, V9fsPDU *pdu, V9fsFidState *fidp,
                           uint64_t off, uint32_t max_count)
{
    ssize_t err;
    size_t offset = 7;
    int read_count;
    int64_t xattr_len;
    V9fsVirtioState *v = container_of(s, V9fsVirtioState, state);
    VirtQueueElement *elem = v->elems[pdu->idx];

    xattr_len = fidp->fs.xattr.len;
    read_count = xattr_len - off;
    if (read_count > max_count) {
        read_count = max_count;
    } else if (read_count < 0) {
        /
        read_count = 0;
    }
    err = pdu_marshal(pdu, offset, "d", read_count);
    if (err < 0) {
        return err;
    }
    offset += err;

    err = v9fs_pack(elem->in_sg, elem->in_num, offset,
                    ((char *)fidp->fs.xattr.value) + off,
                    read_count);
    if (err < 0) {
        return err;
    }
    offset += err;
    return offset;
}
