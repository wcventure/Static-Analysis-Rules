static int do_sd_create(char *filename, int64_t vdi_size,
                        uint32_t base_vid, uint32_t *vdi_id, int snapshot,
                        const char *addr, const char *port)
{
    SheepdogVdiReq hdr;
    SheepdogVdiRsp *rsp = (SheepdogVdiRsp *)&hdr;
    int fd, ret;
    unsigned int wlen, rlen = 0;
    char buf[SD_MAX_VDI_LEN];

    fd = connect_to_sdog(addr, port);
    if (fd < 0) {
        return fd;
    }

    memset(buf, 0, sizeof(buf));
    strncpy(buf, filename, SD_MAX_VDI_LEN);

    memset(&hdr, 0, sizeof(hdr));
    hdr.opcode = SD_OP_NEW_VDI;
    hdr.base_vdi_id = base_vid;

    wlen = SD_MAX_VDI_LEN;

    hdr.flags = SD_FLAG_CMD_WRITE;
    hdr.snapid = snapshot;

    hdr.data_length = wlen;
    hdr.vdi_size = vdi_size;

    ret = do_req(fd, (SheepdogReq *)&hdr, buf, &wlen, &rlen);

    closesocket(fd);

    if (ret) {
        return ret;
    }

    if (rsp->result != SD_RES_SUCCESS) {
        error_report("%s, %s", sd_strerror(rsp->result), filename);
        return -EIO;
    }

    if (vdi_id) {
        *vdi_id = rsp->vdi_id;
    }

    return 0;
}
