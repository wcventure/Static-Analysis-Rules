static struct lowpan_nhdr *
dissect_6lowpan_iphc_nhc(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, gint offset)
{
    gint                length;
    proto_item *        ti = NULL;
    proto_tree *        nhc_tree = NULL;
    struct lowpan_nhdr *nhdr;

    /
    if (tvb_get_bits8(tvb, offset<<3, LOWPAN_NHC_PATTERN_EXT_BITS) == LOWPAN_NHC_PATTERN_EXT) {
        struct ip6_ext  ipv6_ext;
        guint8          ext_flags;
        guint8          ext_len;
        guint8          ext_proto;

        /
        ext_proto = lowpan_parse_nhc_proto(tvb, offset);

        /
        if (tree) {
            ti = proto_tree_add_text(tree, tvb, 0, sizeof(guint16), "IPv6 extension header");
            nhc_tree = proto_item_add_subtree(ti, ett_6lowpan_nhc_ext);
            /
            proto_tree_add_bits_item(nhc_tree, hf_6lowpan_nhc_pattern, tvb, offset<<3, LOWPAN_NHC_PATTERN_EXT_BITS, FALSE);
        }

        /
        ext_flags = tvb_get_guint8(tvb, offset);
        if (tree) {
            proto_tree_add_uint(nhc_tree, hf_6lowpan_nhc_ext_eid, tvb, offset, sizeof(guint8), ext_flags & LOWPAN_NHC_EXT_EID);
            proto_tree_add_boolean(nhc_tree, hf_6lowpan_nhc_ext_nh, tvb, offset, sizeof(guint8), ext_flags & LOWPAN_NHC_EXT_NHDR);
        }
        offset += sizeof(guint8);

        /
        if (!(ext_flags & LOWPAN_NHC_EXT_NHDR)) {
            ipv6_ext.ip6e_nxt = tvb_get_guint8(tvb, offset);
            if (tree) {
                proto_tree_add_uint_format(nhc_tree, hf_6lowpan_nhc_ext_next, tvb, offset, sizeof(guint8), ipv6_ext.ip6e_nxt,
                            "Next header: %s (0x%02x)", ipprotostr(ipv6_ext.ip6e_nxt), ipv6_ext.ip6e_nxt);
                proto_item_set_end(ti, tvb, offset+sizeof(guint8));
            }
            offset += sizeof(guint8);
        }

        /
        ext_len = tvb_get_guint8(tvb, offset);
        if (tree) {
            proto_tree_add_uint(nhc_tree, hf_6lowpan_nhc_ext_length, tvb, offset, sizeof(guint8), ext_len);
        }
        offset += sizeof(guint8);

        /
        length = sizeof(struct ip6_ext) + ext_len;
        length = (length + 7) & ~0x7;

        /
        nhdr = ep_alloc0(sizeof(struct lowpan_nhdr) + sizeof(struct ip6_ext) + length);
        nhdr->next = NULL;
        nhdr->proto = ext_proto;
        nhdr->length = length + sizeof(struct ip6_ext);
        nhdr->reported = length + sizeof(struct ip6_ext);

        /
        if (ext_flags & LOWPAN_NHC_EXT_NHDR) {
            ipv6_ext.ip6e_nxt = lowpan_parse_nhc_proto(tvb, offset+ext_len);
        }
        ipv6_ext.ip6e_len = nhdr->reported>>3;  /
        memcpy(nhdr->hdr, &ipv6_ext, sizeof(struct ip6_ext));

        /
        if (!tvb_bytes_exist(tvb, offset, ext_len)) {
            /
            call_dissector(data_handle, tvb_new_subset(tvb, offset, -1, -1), pinfo, nhc_tree);

            /
            nhdr->length = tvb_length_remaining(tvb, offset) + sizeof(struct ip6_ext);
            tvb_memcpy(tvb, nhdr->hdr + sizeof(struct ip6_ext), offset, tvb_length_remaining(tvb, offset));

            /
            return nhdr;
        }

        /
        call_dissector(data_handle, tvb_new_subset(tvb, offset, ext_len, ext_len), pinfo, nhc_tree);

        /
        tvb_memcpy(tvb, nhdr->hdr + sizeof(struct ip6_ext), offset, ext_len);
        offset += ext_len;

        if (!(ext_flags & LOWPAN_NHC_EXT_NHDR)) {
            /
            nhdr->next = ep_alloc(sizeof(struct lowpan_nhdr) + tvb_length_remaining(tvb, offset));
            nhdr->next->next = NULL;
            nhdr->next->proto = ipv6_ext.ip6e_nxt;
            nhdr->next->length = tvb_length_remaining(tvb, offset);
            nhdr->next->reported = tvb_reported_length_remaining(tvb, offset);
            tvb_memcpy(tvb, nhdr->next->hdr, offset, length);
        }
        else {
            /
            nhdr->next = dissect_6lowpan_iphc_nhc(tvb, pinfo, tree, offset);
        }

        /
        return nhdr;
    }
    /
    if (tvb_get_bits8(tvb, offset<<3, LOWPAN_NHC_PATTERN_UDP_BITS) == LOWPAN_NHC_PATTERN_UDP) {
        struct udp_hdr  udp;
        gint            src_bitlen;
        gint            dst_bitlen;
        guint8          udp_flags;

        /
        if (tree) {
            ti = proto_tree_add_text(tree, tvb, 0, sizeof(guint8), "UDP header compression");
            nhc_tree = proto_item_add_subtree(ti, ett_6lowpan_nhc_udp);
            /
            proto_tree_add_bits_item(nhc_tree, hf_6lowpan_nhc_pattern, tvb, offset<<3, LOWPAN_NHC_PATTERN_UDP_BITS, FALSE);
        }

        /
        udp_flags = tvb_get_guint8(tvb, offset);
        if (tree) {
            proto_tree_add_boolean(nhc_tree, hf_6lowpan_nhc_udp_checksum, tvb, offset, sizeof(guint8), udp_flags & LOWPAN_NHC_UDP_CHECKSUM);
            proto_tree_add_boolean(nhc_tree, hf_6lowpan_nhc_udp_src, tvb, offset, sizeof(guint8), udp_flags & LOWPAN_NHC_UDP_SRCPORT);
            proto_tree_add_boolean(nhc_tree, hf_6lowpan_nhc_udp_dst, tvb, offset, sizeof(guint8), udp_flags & LOWPAN_NHC_UDP_DSTPORT);
        }
        offset += sizeof(guint8);

        /
        switch (udp_flags & (LOWPAN_NHC_UDP_SRCPORT | LOWPAN_NHC_UDP_DSTPORT)) {
            case (LOWPAN_NHC_UDP_SRCPORT | LOWPAN_NHC_UDP_DSTPORT):
                udp.src_port = LOWPAN_PORT_12BIT_OFFSET;
                udp.dst_port = LOWPAN_PORT_12BIT_OFFSET;
                src_bitlen = 4;
                dst_bitlen = 4;
                break;

            case LOWPAN_NHC_UDP_SRCPORT:
                udp.src_port = LOWPAN_PORT_8BIT_OFFSET;
                udp.dst_port = 0;
                src_bitlen = 8;
                dst_bitlen = 16;
                break;

            case LOWPAN_NHC_UDP_DSTPORT:
                udp.src_port = 0;
                udp.dst_port = LOWPAN_PORT_8BIT_OFFSET;
                src_bitlen = 16;
                dst_bitlen = 8;
                break;

            default:
                udp.src_port = 0;
                udp.dst_port = 0;
                src_bitlen = 16;
                dst_bitlen = 16;
                break;
        } /

        /
        udp.src_port += tvb_get_bits16(tvb, offset<<3, src_bitlen, FALSE);
        if (tree) {
            proto_tree_add_uint(tree, hf_6lowpan_udp_src, tvb, offset, BITS_TO_BYTE_LEN(offset<<3, src_bitlen), udp.src_port);
        }
        /
        udp.dst_port += tvb_get_bits16(tvb, (offset<<3)+src_bitlen, dst_bitlen, FALSE);
        if (tree) {
            proto_tree_add_uint(tree, hf_6lowpan_udp_dst, tvb, offset, BITS_TO_BYTE_LEN((offset<<3)+src_bitlen, dst_bitlen), udp.dst_port);
        }
        offset += ((src_bitlen + dst_bitlen)>>3);
        udp.src_port = g_ntohs(udp.src_port);
        udp.dst_port = g_ntohs(udp.dst_port);

        /
        if (!(udp_flags & LOWPAN_NHC_UDP_CHECKSUM)) {
            /
            udp.checksum = tvb_get_ntohs(tvb, offset);
            if (tree) {
                proto_tree_add_uint(tree, hf_6lowpan_udp_checksum, tvb, offset, sizeof(guint16), udp.checksum);
            }
            offset += sizeof(guint16);
            udp.checksum = g_ntohs(udp.checksum);
        }
        else {
            udp.checksum = 0;
        }

        /
        length = tvb_reported_length_remaining(tvb, offset);
        udp.length = g_ntohs(length + sizeof(struct udp_hdr));

        /
        if ((udp_flags & LOWPAN_NHC_UDP_CHECKSUM) && tvb_bytes_exist(tvb, offset, length)) {
            vec_t      cksum_vec[3];
            struct {
                struct e_in6_addr   src;
                struct e_in6_addr   dst;
                guint8              zero;
                guint8              proto;
                guint16             length;
            } cksum_phdr;
            guint16                 cksum;

            /
            memcpy(&cksum_phdr.src, pinfo->src.data, sizeof(struct e_in6_addr));
            memcpy(&cksum_phdr.dst, pinfo->dst.data, sizeof(struct e_in6_addr));
            cksum_phdr.zero = 0;
            cksum_phdr.proto = IP_PROTO_UDP;
            cksum_phdr.length = udp.length;

            /
            cksum_vec[0].ptr = (const guint8 *)&cksum_phdr;
            cksum_vec[0].len = sizeof(cksum_phdr);
            cksum_vec[1].ptr = (const guint8 *)&udp;
            cksum_vec[1].len = sizeof(struct udp_hdr);
            cksum_vec[2].ptr = tvb_get_ptr(tvb, offset, length);
            cksum_vec[2].len = length;
            cksum = in_cksum(cksum_vec, 3);
            udp.checksum = g_ntohs((cksum)?(cksum):(~cksum));
        }

        /
        nhdr = ep_alloc(sizeof(struct lowpan_nhdr) + sizeof(struct udp_hdr) + tvb_length_remaining(tvb, offset));
        nhdr->next = NULL;
        nhdr->proto = IP_PROTO_UDP;
        nhdr->length = tvb_length_remaining(tvb, offset) + sizeof(struct udp_hdr);
        nhdr->reported = g_ntohs(udp.length);

        /
        memcpy(nhdr->hdr, &udp, sizeof(struct udp_hdr));
        tvb_memcpy(tvb, nhdr->hdr + sizeof(struct udp_hdr), offset, length);
        return nhdr;
    }
    /
    return NULL;
} /
