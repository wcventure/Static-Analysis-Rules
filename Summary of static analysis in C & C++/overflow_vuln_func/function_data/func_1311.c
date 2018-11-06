static int
dissect_pktc_ap_reply(packet_info *pinfo, proto_tree *tree, tvbuff_t *tvb, int offset, guint8 doi)
{
    tvbuff_t *pktc_tvb;

    /
    pktc_tvb = tvb_new_subset(tvb, offset, -1, -1); 
    offset += dissect_kerberos_main(pktc_tvb, pinfo, tree, FALSE, NULL);

    /
    offset=dissect_pktc_app_specific_data(pinfo, tree, tvb, offset, doi, KMMID_AP_REPLY);

    /
    offset=dissect_pktc_list_of_ciphersuites(pinfo, tree, tvb, offset, doi);

    /
    proto_tree_add_uint_format(tree, hf_pktc_sec_param_lifetime, tvb, offset, 4,
                               tvb_get_ntohl(tvb, offset), "%s: %s",
                               proto_registrar_get_name(hf_pktc_sec_param_lifetime),
                               time_secs_to_str(tvb_get_ntohl(tvb, offset)));
    offset+=4;

    /
    proto_tree_add_item(tree, hf_pktc_grace_period, tvb, offset, 4, FALSE);
    offset+=4;

    /
    proto_tree_add_item(tree, hf_pktc_reestablish_flag, tvb, offset, 1, FALSE);
    offset+=1;

    /
    proto_tree_add_item(tree, hf_pktc_ack_required_flag, tvb, offset, 1, FALSE);
    offset+=1;

    /
    proto_tree_add_item(tree, hf_pktc_sha1_hmac, tvb, offset, 20, FALSE);
    offset+=20;

    return offset;
}
