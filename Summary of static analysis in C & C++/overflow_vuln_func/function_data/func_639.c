static int dissect_logcat_text_time(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree,
        void *data _U_) {
    static const tGETTER getters[] = { get_time, get_priority, get_tag, get_pid, get_log };
    dissect_info_t dinfo = { time_regex, getters, array_length(getters) };

    return dissect_logcat_text(tvb, tree, pinfo, &dinfo);
}
