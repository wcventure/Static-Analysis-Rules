static int
dissect_pktc_app_specific_data(packet_info *pinfo _U_, proto_tree *parent_tree, tvbuff_t *tvb, int offset, guint8 doi, guint8 kmmid)
{
    int old_offset=offset;
    proto_tree *tree = NULL;
    proto_tree *engineid_tree = NULL;
    proto_item *item = NULL;
    proto_item *engineid_item = NULL;
    guint8 len;

    if (parent_tree) {
        item = proto_tree_add_item(parent_tree, hf_pktc_app_spec_data, tvb, offset, -1, FALSE);
        tree = proto_item_add_subtree(item, ett_pktc_app_spec_data);
    }

    switch(doi){
    case DOI_SNMPv3:
        switch(kmmid){
        /
        case KMMID_AP_REQUEST:
        case KMMID_AP_REPLY:
            /
            len=tvb_get_guint8(tvb, offset);
            proto_tree_add_uint(tree, hf_pktc_snmpEngineID_len, tvb, offset, 1, len);
            offset+=1;

            /
            engineid_item = proto_tree_add_item(tree, hf_pktc_snmpEngineID, tvb, offset, len, FALSE);
	    engineid_tree = proto_item_add_subtree(engineid_item, ett_pktc_engineid);
	    dissect_snmp_engineid(engineid_tree, tvb, offset, len);
            offset+=len;

            /
            proto_tree_add_item(tree, hf_pktc_snmpEngineBoots, tvb, offset, 4, FALSE);
            offset+=4;

            /
            proto_tree_add_item(tree, hf_pktc_snmpEngineTime, tvb, offset, 4, FALSE);
            offset+=4;

            /
            len=tvb_get_guint8(tvb, offset);
            proto_tree_add_uint(tree, hf_pktc_usmUserName_len, tvb, offset, 1, len);
            offset+=1;

            /
            proto_tree_add_item(tree, hf_pktc_usmUserName, tvb, offset, len, FALSE);
            offset+=len;

            break;
        default:
            proto_tree_add_text(tree, tvb, offset, 1, "Unknown KMMID");
            tvb_get_guint8(tvb, 9999); /
        };
        break;
    case DOI_IPSEC:
        switch(kmmid){
        /
        case KMMID_AP_REQUEST:
        case KMMID_AP_REPLY:
        case KMMID_REKEY:
            /
            proto_tree_add_item(tree, hf_pktc_ipsec_spi, tvb, offset, 4, FALSE);
            offset+=4;

	    break;
        default:
            proto_tree_add_text(tree, tvb, offset, 1, "Unknown KMMID");
            tvb_get_guint8(tvb, 9999); /
        };
	break;
    default:
        proto_tree_add_text(tree, tvb, offset, 1, "Unknown DOI");
        tvb_get_guint8(tvb, 9999); /
    }

    proto_item_set_len(item, offset-old_offset);
    return offset;
}
