static tvbuff_t *
dissect_6lowpan_iphc(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree)
{
    gint                offset = 0;
    gint                length;
    proto_tree *        iphc_tree = NULL;
    proto_item *        ti = NULL;
    proto_item *        ti_sam = NULL;
    proto_item *        ti_dam = NULL;
    gboolean            addr_err;
    /
    guint16             iphc_flags;
    guint8              iphc_traffic;
    guint8              iphc_hop_limit;
    guint8              iphc_src_mode;
    guint8              iphc_dst_mode;
    guint8              iphc_ctx = 0;
    /
    guint8              ipv6_class = 0;
    struct ip6_hdr      ipv6;
    tvbuff_t *          ipv6_tvb;
    /
    struct lowpan_nhdr *nhdr_list;

    /
    if (tree) {
        ti = proto_tree_add_text(tree, tvb, 0, sizeof(guint16), "IPHC Header");
        iphc_tree = proto_item_add_subtree(ti, ett_6lowpan_iphc);

        /
        proto_tree_add_bits_item(iphc_tree, hf_6lowpan_pattern, tvb, 0, LOWPAN_PATTERN_IPHC_BITS, FALSE);
    }

    /
    iphc_flags      = tvb_get_ntohs(tvb, offset);
    iphc_traffic    = (iphc_flags & LOWPAN_IPHC_FLAG_FLOW) >> LOWPAN_IPHC_FLAG_OFFSET_FLOW;
    iphc_hop_limit  = (iphc_flags & LOWPAN_IPHC_FLAG_HLIM) >> LOWPAN_IPHC_FLAG_OFFSET_HLIM;
    iphc_src_mode   = (iphc_flags & LOWPAN_IPHC_FLAG_SRC_MODE) >> LOWPAN_IPHC_FLAG_OFFSET_SRC_MODE;
    iphc_dst_mode   = (iphc_flags & LOWPAN_IPHC_FLAG_DST_MODE) >> LOWPAN_IPHC_FLAG_OFFSET_DST_MODE;
    if (tree) {
        proto_tree_add_uint         (iphc_tree, hf_6lowpan_iphc_flag_tf,    tvb, offset, sizeof(guint16), iphc_flags & LOWPAN_IPHC_FLAG_FLOW);
        proto_tree_add_boolean      (iphc_tree, hf_6lowpan_iphc_flag_nhdr,  tvb, offset, sizeof(guint16), iphc_flags & LOWPAN_IPHC_FLAG_NHDR);
        proto_tree_add_uint         (iphc_tree, hf_6lowpan_iphc_flag_hlim,  tvb, offset, sizeof(guint16), iphc_flags & LOWPAN_IPHC_FLAG_HLIM);
        proto_tree_add_boolean      (iphc_tree, hf_6lowpan_iphc_flag_cid,   tvb, offset, sizeof(guint16), iphc_flags & LOWPAN_IPHC_FLAG_CONTEXT_ID);
        proto_tree_add_boolean      (iphc_tree, hf_6lowpan_iphc_flag_sac,   tvb, offset, sizeof(guint16), iphc_flags & LOWPAN_IPHC_FLAG_SRC_COMP);
        ti_sam = proto_tree_add_uint(iphc_tree, hf_6lowpan_iphc_flag_sam,   tvb, offset, sizeof(guint16), iphc_flags & LOWPAN_IPHC_FLAG_SRC_MODE);
        proto_tree_add_boolean      (iphc_tree, hf_6lowpan_iphc_flag_mcast, tvb, offset, sizeof(guint16), iphc_flags & LOWPAN_IPHC_FLAG_MCAST_COMP);
        proto_tree_add_boolean      (iphc_tree, hf_6lowpan_iphc_flag_dac,   tvb, offset, sizeof(guint16), iphc_flags & LOWPAN_IPHC_FLAG_DST_COMP);
        ti_dam = proto_tree_add_uint(iphc_tree, hf_6lowpan_iphc_flag_dam,   tvb, offset, sizeof(guint16), iphc_flags & LOWPAN_IPHC_FLAG_DST_MODE);
    }
    offset += sizeof(guint16);

    /
    if (iphc_flags & LOWPAN_IPHC_FLAG_CONTEXT_ID) {
        iphc_ctx = tvb_get_guint8(tvb, offset);
        if (tree) {
            proto_tree_add_uint(iphc_tree, hf_6lowpan_iphc_sci, tvb, offset, sizeof(guint8), iphc_ctx & LOWPAN_IPHC_FLAG_SCI);
            proto_tree_add_uint(iphc_tree, hf_6lowpan_iphc_dci, tvb, offset, sizeof(guint8), iphc_ctx & LOWPAN_IPHC_FLAG_DCI);
        }
        offset +=  sizeof(guint8);
    }

    /
    offset <<= 3;
    /
    if (iphc_traffic != LOWPAN_IPHC_FLOW_COMPRESSED) {
        ipv6_class |= tvb_get_bits8(tvb, offset, LOWPAN_IPHC_ECN_BITS);
        offset += LOWPAN_IPHC_ECN_BITS;
    }
    /
    if ((iphc_traffic == LOWPAN_IPHC_FLOW_CLASS_LABEL) || (iphc_traffic == LOWPAN_IPHC_FLOW_CLASS)) {
        ipv6_class |= (tvb_get_bits8(tvb, offset, LOWPAN_IPHC_DSCP_BITS) << LOWPAN_IPHC_ECN_BITS);
        offset += LOWPAN_IPHC_DSCP_BITS;
    }
    /
    if (tree) {
        /
        proto_tree *    tf_tree;
        ti = proto_tree_add_uint(tree, hf_6lowpan_traffic_class, tvb, offset>>3, sizeof(guint8), ipv6_class);
        tf_tree = proto_item_add_subtree(ti, ett_6lopwan_traffic_class);

        /
        proto_tree_add_uint(tf_tree, hf_6lowpan_ecn, tvb, offset>>3, sizeof(guint8), ipv6_class & LOWPAN_IPHC_TRAFFIC_ECN);
        proto_tree_add_uint(tf_tree, hf_6lowpan_dscp, tvb, offset>>3, sizeof(guint8), ipv6_class & LOWPAN_IPHC_TRAFFIC_DSCP);
    }

    /
    if ((iphc_traffic == LOWPAN_IPHC_FLOW_CLASS_LABEL) || (iphc_traffic == LOWPAN_IPHC_FLOW_ECN_LABEL)) {
        /
        offset += ((4 - offset) & 0x7);
        ipv6.ip6_flow = tvb_get_bits32(tvb, offset, LOWPAN_IPHC_LABEL_BITS, FALSE);
        if (tree) {
            proto_tree_add_bits_item(tree, hf_6lowpan_flow_label, tvb, offset, LOWPAN_IPHC_LABEL_BITS, FALSE);
        }
        offset += LOWPAN_IPHC_LABEL_BITS;
    }
    else ipv6.ip6_flow = 0;

    /
    ipv6.ip6_flow = g_ntohl(ipv6.ip6_flow) | (ipv6_class << LOWPAN_IPV6_FLOW_LABEL_BITS);
    ipv6.ip6_vfc = (0x6 << 4) | (ipv6_class >> 4);

    /
    offset >>= 3;

    /
    /
    if (!(iphc_flags & LOWPAN_IPHC_FLAG_NHDR)) {
        ipv6.ip6_nxt = tvb_get_guint8(tvb, offset);
        if (tree) {
            proto_tree_add_uint_format(tree, hf_6lowpan_next_header, tvb, offset, sizeof(guint8), ipv6.ip6_nxt,
                    "Next header: %s (0x%02x)", ipprotostr(ipv6.ip6_nxt), ipv6.ip6_nxt);
        }
        offset += sizeof(guint8);
    }

    /
    if (iphc_hop_limit == LOWPAN_IPHC_HLIM_1) {
        ipv6.ip6_hlim = 1;
    }
    else if (iphc_hop_limit == LOWPAN_IPHC_HLIM_64) {
        ipv6.ip6_hlim = 64;
    }
    else if (iphc_hop_limit == LOWPAN_IPHC_HLIM_255) {
        ipv6.ip6_hlim = 255;
    }
    else {
        ipv6.ip6_hlim = tvb_get_guint8(tvb, offset);
        if (tree) {
            proto_tree_add_uint(tree, hf_6lowpan_hop_limit, tvb, offset, sizeof(guint8), ipv6.ip6_hlim);
        }
        offset += sizeof(guint8);
    }

    /
    addr_err = FALSE;
    length = 0;
    memset(&ipv6.ip6_src, 0, sizeof(ipv6.ip6_src));
    /
    if (!(iphc_flags & LOWPAN_IPHC_FLAG_SRC_COMP)) {
        /
        ipv6.ip6_src.bytes[0] = 0xff;
        ipv6.ip6_src.bytes[1] = 0xfe;
        /
        if (iphc_src_mode == LOWPAN_IPHC_ADDR_FULL_INLINE) {
            length = sizeof(ipv6.ip6_src);
            tvb_memcpy(tvb, &ipv6.ip6_src.bytes[sizeof(ipv6.ip6_src) - length], offset, length);
        }
        /
        else if (iphc_src_mode == LOWPAN_IPHC_ADDR_64BIT_INLINE) {
            length = sizeof(guint64);
            tvb_memcpy(tvb, &ipv6.ip6_src.bytes[sizeof(ipv6.ip6_src) - length], offset, length);
        }
        else if (iphc_src_mode == LOWPAN_IPHC_ADDR_16BIT_INLINE) {
            length = sizeof(guint16);
            tvb_memcpy(tvb, &ipv6.ip6_src.bytes[sizeof(ipv6.ip6_src) - length], offset, length);
        }
        /
        else {
            lowpan_dlsrc_to_ifcid(pinfo, &ipv6.ip6_src.bytes[8]);
        }
    }
    /
    else {
        /
        addr_err = TRUE;
        if (iphc_src_mode == LOWPAN_IPHC_ADDR_64BIT_INLINE) length = sizeof(guint64);
        else if (iphc_src_mode == LOWPAN_IPHC_ADDR_16BIT_INLINE) length = sizeof(guint16);
        else if (iphc_src_mode == LOWPAN_IPHC_ADDR_COMPRESSED) length = 0;
        else {
            /
            expert_add_info_format(pinfo, ti_sam, PI_MALFORMED, PI_ERROR, "Illegal source address mode");
            return NULL;
        }
    }

    /
    if (tree) {
        ti = proto_tree_add_ipv6(tree, hf_6lowpan_source, tvb, offset, length, (guint8 *)&ipv6.ip6_src);
    }
    if (addr_err) {
        expert_add_info_format(pinfo, ti, PI_UNDECODED, PI_WARN, "Failed to recover source IPv6 address");
    }
    offset += length;

    /
    addr_err = FALSE;
    length = 0;
    memset(&ipv6.ip6_dst, 0, sizeof(ipv6.ip6_dst));
    /
    if (!(iphc_flags & LOWPAN_IPHC_FLAG_DST_COMP) && !(iphc_flags & LOWPAN_IPHC_FLAG_MCAST_COMP)) {
        /
        ipv6.ip6_dst.bytes[0] = 0xff;
        ipv6.ip6_dst.bytes[1] = 0xfe;
        /
        if (iphc_dst_mode == LOWPAN_IPHC_ADDR_FULL_INLINE) {
            length = sizeof(ipv6.ip6_dst);
            tvb_memcpy(tvb, &ipv6.ip6_dst.bytes[sizeof(ipv6.ip6_dst) - length], offset, length);
        }
        /
        else if (iphc_dst_mode == LOWPAN_IPHC_ADDR_64BIT_INLINE) {
            length = sizeof(guint64);
            tvb_memcpy(tvb, &ipv6.ip6_dst.bytes[sizeof(ipv6.ip6_dst) - length], offset, length);
        }
        else if (iphc_dst_mode == LOWPAN_IPHC_ADDR_16BIT_INLINE) {
            length = sizeof(guint16);
            tvb_memcpy(tvb, &ipv6.ip6_dst.bytes[sizeof(ipv6.ip6_dst) - length], offset, length);
        }
        /
        else {
            lowpan_dldst_to_ifcid(pinfo, &ipv6.ip6_dst.bytes[8]);
        }
    }
    /
    else if (!(iphc_flags & LOWPAN_IPHC_FLAG_DST_COMP) && (iphc_flags & LOWPAN_IPHC_FLAG_MCAST_COMP)) {
        if (iphc_dst_mode == LOWPAN_IPHC_ADDR_FULL_INLINE) {
            ipv6.ip6_dst.bytes[0] = 0xff;
            ipv6.ip6_dst.bytes[1] = tvb_get_guint8(tvb, offset + (length++));
            ipv6.ip6_dst.bytes[11] = tvb_get_guint8(tvb, offset + (length++));
            ipv6.ip6_dst.bytes[12] = tvb_get_guint8(tvb, offset + (length++));
            ipv6.ip6_dst.bytes[13] = tvb_get_guint8(tvb, offset + (length++));
            ipv6.ip6_dst.bytes[14] = tvb_get_guint8(tvb, offset + (length++));
            ipv6.ip6_dst.bytes[15] = tvb_get_guint8(tvb, offset + (length++));
        }
        else if (iphc_dst_mode == LOWPAN_IPHC_ADDR_64BIT_INLINE) {
            ipv6.ip6_dst.bytes[0] = 0xff;
            ipv6.ip6_dst.bytes[1] = tvb_get_guint8(tvb, offset + (length++));
            ipv6.ip6_dst.bytes[13] = tvb_get_guint8(tvb, offset + (length++));
            ipv6.ip6_dst.bytes[14] = tvb_get_guint8(tvb, offset + (length++));
            ipv6.ip6_dst.bytes[15] = tvb_get_guint8(tvb, offset + (length++));
        }
        else if (iphc_dst_mode == LOWPAN_IPHC_ADDR_16BIT_INLINE) {
            guint8          temp = tvb_get_guint8(tvb, offset + (length++));
            ipv6.ip6_dst.bytes[0] = 0xff;
            ipv6.ip6_dst.bytes[1] = (temp >> 4);
            ipv6.ip6_dst.bytes[14] = (temp & 0xf);
            ipv6.ip6_dst.bytes[15] = tvb_get_guint8(tvb, offset + (length++));
        }
        else if (iphc_dst_mode == LOWPAN_IPHC_ADDR_COMPRESSED) {
            ipv6.ip6_dst.bytes[0] = 0xff;
            ipv6.ip6_dst.bytes[1] = 0x02;
            ipv6.ip6_dst.bytes[15] = tvb_get_guint8(tvb, offset + (length++));
        }
    }
    /
    else if ((iphc_flags & LOWPAN_IPHC_FLAG_DST_COMP) && !(iphc_flags & LOWPAN_IPHC_FLAG_MCAST_COMP)) {
        /
        addr_err = TRUE;
        if (iphc_dst_mode == LOWPAN_IPHC_ADDR_64BIT_INLINE) length = sizeof(guint64);
        else if (iphc_dst_mode == LOWPAN_IPHC_ADDR_16BIT_INLINE) length = sizeof(guint16);
        else if (iphc_dst_mode == LOWPAN_IPHC_ADDR_COMPRESSED) length = 0;
        else {
            /
            expert_add_info_format(pinfo, ti_dam, PI_MALFORMED, PI_ERROR, "Illegal destination address mode");
            return NULL;
        }
    }
    /
    else {
        /
        if (iphc_dst_mode == LOWPAN_IPHC_ADDR_FULL_INLINE) {
            length = sizeof(ipv6.ip6_dst);
            tvb_memcpy(tvb, &ipv6.ip6_dst, offset, length);
        }
        /
        else if (iphc_dst_mode == LOWPAN_IPHC_ADDR_64BIT_INLINE) {
            ipv6.ip6_dst.bytes[0] = 0xff;
            ipv6.ip6_dst.bytes[1] = tvb_get_guint8(tvb, offset + (length++));
            ipv6.ip6_dst.bytes[2] = tvb_get_guint8(tvb, offset + (length++));
            /
            addr_err = TRUE;
            ipv6.ip6_dst.bytes[12] = tvb_get_guint8(tvb, offset + (length++));
            ipv6.ip6_dst.bytes[13] = tvb_get_guint8(tvb, offset + (length++));
            ipv6.ip6_dst.bytes[14] = tvb_get_guint8(tvb, offset + (length++));
            ipv6.ip6_dst.bytes[15] = tvb_get_guint8(tvb, offset + (length++));
        }
        else {
            /
            addr_err = TRUE;
            expert_add_info_format(pinfo, ti_dam, PI_MALFORMED, PI_ERROR, "Illegal destination address mode");
            return NULL;
        }

    }

    /
    if (tree) {
        ti = proto_tree_add_ipv6(tree, hf_6lowpan_dest, tvb, offset, length, (guint8 *)&ipv6.ip6_dst);
    }
    if (addr_err) {
        expert_add_info_format(pinfo, ti, PI_UNDECODED, PI_WARN, "Failed to recover destination IPv6 address");
    }
    offset += length;

    /
    /
    if (iphc_flags & LOWPAN_IPHC_FLAG_NHDR) {
        /
        ipv6.ip6_nxt = lowpan_parse_nhc_proto(tvb, offset);

        /
        nhdr_list = dissect_6lowpan_iphc_nhc(tvb, pinfo, tree, offset);
    }
    /
    else {
        nhdr_list = ep_alloc(sizeof(struct lowpan_nhdr) + tvb_length_remaining(tvb, offset));
        nhdr_list->next = NULL;
        nhdr_list->proto = ipv6.ip6_nxt;
        nhdr_list->length = tvb_length_remaining(tvb, offset);
        nhdr_list->reported = tvb_reported_length_remaining(tvb, offset);
        tvb_memcpy(tvb, nhdr_list->hdr, offset, nhdr_list->length);
    }

    /
    /
    ipv6_tvb = lowpan_reassemble_ipv6(&ipv6, nhdr_list);
    tvb_set_child_real_data_tvbuff(tvb, ipv6_tvb);
    add_new_data_source(pinfo, ipv6_tvb, "6LoWPAN header decompression");

    /
    call_dissector(ipv6_handle, ipv6_tvb, pinfo, proto_tree_get_root(tree));
    return NULL;
} /
