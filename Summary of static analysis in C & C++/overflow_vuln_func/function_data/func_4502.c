static int decrypt_init(AVFormatContext *s, ID3v2ExtraMeta *em, uint8_t *header)
{
    OMAContext *oc = s->priv_data;
    ID3v2ExtraMetaGEOB *geob = NULL;
    uint8_t *gdata;

    oc->encrypted = 1;
    av_log(s, AV_LOG_INFO, "File is encrypted\n");

    /
    while (em) {
        if (!strcmp(em->tag, "GEOB") &&
            (geob = em->data) &&
            (!strcmp(geob->description, "OMG_LSI") ||
             !strcmp(geob->description, "OMG_BKLSI"))) {
            break;
        }
        em = em->next;
    }
    if (!em) {
        av_log(s, AV_LOG_ERROR, "No encryption header found\n");
        return -1;
    }

    if (geob->datasize < 64) {
        av_log(s, AV_LOG_ERROR, "Invalid GEOB data size: %u\n", geob->datasize);
        return -1;
    }

    gdata = geob->data;

    if (AV_RB16(gdata) != 1)
        av_log(s, AV_LOG_WARNING, "Unknown version in encryption header\n");

    oc->k_size = AV_RB16(&gdata[2]);
    oc->e_size = AV_RB16(&gdata[4]);
    oc->i_size = AV_RB16(&gdata[6]);
    oc->s_size = AV_RB16(&gdata[8]);

    if (memcmp(&gdata[OMA_ENC_HEADER_SIZE], "KEYRING     ", 12)) {
        av_log(s, AV_LOG_ERROR, "Invalid encryption header\n");
        return -1;
    }
    oc->rid = AV_RB32(&gdata[OMA_ENC_HEADER_SIZE + 28]);
    av_log(s, AV_LOG_DEBUG, "RID: %.8x\n", oc->rid);

    memcpy(oc->iv, &header[0x58], 8);
    hex_log(s, AV_LOG_DEBUG, "IV", oc->iv, 8);

    hex_log(s, AV_LOG_DEBUG, "CBC-MAC", &gdata[OMA_ENC_HEADER_SIZE+oc->k_size+oc->e_size+oc->i_size], 8);

    if (s->keylen > 0) {
        kset(s, s->key, s->key, s->keylen);
    }
    if (!memcmp(oc->r_val, (const uint8_t[8]){0}, 8) ||
        rprobe(s, gdata, oc->r_val) < 0 &&
        nprobe(s, gdata, oc->n_val) < 0) {
        int i;
        for (i = 0; i < FF_ARRAY_ELEMS(leaf_table); i += 2) {
            uint8_t buf[16];
            AV_WL64(buf, leaf_table[i]);
            AV_WL64(&buf[8], leaf_table[i+1]);
            kset(s, buf, buf, 16);
            if (!rprobe(s, gdata, oc->r_val) || !nprobe(s, gdata, oc->n_val))
                break;
        }
        if (i >= sizeof(leaf_table)) {
            av_log(s, AV_LOG_ERROR, "Invalid key\n");
            return -1;
        }
    }

    /
    av_des_init(&oc->av_des, oc->m_val, 64, 0);
    av_des_crypt(&oc->av_des, oc->e_val, &gdata[OMA_ENC_HEADER_SIZE + 40], 1, NULL, 0);
    hex_log(s, AV_LOG_DEBUG, "EK", oc->e_val, 8);

    /
    av_des_init(&oc->av_des, oc->e_val, 64, 1);

    return 0;
}
