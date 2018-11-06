static int dissect_multipart(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, void* data)
{
    proto_tree *subtree;
    proto_item *ti;
    proto_item *type_ti;
    multipart_info_t *m_info = get_multipart_info(pinfo, (const char*)data);
    gint header_start = 0;
    gint body_index = 0;
    gboolean last_boundary = FALSE;

    if (m_info == NULL) {
        /
        proto_tree_add_expert(tree, pinfo, &ei_multipart_no_required_parameter, tvb, 0, -1);
        call_data_dissector(tvb, pinfo, tree);
        return tvb_reported_length(tvb);
    }

    /
    ti = proto_tree_add_item(tree, proto_multipart,
          tvb, 0, -1, ENC_NA);
    subtree = proto_item_add_subtree(ti, ett_multipart);
    proto_item_append_text(ti, ", Type: %s, Boundary: \"%s\"",
          m_info->type, m_info->boundary);

    /
    type_ti = proto_tree_add_string(subtree, hf_multipart_type,
          tvb, 0, 0, pinfo->match_string);
    PROTO_ITEM_SET_GENERATED(type_ti);

    /
    col_set_fence(pinfo->cinfo, COL_INFO);

    /
    header_start = process_preamble(subtree, tvb, m_info, &last_boundary);
    if (header_start == -1) {
        call_data_dissector(tvb, pinfo, subtree);
        return tvb_reported_length(tvb);
    }
    /
    while (last_boundary == FALSE) {
        header_start = process_body_part(subtree, tvb, m_info,
                pinfo, header_start, body_index++, &last_boundary);
        if (header_start == -1) {
            return tvb_reported_length(tvb);
        }
    }
    /
    if (tvb_reported_length_remaining(tvb, header_start) > 0) {
       proto_tree_add_item(subtree, hf_multipart_trailer, tvb, header_start, -1, ENC_NA);
    }

    return tvb_reported_length(tvb);
}
