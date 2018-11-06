static int
dissect_pktc_mtafqdn_krbsafeuserdata(packet_info *pinfo, tvbuff_t *tvb, proto_tree *tree)
{
    int offset=0, string_len=0;
    guint8 msgtype;
    guint32 bignum;
    nstime_t ts;

    /
    msgtype = tvb_get_guint8(tvb, offset);
    proto_tree_add_uint(tree, hf_pktc_mtafqdn_msgtype, tvb, offset, 1, msgtype);
    offset+=1;

    if (check_col(pinfo->cinfo, COL_INFO))
        col_set_str(pinfo->cinfo, COL_INFO, 
                   val_to_str(msgtype, pktc_mtafqdn_msgtype_vals, "MsgType %u"));

    /
    proto_tree_add_uint(tree, hf_pktc_mtafqdn_enterprise, tvb, offset, 4, tvb_get_ntohl(tvb, offset));
    offset+=4;

    /
    proto_tree_add_uint(tree, hf_pktc_mtafqdn_version, tvb, offset, 1, tvb_get_guint8(tvb, offset));
    offset+=1;

    switch(msgtype) {
    case PKTC_MTAFQDN_REQ:
        /
        proto_tree_add_ether(tree, hf_pktc_mtafqdn_mac, tvb, offset, 6, tvb_get_ptr(tvb, offset, 6));
       offset+=6;

       /
       proto_tree_add_item(tree, hf_pktc_mtafqdn_pub_key_hash, tvb, offset, 20, FALSE);
       offset+=20;

       /
       bignum = tvb_get_ntohl(tvb, offset);
       ts.secs = bignum;
       proto_tree_add_time_format(tree, hf_pktc_mtafqdn_manu_cert_revoked, tvb, offset, 4, 
                                  &ts, "%s: %s",
                                  proto_registrar_get_name(hf_pktc_mtafqdn_manu_cert_revoked),
                                  (bignum==0) ? "not revoked" : abs_time_secs_to_str(bignum));
       break;

    case PKTC_MTAFQDN_REP:
        /
        string_len = tvb_length_remaining(tvb, offset) - 4;
       proto_tree_add_item(tree, hf_pktc_mtafqdn_fqdn, tvb, offset, string_len, FALSE); 
       offset+=string_len;

        /
       tvb_memcpy(tvb, (guint8 *)&bignum, offset, sizeof(bignum));
       proto_tree_add_ipv4(tree, hf_pktc_mtafqdn_ip, tvb, offset, 4, bignum);

        break;
    }

    return offset;
}
