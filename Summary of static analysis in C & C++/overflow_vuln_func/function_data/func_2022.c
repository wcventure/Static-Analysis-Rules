static uint16_t nvme_rw(NvmeCtrl *n, NvmeNamespace *ns, NvmeCmd *cmd,
    NvmeRequest *req)
{
    NvmeRwCmd *rw = (NvmeRwCmd *)cmd;
    uint32_t nlb  = le32_to_cpu(rw->nlb) + 1;
    uint64_t slba = le64_to_cpu(rw->slba);
    uint64_t prp1 = le64_to_cpu(rw->prp1);
    uint64_t prp2 = le64_to_cpu(rw->prp2);

    uint8_t lba_index  = NVME_ID_NS_FLBAS_INDEX(ns->id_ns.flbas);
    uint8_t data_shift = ns->id_ns.lbaf[lba_index].ds;
    uint64_t data_size = nlb << data_shift;
    uint64_t aio_slba  = slba << (data_shift - BDRV_SECTOR_BITS);
    int is_write = rw->opcode == NVME_CMD_WRITE ? 1 : 0;

    if ((slba + nlb) > ns->id_ns.nsze) {
        return NVME_LBA_RANGE | NVME_DNR;
    }
    if (nvme_map_prp(&req->qsg, prp1, prp2, data_size, n)) {
        return NVME_INVALID_FIELD | NVME_DNR;
    }
    assert((nlb << data_shift) == req->qsg.size);

    dma_acct_start(n->conf.blk, &req->acct, &req->qsg,
                   is_write ? BLOCK_ACCT_WRITE : BLOCK_ACCT_READ);
    req->aiocb = is_write ?
        dma_blk_write(n->conf.blk, &req->qsg, aio_slba, nvme_rw_cb, req) :
        dma_blk_read(n->conf.blk, &req->qsg, aio_slba, nvme_rw_cb, req);

    return NVME_NO_COMPLETE;
}
