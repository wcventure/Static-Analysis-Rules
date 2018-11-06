static int
dissect_pktc_error_reply(packet_info *pinfo, proto_tree *tree, tvbuff_t *tvb, int offset)
{
    tvbuff_t *pktc_tvb;

    /
    pktc_tvb = tvb_new_subset(tvb, offset, -1, -1); 
    offset += dissect_kerberos_main(pktc_tvb, pinfo, tree, FALSE, NULL);

    return offset;
}
