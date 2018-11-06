static int find_vdi_name(BDRVSheepdogState *s, char *filename, uint32_t snapid,
                         char *tag, uint32_t *vid, int for_snapshot)
{
    int ret, fd;
    SheepdogVdiReq hdr;
    SheepdogVdiRsp *rsp = (SheepdogVdiRsp *)&hdr;
    unsigned int wlen, rlen = 0;
    char buf[SD_MAX_VDI_LEN + SD_MAX_VDI_TAG_LEN];

    fd = connect_to_sdog(s->addr, s->port);
    if (fd < 0) {
        return fd;
    }

    memset(buf, 0, sizeof(buf));
    strncpy(buf, filename, SD_MAX_VDI_LEN);
    strncpy(buf + SD_MAX_VDI_LEN, tag, SD_MAX_VDI_TAG_LEN);

    memset(&hdr, 0, sizeof(hdr));
    if (for_snapshot) {
        hdr.opcode = SD_OP_GET_VDI_INFO;
    } else {
        hdr.opcode = SD_OP_LOCK_VDI;
    }
    wlen = SD_MAX_VDI_LEN + SD_MAX_VDI_TAG_LEN;
    hdr.proto_ver = SD_PROTO_VER;
    hdr.data_length = wlen;
    hdr.snapid = snapid;
    hdr.flags = SD_FLAG_CMD_WRITE;

    ret = do_req(fd, (SheepdogReq *)&hdr, buf, &wlen, &rlen);
    if (ret) {
        goto out;
    }

    if (rsp->result != SD_RES_SUCCESS) {
        error_report("cannot get vdi info, %s, %s %d %s",
                     sd_strerror(rsp->result), filename, snapid, tag);
        if (rsp->result == SD_RES_NO_VDI) {
            ret = -ENOENT;
        } else {
            ret = -EIO;
        }
        goto out;
    }
    *vid = rsp->vdi_id;

    ret = 0;
out:
    closesocket(fd);
    return ret;
}
