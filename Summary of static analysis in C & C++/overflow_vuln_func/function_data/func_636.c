static int dissect_logcat_text_brief(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree,
        void *data _U_) {
    static const tGETTER getters[] = { get_priority, get_tag, get_pid, get_log };
    dissect_info_t dinfo = { brief_regex, getters, array_length(getters) };

    return dissect_logcat_text(tvb, tree, pinfo, &dinfo);
}
