static gint dissect_file_leader(proto_tree *gvsp_tree, tvbuff_t *tvb, packet_info *pinfo _U_, gint offset)
{
    guint file_length = 0;

    /
    proto_tree_add_item(gvsp_tree, hf_gvsp_payloadtype, tvb, offset + 2, 2, ENC_BIG_ENDIAN);

    /
    proto_tree_add_item(gvsp_tree, hf_gvsp_timestamp, tvb, offset + 4, 8, ENC_BIG_ENDIAN);

    /
    proto_tree_add_item(gvsp_tree, hf_gvsp_payloaddatasize, tvb, offset + 12, 8, ENC_BIG_ENDIAN);

    /
    file_length = tvb_strsize(tvb, offset + 20);
    proto_tree_add_item(gvsp_tree, hf_gvsp_filename, tvb, offset + 20, file_length, ENC_ASCII|ENC_NA);

    /
    return 20 + file_length;
}
