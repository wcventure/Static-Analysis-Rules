static guint16 evaluate_meta_item_dxt(proto_tree *meta_tree, tvbuff_t *tvb, packet_info *pinfo, guint16 offs)
{
    guint16             id;
    guint8              type, len, aligned_len, total_len;
    proto_tree         *item_tree;
    proto_item         *subti;
    /
    guint8              dir, nsapi, rat, aal5proto, *apn, *calling, *called;
    guint16             phylinkid, localdevid, remotedevid, tapgroupid;
    guint32             tlli;
    guint64             ts, imsi, imei, cell;
    sscop_payload_info *p_sscop_info;
    const gchar        *imsi_str, *imei_str;
    proto_item         *cell_item, *imsi_item, *imei_item;
    proto_tree         *cell_tree = NULL, *imsi_tree = NULL, *imei_tree = NULL;

    id = tvb_get_letohs(tvb, offs); offs += 2;
    type = tvb_get_guint8(tvb, offs); offs++;
    len = tvb_get_guint8(tvb, offs); offs++;
    aligned_len = (len + 3) & 0xfffc;
    total_len = aligned_len + 4; /

    switch (id) {
        case META_ID_DIRECTION:
            dir = tvb_get_guint8(tvb, offs);
            pinfo->p2p_dir = (dir == META_DIR_UP ? P2P_DIR_RECV : P2P_DIR_SENT);
            proto_tree_add_uint(meta_tree, hf_meta_item_direction, tvb, offs, 1, dir);
            break;
        case META_ID_TIMESTAMP64:
            ts = tvb_get_letoh64(tvb, offs);
            proto_tree_add_uint64(meta_tree, hf_meta_item_ts, tvb, offs, 8, ts);
            break;
        case META_ID_PHYLINKID:
            phylinkid = tvb_get_letohs(tvb, offs);
            pinfo->link_number = phylinkid;
            proto_tree_add_uint(meta_tree, hf_meta_item_phylinkid, tvb,
                offs, 2, phylinkid);
            break;
        case META_ID_NSAPI:
            nsapi = tvb_get_guint8(tvb, offs);
            proto_tree_add_uint(meta_tree, hf_meta_item_nsapi, tvb,
                offs, 1, nsapi);
            break;
        case META_ID_IMSI:
            imsi     = tvb_get_letoh64(tvb, offs);
            imsi_str = tvb_bcd_dig_to_ep_str(tvb, offs, 8, NULL, FALSE);
            imsi_item = proto_tree_add_string(meta_tree, hf_meta_item_imsi_digits, tvb,
                offs, 8, imsi_str);
            imsi_tree = proto_item_add_subtree(imsi_item, ett_meta_imsi);
            proto_tree_add_uint64(imsi_tree, hf_meta_item_imsi_value,
                tvb, offs, 8, imsi);
            break;
        case META_ID_IMEI:
            imei     = tvb_get_letoh64(tvb, offs);
            imei_str = tvb_bcd_dig_to_ep_str(tvb, offs, 8, NULL, FALSE);
            imei_item = proto_tree_add_string(meta_tree, hf_meta_item_imei_digits, tvb,
                offs, 8, imei_str);
            imei_tree = proto_item_add_subtree(imei_item, ett_meta_imei);
            proto_tree_add_uint64(imei_tree, hf_meta_item_imei_value,
                tvb, offs, 8, imei);
            break;
        case META_ID_APN:
            apn = tvb_get_string(tvb, offs, len);
            proto_tree_add_string(meta_tree, hf_meta_item_apn, tvb,
                offs, len, apn);
            g_free(apn);
            break;
        case META_ID_RAT:
            rat = tvb_get_guint8(tvb, offs);
            proto_tree_add_uint(meta_tree, hf_meta_item_rat, tvb,
                offs, 1, rat);
            break;
        case META_ID_CELL:
            cell = tvb_get_ntoh64(tvb, offs);
            cell_item = proto_tree_add_uint64_format(meta_tree, hf_meta_item_cell,
                                                     tvb, offs, 8, cell, "Mobile Cell");
            cell_tree = proto_item_add_subtree(cell_item, ett_meta_cell);
            de_gmm_rai(tvb, cell_tree, pinfo, offs, 8, NULL, 0);
            de_cell_id(tvb, cell_tree, pinfo, offs + 6, 2, NULL, 0);
            break;
        case META_ID_SIGNALING:
            proto_tree_add_boolean(meta_tree, hf_meta_item_signaling, tvb,
                                   offs, 0, 1);
            break;
        case META_ID_INCOMPLETE:
            proto_tree_add_boolean(meta_tree, hf_meta_item_incomplete, tvb,
                offs, 0, 1);
            break;
        case META_ID_DECIPHERED:
            proto_tree_add_boolean(meta_tree, hf_meta_item_deciphered, tvb,
                offs, 0, 1);
            break;
        case META_ID_AAL5PROTO:
            aal5proto    = tvb_get_guint8(tvb, offs);
            p_sscop_info = p_get_proto_data(pinfo->fd, proto_sscop);
            if (!p_sscop_info) {
                p_sscop_info = se_alloc0(sizeof(sscop_payload_info));
                p_add_proto_data(pinfo->fd, proto_sscop, p_sscop_info);
            }
            switch (aal5proto) {
                case META_AAL5PROTO_MTP3:
                    p_sscop_info->subdissector = sscf_nni_handle;
                    /
                    memset(&pinfo->pseudo_header->atm, 0, sizeof(pinfo->pseudo_header->atm));
                    pinfo->pseudo_header->atm.type = TRAF_SSCOP;
                    break;
                case META_AAL5PROTO_ALCAP:
                    p_sscop_info->subdissector = alcap_handle;
                    break;
                case META_AAL5PROTO_NBAP:
                    p_sscop_info->subdissector = nbap_handle;
                    break;
                case META_AAL5PROTO_NS:
                    /
                    memset(&pinfo->pseudo_header->atm, 0, sizeof(pinfo->pseudo_header->atm));
                    pinfo->pseudo_header->atm.type = TRAF_GPRS_NS;
                    break;
                /
                default:
                    /
                    p_remove_proto_data(pinfo->fd, proto_sscop);
            }
            proto_tree_add_uint(meta_tree, hf_meta_item_aal5proto, tvb,
                offs, 1, aal5proto);
            break;
        case META_ID_LOCALDEVID:
            localdevid = tvb_get_letohs(tvb, offs);
            proto_tree_add_uint(meta_tree, hf_meta_item_localdevid, tvb,
                offs, 2, localdevid);
            break;
        case META_ID_REMOTEDEVID:
            remotedevid = tvb_get_letohs(tvb, offs);
            proto_tree_add_uint(meta_tree, hf_meta_item_remotedevid, tvb,
                offs, 2, remotedevid);
            break;
        case META_ID_TAPGROUPID:
            tapgroupid = tvb_get_letohs(tvb, offs);
            proto_tree_add_uint(meta_tree, hf_meta_item_tapgroupid, tvb,
                offs, 2, tapgroupid);
            break;
        case META_ID_TLLI:
            tlli = tvb_get_letohs(tvb, offs);
            proto_tree_add_uint(meta_tree, hf_meta_item_tlli, tvb,
                offs, 4, tlli);
            break;
        case META_ID_CALLING:
            calling = tvb_get_string(tvb, offs, len);
            proto_tree_add_string(meta_tree, hf_meta_item_calling, tvb,
                offs, len, calling);
            g_free(calling);
            break;
        case META_ID_CALLED:
            called = tvb_get_string(tvb, offs, len);
            proto_tree_add_string(meta_tree, hf_meta_item_called, tvb,
                offs, len, called);
            g_free(called);
            break;
        default:
            subti = proto_tree_add_item(meta_tree, hf_meta_item, tvb, offs - 4,
                aligned_len + 4, ENC_NA);
            item_tree = proto_item_add_subtree(subti, ett_meta_item);
            proto_tree_add_uint(item_tree, hf_meta_item_id, tvb, offs - 4, 2, id);
            proto_tree_add_uint(item_tree, hf_meta_item_type, tvb, offs - 2, 1, type);
            proto_tree_add_uint(item_tree, hf_meta_item_len,
                tvb, offs - 1, 1, len);
            if (len > 0)
                proto_tree_add_item(item_tree, hf_meta_item_data,
                    tvb, offs, len, ENC_NA);
    }
    return total_len;
}
