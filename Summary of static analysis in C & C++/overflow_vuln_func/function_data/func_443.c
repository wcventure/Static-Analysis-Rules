static int
decode_prefix4(proto_tree *tree, int hf_addr, tvbuff_t *tvb, gint offset,
    guint16 tlen, char *tag)
{
    proto_item *ti;
    proto_tree *prefix_tree;
    union {
       guint8 addr_bytes[4];
       guint32 addr;
    } ip_addr;        /
    guint8 plen;      /
    int    length;    /

    /
    plen = tvb_get_guint8(tvb, offset);
    if (plen > 32) {
	proto_tree_add_text(tree, tvb, offset, 1, "%s length %u invalid",
	    tag, plen);
	return -1;
    }
    length = (plen + 7) / 8;

    /
    memset((void *)&ip_addr, 0, sizeof(ip_addr));
    tvb_memcpy(tvb, ip_addr.addr_bytes, offset + 1, length);
    if (plen % 8)
	ip_addr.addr_bytes[length - 1] &= ((0xff00 >> (plen % 8)) & 0xff);

    /
    ti = proto_tree_add_text(tree, tvb, offset,
            tlen != 0 ? tlen : 1 + length, "%s/%u",
	ip_to_str(ip_addr.addr_bytes), plen);
    prefix_tree = proto_item_add_subtree(ti, ett_bgp_prefix);
    proto_tree_add_text(prefix_tree, tvb, offset, 1, "%s prefix length: %u",
	tag, plen);
    if (hf_addr != -1) {
	proto_tree_add_ipv4(prefix_tree, hf_addr, tvb, offset + 1, length,
	    ip_addr.addr);
    } else {
	proto_tree_add_text(prefix_tree, tvb, offset + 1, length,
	    "%s prefix: %s", tag, ip_to_str(ip_addr.addr_bytes));
    }
    return(1 + length);
}
