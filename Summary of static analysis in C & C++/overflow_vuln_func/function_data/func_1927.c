void nand_setio(DeviceState *dev, uint32_t value)
{
    int i;
    NANDFlashState *s = NAND(dev);

    if (!s->ce && s->cle) {
        if (nand_flash_ids[s->chip_id].options & NAND_SAMSUNG_LP) {
            if (s->cmd == NAND_CMD_READ0 && value == NAND_CMD_LPREAD2)
                return;
            if (value == NAND_CMD_RANDOMREAD1) {
                s->addr &= ~((1 << s->addr_shift) - 1);
                s->addrlen = 0;
                return;
            }
        }
        if (value == NAND_CMD_READ0) {
            s->offset = 0;
        } else if (value == NAND_CMD_READ1) {
            s->offset = 0x100;
            value = NAND_CMD_READ0;
        } else if (value == NAND_CMD_READ2) {
            s->offset = 1 << s->page_shift;
            value = NAND_CMD_READ0;
        }

        s->cmd = value;

        if (s->cmd == NAND_CMD_READSTATUS ||
                s->cmd == NAND_CMD_PAGEPROGRAM2 ||
                s->cmd == NAND_CMD_BLOCKERASE1 ||
                s->cmd == NAND_CMD_BLOCKERASE2 ||
                s->cmd == NAND_CMD_NOSERIALREAD2 ||
                s->cmd == NAND_CMD_RANDOMREAD2 ||
                s->cmd == NAND_CMD_RESET) {
            nand_command(s);
        }

        if (s->cmd != NAND_CMD_RANDOMREAD2) {
            s->addrlen = 0;
        }
    }

    if (s->ale) {
        unsigned int shift = s->addrlen * 8;
        unsigned int mask = ~(0xff << shift);
        unsigned int v = value << shift;

        s->addr = (s->addr & mask) | v;
        s->addrlen ++;

        switch (s->addrlen) {
        case 1:
            if (s->cmd == NAND_CMD_READID) {
                nand_command(s);
            }
            break;
        case 2: /
            s->addr <<= (s->buswidth - 1);
            break;
        case 3:
            if (!(nand_flash_ids[s->chip_id].options & NAND_SAMSUNG_LP) &&
                    (s->cmd == NAND_CMD_READ0 ||
                     s->cmd == NAND_CMD_PAGEPROGRAM1)) {
                nand_command(s);
            }
            break;
        case 4:
            if ((nand_flash_ids[s->chip_id].options & NAND_SAMSUNG_LP) &&
                    nand_flash_ids[s->chip_id].size < 256 && /
                    (s->cmd == NAND_CMD_READ0 ||
                     s->cmd == NAND_CMD_PAGEPROGRAM1)) {
                nand_command(s);
            }
            break;
        case 5:
            if ((nand_flash_ids[s->chip_id].options & NAND_SAMSUNG_LP) &&
                    nand_flash_ids[s->chip_id].size >= 256 && /
                    (s->cmd == NAND_CMD_READ0 ||
                     s->cmd == NAND_CMD_PAGEPROGRAM1)) {
                nand_command(s);
            }
            break;
        default:
            break;
        }
    }

    if (!s->cle && !s->ale && s->cmd == NAND_CMD_PAGEPROGRAM1) {
        if (s->iolen < (1 << s->page_shift) + (1 << s->oob_shift)) {
            for (i = s->buswidth; i--; value >>= 8) {
                s->io[s->iolen ++] = (uint8_t) (value & 0xff);
            }
        }
    } else if (!s->cle && !s->ale && s->cmd == NAND_CMD_COPYBACKPRG1) {
        if ((s->addr & ((1 << s->addr_shift) - 1)) <
                (1 << s->page_shift) + (1 << s->oob_shift)) {
            for (i = s->buswidth; i--; s->addr++, value >>= 8) {
                s->io[s->iolen + (s->addr & ((1 << s->addr_shift) - 1))] =
                    (uint8_t) (value & 0xff);
            }
        }
    }
}
