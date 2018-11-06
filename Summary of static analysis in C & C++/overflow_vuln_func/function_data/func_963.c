static gboolean vwr_read_rec_data_ethernet(vwr_t *vwr, struct wtap_pkthdr *phdr,
                                           Buffer *buf, const guint8 *rec,
                                           int rec_size, int IS_TX, int *err,
                                           gchar **err_info)
{
    guint8           *data_ptr;
    int              bytes_written = 0;                   /
    const guint8 *s_ptr, *m_ptr;                          /
    guint16          msdu_length, actual_octets;          /
    guint            flow_seq;                            /
    guint64          s_time = LL_ZERO, e_time = LL_ZERO;  /
                                                          /
    guint32          latency = 0;
    guint64          start_time, s_sec = LL_ZERO, s_usec = LL_ZERO; /
    guint64          end_time;                            /
    guint            l4id;
    guint16          info, validityBits;                  /
    guint32          errors;
    guint16          vc_id;                               /
    guint32          flow_id, d_time;                     /
    int              f_flow;                              /
    guint32          frame_type;                          /
    int              mac_len, sig_off, pay_off;           /
    /
    guint64          sig_ts/;                    /
    guint64          delta_b;                             /
    guint16          vw_flags;                            /

    if ((guint)rec_size < vwr->STATS_LEN) {
        *err_info = g_strdup_printf("vwr: Invalid record length %d (must be at least %u)", rec_size, vwr->STATS_LEN);
        *err = WTAP_ERR_BAD_FILE;
        return FALSE;
    }

    /
    /
    m_ptr = &(rec[0]);                              /
    s_ptr = &(rec[rec_size - vwr->STATS_LEN]);      /

    msdu_length = pntoh16(&s_ptr[vwr->OCTET_OFF]);
    actual_octets = msdu_length;

    /
    if (actual_octets > rec_size - vwr->STATS_LEN) {
        *err_info = g_strdup_printf("vwr: Invalid data length %u (runs past the end of the record)",
                                    actual_octets);
        *err = WTAP_ERR_BAD_FILE;
        return FALSE;
    }

    vc_id = pntoh16(&s_ptr[vwr->VCID_OFF]) & vwr->VCID_MASK;
    flow_seq   = s_ptr[vwr->FLOWSEQ_OFF];
    frame_type = pntoh32(&s_ptr[vwr->FRAME_TYPE_OFF]);

    if (vwr->FPGA_VERSION == vVW510024_E_FPGA) {
        validityBits = pntoh16(&s_ptr[vwr->VALID_OFF]);
        f_flow = validityBits & vwr->FLOW_VALID;

        mac_len = (validityBits & vwr->IS_VLAN) ? 16 : 14;           /


        errors = pntoh16(&s_ptr[vwr->ERRORS_OFF]);
    }
    else {
        f_flow  = s_ptr[vwr->VALID_OFF] & vwr->FLOW_VALID;
        mac_len = (frame_type & vwr->IS_VLAN) ? 16 : 14;             /

        /
        errors = pntoh16(&s_ptr[vwr->ERRORS_OFF]);
    }

    info = pntoh16(&s_ptr[vwr->INFO_OFF]);
    /
    flow_id = pntoh24(&s_ptr[vwr->FLOWID_OFF]);

#if 0
    /
    /
    tsid = pcorey48tohll(&s_ptr[vwr->LATVAL_OFF]);
#endif

    l4id = pntoh16(&s_ptr[vwr->L4ID_OFF]);

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
    s_time = pcoreytohll(&s_ptr[vwr->STARTT_OFF]);
    e_time = pcoreytohll(&s_ptr[vwr->ENDT_OFF]);

    /
    d_time = (guint32)((e_time - s_time));  /

    /
    start_time = s_time / NS_IN_US;                     /
    s_sec = (start_time / US_IN_SEC);                   /
    s_usec = start_time - (s_sec * US_IN_SEC);          /

    /
    end_time = e_time / NS_IN_US;                       /

    if (frame_type & vwr->IS_TCP)                       /
    {
        pay_off = mac_len + 40;
    }
    else if (frame_type & vwr->IS_UDP)                  /
    {
        pay_off = mac_len + 28;
    }
    else if (frame_type & vwr->IS_ICMP)                 /
    {
        pay_off = mac_len + 24;
    }
    else if (frame_type & vwr->IS_IGMP)                 /
    {
        pay_off = mac_len + 28;
    }
    else                                                /
    {
        pay_off = mac_len + 20;
    }

    sig_off = find_signature(m_ptr, rec_size, pay_off, flow_id, flow_seq);
    if ((m_ptr[sig_off] == 0xdd) && (sig_off + 15 <= msdu_length) && (f_flow != 0))
        sig_ts = get_signature_ts(m_ptr, sig_off);
    else
        sig_ts = 0;

    /
    if (!IS_TX) {
        if (sig_ts < s_time) {
            latency = (guint32)(s_time - sig_ts);
        } else {
            /
            /
            delta_b = sig_ts - s_time;
            if (delta_b >  0x10000000) {
                latency = 0;
            } else
                latency = (guint32)delta_b;
        }
    }

    /
    phdr->len = STATS_COMMON_FIELDS_LEN + EXT_ETHERNET_FIELDS_LEN + actual_octets;
    phdr->caplen = STATS_COMMON_FIELDS_LEN + EXT_ETHERNET_FIELDS_LEN + actual_octets;

    phdr->ts.secs   = (time_t)s_sec;
    phdr->ts.nsecs  = (int)(s_usec * 1000);
    phdr->pkt_encap = WTAP_ENCAP_IXVERIWAVE;

    phdr->rec_type = REC_TYPE_PACKET;
    phdr->presence_flags = WTAP_HAS_TS;

    /

    ws_buffer_assure_space(buf, phdr->caplen);
    data_ptr = ws_buffer_start_ptr(buf);

    /
    /
    phtole8(&data_ptr[bytes_written], ETHERNET_PORT);
    bytes_written += 1;
    /
    phtole8(&data_ptr[bytes_written], 0);
    bytes_written += 1;

    phtoles(&data_ptr[bytes_written], STATS_COMMON_FIELDS_LEN);
    bytes_written += 2;
    phtoles(&data_ptr[bytes_written], msdu_length);
    bytes_written += 2;
    phtolel(&data_ptr[bytes_written], flow_id);
    bytes_written += 4;
    phtoles(&data_ptr[bytes_written], vc_id);
    bytes_written += 2;
    phtoles(&data_ptr[bytes_written], flow_seq);
    bytes_written += 2;
    if (!IS_TX && (sig_ts != 0)) {
        phtolel(&data_ptr[bytes_written], latency);
    } else {
        phtolel(&data_ptr[bytes_written], 0);
    }
    bytes_written += 4;
    phtolel(&data_ptr[bytes_written], sig_ts);
    bytes_written += 4;
    phtolell(&data_ptr[bytes_written], start_time)                  /
    bytes_written += 8;
    phtolell(&data_ptr[bytes_written], end_time);
    bytes_written += 8;
    phtolel(&data_ptr[bytes_written], d_time);
    bytes_written += 4;

    /
    phtoles(&data_ptr[bytes_written], EXT_ETHERNET_FIELDS_LEN);
    bytes_written += 2;
    vw_flags = 0;
    if (IS_TX)
        vw_flags |= VW_FLAGS_TXF;
    if (errors & vwr->FCS_ERROR)
        vw_flags |= VW_FLAGS_FCSERR;
    phtoles(&data_ptr[bytes_written], vw_flags);
    bytes_written += 2;
    phtoles(&data_ptr[bytes_written], info);
    bytes_written += 2;
    phtolel(&data_ptr[bytes_written], errors);
    bytes_written += 4;
    phtolel(&data_ptr[bytes_written], l4id);
    bytes_written += 4;

    /
    phtolel(&data_ptr[bytes_written], 0);
    bytes_written += 4;

    /
    memcpy(&data_ptr[bytes_written], m_ptr, actual_octets);

    return TRUE;
}
