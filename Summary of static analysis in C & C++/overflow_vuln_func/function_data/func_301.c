static wtap_open_return_val
mp2t_bits_per_second(wtap *wth, guint32 first, guint8 trailer_len,
        guint64 *bitrate, int *err, gchar **err_info)
{
    guint32 pn1, pn2;
    guint64 pcr1, pcr2;
    guint16 pid1, pid2;
    guint32 idx;
    guint64 pcr_delta, bits_passed;

    /
    idx = first;

    if (!mp2t_find_next_pcr(wth, trailer_len, err, err_info, &idx, &pcr1, &pid1)) {
        /
        if (*err == WTAP_ERR_SHORT_READ)
            return WTAP_OPEN_NOT_MINE;    /
        if (*err != 0)
            return WTAP_OPEN_ERROR;

        /
        *bitrate = MP2T_QAM64_BITRATE;
        return WTAP_OPEN_MINE;
    }

    pn1 = idx;
    pn2 = pn1;

    while (pn1 == pn2) {
        if (!mp2t_find_next_pcr(wth, trailer_len, err, err_info, &idx, &pcr2, &pid2)) {
            /
            if (*err == WTAP_ERR_SHORT_READ)
                return WTAP_OPEN_NOT_MINE;    /
            if (*err != 0)
                return WTAP_OPEN_ERROR;

            /
            *bitrate = MP2T_QAM64_BITRATE;
            return WTAP_OPEN_MINE;
        }

        if (pid1 == pid2) {
            pn2 = idx;
        }
    }

    if (pcr2 <= pcr1) {
        /
        return WTAP_OPEN_NOT_MINE;
    }
    pcr_delta = pcr2 - pcr1;
    bits_passed = MP2T_SIZE * (pn2 - pn1) * 8;

    *bitrate = ((MP2T_PCR_CLOCK * bits_passed) / pcr_delta);
    if (*bitrate == 0) {
        /
        return WTAP_OPEN_ERROR;
    }
    return WTAP_OPEN_MINE;
}
