static gboolean vwr_read_s2_W_rec(vwr_t *vwr, struct wtap_pkthdr *phdr,
                                  Buffer *buf, const guint8 *rec, int rec_size,
                                  int IS_TX, int *err, gchar **err_info)
{
    guint8           *data_ptr;
    int              bytes_written = 0;                   /
    const guint8     *s_start_ptr,*s_trail_ptr, *plcp_ptr, *m_ptr; /
    guint32          msdu_length, actual_octets;          /
    guint8           l1p_1, l1p_2, plcp_type, rate_mcs_index, nss;  /
    guint            flow_seq;
    guint64          s_time = LL_ZERO, e_time = LL_ZERO;  /
                                                          /
    guint64          latency = LL_ZERO;
    guint64          start_time, s_sec, s_usec = LL_ZERO; /
    guint64          end_time;                            /
    guint16          info;                                /
    guint32          errors;
    gint8            rssi[] = {0,0,0,0};                  /
    int              f_tx;                                /
    guint16          vc_id, ht_len=0;                     /
    guint32          flow_id, d_time;                     /
    int              sig_off, pay_off;                    /
    guint64          sig_ts, tsid;                        /
    guint16          chanflags = 0;                       /
    guint16          radioflags = 0;                      /
    guint64          delta_b;                             /
    float            rate;
    guint16          phyRate;
    guint16          vw_flags;                            /

    /
    if ((guint)rec_size < vwr->MPDU_OFF + vVW510021_W_STATS_TRAILER_LEN) {
        *err_info = g_strdup_printf("vwr: Invalid record length %d (must be at least %u)",
                                    rec_size,
                                    vwr->MPDU_OFF + vVW510021_W_STATS_TRAILER_LEN);
        *err = WTAP_ERR_BAD_FILE;
        return FALSE;
    }

    /
    /
    s_start_ptr = &(rec[0]);                              /
    s_trail_ptr = &(rec[rec_size - vVW510021_W_STATS_TRAILER_LEN]);      /

    l1p_1 = s_start_ptr[vVW510021_W_L1P_1_OFF];
    l1p_2 = s_start_ptr[vVW510021_W_L1P_2_OFF];
    plcp_type = vVW510021_W_S2_PLCP_TYPE(l1p_2);
    /
    msdu_length = ((s_start_ptr[vVW510021_W_MSDU_LENGTH_OFF+1] & 0x1f) << 8)
                    + s_start_ptr[vVW510021_W_MSDU_LENGTH_OFF];

    vc_id = pntoh16(&s_start_ptr[vVW510021_W_VCID_OFF]);
    if (IS_TX)
    {
        rssi[0] = (s_start_ptr[vVW510021_W_RSSI_TXPOWER_OFF] & 0x80) ?
                   -1 * (s_start_ptr[vVW510021_W_RSSI_TXPOWER_OFF] & 0x7f) :
                   s_start_ptr[vVW510021_W_RSSI_TXPOWER_OFF] & 0x7f;
    }
    else
    {
        rssi[0] = (s_start_ptr[vVW510021_W_RSSI_TXPOWER_OFF] & 0x80) ?
                  (s_start_ptr[vVW510021_W_RSSI_TXPOWER_OFF]- 256) :
                  s_start_ptr[vVW510021_W_RSSI_TXPOWER_OFF];
    }
    rssi[1] = 100;
    rssi[2] = 100;
    rssi[3] = 100;

    plcp_ptr = &(rec[8]);

    actual_octets = msdu_length;

    /
    if (actual_octets > rec_size - (vwr->MPDU_OFF + vVW510021_W_STATS_TRAILER_LEN)) {
        *err_info = g_strdup_printf("vwr: Invalid data length %u (runs past the end of the record)",
                                    actual_octets);
        *err = WTAP_ERR_BAD_FILE;
        return FALSE;
    }

    f_tx = IS_TX;
    flow_seq = s_trail_ptr[vVW510021_W_FLOWSEQ_OFF];

    latency = 0x00000000;                        /
    flow_id = pntoh24(&s_trail_ptr[vVW510021_W_FLOWID_OFF]);         /
    /
    /
    tsid = pcorey48tohll(&s_trail_ptr[vVW510021_W_LATVAL_OFF]);

    errors = pntoh32(&s_trail_ptr[vVW510021_W_ERRORS_OFF]);
    info = pntoh16(&s_trail_ptr[vVW510021_W_INFO_OFF]);
    if ((info & v22_W_AGGREGATE_FLAGS) != 0)
    /
        ht_len = pletoh16(&s_start_ptr[vwr->PLCP_LENGTH_OFF]);


    /
    /
    switch (plcp_type)
    {
    case vVW510021_W_PLCP_LEGACY:
        /
        rate_mcs_index = vVW510021_W_S2_RATE_INDEX(l1p_1);
        if (rate_mcs_index < 4) {
            chanflags |= CHAN_CCK;
        }
        else {
            chanflags |= CHAN_OFDM;
        }
        rate = get_legacy_rate(rate_mcs_index);
        nss = 0;
        break;

    case vVW510021_W_PLCP_MIXED:
        /
        /
        rate_mcs_index = vVW510021_W_S2_MCS_INDEX_HT(l1p_1);
        radioflags |= FLAGS_CHAN_HT | ((plcp_ptr[3] & 0x80) ? FLAGS_CHAN_40MHZ : 0) |
                      ((l1p_1 & vVW510021_W_IS_LONGGI) ? 0 : FLAGS_CHAN_SHORTGI);
        chanflags  |= CHAN_OFDM;
        nss = (rate_mcs_index < MAX_HT_MCS) ? nss_for_mcs[rate_mcs_index] : 0;
        rate = get_ht_rate(rate_mcs_index, radioflags);
        break;

    case vVW510021_W_PLCP_GREENFIELD:
        /
        /
        rate_mcs_index = vVW510021_W_S2_MCS_INDEX_HT(l1p_1);
        radioflags |= FLAGS_CHAN_HT | ((plcp_ptr[0] & 0x80) ? FLAGS_CHAN_40MHZ : 0) |
                      ((l1p_1 & vVW510021_W_IS_LONGGI) ?  0 : FLAGS_CHAN_SHORTGI);
        chanflags  |= CHAN_OFDM;
        nss = (rate_mcs_index < MAX_HT_MCS) ? nss_for_mcs[rate_mcs_index] : 0;
        rate = get_ht_rate(rate_mcs_index, radioflags);
        break;

    case vVW510021_W_PLCP_VHT_MIXED:
        /
        {
            guint8 SBW = vVW510021_W_BANDWIDTH_VHT(l1p_2);
            rate_mcs_index = vVW510021_W_S2_MCS_INDEX_VHT(l1p_1);
            radioflags |= FLAGS_CHAN_VHT | ((l1p_1 & vVW510021_W_IS_LONGGI) ?  0 : FLAGS_CHAN_SHORTGI);
            chanflags |= CHAN_OFDM;
            if (SBW == 3)
                radioflags |= FLAGS_CHAN_40MHZ;
            else if (SBW == 4)
                radioflags |= FLAGS_CHAN_80MHZ;
            nss = vVW510021_W_S2_NSS_VHT(l1p_1);
            rate = get_vht_rate(rate_mcs_index, radioflags, nss);
        }
        break;

    default:
        rate_mcs_index = 0;
        nss = 0;
        rate = 0.0f;
        break;
    }

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
    m_ptr = &(rec[8+12]);
    pay_off = 42;         /
    sig_off = find_signature(m_ptr, rec_size - 20, pay_off, flow_id, flow_seq);
    if ((m_ptr[sig_off] == 0xdd) && (sig_off + 15 <= (rec_size - vVW510021_W_STATS_TRAILER_LEN)))
        sig_ts = get_signature_ts(m_ptr, sig_off);
    else
        sig_ts = 0;

    /
    if (!IS_TX) {
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

    /
    phdr->len = STATS_COMMON_FIELDS_LEN + EXT_WLAN_FIELDS_LEN + actual_octets;
    phdr->caplen = STATS_COMMON_FIELDS_LEN + EXT_WLAN_FIELDS_LEN + actual_octets;

    phdr->ts.secs   = (time_t)s_sec;
    phdr->ts.nsecs  = (int)(s_usec * 1000);
    phdr->pkt_encap = WTAP_ENCAP_IXVERIWAVE;

    phdr->rec_type = REC_TYPE_PACKET;
    phdr->presence_flags = WTAP_HAS_TS;

    ws_buffer_assure_space(buf, phdr->caplen);
    data_ptr = ws_buffer_start_ptr(buf);

    /
    /
    /
    phtole8(&data_ptr[bytes_written], WLAN_PORT);
    bytes_written += 1;
    /
    phtole8(&data_ptr[bytes_written], 0);
    bytes_written += 1;

    phtoles(&data_ptr[bytes_written], STATS_COMMON_FIELDS_LEN); /
    bytes_written += 2;
    phtoles(&data_ptr[bytes_written], msdu_length);
    bytes_written += 2;
    phtolel(&data_ptr[bytes_written], flow_id);
    bytes_written += 4;
    phtoles(&data_ptr[bytes_written], vc_id);
    bytes_written += 2;
    phtoles(&data_ptr[bytes_written], flow_seq);
    bytes_written += 2;
    if (!f_tx && sig_ts != 0) {
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
    phtoles(&data_ptr[bytes_written], EXT_WLAN_FIELDS_LEN);
    bytes_written += 2;
    if (info & vVW510021_W_IS_WEP)
        radioflags |= FLAGS_WEP;
    if (!(l1p_1 & vVW510021_W_IS_LONGPREAMBLE) && (plcp_type == vVW510021_W_PLCP_LEGACY))
        radioflags |= FLAGS_SHORTPRE;
    phtoles(&data_ptr[bytes_written], radioflags);
    bytes_written += 2;
    phtoles(&data_ptr[bytes_written], chanflags);
    bytes_written += 2;
    phyRate = (guint16)(rate * 10);
    phtoles(&data_ptr[bytes_written], phyRate);
    bytes_written += 2;

    data_ptr[bytes_written] = plcp_type;
    bytes_written += 1;

    data_ptr[bytes_written] = rate_mcs_index;
    bytes_written += 1;

    data_ptr[bytes_written] = nss;
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
    data_ptr[bytes_written] = 0;
    bytes_written += 1;

    /
    vw_flags  = 0;
    if (f_tx)
        vw_flags |= VW_FLAGS_TXF;
    if (errors & 0x1f)  /
        vw_flags |= VW_FLAGS_FCSERR;
    if (!f_tx && (errors & vwr->CRYPTO_ERR))
        vw_flags |= VW_FLAGS_DCRERR;
    if (!f_tx && (errors & vwr->RETRY_ERR))
        vw_flags |= VW_FLAGS_RETRERR;
    if (info & vwr->WEPTYPE)
        vw_flags |= VW_FLAGS_IS_WEP;
    else if (info & vwr->TKIPTYPE)
        vw_flags |= VW_FLAGS_IS_TKIP;
    else if (info & vwr->CCMPTYPE)
        vw_flags |= VW_FLAGS_IS_CCMP;
    phtoles(&data_ptr[bytes_written], vw_flags);
    bytes_written += 2;

    phtoles(&data_ptr[bytes_written], ht_len);
    bytes_written += 2;
    phtoles(&data_ptr[bytes_written], info);
    bytes_written += 2;
    phtolel(&data_ptr[bytes_written], errors);
    bytes_written += 4;

    /
    memcpy(&data_ptr[bytes_written], &rec[vwr->MPDU_OFF], actual_octets);

    return TRUE;
}
