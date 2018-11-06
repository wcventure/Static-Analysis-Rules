static guint
fMacAddress (tvbuff_t *tvb, proto_tree *tree, guint offset, const gchar *label, guint32 lvt)
{
    gchar *tmp;
    guint start = offset;
    guint8 tag_no, tag_info;
    proto_tree* subtree = tree;
    proto_item* ti = 0;

    offset += fTagHeader (tvb, offset, &tag_no, &tag_info, &lvt);

    ti = proto_tree_add_text(tree, tvb, offset, 6, "%s", label); /

    if (lvt > 0) {
        if (lvt == 6) { /

            guint32 ip   = tvb_get_ipv4(tvb, offset);
            guint16 port = tvb_get_ntohs(tvb, offset+4);

            proto_tree_add_ipv4(tree, hf_bacapp_tag_IPV4, tvb, offset, 4, ip);
            proto_tree_add_uint(tree, hf_bacapp_tag_PORT, tvb, offset+4, 2, port);

        } else {
            if (lvt == 18) { /
            struct e_in6_addr addr;
            guint16 port =  tvb_get_ntohs(tvb, offset+16);
            tvb_get_ipv6(tvb, offset, &addr);

            proto_tree_add_ipv6(tree, hf_bacapp_tag_IPV6, tvb, offset, 16, (const guint8 *) &addr);
            proto_tree_add_uint(tree, hf_bacapp_tag_PORT, tvb, offset+16, 2, port);

            } else { /
                tmp = tvb_bytes_to_str(tvb, offset, lvt);
                ti = proto_tree_add_text(tree, tvb, offset, lvt, "%s", tmp);
            }
        }
        offset += lvt;
    }

    if (ti)
        subtree = proto_item_add_subtree(ti, ett_bacapp_tag);

    fTagHeaderTree(tvb, subtree, start, &tag_no, &tag_info, &lvt);

    return offset;
}
