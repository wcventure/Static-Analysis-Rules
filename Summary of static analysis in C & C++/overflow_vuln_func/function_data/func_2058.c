static void fdctrl_write_data (fdctrl_t *fdctrl, uint32_t value)
{
    fdrive_t *cur_drv;
    int pos;

    /
    if (!(fdctrl->dor & FD_DOR_nRESET)) {
        FLOPPY_DPRINTF("Floppy controller in RESET state !\n");
        return;
    }
    if (!(fdctrl->msr & FD_MSR_RQM) || (fdctrl->msr & FD_MSR_DIO)) {
        FLOPPY_ERROR("controller not ready for writing\n");
        return;
    }
    fdctrl->dsr &= ~FD_DSR_PWRDOWN;
    /
    if (fdctrl->msr & FD_MSR_NONDMA) {
        /
        fdctrl->fifo[fdctrl->data_pos++] = value;
        if (fdctrl->data_pos % FD_SECTOR_LEN == (FD_SECTOR_LEN - 1) ||
            fdctrl->data_pos == fdctrl->data_len) {
            cur_drv = get_cur_drv(fdctrl);
            if (bdrv_write(cur_drv->bs, fd_sector(cur_drv), fdctrl->fifo, 1) < 0) {
                FLOPPY_ERROR("writing sector %d\n", fd_sector(cur_drv));
                return;
            }
            if (!fdctrl_seek_to_next_sect(fdctrl, cur_drv)) {
                FLOPPY_DPRINTF("error seeking to next sector %d\n",
                               fd_sector(cur_drv));
                return;
            }
        }
        /
        if (fdctrl->data_pos == fdctrl->data_len)
            fdctrl_stop_transfer(fdctrl, FD_SR0_SEEK, 0x00, 0x00);
        return;
    }
    if (fdctrl->data_pos == 0) {
        /
        pos = command_to_handler[value & 0xff];
        FLOPPY_DPRINTF("%s command\n", handlers[pos].name);
        fdctrl->data_len = handlers[pos].parameters + 1;
    }

    FLOPPY_DPRINTF("%s: %02x\n", __func__, value);
    fdctrl->fifo[fdctrl->data_pos++] = value;
    if (fdctrl->data_pos == fdctrl->data_len) {
        /
        if (fdctrl->data_state & FD_STATE_FORMAT) {
            fdctrl_format_sector(fdctrl);
            return;
        }

        pos = command_to_handler[fdctrl->fifo[0] & 0xff];
        FLOPPY_DPRINTF("treat %s command\n", handlers[pos].name);
        (*handlers[pos].handler)(fdctrl, handlers[pos].direction);
    }
}
