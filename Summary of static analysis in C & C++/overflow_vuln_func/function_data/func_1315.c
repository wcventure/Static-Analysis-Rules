static void
dissect_pktc_mtafqdn(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree)
{
    int offset=0;
    proto_tree *pktc_mtafqdn_tree = NULL;
    proto_item *item = NULL;
    tvbuff_t *pktc_mtafqdn_tvb;

    if (check_col(pinfo->cinfo, COL_PROTOCOL))
        col_set_str(pinfo->cinfo, COL_PROTOCOL, "PKTC");

    if (tree) {
        item = proto_tree_add_item(tree, proto_pktc, tvb, 0, 0, FALSE);
        pktc_mtafqdn_tree = proto_item_add_subtree(item, ett_pktc_mtafqdn);
    }

    if (check_col(pinfo->cinfo, COL_INFO)) {
        col_add_fstr(pinfo->cinfo, COL_INFO, "MTA FQDN %s",
                    pinfo->srcport == pinfo->match_port ? "Reply":"Request");
    }


    /
    pktc_mtafqdn_tvb = tvb_new_subset(tvb, offset, -1, -1); 
    offset += dissect_kerberos_main(pktc_mtafqdn_tvb, pinfo, pktc_mtafqdn_tree, FALSE, NULL);

    /
    pktc_mtafqdn_tvb = tvb_new_subset(tvb, offset, -1, -1); 
    offset += dissect_kerberos_main(pktc_mtafqdn_tvb, pinfo, pktc_mtafqdn_tree, FALSE, cb);

    proto_item_set_len(item, offset);
}
