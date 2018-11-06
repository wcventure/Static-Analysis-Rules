static guint
fRestartReason (tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, guint offset)
{
    guint32      val   = 0;
    guint8       tag_no, tag_info;
    guint32      lvt;
    guint        tag_len;
    proto_item  *ti;
    proto_tree  *subtree;
    const gchar *label = "Restart Reason";

    tag_len = fTagHeader (tvb, offset, &tag_no, &tag_info, &lvt);
    if (fUnsigned32 (tvb, offset + tag_len, lvt, &val))
        ti = proto_tree_add_text(tree, tvb, offset, lvt+tag_len,
            "%s: %s (%u)", label,
            val_to_str(val,BACnetRestartReason,"Unknown reason"), val);
    else
        ti = proto_tree_add_text(tree, tvb, offset, lvt+tag_len,
            "%s - %u octets (Unsigned)", label, lvt);
    subtree = proto_item_add_subtree(ti, ett_bacapp_tag);
    fTagHeaderTree (tvb, subtree, offset, &tag_no, &tag_info, &lvt);

    if (lvt != 1) {
        proto_item *expert_item;
        expert_item = proto_tree_add_text(tree, tvb, 0, lvt, "Wrong length indicated. Expected 1, got %u", lvt);
        expert_add_info_format(pinfo, expert_item, PI_MALFORMED, PI_ERROR, "Wrong length indicated. Expected 1, got %u", lvt);
        PROTO_ITEM_SET_GENERATED(expert_item);
        return offset+tag_len+lvt;
    }

    proto_tree_add_item(subtree, hf_BACnetRestartReason, tvb,
        offset+tag_len, lvt, ENC_BIG_ENDIAN);

    return offset+tag_len+lvt;
}
