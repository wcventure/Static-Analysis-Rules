static int scsi_req_length(SCSICommand *cmd, SCSIDevice *dev, uint8_t *buf)
{
    switch (buf[0] >> 5) {
    case 0:
        cmd->xfer = buf[4];
        cmd->len = 6;
        /
        if (cmd->xfer == 0) {
            cmd->xfer = 256;
        }
        break;
    case 1:
    case 2:
        cmd->xfer = lduw_be_p(&buf[7]);
        cmd->len = 10;
        break;
    case 4:
        cmd->xfer = ldl_be_p(&buf[10]) & 0xffffffffULL;
        cmd->len = 16;
        break;
    case 5:
        cmd->xfer = ldl_be_p(&buf[6]) & 0xffffffffULL;
        cmd->len = 12;
        break;
    default:
        return -1;
    }

    switch (buf[0]) {
    case TEST_UNIT_READY:
    case REWIND:
    case START_STOP:
    case SET_CAPACITY:
    case WRITE_FILEMARKS:
    case WRITE_FILEMARKS_16:
    case SPACE:
    case RESERVE:
    case RELEASE:
    case ERASE:
    case ALLOW_MEDIUM_REMOVAL:
    case VERIFY_10:
    case SEEK_10:
    case SYNCHRONIZE_CACHE:
    case SYNCHRONIZE_CACHE_16:
    case LOCATE_16:
    case LOCK_UNLOCK_CACHE:
    case LOAD_UNLOAD:
    case SET_CD_SPEED:
    case SET_LIMITS:
    case WRITE_LONG_10:
    case MOVE_MEDIUM:
    case UPDATE_BLOCK:
    case RESERVE_TRACK:
    case SET_READ_AHEAD:
    case PRE_FETCH:
    case PRE_FETCH_16:
    case ALLOW_OVERWRITE:
        cmd->xfer = 0;
        break;
    case MODE_SENSE:
        break;
    case WRITE_SAME_10:
    case WRITE_SAME_16:
        cmd->xfer = dev->blocksize;
        break;
    case READ_CAPACITY_10:
        cmd->xfer = 8;
        break;
    case READ_BLOCK_LIMITS:
        cmd->xfer = 6;
        break;
    case SEND_VOLUME_TAG:
        /
        if (dev->type == TYPE_ROM) {
            cmd->xfer = buf[10] | (buf[9] << 8);
        } else {
            cmd->xfer = buf[9] | (buf[8] << 8);
        }
        break;
    case WRITE_10:
    case WRITE_VERIFY_10:
    case WRITE_6:
    case WRITE_12:
    case WRITE_VERIFY_12:
    case WRITE_16:
    case WRITE_VERIFY_16:
        cmd->xfer *= dev->blocksize;
        break;
    case READ_10:
    case READ_6:
    case READ_REVERSE:
    case RECOVER_BUFFERED_DATA:
    case READ_12:
    case READ_16:
        cmd->xfer *= dev->blocksize;
        break;
    case FORMAT_UNIT:
        /
        if (dev->type == TYPE_ROM && (buf[1] & 16)) {
            cmd->xfer = 12;
        } else {
            cmd->xfer = (buf[1] & 16) == 0 ? 0 : (buf[1] & 32 ? 8 : 4);
        }
        break;
    case INQUIRY:
    case RECEIVE_DIAGNOSTIC:
    case SEND_DIAGNOSTIC:
        cmd->xfer = buf[4] | (buf[3] << 8);
        break;
    case READ_CD:
    case READ_BUFFER:
    case WRITE_BUFFER:
    case SEND_CUE_SHEET:
        cmd->xfer = buf[8] | (buf[7] << 8) | (buf[6] << 16);
        break;
    case PERSISTENT_RESERVE_OUT:
        cmd->xfer = ldl_be_p(&buf[5]) & 0xffffffffULL;
        break;
    case ERASE_12:
        if (dev->type == TYPE_ROM) {
            /
            cmd->xfer = scsi_get_performance_length(buf[9] | (buf[8] << 8),
                                                    buf[10], buf[1] & 0x1f);
        }
        break;
    case MECHANISM_STATUS:
    case READ_DVD_STRUCTURE:
    case SEND_DVD_STRUCTURE:
    case MAINTENANCE_OUT:
    case MAINTENANCE_IN:
        if (dev->type == TYPE_ROM) {
            /
            cmd->xfer = buf[9] | (buf[8] << 8);
        }
        break;
    }
    return 0;
}
