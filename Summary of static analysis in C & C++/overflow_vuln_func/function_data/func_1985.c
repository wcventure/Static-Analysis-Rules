static int wm8750_tx(I2CSlave *i2c, uint8_t data)
{
    WM8750State *s = (WM8750State *) i2c;
    uint8_t cmd;
    uint16_t value;

    if (s->i2c_len >= 2) {
        printf("%s: long message (%i bytes)\n", __FUNCTION__, s->i2c_len);
#ifdef VERBOSE
        return 1;
#endif
    }
    s->i2c_data[s->i2c_len ++] = data;
    if (s->i2c_len != 2)
        return 0;

    cmd = s->i2c_data[0] >> 1;
    value = ((s->i2c_data[0] << 8) | s->i2c_data[1]) & 0x1ff;

    switch (cmd) {
    case WM8750_LADCIN:	/
        s->diff[0] = (((value >> 6) & 3) == 3);	/
        if (s->diff[0])
            s->in[0] = &s->adc_voice[0 + s->ds * 1];
        else
            s->in[0] = &s->adc_voice[((value >> 6) & 3) * 1 + 0];
        break;

    case WM8750_RADCIN:	/
        s->diff[1] = (((value >> 6) & 3) == 3);	/
        if (s->diff[1])
            s->in[1] = &s->adc_voice[0 + s->ds * 1];
        else
            s->in[1] = &s->adc_voice[((value >> 6) & 3) * 1 + 0];
        break;

    case WM8750_ADCIN:	/
        s->ds = (value >> 8) & 1;	/
        if (s->diff[0])
            s->in[0] = &s->adc_voice[0 + s->ds * 1];
        if (s->diff[1])
            s->in[1] = &s->adc_voice[0 + s->ds * 1];
        s->monomix[0] = (value >> 6) & 3;	/
        break;

    case WM8750_ADCTL1:	/
        s->monomix[1] = (value >> 1) & 1;	/
        break;

    case WM8750_PWR1:	/
        s->enable = ((value >> 6) & 7) == 3;	/
        wm8750_set_format(s);
        break;

    case WM8750_LINVOL:	/
        s->invol[0] = value & 0x3f;		/
        s->inmute[0] = (value >> 7) & 1;	/
        wm8750_vol_update(s);
        break;

    case WM8750_RINVOL:	/
        s->invol[1] = value & 0x3f;		/
        s->inmute[1] = (value >> 7) & 1;	/
        wm8750_vol_update(s);
        break;

    case WM8750_ADCDAC:	/
        s->pol = (value >> 5) & 3;		/
        s->mute = (value >> 3) & 1;		/
        wm8750_vol_update(s);
        break;

    case WM8750_ADCTL3:	/
        break;

    case WM8750_LADC:	/
        s->invol[2] = value & 0xff;		/
        wm8750_vol_update(s);
        break;

    case WM8750_RADC:	/
        s->invol[3] = value & 0xff;		/
        wm8750_vol_update(s);
        break;

    case WM8750_ALC1:	/
        s->alc = (value >> 7) & 3;		/
        break;

    case WM8750_NGATE:	/
    case WM8750_3D:	/
        break;

    case WM8750_LDAC:	/
        s->outvol[0] = value & 0xff;		/
        wm8750_vol_update(s);
        break;

    case WM8750_RDAC:	/
        s->outvol[1] = value & 0xff;		/
        wm8750_vol_update(s);
        break;

    case WM8750_BASS:	/
        break;

    case WM8750_LOUTM1:	/
        s->path[0] = (value >> 8) & 1;		/
        /
        wm8750_vol_update(s);
        break;

    case WM8750_LOUTM2:	/
        s->path[1] = (value >> 8) & 1;		/
        /
        wm8750_vol_update(s);
        break;

    case WM8750_ROUTM1:	/
        s->path[2] = (value >> 8) & 1;		/
        /
        wm8750_vol_update(s);
        break;

    case WM8750_ROUTM2:	/
        s->path[3] = (value >> 8) & 1;		/
        /
        wm8750_vol_update(s);
        break;

    case WM8750_MOUTM1:	/
        s->mpath[0] = (value >> 8) & 1;		/
        /
        wm8750_vol_update(s);
        break;

    case WM8750_MOUTM2:	/
        s->mpath[1] = (value >> 8) & 1;		/
        /
        wm8750_vol_update(s);
        break;

    case WM8750_LOUT1V:	/
        s->outvol[2] = value & 0x7f;		/
        wm8750_vol_update(s);
        break;

    case WM8750_LOUT2V:	/
        s->outvol[4] = value & 0x7f;		/
        wm8750_vol_update(s);
        break;

    case WM8750_ROUT1V:	/
        s->outvol[3] = value & 0x7f;		/
        wm8750_vol_update(s);
        break;

    case WM8750_ROUT2V:	/
        s->outvol[5] = value & 0x7f;		/
        wm8750_vol_update(s);
        break;

    case WM8750_MOUTV:	/
        s->outvol[6] = value & 0x7f;		/
        wm8750_vol_update(s);
        break;

    case WM8750_ADCTL2:	/
        break;

    case WM8750_PWR2:	/
        s->power = value & 0x7e;
        /
        wm8750_vol_update(s);
        break;

    case WM8750_IFACE:	/
        s->format = value;
        s->master = (value >> 6) & 1;			/
        wm8750_clk_update(s, s->master);
        break;

    case WM8750_SRATE:	/
        s->rate = &wm_rate_table[(value >> 1) & 0x1f];
        wm8750_clk_update(s, 0);
        break;

    case WM8750_RESET:	/
        wm8750_reset(&s->i2c);
        break;

#ifdef VERBOSE
    default:
        printf("%s: unknown register %02x\n", __FUNCTION__, cmd);
#endif
    }

    return 0;
}
