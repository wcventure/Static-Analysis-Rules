static gboolean vwr_read_s1_W_rec(vwr_t *vwr, struct wtap_pkthdr *phdr,
                                  Buffer *buf, const guint8 *rec, int rec_size,
                                  int *err, gchar **err_info)
{
    guint8           *data_ptr;
    int              bytes_written = 0;                   /
    const guint8     *s_ptr, *m_ptr;                      /
    guint16          msdu_length, actual_octets;          /
    guint16          plcp_hdr_len;                        /
    guint16          rflags;
    guint8           m_type;                              /
    guint            flow_seq;
    guint64          s_time = LL_ZERO, e_time = LL_ZERO;  /
                                                          /
    guint32          latency;
    guint64          start_time, s_sec, s_usec = LL_ZERO; /
    guint64          end_time;                            /
    guint32          info;                                /
    gint8            rssi;                                /
    int              f_tx;                                /
    guint8           rate_index;                          /
    guint16          vc_id, ht_len=0;                     /
    guint            flow_id;                             /
    guint32          d_time, errors;                      /
    int              sig_off, pay_off;                    /
    guint64          sig_ts;                              /
    guint16          phyRate;
    guint16          vw_flags;                            /

    /
    if (rec_size < v22_W_STATS_LEN) {
        *err_info = g_strdup_printf("vwr: Invalid record length %d (must be at least %u)",
                                    rec_size, v22_W_STATS_LEN);
        *err = WTAP_ERR_BAD_FILE;
        return FALSE;
    }

    /
    /
    s_ptr    = &(rec[rec_size - v22_W_STATS_LEN]); /
    m_type   = s_ptr[v22_W_MTYPE_OFF] & v22_E_MT_MASK;
    f_tx     = !(s_ptr[v22_W_MTYPE_OFF] & v22_E_IS_RX);
    actual_octets   = pntoh16(&s_ptr[v22_W_OCTET_OFF]);
    vc_id    = pntoh16(&s_ptr[v22_W_VCID_OFF]) & v22_E_VCID_MASK;
    flow_seq = s_ptr[v22_W_FLOWSEQ_OFF];

    latency = (guint32)pcorey48tohll(&s_ptr[v22_W_LATVAL_OFF]);

    flow_id = pntoh16(&s_ptr[v22_W_FLOWID_OFF+1]);  /
    errors  = pntoh16(&s_ptr[v22_W_ERRORS_OFF]);

    info = pntoh16(&s_ptr[v22_W_INFO_OFF]);
    rssi = (s_ptr[v22_W_RSSI_OFF] & 0x80) ? (-1 * (s_ptr[v22_W_RSSI_OFF] & 0x7f)) : s_ptr[v22_W_RSSI_OFF];

    /
    if (actual_octets > rec_size - v22_W_STATS_LEN) {
        *err_info = g_strdup_printf("vwr: Invalid data length %u (runs past the end of the record)",
                                    actual_octets);
        *err = WTAP_ERR_BAD_FILE;
        return FALSE;
    }

    /
    /
    if (m_type == vwr->MT_OFDM)
        rate_index = get_ofdm_rate(rec);
    else if ((m_type == vwr->MT_CCKL) || (m_type == vwr->MT_CCKS))
        rate_index = get_cck_rate(rec);
    else
        rate_index = 1;
    rflags  = (m_type == vwr->MT_CCKS) ? FLAGS_SHORTPRE : 0;
    /
    /
    /
    plcp_hdr_len = (m_type == vwr->MT_OFDM) ? 4 : 6;
    if (actual_octets >= plcp_hdr_len)
       actual_octets -= plcp_hdr_len;
    else {
        *err_info = g_strdup_printf("vwr: Invalid data length %u (too short to include %u-byte PLCP header)",
                                    actual_octets, plcp_hdr_len);
        *err = WTAP_ERR_BAD_FILE;
        return FALSE;
    }
    m_ptr = &rec[plcp_hdr_len];
    msdu_length = actual_octets;

    /
    if (actual_octets < 4) {
        if (actual_octets != 0) {
            *err_info = g_strdup_printf("vwr: Invalid data length %u (too short to include %u-byte PLCP header and 4 bytes of FCS)",
                                        actual_octets, plcp_hdr_len);
            *err = WTAP_ERR_BAD_FILE;
            return FALSE;
        }
    } else {
        actual_octets -= 4;
    }

    /
    /
    s_time = pcoreytohll(&s_ptr[v22_W_STARTT_OFF]);
    e_time = pcoreytohll(&s_ptr[v22_W_ENDT_OFF]);

    /
    d_time = (guint32)((e_time - s_time) / NS_IN_US);   /

    /
    start_time = s_time / NS_IN_US;                /
    s_sec      = (start_time / US_IN_SEC);         /
    s_usec     = start_time - (s_sec * US_IN_SEC); /

    /
    end_time = e_time / NS_IN_US;                       /

    /
    pay_off = 42;    /
    sig_off = find_signature(m_ptr, rec_size - 6, pay_off, flow_id, flow_seq);
    if ((m_ptr[sig_off] == 0xdd) && (sig_off + 15 <= (rec_size - v22_W_STATS_LEN)))
        sig_ts = get_signature_ts(m_ptr, sig_off);
    else
        sig_ts = 0;

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
    phtoles(&data_ptr[bytes_written], rflags);
    bytes_written += 2;
    if (m_type == vwr->MT_OFDM) {
        phtoles(&data_ptr[bytes_written], CHAN_OFDM);
    } else {
        phtoles(&data_ptr[bytes_written], CHAN_CCK);
    }
    bytes_written += 2;
    phyRate = (guint16)(get_legacy_rate(rate_index) * 10);
    phtoles(&data_ptr[bytes_written], phyRate);
    bytes_written += 2;
    data_ptr[bytes_written] = vVW510021_W_PLCP_LEGACY; /
    bytes_written += 1;
    data_ptr[bytes_written] = rate_index;
    bytes_written += 1;
    data_ptr[bytes_written] = 1; /
    bytes_written += 1;
    data_ptr[bytes_written] = rssi;
    bytes_written += 1;
    /
    data_ptr[bytes_written] = 100;
    bytes_written += 1;
    data_ptr[bytes_written] = 100;
    bytes_written += 1;
    data_ptr[bytes_written] = 100;
    bytes_written += 1;
    /
    data_ptr[bytes_written] = 0;
    bytes_written += 1;

    /
    vw_flags = 0;
    if (f_tx)
        vw_flags |= VW_FLAGS_TXF;
    if (errors & vwr->FCS_ERROR)
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
    memcpy(&data_ptr[bytes_written], &rec[plcp_hdr_len], actual_octets);

    return TRUE;
}
