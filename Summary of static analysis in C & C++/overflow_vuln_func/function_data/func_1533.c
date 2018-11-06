static int
dissect_sir(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, void* data _U_)
{
    guint8      version;
    guint32     val_len;
    guint32     val_len_save;
    guint32     len;
    guint32     offset = 0;
    guint32     i;
    tvbuff_t   *tmp_tvb;
    proto_tree *subtree;
    proto_item *ti;

    /
    col_append_str(pinfo->cinfo, COL_INFO, ": WAP Session Initiation Request");

    ti = proto_tree_add_item(tree, hf_sir_section,
            tvb, 0, -1, ENC_NA);
    subtree = proto_item_add_subtree(ti, ett_sir);

    /
    version = tvb_get_guint8(tvb, 0);
    proto_tree_add_uint(subtree, hf_sir_version,
            tvb, 0, 1, version);

    /
    val_len = tvb_get_guintvar(tvb, 1, &len);
    proto_tree_add_uint(subtree, hf_sir_app_id_list_len,
            tvb, 1, len, val_len);
    offset = 1 + len;
    /
    tmp_tvb = tvb_new_subset_length(tvb, offset, val_len);
    add_headers (subtree, tmp_tvb, hf_sir_app_id_list, pinfo);
    offset += val_len;

    /
    val_len = tvb_get_guintvar(tvb, offset, &len);
    proto_tree_add_uint(subtree, hf_sir_wsp_contact_points_len,
            tvb, offset, len, val_len);
    offset += len;
    /
    tmp_tvb = tvb_new_subset_length (tvb, offset, val_len);
    add_addresses(subtree, tmp_tvb, hf_sir_wsp_contact_points);

    /
    if (version == 0)
        return offset;

    offset += val_len;

    /
    val_len = tvb_get_guintvar(tvb, offset, &len);
    proto_tree_add_uint(subtree, hf_sir_contact_points_len,
            tvb, offset, len, val_len);
    offset += len;
    /
    tmp_tvb = tvb_new_subset_length(tvb, offset, val_len);
    add_addresses(subtree, tmp_tvb, hf_sir_contact_points);

    offset += val_len;

    /
    val_len = tvb_get_guintvar(tvb, offset, &len);
    proto_tree_add_uint(subtree, hf_sir_protocol_options_len,
            tvb, offset, len, val_len);
    offset += len;
    /

    val_len_save = val_len;
    for (i = 0; i < val_len_save; i++) {
        val_len = tvb_get_guintvar(tvb, offset, &len);
        proto_tree_add_uint(subtree, hf_sir_protocol_options,
                tvb, offset, len, val_len);
        offset += len;
    }

    /
    val_len = tvb_get_guintvar(tvb, offset, &len);
    proto_tree_add_uint(subtree, hf_sir_prov_url_len,
            tvb, offset, len, val_len);
    offset += len;
    /
    proto_tree_add_item (tree, hf_sir_prov_url,
            tvb, offset, val_len, ENC_ASCII|ENC_NA);
    offset += val_len;

    /
    val_len = tvb_get_guintvar(tvb, offset, &len);
    proto_tree_add_uint(subtree, hf_sir_cpi_tag_len,
            tvb, offset, len, val_len);
    offset += len;

    /
    for (i = 0; i < val_len; i++) {
        proto_tree_add_item(subtree, hf_sir_cpi_tag,
                            tvb, offset, 4, ENC_NA);
        offset += 4;
    }
    return tvb_captured_length(tvb);
}
