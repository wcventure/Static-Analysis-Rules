static int dissect_logcat_text(tvbuff_t *tvb, proto_tree *tree, packet_info *pinfo,
        const dissect_info_t *dinfo) {
    gchar **tokens;
    guint i;
    gchar *frame = tvb_get_string_enc(wmem_packet_scope(), tvb, 0, tvb_captured_length(tvb),
            ENC_ASCII);
    proto_item *mainitem = proto_tree_add_item(tree, proto_logcat_text, tvb, 0, -1, ENC_NA);
    proto_tree *maintree = proto_item_add_subtree(mainitem, ett_logcat);
    gint offset = 0;

    col_set_str(pinfo->cinfo, COL_PROTOCOL, dissector_name);

    if (!g_regex_match(special_regex, frame, G_REGEX_MATCH_NOTEMPTY, NULL)) {

        tokens = g_regex_split(dinfo->regex, frame, G_REGEX_MATCH_NOTEMPTY);
        if (NULL == tokens) return 0;
        if (g_strv_length(tokens) != dinfo->no_of_getters + 2) {
            proto_tree_add_expert(maintree, pinfo, &ei_malformed_token, tvb, offset, -1);
            g_strfreev(tokens);
            return 0;
        }

        for (i = 0; i < dinfo->no_of_getters; ++i) {
            offset = ((*dinfo->getters[i])(frame, tokens[i + 1], tvb, maintree, offset, pinfo));
        }
    } else {
        tokens = g_regex_split(special_regex, frame, G_REGEX_MATCH_NOTEMPTY);
        if (NULL == tokens) return 0;
        offset = get_log(frame, tokens[1], tvb, maintree, 0, pinfo);
    }
    g_strfreev(tokens);
    return offset;
}
