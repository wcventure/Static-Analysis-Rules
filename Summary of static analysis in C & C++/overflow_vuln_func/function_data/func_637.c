static int dissect_logcat_text_tag(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree,
        void *data _U_) {
    static const tGETTER getters[] = { get_priority, get_tag, get_log };
    dissect_info_t dinfo = { tag_regex, getters, array_length(getters) };

    return dissect_logcat_text(tvb, tree, pinfo, &dinfo);
}
