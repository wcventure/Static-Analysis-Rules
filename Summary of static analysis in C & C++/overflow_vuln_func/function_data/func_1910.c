static void fdctrl_start_transfer (fdctrl_t *fdctrl, int direction)
{
    fdrive_t *cur_drv;
    uint8_t kh, kt, ks;
    int did_seek;

    fdctrl->cur_drv = fdctrl->fifo[1] & 1;
    cur_drv = get_cur_drv(fdctrl);
    kt = fdctrl->fifo[2];
    kh = fdctrl->fifo[3];
    ks = fdctrl->fifo[4];
    FLOPPY_DPRINTF("Start transfer at %d %d %02x %02x (%d)\n",
                   fdctrl->cur_drv, kh, kt, ks,
                   _fd_sector(kh, kt, ks, cur_drv->last_sect));
    did_seek = 0;
    switch (fd_seek(cur_drv, kh, kt, ks, fdctrl->config & 0x40)) {
    case 2:
        /
        fdctrl_stop_transfer(fdctrl, 0x40, 0x00, 0x00);
        fdctrl->fifo[3] = kt;
        fdctrl->fifo[4] = kh;
        fdctrl->fifo[5] = ks;
        return;
    case 3:
        /
        fdctrl_stop_transfer(fdctrl, 0x40, 0x80, 0x00);
        fdctrl->fifo[3] = kt;
        fdctrl->fifo[4] = kh;
        fdctrl->fifo[5] = ks;
        return;
    case 4:
        /
        fdctrl_stop_transfer(fdctrl, 0x40, 0x00, 0x00);
        fdctrl->fifo[3] = kt;
        fdctrl->fifo[4] = kh;
        fdctrl->fifo[5] = ks;
        return;
    case 1:
        did_seek = 1;
        break;
    default:
        break;
    }
    /
    fdctrl->data_dir = direction;
    fdctrl->data_pos = 0;
    FD_SET_STATE(fdctrl->data_state, FD_STATE_DATA); /
    if (fdctrl->fifo[0] & 0x80)
        fdctrl->data_state |= FD_STATE_MULTI;
    else
        fdctrl->data_state &= ~FD_STATE_MULTI;
    if (did_seek)
        fdctrl->data_state |= FD_STATE_SEEK;
    else
        fdctrl->data_state &= ~FD_STATE_SEEK;
    if (fdctrl->fifo[5] == 00) {
        fdctrl->data_len = fdctrl->fifo[8];
    } else {
	int tmp;
        fdctrl->data_len = 128 << fdctrl->fifo[5];
        tmp = (cur_drv->last_sect - ks + 1);
        if (fdctrl->fifo[0] & 0x80)
            tmp += cur_drv->last_sect;
	fdctrl->data_len *= tmp;
    }
    fdctrl->eot = fdctrl->fifo[6];
    if (fdctrl->dma_en) {
        int dma_mode;
        /
        dma_mode = DMA_get_channel_mode(fdctrl->dma_chann);
        dma_mode = (dma_mode >> 2) & 3;
        FLOPPY_DPRINTF("dma_mode=%d direction=%d (%d - %d)\n",
		       dma_mode, direction,
                       (128 << fdctrl->fifo[5]) *
		       (cur_drv->last_sect - ks + 1), fdctrl->data_len);
        if (((direction == FD_DIR_SCANE || direction == FD_DIR_SCANL ||
              direction == FD_DIR_SCANH) && dma_mode == 0) ||
            (direction == FD_DIR_WRITE && dma_mode == 2) ||
            (direction == FD_DIR_READ && dma_mode == 1)) {
            /
            fdctrl->state |= FD_CTRL_BUSY;
            /
            DMA_hold_DREQ(fdctrl->dma_chann);
            DMA_schedule(fdctrl->dma_chann);
            return;
        } else {
	    FLOPPY_ERROR("dma_mode=%d direction=%d\n", dma_mode, direction);
        }
    }
    FLOPPY_DPRINTF("start non-DMA transfer\n");
    /
    fdctrl_raise_irq(fdctrl, 0x00);

    return;
}
