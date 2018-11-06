static void
dissect_ospf(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree)
{
    proto_tree *ospf_tree = NULL;
    proto_item *ti;
    proto_tree *ospf_header_tree;
    guint8  version;
    guint8  packet_type;
    guint16 ospflen;
    vec_t cksum_vec[4];
    int cksum_vec_len;
    guint32 phdr[2];
    guint16 cksum, computed_cksum;
    guint length, reported_length;
    guint16 auth_type;
    char auth_data[8+1];
    int crypto_len = 0;
    unsigned int ospf_header_length;
    guint8 instance_ID;
    guint8 reserved;
    guint32 areaid;


    if (check_col(pinfo->cinfo, COL_PROTOCOL))
	col_set_str(pinfo->cinfo, COL_PROTOCOL, "OSPF");
    if (check_col(pinfo->cinfo, COL_INFO))
	col_clear(pinfo->cinfo, COL_INFO);

    version = tvb_get_guint8(tvb, 0);
    switch (version) {
        case OSPF_VERSION_2:
            ospf_header_length = OSPF_VERSION_2_HEADER_LENGTH;
            break;
        case OSPF_VERSION_3:
            ospf_header_length = OSPF_VERSION_3_HEADER_LENGTH;
            break;
        default:
	    ospf_header_length = 14;
            break;
    }

    packet_type = tvb_get_guint8(tvb, 1);
    if (check_col(pinfo->cinfo, COL_INFO)) {
	col_add_str(pinfo->cinfo, COL_INFO,
		    val_to_str(packet_type, pt_vals, "Unknown (%u)"));
    }

    if (tree) {
	ospflen = tvb_get_ntohs(tvb, 2);

	ti = proto_tree_add_item(tree, proto_ospf, tvb, 0, -1, FALSE);
	ospf_tree = proto_item_add_subtree(ti, ett_ospf);

	ti = proto_tree_add_text(ospf_tree, tvb, 0, ospf_header_length,
				 "OSPF Header");
	ospf_header_tree = proto_item_add_subtree(ti, ett_ospf_hdr);

        proto_tree_add_text(ospf_header_tree, tvb, 0, 1, "OSPF Version: %u",
			    version);
	proto_tree_add_item(ospf_header_tree, ospf_filter[OSPFF_MSG_TYPE],
			    tvb, 1, 1, FALSE);
	proto_tree_add_item_hidden(ospf_header_tree,
				   ospf_filter[ospf_msg_type_to_filter(packet_type)],
				   tvb, 1, 1, FALSE);
 	proto_tree_add_text(ospf_header_tree, tvb, 2, 2, "Packet Length: %u",
			    ospflen);
	proto_tree_add_item(ospf_header_tree, ospf_filter[OSPFF_SRC_ROUTER],
			    tvb, 4, 4, FALSE);
	areaid=tvb_get_ntohl(tvb,8);
	proto_tree_add_text(ospf_header_tree, tvb, 8, 4, "Area ID: %s%s",
			       ip_to_str(tvb_get_ptr(tvb, 8, 4)), areaid == 0 ? " (Backbone)" : "");

	/
	switch (version) {

	case OSPF_VERSION_2:
	case OSPF_VERSION_3:
	    break;

	default:
	    cksum = tvb_get_ntohs(tvb, 12);
	    if (cksum == 0) {
		/
		proto_tree_add_text(ospf_header_tree, tvb, 12, 2,
		    "Packet Checksum: 0x%04x (none)", cksum);
	    } else {
		proto_tree_add_text(ospf_header_tree, tvb, 12, 2,
		    "Packet Checksum: 0x%04x", cksum);
	    }
	    proto_tree_add_text(ospf_tree, tvb, 14, -1,
		"Unknown OSPF version %u", version);
	    return;
	}

	cksum = tvb_get_ntohs(tvb, 12);
	length = tvb_length(tvb);
	/
	reported_length = tvb_reported_length(tvb);
	if (cksum == 0) {
	    /
	    proto_tree_add_text(ospf_header_tree, tvb, 12, 2,
		"Packet Checksum: 0x%04x (none)", cksum);
	} else if (!pinfo->fragmented && length >= reported_length
		&& length >= ospf_header_length) {
	    /

	    switch (version) {

	    case OSPF_VERSION_2:
		/
		cksum_vec[0].ptr = tvb_get_ptr(tvb, 0, 16);
		cksum_vec[0].len = 16;
		if (length > ospf_header_length) {
		    /
		    reported_length -= ospf_header_length;
		    cksum_vec[1].ptr = tvb_get_ptr(tvb, ospf_header_length, reported_length);
		    cksum_vec[1].len = reported_length;
		    cksum_vec_len = 2;
		} else {
		    /
		    cksum_vec_len = 1;
		}
		break;

	    case OSPF_VERSION_3:
		/

		/
		cksum_vec[0].ptr = pinfo->src.data;
		cksum_vec[0].len = pinfo->src.len;
		cksum_vec[1].ptr = pinfo->dst.data;
		cksum_vec[1].len = pinfo->dst.len;
		cksum_vec[2].ptr = (const guint8 *)&phdr;
	        phdr[0] = g_htonl(ospflen);
	        phdr[1] = g_htonl(IP_PROTO_OSPF);
	        cksum_vec[2].len = 8;

		cksum_vec[3].ptr = tvb_get_ptr(tvb, 0, reported_length);
		cksum_vec[3].len = reported_length;
		cksum_vec_len = 4;
		break;

	    default:
		DISSECTOR_ASSERT_NOT_REACHED();
		cksum_vec_len = 0;
		break;
	    }
	    computed_cksum = in_cksum(cksum_vec, cksum_vec_len);
	    if (computed_cksum == 0) {
		proto_tree_add_text(ospf_header_tree, tvb, 12, 2,
			"Packet Checksum: 0x%04x [correct]", cksum);
	    } else {
		proto_tree_add_text(ospf_header_tree, tvb, 12, 2,
			"Packet Checksum: 0x%04x [incorrect, should be 0x%04x]",
			cksum, in_cksum_shouldbe(cksum, computed_cksum));
	    }
	} else {
	    proto_tree_add_text(ospf_header_tree, tvb, 12, 2,
	    	"Packet Checksum: 0x%04x", cksum);
	}


	switch (version) {

	case OSPF_VERSION_2:
	    /
            auth_type = tvb_get_ntohs(tvb, 14);
	    proto_tree_add_text(ospf_header_tree, tvb, 14, 2, "Auth Type: %s",
		    	    val_to_str(auth_type, auth_vals, "Unknown (%u)"));
	    switch (auth_type) {

	    case OSPF_AUTH_NONE:
	        proto_tree_add_text(ospf_header_tree, tvb, 16, 8, "Auth Data (none)");
	        break;

            case OSPF_AUTH_SIMPLE:
	        tvb_get_nstringz0(tvb, 16, 8+1, auth_data);
	        proto_tree_add_text(ospf_header_tree, tvb, 16, 8, "Auth Data: %s", auth_data);
	        break;

	    case OSPF_AUTH_CRYPT:
	        proto_tree_add_text(ospf_header_tree, tvb, 18, 1, "Auth Key ID: %u",
				tvb_get_guint8(tvb, 18));
	        crypto_len = tvb_get_guint8(tvb, 19);
	        proto_tree_add_text(ospf_header_tree, tvb, 19, 1, "Auth Data Length: %u",
				crypto_len);
	        proto_tree_add_text(ospf_header_tree, tvb, 20, 4, "Auth Crypto Sequence Number: 0x%x",
				tvb_get_ntohl(tvb, 20));

	        /
	        if (tvb_bytes_exist(tvb, ospflen, crypto_len)) {
		    proto_tree_add_text(ospf_header_tree, tvb, ospflen, crypto_len,
				    "Auth Data: %s",
				    tvb_bytes_to_str(tvb, ospflen, crypto_len));
	        }
	        break;

	    default:
	        proto_tree_add_text(ospf_header_tree, tvb, 16, 8, "Auth Data (unknown)");
	        break;
	    }
	    break;

	case OSPF_VERSION_3:
	    /
	    instance_ID = tvb_get_guint8(tvb, 14);
 	    proto_tree_add_text(ospf_header_tree, tvb, 14, 1, "Instance ID: %u",
			    instance_ID);
 	    reserved = tvb_get_guint8(tvb, 15);
	    proto_tree_add_text(ospf_header_tree, tvb, 15, 1, (reserved == 0 ? "Reserved: %u" : "Reserved: %u [incorrect, should be 0]"),
				reserved);
	    break;
	}

	switch (packet_type){

	case OSPF_HELLO:
	    dissect_ospf_hello(tvb, ospf_header_length, ospf_tree, version, 
			    ospflen - ospf_header_length);
	    break;

	case OSPF_DB_DESC:
	    dissect_ospf_db_desc(tvb, ospf_header_length, ospf_tree, version, 
			    ospflen - ospf_header_length);
	    break;

	case OSPF_LS_REQ:
	    dissect_ospf_ls_req(tvb, ospf_header_length, ospf_tree, version, 
			    ospflen - ospf_header_length);
	    break;

	case OSPF_LS_UPD:
	    dissect_ospf_ls_upd(tvb, ospf_header_length, ospf_tree, version, 
			    ospflen - ospf_header_length);
	    break;

	case OSPF_LS_ACK:
	    dissect_ospf_ls_ack(tvb, ospf_header_length, ospf_tree, version, 
			    ospflen - ospf_header_length);
	    break;

	default:
	    call_dissector(data_handle,
	        tvb_new_subset(tvb, ospf_header_length, -1, -1), pinfo, tree);
	    break;
	}

	/
	if (ospf_has_lls_block(tvb, ospf_header_length, packet_type))
	    dissect_ospf_lls_data_block(tvb, ospflen + crypto_len, ospf_tree, 
			    version);
    }
}
