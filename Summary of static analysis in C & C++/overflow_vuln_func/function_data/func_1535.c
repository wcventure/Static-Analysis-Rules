static void
add_uri (proto_tree *tree, packet_info *pinfo, tvbuff_t *tvb,
        guint URILenOffset, guint URIOffset, proto_item *proto_ti)
{
    guint  count  = 0;
    guint  uriLen = tvb_get_guintvar (tvb, URILenOffset, &count);
    gchar *str;

    proto_tree_add_uint (tree, hf_wsp_header_uri_len,
            tvb, URILenOffset, count, uriLen);

    proto_tree_add_item (tree, hf_wsp_header_uri,
            tvb, URIOffset, uriLen, ENC_ASCII|ENC_NA);

    str = tvb_format_text (tvb, URIOffset, uriLen);
    /
    col_append_fstr(pinfo->cinfo, COL_INFO, " %s", str);

    if (proto_ti)
        proto_item_append_text(proto_ti, ", URI: %s", str);
}
