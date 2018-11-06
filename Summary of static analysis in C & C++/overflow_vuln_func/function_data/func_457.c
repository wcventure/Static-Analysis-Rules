static void
dissect_wai(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree)
{
/
#define WAI_MESSAGE_LENGTH	12 /
#define WAI_DATA_OFFSET 	WAI_MESSAGE_LENGTH
    guint16		version;
    guint8		type;
    guint8		subtype;
    guint16		length;
    guint16		packet_num;
    guint8		fragment_num;
    guint8		flags;
    fragment_data	*frag_msg;
    const gchar		*subtype_name = "Unknown type";

    length = tvb_get_ntohs(tvb, 6)-WAI_MESSAGE_LENGTH;
    subtype = tvb_get_guint8(tvb, 3);

    /
    if ((length != tvb_reported_length (tvb)-WAI_MESSAGE_LENGTH) || (subtype > WAI_SUB_MULTICAST_ANNOUNCE_RESP)) {
        return;
    }

    col_set_str(pinfo->cinfo, COL_PROTOCOL, "WAI");
    col_clear (pinfo->cinfo, COL_INFO);
    version = tvb_get_ntohs(tvb, 0);

    if (version == 1) {
        subtype_name = val_to_str_const(subtype, wai_subtype_names, "Unknown type");
    }
    col_append_fstr(pinfo->cinfo, COL_INFO, "%s", subtype_name);

    /
    type = tvb_get_guint8(tvb, 2);
    packet_num = tvb_get_ntohs(tvb, 8);
    fragment_num = tvb_get_guint8(tvb, 10);
    flags = tvb_get_guint8(tvb, 11);

    frag_msg =  fragment_add_seq_check (tvb, WAI_DATA_OFFSET, pinfo,
    					packet_num,
    					wai_fragment_table,
    					wai_reassembled_table,
    					fragment_num,
    					length,
    					flags);

    if (tree) {
        proto_item	*wai_item;
        proto_tree	*wai_tree;
        tvbuff_t	*next_tvb;
        tvbuff_t	*new_tvb;


        wai_item = proto_tree_add_item(tree, proto_wai, tvb, 0, -1, ENC_NA);

        proto_item_set_text (wai_item, "WAI Protocol (%s)",
        			val_to_str_const(subtype, wai_subtype_names, "Unknown type"));
        wai_tree = proto_item_add_subtree(wai_item, ett_wai);

        /
        proto_tree_add_item(wai_tree, hf_wai_version, tvb, 0, 2, FALSE);
        proto_tree_add_item(wai_tree, hf_wai_type, tvb, 2, 1, FALSE);
        proto_tree_add_item(wai_tree, hf_wai_subtype, tvb, 3, 1, FALSE);
        proto_tree_add_item(wai_tree, hf_wai_reserved, tvb, 4, 2, FALSE);
        proto_tree_add_item(wai_tree, hf_wai_length, tvb, 6,2, FALSE);
        proto_tree_add_item(wai_tree, hf_wai_seq, tvb, 8, 2, FALSE);
        proto_tree_add_item(wai_tree, hf_wai_fragm_seq, tvb, 10, 1, FALSE);
        proto_tree_add_item(wai_tree, hf_wai_flag, tvb, 11, 1, FALSE);

        next_tvb = tvb_new_subset_remaining(tvb, WAI_DATA_OFFSET);

        /
        if (flags) {
            col_add_fstr(pinfo->cinfo, COL_INFO,
            		"Fragment (%d) of message, data not dissected", fragment_num);

            process_reassembled_data(tvb, WAI_DATA_OFFSET, pinfo,
                                     "Reassembled WAI", frag_msg, &wai_frag_items,
                                     NULL, wai_tree);

            call_dissector(data_handle, next_tvb, pinfo, tree);
        } else {
            /
            if (fragment_num > 0) {
                new_tvb = process_reassembled_data(tvb, WAI_DATA_OFFSET, pinfo, "Reassembled WAI", frag_msg, &wai_frag_items,
                					NULL, wai_tree);

                if (new_tvb) {
                    col_add_str(pinfo->cinfo, COL_INFO, "Last fragment of message, data dissected");
                    col_append_sep_str(pinfo->cinfo, COL_INFO, ": ", subtype_name);
                    next_tvb=new_tvb;
                    length = tvb_reported_length (next_tvb);
                }
            }
            /
            dissect_wai_data(next_tvb, wai_tree, subtype, length);
        }
    }

}
