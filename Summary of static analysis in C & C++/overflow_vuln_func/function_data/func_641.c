static int dissect_logcat_text_threadtime(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree,
        void *data _U_) {
    static const tGETTER getters[] = { get_time, get_pid, get_tid, get_priority, get_tag, get_log };
    dissect_info_t dinfo = { threadtime_regex, getters, array_length(getters) };

    return dissect_logcat_text(tvb, tree, pinfo, &dinfo);
}
