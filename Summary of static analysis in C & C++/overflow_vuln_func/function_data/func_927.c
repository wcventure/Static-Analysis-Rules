static guint16 skip_item(proto_tree *meta_tree, tvbuff_t *tvb, packet_info *pinfo _U_, guint16 offs)
{
    guint16     id;
    guint8      type, len, aligned_len, total_len;
    proto_tree *item_tree;
    proto_item *subti;

    id          = tvb_get_letohs(tvb, offs); offs += 2;
    type        = tvb_get_guint8(tvb, offs); offs++;
    len         = tvb_get_guint8(tvb, offs); offs++;
    aligned_len = (len + 3) & 0xfffc;
    total_len   = aligned_len + 4; /

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

    return total_len;
}
