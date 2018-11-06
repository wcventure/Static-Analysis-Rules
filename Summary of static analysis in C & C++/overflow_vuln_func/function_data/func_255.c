static int dissect_uaudp(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, void* data _U_)
{
    /
    if (use_sys_ip) {
        /
        if (memcmp((pinfo->src).data, sys_ip, 4*sizeof(guint8)) == 0)
        {
            _dissect_uaudp(tvb, pinfo, tree, SYS_TO_TERM);
            return tvb_captured_length(tvb);
        }
        else if (memcmp((pinfo->dst).data, sys_ip, 4*sizeof(guint8)) == 0)
        {
            _dissect_uaudp(tvb, pinfo, tree, TERM_TO_SYS);
            return tvb_captured_length(tvb);
        }
    }

    /
    if (find_terminal_port(pinfo->srcport))
    {
        _dissect_uaudp(tvb, pinfo, tree, TERM_TO_SYS);
        return tvb_captured_length(tvb);
    }
    else if (find_terminal_port(pinfo->destport))
    {
        _dissect_uaudp(tvb, pinfo, tree, SYS_TO_TERM);
        return tvb_captured_length(tvb);
    }

    _dissect_uaudp(tvb, pinfo, tree, DIR_UNKNOWN);
    return tvb_captured_length(tvb);
}
