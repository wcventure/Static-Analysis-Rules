static int
decode_prefix6(proto_tree *tree, int hf_addr, tvbuff_t *tvb, gint offset,
    guint16 tlen, char *tag)
{
    proto_item        *ti;
    proto_tree        *prefix_tree;
    struct e_in6_addr addr;     /
    int               plen;     /
    int               length;   /

    /
    plen = tvb_get_guint8(tvb, offset);
    if (plen > 128) {
	proto_tree_add_text(tree, tvb, offset, 1, "%s length %u invalid",
	    tag, plen);
	return -1;
    }
    length = (plen + 7) / 8;

    /
    memset(&addr, 0, sizeof(addr));
    tvb_memcpy(tvb, (guint8 *)&addr, offset + 1, length);
    if (plen % 8)
	addr.s6_addr[length - 1] &= ((0xff00 >> (plen % 8)) & 0xff);

    /
    ti = proto_tree_add_text(tree, tvb, offset,
            tlen != 0 ? tlen : 1 + length, "%s/%u",
            ip6_to_str(&addr), plen);
    prefix_tree = proto_item_add_subtree(ti, ett_bgp_prefix);
    proto_tree_add_text(prefix_tree, tvb, offset, 1, "%s prefix length: %u",
	tag, plen);
    if (hf_addr != -1) {
	proto_tree_add_ipv6(prefix_tree, hf_addr, tvb, offset + 1, length,
	    addr.s6_addr);
    } else {
	proto_tree_add_text(prefix_tree, tvb, offset + 1, length,
	    "%s prefix: %s", tag, ip6_to_str(&addr));
    }
    return(1 + length);
}
