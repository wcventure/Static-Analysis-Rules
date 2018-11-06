static int nprobe(AVFormatContext *s, uint8_t *enc_header, const uint8_t *n_val)
{
    OMAContext *oc = s->priv_data;
    uint32_t pos, taglen, datalen;
    struct AVDES av_des;

    if (!enc_header || !n_val)
        return -1;

    pos = OMA_ENC_HEADER_SIZE + oc->k_size;
    if (!memcmp(&enc_header[pos], "EKB ", 4))
        pos += 32;

    if (AV_RB32(&enc_header[pos]) != oc->rid)
        av_log(s, AV_LOG_DEBUG, "Mismatching RID\n");

    taglen = AV_RB32(&enc_header[pos+32]);
    datalen = AV_RB32(&enc_header[pos+36]) >> 4;

    pos += 44 + taglen;

    av_des_init(&av_des, n_val, 192, 1);
    while (datalen-- > 0) {
        av_des_crypt(&av_des, oc->r_val, &enc_header[pos], 2, NULL, 1);
        kset(s, oc->r_val, NULL, 16);
        if (!rprobe(s, enc_header, oc->r_val))
            return 0;
        pos += 16;
    }

    return -1;
}
