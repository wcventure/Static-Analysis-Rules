static int
dissect_pktc_ap_request(packet_info *pinfo, proto_tree *tree, tvbuff_t *tvb, int offset, guint8 doi)
{
    tvbuff_t *pktc_tvb;
    guint32 snonce;

    /
    pktc_tvb = tvb_new_subset(tvb, offset, -1, -1); 
    offset += dissect_kerberos_main(pktc_tvb, pinfo, tree, FALSE, NULL);

    /
    snonce=tvb_get_ntohl(tvb, offset);
    proto_tree_add_uint(tree, hf_pktc_server_nonce, tvb, offset, 4, snonce);
    offset+=4;

    /
    offset=dissect_pktc_app_specific_data(pinfo, tree, tvb, offset, doi, KMMID_AP_REQUEST);

    /
    offset=dissect_pktc_list_of_ciphersuites(pinfo, tree, tvb, offset, doi);

    /
    proto_tree_add_item(tree, hf_pktc_reestablish_flag, tvb, offset, 1, FALSE);
    offset+=1;

    /
    proto_tree_add_item(tree, hf_pktc_sha1_hmac, tvb, offset, 20, FALSE);
    offset+=20;

    return offset;
}
