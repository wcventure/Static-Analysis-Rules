static tvbuff_t *
dissect_6lowpan_hc1(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree)
{
    gint                offset = 0;
    gint                bit_offset;
    int                 i;
    guint8              hc1_encoding;
    guint8              hc_udp_encoding = 0;
    guint8              next_header;
    proto_tree *        hc_tree = NULL;
    proto_item *        ti;
    tvbuff_t *          ipv6_tvb;
    /
    guint8              ipv6_class;
    struct ip6_hdr      ipv6;
    struct lowpan_nhdr *nhdr_list;

    /
    /
    if (tree) {
        ti = proto_tree_add_text(tree, tvb, 0, sizeof(guint16), "HC1 Encoding");
        hc_tree = proto_item_add_subtree(ti, ett_6lowpan_hc1);

	/
        proto_tree_add_bits_item(hc_tree, hf_6lowpan_pattern, tvb, 0, LOWPAN_PATTERN_HC1_BITS, FALSE);
    }
    offset += sizeof(guint8);

    /
    hc1_encoding = tvb_get_guint8(tvb, offset);
    next_header = ((hc1_encoding & LOWPAN_HC1_NEXT) >> 1);
    if (tree) {
        proto_tree_add_boolean(hc_tree, hf_6lowpan_hc1_source_prefix, tvb, offset, sizeof(guint8), hc1_encoding & LOWPAN_HC1_SOURCE_PREFIX);
        proto_tree_add_boolean(hc_tree, hf_6lowpan_hc1_source_ifc, tvb, offset, sizeof(guint8), hc1_encoding & LOWPAN_HC1_SOURCE_IFC);
        proto_tree_add_boolean(hc_tree, hf_6lowpan_hc1_dest_prefix, tvb, offset, sizeof(guint8), hc1_encoding & LOWPAN_HC1_DEST_PREFIX);
        proto_tree_add_boolean(hc_tree, hf_6lowpan_hc1_dest_ifc, tvb, offset, sizeof(guint8), hc1_encoding & LOWPAN_HC1_DEST_IFC);
        proto_tree_add_boolean(hc_tree, hf_6lowpan_hc1_class, tvb, offset, sizeof(guint8), hc1_encoding & LOWPAN_HC1_TRAFFIC_CLASS);
        proto_tree_add_uint(hc_tree, hf_6lowpan_hc1_next, tvb, offset, sizeof(guint8), hc1_encoding & LOWPAN_HC1_NEXT);
        proto_tree_add_boolean(hc_tree, hf_6lowpan_hc1_more, tvb, offset, sizeof(guint8), hc1_encoding & LOWPAN_HC1_MORE);
    }
    offset += sizeof(guint8);

    /
    if (hc1_encoding & LOWPAN_HC1_MORE) {
        if (next_header == LOWPAN_HC1_NEXT_UDP) {
            hc_udp_encoding = tvb_get_guint8(tvb, offset);
            if (tree) {
                ti = proto_tree_add_text(tree, tvb, 0, sizeof(guint8), "HC_UDP Encoding");
                hc_tree = proto_item_add_subtree(ti, ett_6lowpan_hc2_udp);
                proto_tree_add_boolean(hc_tree, hf_6lowpan_hc2_udp_src, tvb, offset, sizeof(guint8), hc_udp_encoding & LOWPAN_HC2_UDP_SRCPORT);
                proto_tree_add_boolean(hc_tree, hf_6lowpan_hc2_udp_dst, tvb, offset, sizeof(guint8), hc_udp_encoding & LOWPAN_HC2_UDP_DSTPORT);
                proto_tree_add_boolean(hc_tree, hf_6lowpan_hc2_udp_len, tvb, offset, sizeof(guint8), hc_udp_encoding & LOWPAN_HC2_UDP_LENGTH);
            }
            offset += sizeof(guint8);
        }
        else {
            /
            expert_add_info_format(pinfo, NULL, PI_MALFORMED, PI_ERROR, "HC1 more bits expected for illegal next header type.");
            return NULL;
        }
    }

    /
    /
    bit_offset = offset << 3;

    /
    ipv6_class = 0;
    ipv6.ip6_flow = 0;
    if (!(hc1_encoding & LOWPAN_HC1_TRAFFIC_CLASS)) {
        /
        ipv6_class = tvb_get_bits8(tvb, bit_offset, LOWPAN_IPV6_TRAFFIC_CLASS_BITS);
        if (tree) {
            proto_tree_add_uint(tree, hf_6lowpan_traffic_class, tvb, bit_offset>>3,
                    BITS_TO_BYTE_LEN(bit_offset, LOWPAN_IPV6_TRAFFIC_CLASS_BITS), ipv6_class);
        }
        bit_offset += LOWPAN_IPV6_TRAFFIC_CLASS_BITS;

        /
        ipv6.ip6_flow = tvb_get_bits32(tvb, bit_offset, LOWPAN_IPV6_FLOW_LABEL_BITS, FALSE);
        if (tree) {
            proto_tree_add_uint(tree, hf_6lowpan_flow_label, tvb, bit_offset>>3,
                    BITS_TO_BYTE_LEN(bit_offset, LOWPAN_IPV6_FLOW_LABEL_BITS), ipv6.ip6_flow);
        }
        bit_offset += LOWPAN_IPV6_FLOW_LABEL_BITS;
    }
    ipv6.ip6_flow = g_ntohl(ipv6.ip6_flow | (ipv6_class << LOWPAN_IPV6_FLOW_LABEL_BITS));
    ipv6.ip6_vfc = ((0x6 << 4) | (ipv6_class >> 4));

    /
    if (next_header == LOWPAN_HC1_NEXT_UDP) {
        ipv6.ip6_nxt = IP_PROTO_UDP;
    }
    else if (next_header == LOWPAN_HC1_NEXT_ICMP) {
        ipv6.ip6_nxt = IP_PROTO_ICMPV6;
    }
    else if (next_header == LOWPAN_HC1_NEXT_TCP) {
        ipv6.ip6_nxt = IP_PROTO_TCP;
    }
    else {
        /
        ipv6.ip6_nxt = tvb_get_bits8(tvb, bit_offset, LOWPAN_IPV6_NEXT_HEADER_BITS);
        if (tree) {
            proto_tree_add_uint_format(tree, hf_6lowpan_next_header, tvb, bit_offset>>3,
                    BITS_TO_BYTE_LEN(bit_offset, LOWPAN_IPV6_NEXT_HEADER_BITS), ipv6.ip6_nxt,
                    "Next header: %s (0x%02x)", ipprotostr(ipv6.ip6_nxt), ipv6.ip6_nxt);
        }
        bit_offset += LOWPAN_IPV6_NEXT_HEADER_BITS;
    }

    ipv6.ip6_hops = tvb_get_bits8(tvb, bit_offset, LOWPAN_IPV6_HOP_LIMIT_BITS);
    if (tree) {
        proto_tree_add_uint(tree, hf_6lowpan_hop_limit, tvb, bit_offset>>3,
                BITS_TO_BYTE_LEN(bit_offset, LOWPAN_IPV6_HOP_LIMIT_BITS), ipv6.ip6_hops);
    }
    bit_offset += LOWPAN_IPV6_HOP_LIMIT_BITS;

    /
    offset = bit_offset;
    if (!(hc1_encoding & LOWPAN_HC1_SOURCE_PREFIX)) {
        for (i=0; i<8; i++, bit_offset += 8) {
            ipv6.ip6_src.bytes[i] = tvb_get_bits8(tvb, bit_offset, 8);
        }
    }
    else {
        memcpy(ipv6.ip6_src.bytes, lowpan_llprefix, sizeof(lowpan_llprefix));
    }
    if (!(hc1_encoding & LOWPAN_HC1_SOURCE_IFC)) {
        for (i=8; i<16; i++, bit_offset += 8) {
            ipv6.ip6_src.bytes[i] = tvb_get_bits8(tvb, bit_offset, 8);
        }
    }
    /
    else if (pinfo->src.type == AT_EUI64) {
        memcpy(&ipv6.ip6_src.bytes[8], pinfo->src.data, 8);
    }
    /
    else {
        lowpan_dlsrc_to_ifcid(pinfo, &ipv6.ip6_src.bytes[8]);
    }
    /
    if (tree) {
        proto_tree_add_ipv6(tree, hf_6lowpan_source, tvb, offset>>3,
                BITS_TO_BYTE_LEN(offset, (bit_offset-offset)), (guint8 *)&ipv6.ip6_src);
    }

    /
    offset = bit_offset;
    if (!(hc1_encoding & LOWPAN_HC1_DEST_PREFIX)) {
        for (i=0; i<8; i++, bit_offset += 8) {
            ipv6.ip6_dst.bytes[i] = tvb_get_bits8(tvb, bit_offset, 8);
        }
    }
    else {
        memcpy(ipv6.ip6_dst.bytes, lowpan_llprefix, sizeof(lowpan_llprefix));
    }
    if (!(hc1_encoding & LOWPAN_HC1_DEST_IFC)) {
        for (i=8; i<16; i++, bit_offset += 8) {
            ipv6.ip6_dst.bytes[i] = tvb_get_bits8(tvb, bit_offset, 8);
        }
    }
    /
    else if (pinfo->dst.type == AT_EUI64) {
        memcpy(&ipv6.ip6_dst.bytes[8], pinfo->dst.data, 8);
    }
    /
    else {
        lowpan_dldst_to_ifcid(pinfo, &ipv6.ip6_dst.bytes[8]);
    }
    /
    if (tree) {
        proto_tree_add_ipv6(tree, hf_6lowpan_dest, tvb, offset>>3,
                BITS_TO_BYTE_LEN(offset, (bit_offset-offset)), (guint8 *)&ipv6.ip6_dst);
    }

    /
    if ((hc1_encoding & LOWPAN_HC1_MORE) && (next_header == LOWPAN_HC1_NEXT_UDP)) {
        struct udp_hdr  udp;
        gint            length;

        /
        offset = bit_offset;
        if (hc_udp_encoding & LOWPAN_HC2_UDP_SRCPORT) {
            udp.src_port = tvb_get_bits8(tvb, bit_offset, LOWPAN_UDP_PORT_COMPRESSED_BITS) + LOWPAN_PORT_12BIT_OFFSET;
            bit_offset += LOWPAN_UDP_PORT_COMPRESSED_BITS;
        }
        else {
            udp.src_port = tvb_get_bits16(tvb, bit_offset, LOWPAN_UDP_PORT_BITS, FALSE);
            bit_offset += LOWPAN_UDP_PORT_BITS;
        }
        if (tree) {
            proto_tree_add_uint(tree, hf_6lowpan_udp_src, tvb, offset>>3,
                    BITS_TO_BYTE_LEN(offset, (bit_offset-offset)), udp.src_port);
        }
        udp.src_port = g_ntohs(udp.src_port);

        /
        offset = bit_offset;
        if (hc_udp_encoding & LOWPAN_HC2_UDP_DSTPORT) {
            udp.dst_port = tvb_get_bits8(tvb, bit_offset, LOWPAN_UDP_PORT_COMPRESSED_BITS) + LOWPAN_PORT_12BIT_OFFSET;
            bit_offset += LOWPAN_UDP_PORT_COMPRESSED_BITS;
        }
        else {
            udp.dst_port = tvb_get_bits16(tvb, bit_offset, LOWPAN_UDP_PORT_BITS, FALSE);
            bit_offset += LOWPAN_UDP_PORT_BITS;
        }
        if (tree) {
            proto_tree_add_uint(tree, hf_6lowpan_udp_dst, tvb, offset>>3,
                    BITS_TO_BYTE_LEN(offset, (bit_offset-offset)), udp.dst_port);
        }
        udp.dst_port = g_ntohs(udp.dst_port);

        /
        if (!(hc1_encoding & LOWPAN_HC2_UDP_LENGTH)) {
            udp.length = tvb_get_bits16(tvb, bit_offset, LOWPAN_UDP_LENGTH_BITS, FALSE);
            if (tree) {
                proto_tree_add_uint(tree, hf_6lowpan_udp_len, tvb, bit_offset>>3,
                        BITS_TO_BYTE_LEN(bit_offset, LOWPAN_UDP_LENGTH_BITS), udp.length);

            }
            bit_offset += LOWPAN_UDP_LENGTH_BITS;
        }
        /
        else {
            udp.length = tvb_reported_length(tvb);
            udp.length -= BITS_TO_BYTE_LEN(0, bit_offset + LOWPAN_UDP_CHECKSUM_BITS);
            udp.length += sizeof(struct udp_hdr);
        }
        udp.length = g_ntohs(udp.length);

        /
        udp.checksum = tvb_get_bits16(tvb, bit_offset, LOWPAN_UDP_CHECKSUM_BITS, FALSE);
        if (tree) {
            proto_tree_add_uint(tree, hf_6lowpan_udp_checksum, tvb, bit_offset>>3,
                    BITS_TO_BYTE_LEN(bit_offset, LOWPAN_UDP_CHECKSUM_BITS), udp.checksum);
        }
        bit_offset += LOWPAN_UDP_CHECKSUM_BITS;
        udp.checksum = g_ntohs(udp.checksum);

        /
        offset = BITS_TO_BYTE_LEN(0, bit_offset);
        length = tvb_length_remaining(tvb, offset);
        nhdr_list = ep_alloc(sizeof(struct lowpan_nhdr) + sizeof(struct udp_hdr) + length);
        nhdr_list->next = NULL;
        nhdr_list->proto = IP_PROTO_UDP;
        nhdr_list->length = length + sizeof(struct udp_hdr);
        nhdr_list->reported = g_ntohs(udp.length);

        /
        memcpy(nhdr_list->hdr, &udp, sizeof(struct udp_hdr));
        tvb_memcpy(tvb, nhdr_list->hdr + sizeof(struct udp_hdr), offset, length);
    }
    /
    else {
        offset = BITS_TO_BYTE_LEN(0, bit_offset);
        nhdr_list = ep_alloc(sizeof(struct lowpan_nhdr) + tvb_length_remaining(tvb, offset));
        nhdr_list->next = NULL;
        nhdr_list->proto = ipv6.ip6_nxt;
        nhdr_list->length = tvb_length_remaining(tvb, offset);
        nhdr_list->reported = tvb_reported_length_remaining(tvb, offset);
        tvb_memcpy(tvb, nhdr_list->hdr, offset, nhdr_list->length);
    }

    /
    ipv6_tvb = lowpan_reassemble_ipv6(&ipv6, nhdr_list);
    tvb_set_child_real_data_tvbuff(tvb, ipv6_tvb);
    add_new_data_source(pinfo, ipv6_tvb, "6LoWPAN header decompression");

    /
    call_dissector(ipv6_handle, ipv6_tvb, pinfo, proto_tree_get_root(tree));
    return NULL;
} /
