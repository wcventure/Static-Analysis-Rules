static guint
fProcessId (tvbuff_t *tvb, proto_tree *tree, guint offset)
{
    guint32     val = 0, lvt;
    guint8      tag_no, tag_info;
    proto_item *ti;
    proto_tree *subtree;
    guint       tag_len;

    tag_len = fTagHeader (tvb, offset, &tag_no, &tag_info, &lvt);
    if (fUnsigned32 (tvb, offset+tag_len, lvt, &val))
        ti = proto_tree_add_uint(tree, hf_bacapp_tag_ProcessId,
            tvb, offset, lvt+tag_len, val);
    else
        ti = proto_tree_add_text(tree, tvb, offset, lvt+tag_len,
            "Process Identifier - %u octets (Signed)", lvt);
    subtree = proto_item_add_subtree(ti, ett_bacapp_tag);
    fTagHeaderTree(tvb, subtree, offset, &tag_no, &tag_info, &lvt);
    offset += tag_len + lvt;

    return offset;
}
