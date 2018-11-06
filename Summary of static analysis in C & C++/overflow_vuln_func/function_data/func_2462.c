static void omap_rtc_write(void *opaque, hwaddr addr,
                           uint64_t value, unsigned size)
{
    struct omap_rtc_s *s = (struct omap_rtc_s *) opaque;
    int offset = addr & OMAP_MPUI_REG_MASK;
    struct tm new_tm;
    time_t ti[2];

    if (size != 1) {
        return omap_badwidth_write8(opaque, addr, value);
    }

    switch (offset) {
    case 0x00:	/
#ifdef ALMDEBUG
        printf("RTC SEC_REG <-- %02x\n", value);
#endif
        s->ti -= s->current_tm.tm_sec;
        s->ti += from_bcd(value);
        return;

    case 0x04:	/
#ifdef ALMDEBUG
        printf("RTC MIN_REG <-- %02x\n", value);
#endif
        s->ti -= s->current_tm.tm_min * 60;
        s->ti += from_bcd(value) * 60;
        return;

    case 0x08:	/
#ifdef ALMDEBUG
        printf("RTC HRS_REG <-- %02x\n", value);
#endif
        s->ti -= s->current_tm.tm_hour * 3600;
        if (s->pm_am) {
            s->ti += (from_bcd(value & 0x3f) & 12) * 3600;
            s->ti += ((value >> 7) & 1) * 43200;
        } else
            s->ti += from_bcd(value & 0x3f) * 3600;
        return;

    case 0x0c:	/
#ifdef ALMDEBUG
        printf("RTC DAY_REG <-- %02x\n", value);
#endif
        s->ti -= s->current_tm.tm_mday * 86400;
        s->ti += from_bcd(value) * 86400;
        return;

    case 0x10:	/
#ifdef ALMDEBUG
        printf("RTC MTH_REG <-- %02x\n", value);
#endif
        memcpy(&new_tm, &s->current_tm, sizeof(new_tm));
        new_tm.tm_mon = from_bcd(value);
        ti[0] = mktimegm(&s->current_tm);
        ti[1] = mktimegm(&new_tm);

        if (ti[0] != -1 && ti[1] != -1) {
            s->ti -= ti[0];
            s->ti += ti[1];
        } else {
            /
            s->ti -= s->current_tm.tm_mon * 2592000;
            s->ti += from_bcd(value) * 2592000;
        }
        return;

    case 0x14:	/
#ifdef ALMDEBUG
        printf("RTC YRS_REG <-- %02x\n", value);
#endif
        memcpy(&new_tm, &s->current_tm, sizeof(new_tm));
        new_tm.tm_year += from_bcd(value) - (new_tm.tm_year % 100);
        ti[0] = mktimegm(&s->current_tm);
        ti[1] = mktimegm(&new_tm);

        if (ti[0] != -1 && ti[1] != -1) {
            s->ti -= ti[0];
            s->ti += ti[1];
        } else {
            /
            s->ti -= (s->current_tm.tm_year % 100) * 31536000;
            s->ti += from_bcd(value) * 31536000;
        }
        return;

    case 0x18:	/
        return;	/

    case 0x20:	/
#ifdef ALMDEBUG
        printf("ALM SEC_REG <-- %02x\n", value);
#endif
        s->alarm_tm.tm_sec = from_bcd(value);
        omap_rtc_alarm_update(s);
        return;

    case 0x24:	/
#ifdef ALMDEBUG
        printf("ALM MIN_REG <-- %02x\n", value);
#endif
        s->alarm_tm.tm_min = from_bcd(value);
        omap_rtc_alarm_update(s);
        return;

    case 0x28:	/
#ifdef ALMDEBUG
        printf("ALM HRS_REG <-- %02x\n", value);
#endif
        if (s->pm_am)
            s->alarm_tm.tm_hour =
                    ((from_bcd(value & 0x3f)) % 12) +
                    ((value >> 7) & 1) * 12;
        else
            s->alarm_tm.tm_hour = from_bcd(value);
        omap_rtc_alarm_update(s);
        return;

    case 0x2c:	/
#ifdef ALMDEBUG
        printf("ALM DAY_REG <-- %02x\n", value);
#endif
        s->alarm_tm.tm_mday = from_bcd(value);
        omap_rtc_alarm_update(s);
        return;

    case 0x30:	/
#ifdef ALMDEBUG
        printf("ALM MON_REG <-- %02x\n", value);
#endif
        s->alarm_tm.tm_mon = from_bcd(value);
        omap_rtc_alarm_update(s);
        return;

    case 0x34:	/
#ifdef ALMDEBUG
        printf("ALM YRS_REG <-- %02x\n", value);
#endif
        s->alarm_tm.tm_year = from_bcd(value);
        omap_rtc_alarm_update(s);
        return;

    case 0x40:	/
#ifdef ALMDEBUG
        printf("RTC CONTROL <-- %02x\n", value);
#endif
        s->pm_am = (value >> 3) & 1;
        s->auto_comp = (value >> 2) & 1;
        s->round = (value >> 1) & 1;
        s->running = value & 1;
        s->status &= 0xfd;
        s->status |= s->running << 1;
        return;

    case 0x44:	/
#ifdef ALMDEBUG
        printf("RTC STATUSL <-- %02x\n", value);
#endif
        s->status &= ~((value & 0xc0) ^ 0x80);
        omap_rtc_interrupts_update(s);
        return;

    case 0x48:	/
#ifdef ALMDEBUG
        printf("RTC INTRS <-- %02x\n", value);
#endif
        s->interrupts = value;
        return;

    case 0x4c:	/
#ifdef ALMDEBUG
        printf("RTC COMPLSB <-- %02x\n", value);
#endif
        s->comp_reg &= 0xff00;
        s->comp_reg |= 0x00ff & value;
        return;

    case 0x50:	/
#ifdef ALMDEBUG
        printf("RTC COMPMSB <-- %02x\n", value);
#endif
        s->comp_reg &= 0x00ff;
        s->comp_reg |= 0xff00 & (value << 8);
        return;

    default:
        OMAP_BAD_REG(addr);
        return;
    }
}
