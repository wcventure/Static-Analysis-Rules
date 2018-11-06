static gboolean vwr_read_s3_W_rec(vwr_t *vwr, struct wtap_pkthdr *phdr,
                                  Buffer *buf, const guint8 *rec, int rec_size,
                                  int IS_TX, int log_mode, int *err,
                                  gchar **err_info)
{
    guint8           *data_ptr;
    int              bytes_written = 0;                  /
    int              i;
    int              stats_offset = 0;
    const guint8     *s_start_ptr = NULL,*s_trail_ptr = NULL, *plcp_ptr, *m_ptr; /
    guint32          msdu_length = 0, actual_octets = 0; /
    guint8           l1p_1 = 0,l1p_2 = 0, plcp_type, rate_mcs_index, nss;   /
    guint64          s_time = LL_ZERO, e_time = LL_ZERO; /
                                                         /
    guint64          latency = LL_ZERO;
    guint64          start_time = 0, s_sec = 0, s_usec = LL_ZERO; /
    guint64          end_time = 0;                                /
    guint16          info = 0;                           /
    guint32          errors = 0;
    gint8            info_2nd = 0,rssi[] = {0,0,0,0};    /
    int              frame_size;
    guint32          d_time = 0, flow_id = 0;            /
    int              sig_off, pay_off;                   /
    guint64          sig_ts = 0, tsid;                   /
    guint64          delta_b;                            /
    guint8           L1InfoC,port_type,ver_fpga = 0;
    guint8           flow_seq =0,plcp_hdr_flag = 0,rf_id = 0;    /
    const guint8    *rf_ptr = NULL;
    float            rate;
    guint16          phyRate;

    /
    if (IS_TX == 3) {       /
        if ((guint)rec_size < OCTO_MODIFIED_RF_LEN) {
            *err_info = g_strdup_printf("vwr: Invalid record length %d (must be at least %u)",
                                        rec_size,
                                        OCTO_MODIFIED_RF_LEN);
            *err = WTAP_ERR_BAD_FILE;
            return FALSE;
        }
        rf_ptr = &(rec[0]);
        rf_id = rf_ptr[0];

        /
        phdr->len = OCTO_MODIFIED_RF_LEN + 1;       /
        phdr->caplen = OCTO_MODIFIED_RF_LEN + 1;

        phdr->ts.secs   = (time_t)s_sec;
        phdr->ts.nsecs  = (int)(s_usec * 1000);
        phdr->pkt_encap = WTAP_ENCAP_IXVERIWAVE;

        phdr->rec_type = REC_TYPE_PACKET;
        phdr->presence_flags = WTAP_HAS_TS;

        ws_buffer_assure_space(buf, phdr->caplen);
        data_ptr = ws_buffer_start_ptr(buf);

        port_type = IS_TX << 4;

        nss = 0;
        phyRate = 0;
    }
    else {
        /
        /
        /
        if (IS_TX == 4)     /
        {
            stats_offset = OCTO_RF_MOD_ACTUAL_LEN;
            if ((guint)rec_size < stats_offset + vwr->MPDU_OFF + vVW510021_W_STATS_TRAILER_LEN) {
                *err_info = g_strdup_printf("vwr: Invalid record length %d (must be at least %u)",
                                            rec_size,
                                            stats_offset + vwr->MPDU_OFF + vVW510021_W_STATS_TRAILER_LEN);
                *err = WTAP_ERR_BAD_FILE;
                return FALSE;
            }
            rf_ptr = &(rec[0]);
            rf_id = rf_ptr[0];
        }
        else
        {
            stats_offset = 0;
            if ((guint)rec_size < vwr->MPDU_OFF + vVW510021_W_STATS_TRAILER_LEN) {
                *err_info = g_strdup_printf("vwr: Invalid record length %d (must be at least %u)",
                                            rec_size,
                                            vwr->MPDU_OFF + vVW510021_W_STATS_TRAILER_LEN);
                *err = WTAP_ERR_BAD_FILE;
                return FALSE;
            }
        }

        s_start_ptr = &(rec[stats_offset]);         /
        s_trail_ptr = &(rec[rec_size - vVW510021_W_STATS_TRAILER_LEN] );      /

        l1p_1 = s_start_ptr[vVW510021_W_L1P_1_OFF];
        l1p_2 = s_start_ptr[vVW510021_W_L1P_2_OFF];

        plcp_type = vVW510021_W_S3_PLCP_TYPE(l1p_2);
        switch (plcp_type)
        {
        case vVW510021_W_PLCP_LEGACY:
            /
            rate_mcs_index = vVW510021_W_S3_RATE_INDEX(l1p_1);
            nss = 0;
            break;

        case vVW510021_W_PLCP_MIXED:
        case vVW510021_W_PLCP_GREENFIELD:
            rate_mcs_index = vVW510021_W_S3_MCS_INDEX_HT(l1p_1);
            nss = (rate_mcs_index < MAX_HT_MCS) ? nss_for_mcs[rate_mcs_index] : 0;
            break;

        case vVW510021_W_PLCP_VHT_MIXED:
            rate_mcs_index = vVW510021_W_S3_MCS_INDEX_VHT(l1p_1);
            nss = vVW510021_W_S3_NSS_VHT(l1p_1);
            plcp_hdr_flag = 1;
            break;

        default:
            rate_mcs_index = 0;
            nss = 0;
            plcp_hdr_flag = 0;
            break;
        }

        for (i = 0; i < 4; i++)
        {
            if (IS_TX == 1)
            {
                rssi[i] = (s_start_ptr[4+i] & 0x80) ? -1 * (s_start_ptr[4+i] & 0x7f) : s_start_ptr[4+i] & 0x7f;
            }
            else
            {
                rssi[i] = (s_start_ptr[4+i] >= 128) ? (s_start_ptr[4+i] - 256) : s_start_ptr[4+i];
            }
        }

        if (IS_TX == 0 || IS_TX == 4){
            L1InfoC = s_start_ptr[8];
        }

        msdu_length = pntoh24(&s_start_ptr[9]);

        /
        plcp_ptr = &(rec[stats_offset+16]);

        /
        if (log_mode == 3) {
            frame_size = rec_size - (stats_offset + vwr->MPDU_OFF + vVW510021_W_STATS_TRAILER_LEN);
            if (frame_size > ((int) msdu_length))
                actual_octets = msdu_length;
            else {
                /
                actual_octets = frame_size;
            }
        }
        else
        {
            actual_octets = msdu_length;
        }
        /
        if (actual_octets > rec_size - (stats_offset + vwr->MPDU_OFF + vVW510021_W_STATS_TRAILER_LEN)) {
            *err_info = g_strdup_printf("vwr: Invalid data length %u (runs past the end of the record)",
                                        actual_octets);
            *err = WTAP_ERR_BAD_FILE;
            return FALSE;
        }

        flow_seq = s_trail_ptr[vVW510021_W_FLOWSEQ_OFF];

        latency = 0x00000000;                        /
        flow_id = pntoh24(&s_trail_ptr[vVW510021_W_FLOWID_OFF]);         /
        /
        /
        tsid = pcorey48tohll(&s_trail_ptr[vVW510021_W_LATVAL_OFF]);

        errors = pntoh32(&s_trail_ptr[vVW510021_W_ERRORS_OFF]);
        info = pntoh16(&s_trail_ptr[vVW510021_W_INFO_OFF]);

        if (IS_TX == 0 || IS_TX == 4)
            info_2nd = s_trail_ptr[41];

        /
        switch (plcp_type)
        {
        case vVW510021_W_PLCP_LEGACY:
            rate = get_legacy_rate(rate_mcs_index);
            break;

        case vVW510021_W_PLCP_MIXED:
            /
            {
                /
                guint16 radioflags = FLAGS_CHAN_HT | ((plcp_ptr[3] & 0x80) ? FLAGS_CHAN_40MHZ : 0) |
                                   ((l1p_1 & vVW510021_W_IS_LONGGI) ? 0 : FLAGS_CHAN_SHORTGI);
                rate = get_ht_rate(rate_mcs_index, radioflags);
            }
            break;

        case vVW510021_W_PLCP_GREENFIELD:
            /
            {
                /
                guint16 radioflags = FLAGS_CHAN_HT | ((plcp_ptr[0] & 0x80) ? FLAGS_CHAN_40MHZ : 0) |
                                   ((l1p_1 & vVW510021_W_IS_LONGGI) ?  0 : FLAGS_CHAN_SHORTGI);
                rate = get_ht_rate(rate_mcs_index, radioflags);
            }
            break;

        case vVW510021_W_PLCP_VHT_MIXED:
            /
            {
                guint8 SBW = vVW510021_W_BANDWIDTH_VHT(l1p_2);
                guint16 radioflags = FLAGS_CHAN_VHT | ((l1p_1 & vVW510021_W_IS_LONGGI) ?  0 : FLAGS_CHAN_SHORTGI);
                if (SBW == 3)
                    radioflags |= FLAGS_CHAN_40MHZ;
                else if (SBW == 4)
                    radioflags |= FLAGS_CHAN_80MHZ;
                rate = get_vht_rate(rate_mcs_index, radioflags, nss);
            }
            break;

        default:
            rate = 0.0f;
            break;
        }
        phyRate = (guint16)(rate * 10);
        /

        /
        if (log_mode == 3) {
            if (frame_size >= (int) msdu_length) {
                /
                if (actual_octets < 4) {
                    if (actual_octets != 0) {
                        *err_info = g_strdup_printf("vwr: Invalid data length %u (too short to include 4 bytes of FCS)",
                                                    actual_octets);
                        *err = WTAP_ERR_BAD_FILE;
                        return FALSE;
                    }
                } else {
                    actual_octets -= 4;
                }
            }
            ver_fpga = 0x11;
        } else {
            ver_fpga = 0x01;
        }

        /
        /
        s_time = pcoreytohll(&s_trail_ptr[vVW510021_W_STARTT_OFF]);
        e_time = pcoreytohll(&s_trail_ptr[vVW510021_W_ENDT_OFF]);

        /
        d_time = (guint32)((e_time - s_time) / NS_IN_US);  /

        /
        start_time = s_time / NS_IN_US;                     /
        s_sec = (start_time / US_IN_SEC);                   /
        s_usec = start_time - (s_sec * US_IN_SEC);          /

        /
        end_time = e_time / NS_IN_US;                       /

        /
        m_ptr = &(rec[stats_offset+8+12]);
        pay_off = 42;         /
        sig_off = find_signature(m_ptr, rec_size - 20, pay_off, flow_id, flow_seq);
        if ((m_ptr[sig_off] == 0xdd) && (sig_off + 15 <= (rec_size - vVW510021_W_STATS_TRAILER_LEN)))
            sig_ts = get_signature_ts(m_ptr, sig_off);
        else
            sig_ts = 0;

        /
        if (IS_TX == 0 || IS_TX == 4) {
            if (tsid < s_time) {
                latency = s_time - tsid;
            } else {
                /
                /
                delta_b = tsid - s_time;
                if (delta_b >  0x10000000)
                    latency = 0;
                else
                    latency = delta_b;
            }
        }

        port_type = IS_TX << 4;

        /
        if (IS_TX == 4) {
            phdr->len = OCTO_MODIFIED_RF_LEN + OCTO_TIMESTAMP_FIELDS_LEN + OCTO_LAYER1TO4_LEN + actual_octets;
            phdr->caplen = OCTO_MODIFIED_RF_LEN + OCTO_TIMESTAMP_FIELDS_LEN + OCTO_LAYER1TO4_LEN + actual_octets;
        } else {
            phdr->len = OCTO_TIMESTAMP_FIELDS_LEN + OCTO_LAYER1TO4_LEN + actual_octets;
            phdr->caplen = OCTO_TIMESTAMP_FIELDS_LEN + OCTO_LAYER1TO4_LEN + actual_octets;
        }
        if (phdr->caplen > WTAP_MAX_PACKET_SIZE_STANDARD) {
            /
            *err_info = g_strdup_printf("vwr: File has %u-byte packet, bigger than maximum of %u",
                                        phdr->caplen, WTAP_MAX_PACKET_SIZE_STANDARD);
            *err = WTAP_ERR_BAD_FILE;
            return FALSE;
        }

        phdr->ts.secs   = (time_t)s_sec;
        phdr->ts.nsecs  = (int)(s_usec * 1000);
        phdr->pkt_encap = WTAP_ENCAP_IXVERIWAVE;

        phdr->rec_type = REC_TYPE_PACKET;
        phdr->presence_flags = WTAP_HAS_TS;

        ws_buffer_assure_space(buf, phdr->caplen);
        data_ptr = ws_buffer_start_ptr(buf);
    }

    /
    /

    /
    phtole8(&data_ptr[bytes_written], port_type);
    bytes_written += 1;

    if (IS_TX != 3) {
        phtole8(&data_ptr[bytes_written], ver_fpga); /
        bytes_written += 1;

        phtoles(&data_ptr[bytes_written], OCTO_TIMESTAMP_FIELDS_LEN); /
        bytes_written += 2;

    /
        if (IS_TX == 1 && sig_ts != 0) {
            phtolel(&data_ptr[bytes_written], latency);
        } else {
            phtolel(&data_ptr[bytes_written], 0);
        }
        bytes_written += 4;
        phtolel(&data_ptr[bytes_written], sig_ts); /
        bytes_written += 4;
        phtolell(&data_ptr[bytes_written], start_time); /
        bytes_written += 8;
        phtolell(&data_ptr[bytes_written], end_time);
        bytes_written += 8;
        phtolel(&data_ptr[bytes_written], d_time);
        bytes_written += 4;
    /
    }

    /
    if (IS_TX == 3 || IS_TX == 4) {
        phtole8(&data_ptr[bytes_written], rf_id);
        bytes_written += 1;
        data_ptr[bytes_written] = 0;
        bytes_written += 1;
        data_ptr[bytes_written] = 0;
        bytes_written += 1;
        data_ptr[bytes_written] = 0;
        bytes_written += 1;

        /
        for (i = 0; i < RF_NUMBER_OF_PORTS; i++)
        {
            if (pntoh16(&rf_ptr[RF_PORT_1_NOISE_OFF+i*RF_INTER_PORT_GAP_OFF]) == 0) {
                phtoles(&data_ptr[bytes_written], 0);
                bytes_written += 2;
            } else {
                data_ptr[bytes_written] = rf_ptr[RF_PORT_1_NOISE_OFF+i*RF_INTER_PORT_GAP_OFF];
                bytes_written += 1;
                data_ptr[bytes_written] = rf_ptr[RF_PORT_1_NOISE_OFF+1+i*RF_INTER_PORT_GAP_OFF];
                bytes_written += 1;
            }
        }

        /
        for (i = 0; i < RF_NUMBER_OF_PORTS; i++)
        {
            if (pntoh16(&rf_ptr[RF_PORT_1_SNR_OFF+i*RF_INTER_PORT_GAP_OFF]) == 0) {
                phtoles(&data_ptr[bytes_written], 0);
                bytes_written += 2;
            } else {
                data_ptr[bytes_written] = rf_ptr[RF_PORT_1_SNR_OFF+i*RF_INTER_PORT_GAP_OFF];
                bytes_written += 1;
                data_ptr[bytes_written] = rf_ptr[RF_PORT_1_SNR_OFF+1+i*RF_INTER_PORT_GAP_OFF];
                bytes_written += 1;
            }
        }

        /
        for (i = 0; i < RF_NUMBER_OF_PORTS; i++)
        {
            if (pntoh16(&rf_ptr[RF_PORT_1_PFE_OFF+i*RF_INTER_PORT_GAP_OFF]) == 0) {
                phtoles(&data_ptr[bytes_written], 0);
                bytes_written += 2;
            } else {
                data_ptr[bytes_written] = rf_ptr[RF_PORT_1_PFE_OFF+i*RF_INTER_PORT_GAP_OFF];
                bytes_written += 1;
                data_ptr[bytes_written] = rf_ptr[RF_PORT_1_PFE_OFF+1+i*RF_INTER_PORT_GAP_OFF];
                bytes_written += 1;
            }
        }

        /
        for (i = 0; i < RF_NUMBER_OF_PORTS; i++)
        {
            if (pntoh16(&rf_ptr[RF_PORT_1_EVM_SD_SIG_OFF+i*RF_INTER_PORT_GAP_OFF]) == 0) {
                phtoles(&data_ptr[bytes_written], 0);
                bytes_written += 2;
            } else {
                data_ptr[bytes_written] = rf_ptr[RF_PORT_1_EVM_SD_SIG_OFF+i*RF_INTER_PORT_GAP_OFF];
                bytes_written += 1;
                data_ptr[bytes_written] = rf_ptr[RF_PORT_1_EVM_SD_SIG_OFF+1+i*RF_INTER_PORT_GAP_OFF];
                bytes_written += 1;
            }
        }

        /
        for (i = 0; i < RF_NUMBER_OF_PORTS; i++)
        {
            if (pntoh16(&rf_ptr[RF_PORT_1_EVM_SP_SIG_OFF+i*RF_INTER_PORT_GAP_OFF]) == 0) {
                phtoles(&data_ptr[bytes_written], 0);
                bytes_written += 2;
            } else {
                data_ptr[bytes_written] = rf_ptr[RF_PORT_1_EVM_SP_SIG_OFF+i*RF_INTER_PORT_GAP_OFF];
                bytes_written += 1;
                data_ptr[bytes_written] = rf_ptr[RF_PORT_1_EVM_SP_SIG_OFF+1+i*RF_INTER_PORT_GAP_OFF];
                bytes_written += 1;
            }
        }

        /
        for (i = 0; i < RF_NUMBER_OF_PORTS; i++)
        {
            if (pntoh16(&rf_ptr[RF_PORT_1_EVM_SD_DATA_OFF+i*RF_INTER_PORT_GAP_OFF]) == 0) {
                phtoles(&data_ptr[bytes_written], 0);
                bytes_written += 2;
            } else {
                data_ptr[bytes_written] = rf_ptr[RF_PORT_1_EVM_SD_DATA_OFF+i*RF_INTER_PORT_GAP_OFF];
                bytes_written += 1;
                data_ptr[bytes_written] = rf_ptr[RF_PORT_1_EVM_SD_DATA_OFF+1+i*RF_INTER_PORT_GAP_OFF];
                bytes_written += 1;
            }
        }

        /
        for (i = 0; i < RF_NUMBER_OF_PORTS; i++)
        {
            if (pntoh16(&rf_ptr[RF_PORT_1_EVM_SP_DATA_OFF+i*RF_INTER_PORT_GAP_OFF]) == 0) {
                phtoles(&data_ptr[bytes_written], 0);
                bytes_written += 2;
            } else {
                data_ptr[bytes_written] = rf_ptr[RF_PORT_1_EVM_SP_DATA_OFF+i*RF_INTER_PORT_GAP_OFF];
                bytes_written += 1;
                data_ptr[bytes_written] = rf_ptr[RF_PORT_1_EVM_SP_DATA_OFF+1+i*RF_INTER_PORT_GAP_OFF];
                bytes_written += 1;
            }
        }

        /
        for (i = 0; i < RF_NUMBER_OF_PORTS; i++)
        {
            if (pntoh16(&rf_ptr[RF_PORT_1_DSYMBOL_IDX_OFF+i*RF_INTER_PORT_GAP_OFF]) == 0) {
                phtoles(&data_ptr[bytes_written], 0);
                bytes_written += 2;
            } else {
                data_ptr[bytes_written] = rf_ptr[RF_PORT_1_DSYMBOL_IDX_OFF+i*RF_INTER_PORT_GAP_OFF];
                bytes_written += 1;
                data_ptr[bytes_written] = rf_ptr[RF_PORT_1_DSYMBOL_IDX_OFF+1+i*RF_INTER_PORT_GAP_OFF];
                bytes_written += 1;
            }
        }

        /
        for (i = 0; i < RF_NUMBER_OF_PORTS; i++)
        {
            if (pntoh16(&rf_ptr[RF_PORT_1_CONTEXT_OFF+i*RF_INTER_PORT_GAP_OFF]) == 0) {
                phtoles(&data_ptr[bytes_written], 0);
                bytes_written += 2;
            } else {
                data_ptr[bytes_written] = rf_ptr[RF_PORT_1_CONTEXT_OFF+i*RF_INTER_PORT_GAP_OFF];
                bytes_written += 1;
                data_ptr[bytes_written] = rf_ptr[RF_PORT_1_CONTEXT_OFF+1+i*RF_INTER_PORT_GAP_OFF];
                bytes_written += 1;
            }
        }

        /
/
    }
    /

    if (IS_TX != 3) {
        /
        phtoles(&data_ptr[bytes_written], OCTO_LAYER1TO4_LEN);
        bytes_written += 2;

        /
        data_ptr[bytes_written] = l1p_1;
        bytes_written += 1;

        data_ptr[bytes_written] = (nss << 4) | IS_TX;
        bytes_written += 1;

        phtoles(&data_ptr[bytes_written], phyRate);     /
        bytes_written += 2;

        data_ptr[bytes_written] = l1p_2;
        bytes_written += 1;

        data_ptr[bytes_written] = rssi[0];
        bytes_written += 1;
        data_ptr[bytes_written] = rssi[1];
        bytes_written += 1;
        data_ptr[bytes_written] = rssi[2];
        bytes_written += 1;
        data_ptr[bytes_written] = rssi[3];
        bytes_written += 1;

        /

        data_ptr[bytes_written] = s_start_ptr[2];    /
        bytes_written += 1;
        data_ptr[bytes_written] = s_start_ptr[3];    /
        bytes_written += 1;

        if (plcp_hdr_flag == 1 && (IS_TX == 0 || IS_TX == 4)) {
            data_ptr[bytes_written] = L1InfoC;  /
        } else {
            data_ptr[bytes_written] = 0;    /
        }
        bytes_written += 1;

        phtoles(&data_ptr[bytes_written], msdu_length);
        bytes_written += 2;
        /

        /
        memcpy(&data_ptr[bytes_written], &rec[stats_offset+16], 16);
        bytes_written += 16;
        /

        /

        phtolel(&data_ptr[bytes_written], pntoh32(&s_start_ptr[12]));   /
        bytes_written += 4;
        phtoles(&data_ptr[bytes_written], pntoh16(&s_trail_ptr[20]));   /
        bytes_written += 2;
        data_ptr[bytes_written] = flow_seq;
        bytes_written += 1;
        phtole24(&data_ptr[bytes_written], flow_id);
        bytes_written += 3;
        phtoles(&data_ptr[bytes_written], pntoh16(&s_trail_ptr[28]));   /
        bytes_written += 2;
        phtolel(&data_ptr[bytes_written], pntoh32(&s_trail_ptr[24]));   /
        bytes_written += 4;

        /
        if (IS_TX == 0 || IS_TX == 4) {
            phtoles(&data_ptr[bytes_written], info);
            bytes_written += 2;
            data_ptr[bytes_written] = info_2nd;
            bytes_written += 1;
        }
        else {
            phtoles(&data_ptr[bytes_written], info);
            bytes_written += 2;
            data_ptr[bytes_written] = 0;
            bytes_written += 1;
        }

        phtolel(&data_ptr[bytes_written], errors);
        bytes_written += 4;
        /

        /
        memcpy(&data_ptr[bytes_written], &rec[stats_offset+(vwr->MPDU_OFF)], actual_octets);
    }

    return TRUE;
}
