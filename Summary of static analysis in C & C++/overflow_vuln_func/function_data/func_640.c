static int dissect_logcat_text_thread(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree,
        void *data _U_) {
    static const tGETTER getters[] = { get_priority, get_pid, get_tid, get_log };
    dissect_info_t dinfo = { thread_regex, getters, array_length(getters) };

    SET_ADDRESS(&pinfo->dst, AT_STRINGZ, 0, "");
    SET_ADDRESS(&pinfo->src, AT_STRINGZ, 0, "");

    return dissect_logcat_text(tvb, tree, pinfo, &dinfo);
}
