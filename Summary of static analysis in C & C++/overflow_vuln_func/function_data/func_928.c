static guint16 evaluate_meta_item_pcap(proto_tree *meta_tree, tvbuff_t *tvb, packet_info *pinfo, guint16 offs)
{
    guint16     id;
    guint8      type, len, aligned_len, total_len;
    proto_tree *item_tree;
    proto_item *subti;
    /
    guint8      dir;
    guint64     ts;

    id = tvb_get_letohs(tvb, offs); offs += 2;
    type = tvb_get_guint8(tvb, offs); offs++;
    len = tvb_get_guint8(tvb, offs); offs++;
    aligned_len = (len + 3) & 0xfffc;
    total_len = aligned_len + 4; /

    switch (id) {
        case META_ID_DIRECTION:
            dir = tvb_get_guint8(tvb, offs);
            pinfo->p2p_dir = dir == META_DIR_UP ? P2P_DIR_RECV : P2P_DIR_SENT;
            proto_tree_add_uint(meta_tree, hf_meta_item_direction, tvb, offs, 1, dir);
            break;
        case META_ID_TIMESTAMP64:
            ts = tvb_get_letoh64(tvb, offs);
            proto_tree_add_uint64(meta_tree, hf_meta_item_ts, tvb, offs, 8, ts);
            break;
        case META_ID_SIGNALING:
            proto_tree_add_boolean(meta_tree, hf_meta_item_signaling, tvb,
                offs, 0, 1);
            break;
        case META_ID_INCOMPLETE:
            proto_tree_add_boolean(meta_tree, hf_meta_item_incomplete, tvb,
                offs, 0, 1);
            break;
        default:
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
    }
    return total_len;
}
