static int
decode_prefix_MP(proto_tree *tree, int hf_addr4, int hf_addr6,
    guint16 afi, guint8 safi, tvbuff_t *tvb, gint offset, char *tag)
{
    int                 start_offset = offset;
    proto_item          *ti;
    proto_tree          *prefix_tree;
    int                 total_length;       /
    int                 length;             /
    guint               plen;               /
    guint               labnum;             /
    int                 ce_id,labblk_off;
    union {
       guint8 addr_bytes[4];
       guint32 addr;
    } ip4addr, ip4addr2;                    /
    struct e_in6_addr   ip6addr;            /
    guint16             rd_type;            /
    char                lab_stk[256];       /

    switch (afi) {

    case AFNUM_INET:
        switch (safi) {

        case SAFNUM_UNICAST:
        case SAFNUM_MULCAST:
        case SAFNUM_UNIMULC:
            total_length = decode_prefix4(tree, hf_addr4, tvb, offset, 0, tag);
            if (total_length < 0)
                return -1;
            break;

        case SAFNUM_MPLS_LABEL:
            plen =  tvb_get_guint8(tvb, offset);
            labnum = decode_MPLS_stack(tvb, offset + 1, lab_stk, sizeof(lab_stk));

            offset += (1 + labnum * 3);
            if (plen <= (labnum * 3*8)) {
                proto_tree_add_text(tree, tvb, start_offset, 1,
                        "%s IPv4 prefix length %u invalid", tag, plen);
                return -1;
            }
            plen -= (labnum * 3*8);
            if (plen > 32) {
                proto_tree_add_text(tree, tvb, start_offset, 1,
                        "%s IPv4 prefix length %u invalid",
                        tag, plen + (labnum * 3*8));
                return -1;
            }

            length = (plen + 7) / 8;
            memset((void *)&ip4addr, 0, sizeof(ip4addr));
            tvb_memcpy(tvb, (void *)ip4addr.addr_bytes, offset, length);
            if (plen % 8)
                ip4addr.addr_bytes[length - 1] &= ((0xff00 >> (plen % 8)) & 0xff);

            ti = proto_tree_add_text(tree, tvb, start_offset,
                    (offset + 1 + length) - start_offset,
                    "Label Stack=%s IP=%s/%u",
                    lab_stk, ip_to_str(ip4addr.addr_bytes), plen);
            prefix_tree = proto_item_add_subtree(ti, ett_bgp_prefix);
            if (hf_addr4 != -1) {
                proto_tree_add_ipv4(prefix_tree, hf_addr4, tvb, offset + 1,
                        length, ip4addr.addr);
            } else {
                proto_tree_add_text(prefix_tree, tvb, offset + 1, length,
                        "%s IPv4 prefix: %s",
                        tag, ip_to_str(ip4addr.addr_bytes));
            }
            total_length = (1 + labnum*3) + length;
            break;

        case SAFNUM_LAB_VPNUNICAST:
        case SAFNUM_LAB_VPNMULCAST:
        case SAFNUM_LAB_VPNUNIMULC:
            plen =  tvb_get_guint8(tvb, offset);
            labnum = decode_MPLS_stack(tvb, offset + 1, lab_stk, sizeof(lab_stk));

            offset += (1 + labnum * 3);
            if (plen <= (labnum * 3*8)) {
                proto_tree_add_text(tree, tvb, start_offset, 1,
                        "%s IPv4 prefix length %u invalid", tag, plen);
                return -1;
            }
            plen -= (labnum * 3*8);

            rd_type = tvb_get_ntohs(tvb, offset);
            if (plen <= 8*8) {
                proto_tree_add_text(tree, tvb, start_offset, 1,
                        "%s IPv4 prefix length %u invalid",
                        tag, plen + (labnum * 3*8));
                return -1;
            }
            plen -= 8*8;

            switch (rd_type) {

            case FORMAT_AS2_LOC: /
                if (plen > 32) {
                    proto_tree_add_text(tree, tvb, start_offset, 1,
                            "%s IPv4 prefix length %u invalid",
                            tag, plen + (labnum * 3*8) + 8*8);
                    return -1;
                }

                length = (plen + 7) / 8;
                memset(ip4addr.addr_bytes, 0, 4);
                tvb_memcpy(tvb, ip4addr.addr_bytes, offset + 8, length);
                if (plen % 8)
                    ip4addr.addr_bytes[length - 1] &= ((0xff00 >> (plen % 8)) & 0xff);

                ti = proto_tree_add_text(tree, tvb, start_offset,
                        (offset + 8 + length) - start_offset,
                        "Label Stack=%s RD=%u:%u, IP=%s/%u",
                        lab_stk,
                        tvb_get_ntohs(tvb, offset + 2),
                        tvb_get_ntohl(tvb, offset + 4),
                        ip_to_str(ip4addr.addr_bytes), plen);
                prefix_tree = proto_item_add_subtree(ti, ett_bgp_prefix);
                if (hf_addr4 != -1) {
                    proto_tree_add_ipv4(prefix_tree, hf_addr4, tvb,
                            offset + 8, length, ip4addr.addr);
                } else {
                    proto_tree_add_text(prefix_tree, tvb, offset + 8,
                            length, "%s IPv4 prefix: %s", tag,
                            ip_to_str(ip4addr.addr_bytes));
                }
                total_length = (1 + labnum * 3 + 8) + length;
                break;

            case FORMAT_IP_LOC: /
                tvb_memcpy(tvb, ip4addr.addr_bytes, offset + 2, 4);

                if (plen > 32) {
                        proto_tree_add_text(tree, tvb, start_offset, 1,
                                "%s IPv4 prefix length %u invalid",
                                tag, plen + (labnum * 3*8) + 8*8);
                        return -1;
                }

                length = (plen + 7) / 8;
                memset((void *)&ip4addr2, 0, sizeof(ip4addr2));
                tvb_memcpy(tvb, ip4addr2.addr_bytes, offset + 8, length);
                if (plen % 8)
                    ip4addr2.addr_bytes[length - 1] &= ((0xff00 >> (plen % 8)) & 0xff);

                ti = proto_tree_add_text(tree, tvb, start_offset,
                        (offset + 8 + length) - start_offset,
                        "Label Stack=%s RD=%s:%u, IP=%s/%u",
                        lab_stk,
                        ip_to_str(ip4addr.addr_bytes),
                        tvb_get_ntohs(tvb, offset + 6),
                        ip_to_str(ip4addr2.addr_bytes),
                        plen);
                total_length = (1 + labnum * 3 + 8) + length;
                break;

            default:
                proto_tree_add_text(tree, tvb, start_offset,
                        (offset - start_offset) + 2,
                        "Unknown labeled VPN address format %u", rd_type);
                return -1;
            }
            break;

        default:
            proto_tree_add_text(tree, tvb, start_offset, 0,
                    "Unknown SAFI (%u) for AFI %u", safi, afi);
            return -1;
        }
        break;

    case AFNUM_INET6:
        switch (safi) {

        case SAFNUM_UNICAST:
        case SAFNUM_MULCAST:
        case SAFNUM_UNIMULC:
            total_length = decode_prefix6(tree, hf_addr6, tvb, offset, 0, tag);
            if (total_length < 0)
                return -1;
            break;

        case SAFNUM_MPLS_LABEL:
            plen =  tvb_get_guint8(tvb, offset);
            labnum = decode_MPLS_stack(tvb, offset + 1, lab_stk, sizeof(lab_stk));

            offset += (1 + labnum * 3);
            if (plen <= (labnum * 3*8)) {
                proto_tree_add_text(tree, tvb, start_offset, 1,
                        "%s IPv6 prefix length %u invalid", tag, plen);
                return -1;
            }
            plen -= (labnum * 3*8);

	    length = (plen + 7) / 8;
	    memset(ip6addr.u6_addr.u6_addr8, 0, 16);
	    tvb_memcpy(tvb, ip6addr.u6_addr.u6_addr8, offset, length);
	    if (plen % 8)
		ip6addr.u6_addr.u6_addr8[length - 1] &= ((0xff00 >> (plen % 8)) & 0xff);

	    ti = proto_tree_add_text(tree, tvb, start_offset,
		 (offset + length) - start_offset,
                 "Label Stack=%s, IP=%s/%u",
                 lab_stk,
                 ip6_to_str(&ip6addr), plen);
	    total_length = (1 + labnum * 3) + length;
	    break;

        case SAFNUM_LAB_VPNUNICAST:
        case SAFNUM_LAB_VPNMULCAST:
        case SAFNUM_LAB_VPNUNIMULC:
            plen =  tvb_get_guint8(tvb, offset);
            labnum = decode_MPLS_stack(tvb, offset + 1, lab_stk, sizeof(lab_stk));

            offset += (1 + labnum * 3);
            if (plen <= (labnum * 3*8)) {
                proto_tree_add_text(tree, tvb, start_offset, 1,
                        "%s IPv6 prefix length %u invalid", tag, plen);
                return -1;
            }
            plen -= (labnum * 3*8);

            rd_type = tvb_get_ntohs(tvb,offset);
            if (plen <= 8*8) {
                proto_tree_add_text(tree, tvb, start_offset, 1,
                        "%s IPv6 prefix length %u invalid",
                        tag, plen + (labnum * 3*8));
                return -1;
            }
            plen -= 8*8;

            switch (rd_type) {

            case FORMAT_AS2_LOC:
                if (plen > 128) {
                    proto_tree_add_text(tree, tvb, start_offset, 1,
                            "%s IPv6 prefix length %u invalid",
                            tag, plen + (labnum * 3*8) + 8*8);
                    return -1;
                }

                length = (plen + 7) / 8;
                memset(ip6addr.u6_addr.u6_addr8, 0, 16);
                tvb_memcpy(tvb, ip6addr.u6_addr.u6_addr8, offset + 8, length);
                if (plen % 8)
                    ip6addr.u6_addr.u6_addr8[length - 1] &= ((0xff00 >> (plen % 8)) & 0xff);

                ti = proto_tree_add_text(tree, tvb, start_offset,
                        (offset + 8 + length) - start_offset,
                        "Label Stack=%s RD=%u:%u, IP=%s/%u",
                        lab_stk,
                        tvb_get_ntohs(tvb, offset + 2),
                        tvb_get_ntohl(tvb, offset + 4),
                        ip6_to_str(&ip6addr), plen);
                total_length = (1 + labnum * 3 + 8) + length;
                break;

            case FORMAT_IP_LOC: 
                tvb_memcpy(tvb, ip4addr.addr_bytes, offset + 2, 4);

                if (plen > 128) {
                    proto_tree_add_text(tree, tvb, start_offset, 1,
                            "%s IPv6 prefix length %u invalid",
                            tag, plen + (labnum * 3*8) + 8*8);
                    return -1;
                }
                length = (plen + 7) / 8;
                memset(ip6addr.u6_addr.u6_addr8, 0, 16);
                tvb_memcpy(tvb, ip6addr.u6_addr.u6_addr8, offset + 8, length);
                if (plen % 8)
                    ip6addr.u6_addr.u6_addr8[length - 1] &= ((0xff00 >> (plen % 8)) & 0xff);

                ti = proto_tree_add_text(tree, tvb, start_offset,
                        (offset + 8 + length) - start_offset,
                        "Label Stack=%s RD=%s:%u, IP=%s/%u",
                        lab_stk,
                        ip_to_str(ip4addr.addr_bytes),
                        tvb_get_ntohs(tvb, offset + 6),
                        ip6_to_str(&ip6addr), plen);
                total_length = (1 + labnum * 3 + 8) + length;
                break;

            default:
                proto_tree_add_text(tree, tvb, start_offset, 0,
                        "Unknown labeled VPN address format %u", rd_type);
                return -1;
            }
            break;

        default:
            proto_tree_add_text(tree, tvb, start_offset, 0,
                    "Unknown SAFI (%u) for AFI %u", safi, afi);
            return -1;
        }
        break;

    case AFNUM_L2VPN:
        switch (safi) {

        case SAFNUM_LAB_VPNUNICAST:
        case SAFNUM_LAB_VPNMULCAST:
        case SAFNUM_LAB_VPNUNIMULC:
            plen =  tvb_get_ntohs(tvb,offset);
            rd_type=tvb_get_ntohs(tvb,offset+2);
            ce_id=tvb_get_ntohs(tvb,offset+10);
            labblk_off=tvb_get_ntohs(tvb,offset+12);
            labnum = decode_MPLS_stack(tvb, offset + 14, lab_stk, sizeof(lab_stk));

            switch (rd_type) {

            case FORMAT_AS2_LOC:
                tvb_memcpy(tvb, ip4addr.addr_bytes, offset + 6, 4);
                proto_tree_add_text(tree, tvb, start_offset,
                        (offset + plen + 1) - start_offset,
                        "RD: %u:%s, CE-ID: %u, Label-Block Offset: %u, Label Base %s",
                        tvb_get_ntohs(tvb, offset + 4),
                        ip_to_str(ip4addr.addr_bytes),
                        ce_id,
                        labblk_off,
                        lab_stk);
                break;

            case FORMAT_IP_LOC:
                tvb_memcpy(tvb, ip4addr.addr_bytes, offset + 4, 4);
                proto_tree_add_text(tree, tvb, offset,
                        (offset + plen + 1) - start_offset,
                        "RD: %s:%u, CE-ID: %u, Label-Block Offset: %u, Label Base %s",
                        ip_to_str(ip4addr.addr_bytes),
                        tvb_get_ntohs(tvb, offset + 8),
                        ce_id,
                        labblk_off,
                        lab_stk);
                break;

            default:
                proto_tree_add_text(tree, tvb, start_offset,
                        (offset - start_offset) + 2,
                        "Unknown labeled VPN address format %u", rd_type);
                return -1;
            }
            /
            total_length = plen+2;
            break;

        default:
            proto_tree_add_text(tree, tvb, start_offset, 0,
                    "Unknown SAFI (%u) for AFI %u", safi, afi);
            return -1;
        }
        break;

    default:
        proto_tree_add_text(tree, tvb, start_offset, 0,
                "Unknown AFI (%u) value", afi);
        return -1;
    }
    return(total_length);
}
