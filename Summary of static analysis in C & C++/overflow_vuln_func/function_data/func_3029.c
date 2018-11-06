void ide_exec_cmd(IDEBus *bus, uint32_t val)
{
    uint16_t *identify_data;
    IDEState *s;
    int n;
    int lba48 = 0;

#if defined(DEBUG_IDE)
    printf("ide: CMD=%02x\n", val);
#endif
    s = idebus_active_if(bus);
    /
    if (s != bus->ifs && !s->bs)
        return;

    /
    if ((s->status & (BUSY_STAT|DRQ_STAT)) && val != WIN_DEVICE_RESET)
        return;

    if (!ide_cmd_permitted(s, val)) {
        goto abort_cmd;
    }

    switch(val) {
    case WIN_DSM:
        switch (s->feature) {
        case DSM_TRIM:
            if (!s->bs) {
                goto abort_cmd;
            }
            ide_sector_start_dma(s, IDE_DMA_TRIM);
            break;
        default:
            goto abort_cmd;
        }
        break;
    case WIN_IDENTIFY:
        if (s->bs && s->drive_kind != IDE_CD) {
            if (s->drive_kind != IDE_CFATA)
                ide_identify(s);
            else
                ide_cfata_identify(s);
            s->status = READY_STAT | SEEK_STAT;
            ide_transfer_start(s, s->io_buffer, 512, ide_transfer_stop);
        } else {
            if (s->drive_kind == IDE_CD) {
                ide_set_signature(s);
            }
            ide_abort_command(s);
        }
        ide_set_irq(s->bus);
        break;
    case WIN_SPECIFY:
    case WIN_RECAL:
        s->error = 0;
        s->status = READY_STAT | SEEK_STAT;
        ide_set_irq(s->bus);
        break;
    case WIN_SETMULT:
        if (s->drive_kind == IDE_CFATA && s->nsector == 0) {
            /
            s->mult_sectors = 0;
            s->status = READY_STAT | SEEK_STAT;
        } else if ((s->nsector & 0xff) != 0 &&
            ((s->nsector & 0xff) > MAX_MULT_SECTORS ||
             (s->nsector & (s->nsector - 1)) != 0)) {
            ide_abort_command(s);
        } else {
            s->mult_sectors = s->nsector & 0xff;
            s->status = READY_STAT | SEEK_STAT;
        }
        ide_set_irq(s->bus);
        break;
    case WIN_VERIFY_EXT:
	lba48 = 1;
    case WIN_VERIFY:
    case WIN_VERIFY_ONCE:
        /
	ide_cmd_lba48_transform(s, lba48);
        s->status = READY_STAT | SEEK_STAT;
        ide_set_irq(s->bus);
        break;
    case WIN_READ_EXT:
	lba48 = 1;
    case WIN_READ:
    case WIN_READ_ONCE:
        if (s->drive_kind == IDE_CD) {
            ide_set_signature(s); /
            goto abort_cmd;
        }
        if (!s->bs) {
            goto abort_cmd;
        }
	ide_cmd_lba48_transform(s, lba48);
        s->req_nb_sectors = 1;
        ide_sector_read(s);
        break;
    case WIN_WRITE_EXT:
	lba48 = 1;
    case WIN_WRITE:
    case WIN_WRITE_ONCE:
    case CFA_WRITE_SECT_WO_ERASE:
    case WIN_WRITE_VERIFY:
        if (!s->bs) {
            goto abort_cmd;
        }
	ide_cmd_lba48_transform(s, lba48);
        s->error = 0;
        s->status = SEEK_STAT | READY_STAT;
        s->req_nb_sectors = 1;
        ide_transfer_start(s, s->io_buffer, 512, ide_sector_write);
        s->media_changed = 1;
        break;
    case WIN_MULTREAD_EXT:
	lba48 = 1;
    case WIN_MULTREAD:
        if (!s->bs) {
            goto abort_cmd;
        }
        if (!s->mult_sectors) {
            goto abort_cmd;
        }
	ide_cmd_lba48_transform(s, lba48);
        s->req_nb_sectors = s->mult_sectors;
        ide_sector_read(s);
        break;
    case WIN_MULTWRITE_EXT:
	lba48 = 1;
    case WIN_MULTWRITE:
    case CFA_WRITE_MULTI_WO_ERASE:
        if (!s->bs) {
            goto abort_cmd;
        }
        if (!s->mult_sectors) {
            goto abort_cmd;
        }
	ide_cmd_lba48_transform(s, lba48);
        s->error = 0;
        s->status = SEEK_STAT | READY_STAT;
        s->req_nb_sectors = s->mult_sectors;
        n = s->nsector;
        if (n > s->req_nb_sectors)
            n = s->req_nb_sectors;
        ide_transfer_start(s, s->io_buffer, 512 * n, ide_sector_write);
        s->media_changed = 1;
        break;
    case WIN_READDMA_EXT:
	lba48 = 1;
    case WIN_READDMA:
    case WIN_READDMA_ONCE:
        if (!s->bs) {
            goto abort_cmd;
        }
	ide_cmd_lba48_transform(s, lba48);
        ide_sector_start_dma(s, IDE_DMA_READ);
        break;
    case WIN_WRITEDMA_EXT:
	lba48 = 1;
    case WIN_WRITEDMA:
    case WIN_WRITEDMA_ONCE:
        if (!s->bs) {
            goto abort_cmd;
        }
	ide_cmd_lba48_transform(s, lba48);
        ide_sector_start_dma(s, IDE_DMA_WRITE);
        s->media_changed = 1;
        break;
    case WIN_READ_NATIVE_MAX_EXT:
	lba48 = 1;
    case WIN_READ_NATIVE_MAX:
	ide_cmd_lba48_transform(s, lba48);
        ide_set_sector(s, s->nb_sectors - 1);
        s->status = READY_STAT | SEEK_STAT;
        ide_set_irq(s->bus);
        break;
    case WIN_CHECKPOWERMODE1:
    case WIN_CHECKPOWERMODE2:
        s->error = 0;
        s->nsector = 0xff; /
        s->status = READY_STAT | SEEK_STAT;
        ide_set_irq(s->bus);
        break;
    case WIN_SETFEATURES:
        if (!s->bs)
            goto abort_cmd;
        /
        switch(s->feature) {
        case 0x02: /
            bdrv_set_enable_write_cache(s->bs, true);
            identify_data = (uint16_t *)s->identify_data;
            put_le16(identify_data + 85, (1 << 14) | (1 << 5) | 1);
            s->status = READY_STAT | SEEK_STAT;
            ide_set_irq(s->bus);
            break;
        case 0x82: /
            bdrv_set_enable_write_cache(s->bs, false);
            identify_data = (uint16_t *)s->identify_data;
            put_le16(identify_data + 85, (1 << 14) | 1);
            ide_flush_cache(s);
            break;
        case 0xcc: /
        case 0x66: /
        case 0xaa: /
        case 0x55: /
        case 0x05: /
        case 0x85: /
        case 0x69: /
        case 0x67: /
        case 0x96: /
        case 0x9a: /
        case 0x42: /
        case 0xc2: /
            s->status = READY_STAT | SEEK_STAT;
            ide_set_irq(s->bus);
            break;
        case 0x03: { /
		uint8_t val = s->nsector & 0x07;
		identify_data = (uint16_t *)s->identify_data;

		switch (s->nsector >> 3) {
		case 0x00: /
		case 0x01: /
			put_le16(identify_data + 62,0x07);
			put_le16(identify_data + 63,0x07);
			put_le16(identify_data + 88,0x3f);
			break;
                case 0x02: /
			put_le16(identify_data + 62,0x07 | (1 << (val + 8)));
			put_le16(identify_data + 63,0x07);
			put_le16(identify_data + 88,0x3f);
			break;
		case 0x04: /
			put_le16(identify_data + 62,0x07);
			put_le16(identify_data + 63,0x07 | (1 << (val + 8)));
			put_le16(identify_data + 88,0x3f);
			break;
		case 0x08: /
			put_le16(identify_data + 62,0x07);
			put_le16(identify_data + 63,0x07);
			put_le16(identify_data + 88,0x3f | (1 << (val + 8)));
			break;
		default:
			goto abort_cmd;
		}
            s->status = READY_STAT | SEEK_STAT;
            ide_set_irq(s->bus);
            break;
	}
        default:
            goto abort_cmd;
        }
        break;
    case WIN_FLUSH_CACHE:
    case WIN_FLUSH_CACHE_EXT:
        ide_flush_cache(s);
        break;
    case WIN_STANDBY:
    case WIN_STANDBY2:
    case WIN_STANDBYNOW1:
    case WIN_STANDBYNOW2:
    case WIN_IDLEIMMEDIATE:
    case WIN_IDLEIMMEDIATE2:
    case WIN_SETIDLE1:
    case WIN_SETIDLE2:
    case WIN_SLEEPNOW1:
    case WIN_SLEEPNOW2:
        s->status = READY_STAT;
        ide_set_irq(s->bus);
        break;
    case WIN_SEEK:
        /
        s->status = READY_STAT | SEEK_STAT;
        ide_set_irq(s->bus);
        break;
        /
    case WIN_PIDENTIFY:
        ide_atapi_identify(s);
        s->status = READY_STAT | SEEK_STAT;
        ide_transfer_start(s, s->io_buffer, 512, ide_transfer_stop);
        ide_set_irq(s->bus);
        break;
    case WIN_DIAGNOSE:
        ide_set_signature(s);
        if (s->drive_kind == IDE_CD)
            s->status = 0; /
        else
            s->status = READY_STAT | SEEK_STAT;
        s->error = 0x01; /
        ide_set_irq(s->bus);
        break;
    case WIN_DEVICE_RESET:
        ide_set_signature(s);
        s->status = 0x00; /
        s->error = 0x01;
        break;
    case WIN_PACKETCMD:
        /
        if (s->feature & 0x02)
            goto abort_cmd;
        s->status = READY_STAT | SEEK_STAT;
        s->atapi_dma = s->feature & 1;
        s->nsector = 1;
        ide_transfer_start(s, s->io_buffer, ATAPI_PACKET_SIZE,
                           ide_atapi_cmd);
        break;
    /
    case CFA_REQ_EXT_ERROR_CODE:
        s->error = 0x09;    /
        s->status = READY_STAT | SEEK_STAT;
        ide_set_irq(s->bus);
        break;
    case CFA_ERASE_SECTORS:
    case CFA_WEAR_LEVEL:
#if 0
    /       Windows 8 to work with AHCI */
    case WIN_SECURITY_FREEZE_LOCK:
#endif
        if (val == CFA_WEAR_LEVEL)
            s->nsector = 0;
        if (val == CFA_ERASE_SECTORS)
            s->media_changed = 1;
        s->error = 0x00;
        s->status = READY_STAT | SEEK_STAT;
        ide_set_irq(s->bus);
        break;
    case CFA_TRANSLATE_SECTOR:
        s->error = 0x00;
        s->status = READY_STAT | SEEK_STAT;
        memset(s->io_buffer, 0, 0x200);
        s->io_buffer[0x00] = s->hcyl;			/
        s->io_buffer[0x01] = s->lcyl;			/
        s->io_buffer[0x02] = s->select;			/
        s->io_buffer[0x03] = s->sector;			/
        s->io_buffer[0x04] = ide_get_sector(s) >> 16;	/
        s->io_buffer[0x05] = ide_get_sector(s) >> 8;	/
        s->io_buffer[0x06] = ide_get_sector(s) >> 0;	/
        s->io_buffer[0x13] = 0x00;				/
        s->io_buffer[0x18] = 0x00;				/
        s->io_buffer[0x19] = 0x00;				/
        s->io_buffer[0x1a] = 0x01;				/
        ide_transfer_start(s, s->io_buffer, 0x200, ide_transfer_stop);
        ide_set_irq(s->bus);
        break;
    case CFA_ACCESS_METADATA_STORAGE:
        switch (s->feature) {
        case 0x02:	/
            ide_cfata_metadata_inquiry(s);
            break;
        case 0x03:	/
            ide_cfata_metadata_read(s);
            break;
        case 0x04:	/
            ide_cfata_metadata_write(s);
            break;
        default:
            goto abort_cmd;
        }
        ide_transfer_start(s, s->io_buffer, 0x200, ide_transfer_stop);
        s->status = 0x00; /
        ide_set_irq(s->bus);
        break;
    case IBM_SENSE_CONDITION:
        switch (s->feature) {
        case 0x01:  /
            s->nsector = 0x50;      /
            break;
        default:
            goto abort_cmd;
        }
        s->status = READY_STAT | SEEK_STAT;
        ide_set_irq(s->bus);
        break;

    case WIN_SMART:
	if (s->hcyl != 0xc2 || s->lcyl != 0x4f)
		goto abort_cmd;
	if (!s->smart_enabled && s->feature != SMART_ENABLE)
		goto abort_cmd;
	switch (s->feature) {
	case SMART_DISABLE:
		s->smart_enabled = 0;
		s->status = READY_STAT | SEEK_STAT;
		ide_set_irq(s->bus);
		break;
	case SMART_ENABLE:
		s->smart_enabled = 1;
		s->status = READY_STAT | SEEK_STAT;
		ide_set_irq(s->bus);
		break;
	case SMART_ATTR_AUTOSAVE:
		switch (s->sector) {
		case 0x00:
		s->smart_autosave = 0;
		break;
		case 0xf1:
		s->smart_autosave = 1;
		break;
		default:
		goto abort_cmd;
		}
		s->status = READY_STAT | SEEK_STAT;
		ide_set_irq(s->bus);
		break;
	case SMART_STATUS:
		if (!s->smart_errors) {
		s->hcyl = 0xc2;
		s->lcyl = 0x4f;
		} else {
		s->hcyl = 0x2c;
		s->lcyl = 0xf4;
		}
		s->status = READY_STAT | SEEK_STAT;
		ide_set_irq(s->bus);
		break;
	case SMART_READ_THRESH:
		memset(s->io_buffer, 0, 0x200);
		s->io_buffer[0] = 0x01; /
		for (n=0; n<30; n++) {
		if (smart_attributes[n][0] == 0)
			break;
		s->io_buffer[2+0+(n*12)] = smart_attributes[n][0];
		s->io_buffer[2+1+(n*12)] = smart_attributes[n][11];
		}
		for (n=0; n<511; n++) /
		s->io_buffer[511] += s->io_buffer[n];
		s->io_buffer[511] = 0x100 - s->io_buffer[511];
		s->status = READY_STAT | SEEK_STAT;
		ide_transfer_start(s, s->io_buffer, 0x200, ide_transfer_stop);
		ide_set_irq(s->bus);
		break;
	case SMART_READ_DATA:
		memset(s->io_buffer, 0, 0x200);
		s->io_buffer[0] = 0x01; /
		for (n=0; n<30; n++) {
		    if (smart_attributes[n][0] == 0) {
			break;
		    }
		    int i;
		    for(i = 0; i < 11; i++) {
			s->io_buffer[2+i+(n*12)] = smart_attributes[n][i];
		    }
		}
		s->io_buffer[362] = 0x02 | (s->smart_autosave?0x80:0x00);
		if (s->smart_selftest_count == 0) {
		s->io_buffer[363] = 0;
		} else {
		s->io_buffer[363] =
			s->smart_selftest_data[3 + 
					   (s->smart_selftest_count - 1) *
					   24];
		}
		s->io_buffer[364] = 0x20; 
		s->io_buffer[365] = 0x01; 
		/
		s->io_buffer[367] = (1<<4 | 1<<3 | 1); 
		s->io_buffer[368] = 0x03; /
		s->io_buffer[369] = 0x00; /
		s->io_buffer[370] = 0x01; /
		s->io_buffer[372] = 0x02; /
		s->io_buffer[373] = 0x36; /
		s->io_buffer[374] = 0x01; /

		for (n=0; n<511; n++) 
		s->io_buffer[511] += s->io_buffer[n];
		s->io_buffer[511] = 0x100 - s->io_buffer[511];
		s->status = READY_STAT | SEEK_STAT;
		ide_transfer_start(s, s->io_buffer, 0x200, ide_transfer_stop);
		ide_set_irq(s->bus);
		break;
	case SMART_READ_LOG:
		switch (s->sector) {
		case 0x01: /
		memset(s->io_buffer, 0, 0x200);
		s->io_buffer[0] = 0x01;
		s->io_buffer[1] = 0x00; /
		s->io_buffer[452] = s->smart_errors & 0xff;
		s->io_buffer[453] = (s->smart_errors & 0xff00) >> 8;

		for (n=0; n<511; n++)
			s->io_buffer[511] += s->io_buffer[n];
		s->io_buffer[511] = 0x100 - s->io_buffer[511];
		break;
		case 0x06: /
		memset(s->io_buffer, 0, 0x200);
		s->io_buffer[0] = 0x01;
		if (s->smart_selftest_count == 0) {
			s->io_buffer[508] = 0;
		} else {
			s->io_buffer[508] = s->smart_selftest_count;
			for (n=2; n<506; n++) 
			s->io_buffer[n] = s->smart_selftest_data[n];
		}
		for (n=0; n<511; n++)
			s->io_buffer[511] += s->io_buffer[n];
		s->io_buffer[511] = 0x100 - s->io_buffer[511];
		break;
		default:
		goto abort_cmd;
		}
		s->status = READY_STAT | SEEK_STAT;
		ide_transfer_start(s, s->io_buffer, 0x200, ide_transfer_stop);
		ide_set_irq(s->bus);
		break;
	case SMART_EXECUTE_OFFLINE:
		switch (s->sector) {
		case 0: /
		case 1: /
		case 2: /
		s->smart_selftest_count++;
		if(s->smart_selftest_count > 21)
			s->smart_selftest_count = 0;
		n = 2 + (s->smart_selftest_count - 1) * 24;
		s->smart_selftest_data[n] = s->sector;
		s->smart_selftest_data[n+1] = 0x00; /
		s->smart_selftest_data[n+2] = 0x34; /
		s->smart_selftest_data[n+3] = 0x12; /
		s->status = READY_STAT | SEEK_STAT;
		ide_set_irq(s->bus);
		break;
		default:
		goto abort_cmd;
		}
		break;
	default:
		goto abort_cmd;
	}
	break;
    default:
        /
    abort_cmd:
        ide_abort_command(s);
        ide_set_irq(s->bus);
        break;
    }
}
