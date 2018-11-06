static int
dissect_pktc_rekey(packet_info *pinfo, proto_tree *tree, tvbuff_t *tvb, int offset, guint8 doi)
{
    guint32 snonce;
    guint string_len;
    const guint8 *timestr;

    /
    snonce=tvb_get_ntohl(tvb, offset);
    proto_tree_add_uint(tree, hf_pktc_server_nonce, tvb, offset, 4, snonce);
    offset+=4;

    /
    string_len=tvb_strsize(tvb, offset);
    proto_tree_add_item(tree, hf_pktc_server_principal, tvb, offset, string_len, FALSE);
    offset+=string_len;

    /
    /
    timestr=tvb_get_ptr(tvb, offset, 13);
    proto_tree_add_string_format(tree, hf_pktc_timestamp, tvb, offset, 13, timestr, 
                                "%s: %.2s-%.2s-%.2s %.2s:%.2s:%.2s", 
                                proto_registrar_get_name(hf_pktc_timestamp),
				 timestr, timestr+2, timestr+4, timestr+6, timestr+8, timestr+10);
    offset+=13;

    /
    offset=dissect_pktc_app_specific_data(pinfo, tree, tvb, offset, doi, KMMID_REKEY);

    /
    offset=dissect_pktc_list_of_ciphersuites(pinfo, tree, tvb, offset, doi);

    /
    proto_tree_add_item(tree, hf_pktc_sec_param_lifetime, tvb, offset, 4, FALSE);
    offset+=4;

    /
    proto_tree_add_item(tree, hf_pktc_grace_period, tvb, offset, 4, FALSE);
    offset+=4;

    /
    proto_tree_add_item(tree, hf_pktc_reestablish_flag, tvb, offset, 1, FALSE);
    offset+=1;

    /
    proto_tree_add_item(tree, hf_pktc_sha1_hmac, tvb, offset, 20, FALSE);
    offset+=20;

    return offset;
}
