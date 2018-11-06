static void
dissect_pktc(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree)
{
    guint8 kmmid, doi, version;
    int offset=0;
    proto_tree *pktc_tree = NULL;
    proto_item *item = NULL;

    if (check_col(pinfo->cinfo, COL_PROTOCOL))
        col_set_str(pinfo->cinfo, COL_PROTOCOL, "PKTC");

    if (tree) {
        item = proto_tree_add_item(tree, proto_pktc, tvb, 0, 3, FALSE);
        pktc_tree = proto_item_add_subtree(item, ett_pktc);
    }

    /
    kmmid=tvb_get_guint8(tvb, offset);
    proto_tree_add_uint(pktc_tree, hf_pktc_kmmid, tvb, offset, 1, kmmid);
    offset+=1;

    /
    doi=tvb_get_guint8(tvb, offset);
    proto_tree_add_uint(pktc_tree, hf_pktc_doi, tvb, offset, 1, doi);
    offset+=1;
    
    /
    version=tvb_get_guint8(tvb, offset);
    proto_tree_add_text(pktc_tree, tvb, offset, 1, "Version: %d.%d", (version>>4)&0x0f, (version)&0x0f);
    proto_tree_add_uint_hidden(pktc_tree, hf_pktc_version_major, tvb, offset, 1, (version>>4)&0x0f);
    proto_tree_add_uint_hidden(pktc_tree, hf_pktc_version_minor, tvb, offset, 1, (version)&0x0f);
    offset+=1;

    /
    if (check_col(pinfo->cinfo, COL_INFO)) {
        col_add_str(pinfo->cinfo, COL_INFO, 
		    val_to_str(kmmid, kmmid_types, "Unknown KMMID %#x"));
	col_append_fstr(pinfo->cinfo, COL_INFO, " (%s)",
		        val_to_str(doi, doi_types, "Unknown DOI %#x"));
    }

    switch(kmmid){
    case KMMID_WAKEUP:
        offset=dissect_pktc_wakeup(pktc_tree, tvb, offset);
	break;
    case KMMID_AP_REQUEST:
        offset=dissect_pktc_ap_request(pinfo, pktc_tree, tvb, offset, doi);
        break;
    case KMMID_AP_REPLY:
        offset=dissect_pktc_ap_reply(pinfo, pktc_tree, tvb, offset, doi);
        break;
    case KMMID_SEC_PARAM_REC:
        offset=dissect_pktc_sec_param_rec(pktc_tree, tvb, offset);
	break;
    case KMMID_REKEY:
        offset=dissect_pktc_rekey(pinfo, pktc_tree, tvb, offset, doi);
	break;
    case KMMID_ERROR_REPLY:
        offset=dissect_pktc_error_reply(pinfo, pktc_tree, tvb, offset);
	break;
    };

    proto_item_set_len(item, offset);
}
