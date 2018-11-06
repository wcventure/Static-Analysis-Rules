static guint16
dissect_ecdh_parameter(tvbuff_t * tvb, const gint offset, proto_tree * tree)
{
    proto_item	*ecdh_item;
    proto_tree	*ecdh_tree;
    proto_item	*ecdh_id_item;
    guint8	ecdh_id;
    guint16	ecdh_len;
    const gchar	*id_name = "unknown";

    ecdh_id = tvb_get_guint8(tvb, offset);

    if (ecdh_id == 1) {
        id_name = "OID";
    } else if (ecdh_id == 2) {
        id_name = "Detailed parameter";
    }

    ecdh_len = tvb_get_ntohs(tvb, offset+1);

    ecdh_item = proto_tree_add_item(tree, hf_wai_ecdh, tvb, offset, ecdh_len+3, FALSE);
    ecdh_tree = proto_item_add_subtree(ecdh_item, ett_wai_ecdh_param);
    ecdh_id_item = proto_tree_add_item(ecdh_tree, hf_wai_ecdh_id, tvb, offset, 1, FALSE);
    proto_item_set_text(ecdh_id_item, "ID: %s (%#x)", id_name, ecdh_id);
    proto_tree_add_item(ecdh_tree, hf_wai_ecdh_len, tvb, offset+1, 2, FALSE);
    proto_tree_add_item(ecdh_tree, hf_wai_ecdh_content, tvb, offset+2, ecdh_len, FALSE);

    return ecdh_len + 3;
}
