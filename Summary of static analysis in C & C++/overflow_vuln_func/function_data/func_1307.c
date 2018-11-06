static void
dissect_ospf_lsa_mpls(tvbuff_t *tvb, int offset, proto_tree *tree,
		      guint32 length)
{
    proto_item *ti;
    proto_tree *mpls_tree;
    proto_tree *tlv_tree;
    proto_tree *stlv_tree;
    proto_tree *stlv_admingrp_tree = NULL;

    int tlv_type;
    int tlv_length;
    int tlv_end_offset;

    int stlv_type, stlv_len, stlv_offset;
    const char *stlv_name;
    guint32 stlv_admingrp, mask;
    int i;
    guint8 switch_cap;

    ti = proto_tree_add_text(tree, tvb, offset, length,
			     "MPLS Traffic Engineering LSA");
    proto_tree_add_item_hidden(tree, ospf_filter[OSPFF_LS_MPLS],
			       tvb, offset, 2, FALSE);
    mpls_tree = proto_item_add_subtree(ti, ett_ospf_lsa_mpls);

    while (length != 0) {
	tlv_type = tvb_get_ntohs(tvb, offset);
	tlv_length = tvb_get_ntohs(tvb, offset + 2);
	tlv_end_offset = offset + tlv_length + 4;

	switch (tlv_type) {

	case MPLS_TLV_ROUTER:
	    ti = proto_tree_add_text(mpls_tree, tvb, offset, tlv_length+4,
				     "Router Address: %s",
				     ip_to_str(tvb_get_ptr(tvb, offset+4, 4)));
	    tlv_tree = proto_item_add_subtree(ti, ett_ospf_lsa_mpls_router);
	    proto_tree_add_text(tlv_tree, tvb, offset, 2, "TLV Type: 1 - Router Address");
	    proto_tree_add_text(tlv_tree, tvb, offset+2, 2, "TLV Length: %u",
	    			tlv_length);
	    proto_tree_add_item(tlv_tree, ospf_filter[OSPFF_LS_MPLS_ROUTERID],
				tvb, offset+4, 4, FALSE);
	    break;

	case MPLS_TLV_LINK:
	    ti = proto_tree_add_text(mpls_tree, tvb, offset, tlv_length+4,
				     "Link Information");
	    tlv_tree = proto_item_add_subtree(ti, ett_ospf_lsa_mpls_link);
	    proto_tree_add_text(tlv_tree, tvb, offset, 2, "TLV Type: 2 - Link Information");
	    proto_tree_add_text(tlv_tree, tvb, offset+2, 2, "TLV Length: %u",
				tlv_length);
	    stlv_offset = offset + 4;

	    /
	    while (stlv_offset < tlv_end_offset) {
		stlv_type = tvb_get_ntohs(tvb, stlv_offset);
		stlv_len = tvb_get_ntohs(tvb, stlv_offset + 2);
		stlv_name = val_to_str(stlv_type, mpls_link_stlv_str, "Unknown sub-TLV");
		switch (stlv_type) {

		case MPLS_LINK_TYPE:
		    ti = proto_tree_add_text(tlv_tree, tvb, stlv_offset, stlv_len+4,
				     "%s: %u - %s", stlv_name,
				     tvb_get_guint8(tvb, stlv_offset + 4),
				     val_to_str(tvb_get_guint8(tvb, stlv_offset + 4), 
					mpls_link_stlv_ltype_str, "Unknown Link Type"));
		    stlv_tree = proto_item_add_subtree(ti, ett_ospf_lsa_mpls_link_stlv);
		    proto_tree_add_text(stlv_tree, tvb, stlv_offset, 2,
					"TLV Type: %u: %s", stlv_type, stlv_name);
		    proto_tree_add_text(stlv_tree, tvb, stlv_offset+2, 2, "TLV Length: %u",
		    			stlv_len);
		    proto_tree_add_item(stlv_tree, ospf_filter[OSPFF_LS_MPLS_LINKTYPE],
					tvb, stlv_offset+4, 1,FALSE);
		    break;

		case MPLS_LINK_ID:
		    ti = proto_tree_add_text(tlv_tree, tvb, stlv_offset, stlv_len+4,
					     "%s: %s", stlv_name,
					     ip_to_str(tvb_get_ptr(tvb, stlv_offset + 4, 4)));
		    stlv_tree = proto_item_add_subtree(ti, ett_ospf_lsa_mpls_link_stlv);
		    proto_tree_add_text(stlv_tree, tvb, stlv_offset, 2,
					"TLV Type: %u: %s", stlv_type, stlv_name);
		    proto_tree_add_text(stlv_tree, tvb, stlv_offset+2, 2, "TLV Length: %u",
					stlv_len);
		    proto_tree_add_item(stlv_tree, ospf_filter[OSPFF_LS_MPLS_LINKID],
					tvb, stlv_offset+4, 4, FALSE);
		    break;

		case MPLS_LINK_LOCAL_IF:
		case MPLS_LINK_REMOTE_IF:
		    ti = proto_tree_add_text(tlv_tree, tvb, stlv_offset, stlv_len+4,
					     "%s", stlv_name);
		    stlv_tree = proto_item_add_subtree(ti, ett_ospf_lsa_mpls_link_stlv);
		    proto_tree_add_text(stlv_tree, tvb, stlv_offset, 2,
					"TLV Type: %u: %s", stlv_type, stlv_name);
		    proto_tree_add_text(stlv_tree, tvb, stlv_offset+2, 2, "TLV Length: %u",
					stlv_len);
		    /
		    for (i=0; i < stlv_len; i+=4)
		      proto_tree_add_item(stlv_tree,
					  stlv_type==MPLS_LINK_LOCAL_IF ?
					  ospf_filter[OSPFF_LS_MPLS_LOCAL_ADDR] :
					  ospf_filter[OSPFF_LS_MPLS_REMOTE_ADDR],
					  tvb, stlv_offset+4+i, 4, FALSE);
		    break;

		case MPLS_LINK_TE_METRIC:
		    ti = proto_tree_add_text(tlv_tree, tvb, stlv_offset, stlv_len+4,
					     "%s: %u", stlv_name,
					     tvb_get_ntohl(tvb, stlv_offset + 4));
		    stlv_tree = proto_item_add_subtree(ti, ett_ospf_lsa_mpls_link_stlv);
		    proto_tree_add_text(stlv_tree, tvb, stlv_offset, 2,
					"TLV Type: %u: %s", stlv_type, stlv_name);
		    proto_tree_add_text(stlv_tree, tvb, stlv_offset+2, 2, "TLV Length: %u",
					stlv_len);
		    proto_tree_add_text(stlv_tree, tvb, stlv_offset+4, 4, "%s: %u", stlv_name,
					tvb_get_ntohl(tvb, stlv_offset + 4));
		    break;

		case MPLS_LINK_COLOR:
		    ti = proto_tree_add_text(tlv_tree, tvb, stlv_offset, stlv_len+4,
					     "%s: 0x%08x", stlv_name,
					     tvb_get_ntohl(tvb, stlv_offset + 4));
		    stlv_tree = proto_item_add_subtree(ti, ett_ospf_lsa_mpls_link_stlv);
		    proto_tree_add_text(stlv_tree, tvb, stlv_offset, 2,
					"TLV Type: %u: %s", stlv_type, stlv_name);
		    proto_tree_add_text(stlv_tree, tvb, stlv_offset+2, 2, "TLV Length: %u",
					stlv_len);
		    stlv_admingrp = tvb_get_ntohl(tvb, stlv_offset + 4);
		    mask = 1;
		    ti = proto_tree_add_item(stlv_tree, ospf_filter[OSPFF_LS_MPLS_LINKCOLOR],
                                        tvb, stlv_offset+4, 4, FALSE);
		    stlv_admingrp_tree = proto_item_add_subtree(ti, ett_ospf_lsa_mpls_link_stlv_admingrp);
		    if (stlv_admingrp_tree == NULL)
			return;
		    for (i = 0 ; i < 32 ; i++) {
			if ((stlv_admingrp & mask) != 0) {
			    proto_tree_add_text(stlv_admingrp_tree, tvb, stlv_offset+4,
				4, "Group %d", i);
			}
			mask <<= 1;
		    }
		    break;

		case MPLS_LINK_MAX_BW:
		case MPLS_LINK_MAX_RES_BW:
		    ti = proto_tree_add_text(tlv_tree, tvb, stlv_offset, stlv_len+4,
					     "%s: %.10g bytes/s (%.0f bits/s)", stlv_name,
					     tvb_get_ntohieee_float(tvb, stlv_offset + 4),
					     tvb_get_ntohieee_float(tvb, stlv_offset + 4) * 8.0);
		    stlv_tree = proto_item_add_subtree(ti, ett_ospf_lsa_mpls_link_stlv);
		    proto_tree_add_text(stlv_tree, tvb, stlv_offset, 2,
					"TLV Type: %u: %s", stlv_type, stlv_name);
		    proto_tree_add_text(stlv_tree, tvb, stlv_offset+2, 2, "TLV Length: %u",
					stlv_len);
		    proto_tree_add_text(stlv_tree, tvb, stlv_offset+4, 4, "%s: %.10g bytes/s (%.0f bits/s)", stlv_name,
					tvb_get_ntohieee_float(tvb, stlv_offset + 4),
					tvb_get_ntohieee_float(tvb, stlv_offset + 4) * 8.0);
		    break;

		case MPLS_LINK_UNRES_BW:
		    ti = proto_tree_add_text(tlv_tree, tvb, stlv_offset, stlv_len+4,
					     "%s", stlv_name);
		    stlv_tree = proto_item_add_subtree(ti, ett_ospf_lsa_mpls_link_stlv);
		    proto_tree_add_text(stlv_tree, tvb, stlv_offset, 2,
					"TLV Type: %u: %s", stlv_type, stlv_name);
		    proto_tree_add_text(stlv_tree, tvb, stlv_offset+2, 2, "TLV Length: %u",
					stlv_len);
		    for (i = 0; i < 8; i++) {
			proto_tree_add_text(stlv_tree, tvb, stlv_offset+4+(i*4), 4,
					    "Pri %d: %.10g bytes/s (%.0f bits/s)", i,
					    tvb_get_ntohieee_float(tvb, stlv_offset + 4 + i*4),
					    tvb_get_ntohieee_float(tvb, stlv_offset + 4 + i*4) * 8.0);
		    }
		    break;

		case MPLS_LINK_LOCAL_REMOTE_ID:
		    ti = proto_tree_add_text(tlv_tree, tvb, stlv_offset, stlv_len+4,
		    			     "%s: %d (0x%x) - %d (0x%x)", stlv_name,
                                             tvb_get_ntohl(tvb, stlv_offset + 4),
                                             tvb_get_ntohl(tvb, stlv_offset + 4),
                                             tvb_get_ntohl(tvb, stlv_offset + 8),
                                             tvb_get_ntohl(tvb, stlv_offset + 8));
		    stlv_tree = proto_item_add_subtree(ti, ett_ospf_lsa_mpls_link_stlv);
		    
		    proto_tree_add_text(stlv_tree, tvb, stlv_offset, 2,
					"TLV Type: %u: %s", stlv_type, stlv_name);
		    proto_tree_add_text(stlv_tree, tvb, stlv_offset+2, 2, "TLV Length: %u",
					stlv_len);			
                    proto_tree_add_item(stlv_tree,
                                        ospf_filter[OSPFF_LS_MPLS_LOCAL_IFID],
                                        tvb, stlv_offset+4, 4, FALSE);
                    proto_tree_add_item(stlv_tree,
                                        ospf_filter[OSPFF_LS_MPLS_REMOTE_IFID],
                                        tvb, stlv_offset+8, 4, FALSE);
		    break;

		case MPLS_LINK_IF_SWITCHING_DESC:
		    ti = proto_tree_add_text(tlv_tree, tvb, stlv_offset, stlv_len+4,
					     "%s", stlv_name);
		    stlv_tree = proto_item_add_subtree(ti, ett_ospf_lsa_mpls_link_stlv);
		    proto_tree_add_text(stlv_tree, tvb, stlv_offset, 2,
					"TLV Type: %u: %s", stlv_type, stlv_name);
		    proto_tree_add_text(stlv_tree, tvb, stlv_offset+2, 2, "TLV Length: %u",
					stlv_len);
                    switch_cap = tvb_get_guint8 (tvb, stlv_offset+4);
		    proto_tree_add_text(stlv_tree, tvb, stlv_offset+4, 1, "Switching Type: %s",
					val_to_str(tvb_get_guint8(tvb,stlv_offset+4),
						   gmpls_switching_type_str, "Unknown (%d)"));
		    proto_tree_add_text(stlv_tree, tvb, stlv_offset+5, 1, "Encoding: %s",
					val_to_str(tvb_get_guint8(tvb,stlv_offset+5),
						   gmpls_lsp_enc_str, "Unknown (%d)"));
		    for (i = 0; i < 8; i++) {
			proto_tree_add_text(stlv_tree, tvb, stlv_offset+8+(i*4), 4,
					    "Pri %d: %.10g bytes/s (%.0f bits/s)", i,
					    tvb_get_ntohieee_float(tvb, stlv_offset + 8 + i*4),
					    tvb_get_ntohieee_float(tvb, stlv_offset + 8 + i*4) * 8.0);
		    }
                    if (switch_cap >=1 && switch_cap <=4) {           /
                        proto_tree_add_text(stlv_tree, tvb, stlv_offset+40, 4,
                                            "Minimum LSP bandwidth: %.10g bytes/s (%.0f bits/s)",
                                            tvb_get_ntohieee_float(tvb, stlv_offset + 40),
                                            tvb_get_ntohieee_float(tvb, stlv_offset + 40) * 8.0);
                        proto_tree_add_text(stlv_tree, tvb, stlv_offset+44, 2,
                                            "Interface MTU: %d", tvb_get_ntohs(tvb, stlv_offset+44));
                    }

                    if (switch_cap == 100) {                         /
                        proto_tree_add_text(stlv_tree, tvb, stlv_offset+40, 4,
                                            "Minimum LSP bandwidth: %.10g bytes/s (%.0f bits/s)",
                                            tvb_get_ntohieee_float(tvb, stlv_offset + 40),
                                            tvb_get_ntohieee_float(tvb, stlv_offset + 40) * 8.0);
                        proto_tree_add_text(stlv_tree, tvb, stlv_offset+44, 2,
                                            "SONET/SDH: %s",
                                            tvb_get_guint8(tvb, stlv_offset+44) ?
                                            "Arbitrary" : "Standard");
                    }
		    break;
		case MPLS_LINK_PROTECTION:
		    ti = proto_tree_add_text(tlv_tree, tvb, stlv_offset, stlv_len+4,
					     "%s", stlv_name);
		    stlv_tree = proto_item_add_subtree(ti, ett_ospf_lsa_mpls_link_stlv);
		    proto_tree_add_text(stlv_tree, tvb, stlv_offset, 2,
					"TLV Type: %u: %s", stlv_type, stlv_name);
		    proto_tree_add_text(stlv_tree, tvb, stlv_offset+2, 2, "TLV Length: %u",
					stlv_len);
		    proto_tree_add_text(stlv_tree, tvb, stlv_offset+4, 1, "Protection Capability: %s (0x%x)",
					val_to_str(tvb_get_guint8(tvb,stlv_offset+4), gmpls_protection_cap_str, "Unknown (%d)"),tvb_get_guint8(tvb,stlv_offset+4));
		    break;
    		
		case MPLS_LINK_SHARED_RISK_GROUP:
		    ti = proto_tree_add_text(tlv_tree, tvb, stlv_offset, stlv_len+4,
					     "%s", stlv_name);
		    stlv_tree = proto_item_add_subtree(ti, ett_ospf_lsa_mpls_link_stlv);
		    proto_tree_add_text(stlv_tree, tvb, stlv_offset, 2,
					"TLV Type: %u: %s", stlv_type, stlv_name);
		    proto_tree_add_text(stlv_tree, tvb, stlv_offset+2, 2, "TLV Length: %u",
					stlv_len);
		    for (i=0; i < stlv_len; i+=4)
 		        proto_tree_add_text(stlv_tree, tvb, stlv_offset+4+i, 4, "Shared Risk Link Group: %u", 
			                tvb_get_ntohl(tvb,stlv_offset+4+i)); 
		    break;

		case OIF_LOCAL_NODE_ID:
		    ti = proto_tree_add_text(tlv_tree, tvb, stlv_offset, stlv_len+4,
					     "%s: %s", stlv_name,
					     ip_to_str(tvb_get_ptr(tvb, stlv_offset + 4, 4)));
		    stlv_tree = proto_item_add_subtree(ti, ett_ospf_lsa_mpls_link_stlv);
		    proto_tree_add_text(stlv_tree, tvb, stlv_offset, 2,
					"TLV Type: %u: %s", stlv_type, stlv_name);
		    proto_tree_add_text(stlv_tree, tvb, stlv_offset+2, 2, "TLV Length: %u",
					stlv_len);
		    proto_tree_add_text(stlv_tree, tvb, stlv_offset+4, 4, "Local Node ID: %s",
					ip_to_str(tvb_get_ptr(tvb, stlv_offset + 4, 4)));
		    break;

		case OIF_REMOTE_NODE_ID:
		    ti = proto_tree_add_text(tlv_tree, tvb, stlv_offset, stlv_len+4,
					     "%s: %s", stlv_name,
					     ip_to_str(tvb_get_ptr(tvb, stlv_offset + 4, 4)));
		    stlv_tree = proto_item_add_subtree(ti, ett_ospf_lsa_mpls_link_stlv);
		    proto_tree_add_text(stlv_tree, tvb, stlv_offset, 2,
					"TLV Type: %u: %s", stlv_type, stlv_name);
		    proto_tree_add_text(stlv_tree, tvb, stlv_offset+2, 2, "TLV Length: %u",
					stlv_len);
		    proto_tree_add_text(stlv_tree, tvb, stlv_offset+4, 4, "Remote Node ID: %s",
					ip_to_str(tvb_get_ptr(tvb, stlv_offset + 4, 4)));
		    break;

		case OIF_SONET_SDH_SWITCHING_CAPABILITY:
		    ti = proto_tree_add_text(tlv_tree, tvb, stlv_offset, stlv_len+4, "%s", stlv_name);
		    stlv_tree = proto_item_add_subtree(ti, ett_ospf_lsa_mpls_link_stlv);
		    proto_tree_add_text(stlv_tree, tvb, stlv_offset, 2,
					"TLV Type: %u: %s", stlv_type, stlv_name);
		    proto_tree_add_text(stlv_tree, tvb, stlv_offset+2, 2, "TLV Length: %u",
					stlv_len);
		    proto_tree_add_text(stlv_tree, tvb, stlv_offset+4, 1, "Switching Cap: %s",
					val_to_str(tvb_get_guint8 (tvb, stlv_offset+4),
						   gmpls_switching_type_str, "Unknown (%d)"));
		    proto_tree_add_text(stlv_tree, tvb, stlv_offset+5, 1, "Encoding: %s",
					val_to_str(tvb_get_guint8(tvb,stlv_offset+5),
						   gmpls_lsp_enc_str, "Unknown (%d)"));
		    for (i = 0; i < (stlv_len - 4) / 4; i++) {
			proto_tree_add_text(stlv_tree, tvb, stlv_offset+8+(i*4), 4,
					    "%s: %d free timeslots",
                                            val_to_str(tvb_get_guint8(tvb, stlv_offset+8+(i*4)),
                                                       gmpls_sonet_signal_type_str,
                                                       "Unknown Signal Type (%d)"),
					    tvb_get_ntoh24(tvb, stlv_offset + 9 + i*4));
		    }

		    break;
		default:
		    ti = proto_tree_add_text(tlv_tree, tvb, stlv_offset, stlv_len+4,
					"Unknown Link sub-TLV: %u", stlv_type);
		    stlv_tree = proto_item_add_subtree(ti, ett_ospf_lsa_mpls_link_stlv);
		    proto_tree_add_text(stlv_tree, tvb, stlv_offset, 2,
					"TLV Type: %u: %s", stlv_type, stlv_name);
		    proto_tree_add_text(stlv_tree, tvb, stlv_offset+2, 2, "TLV Length: %u",
					stlv_len);
		    proto_tree_add_text(stlv_tree, tvb, stlv_offset+4, stlv_len,
					"TLV Value");
		    break;
		}
		stlv_offset += ((stlv_len+4+3)/4)*4;
	    }
	    break;

	case OIF_TLV_TNA:
	    ti = proto_tree_add_text(mpls_tree, tvb, offset, tlv_length+4,
				     "TNA Information");
	    tlv_tree = proto_item_add_subtree(ti, ett_ospf_lsa_oif_tna);
	    proto_tree_add_text(tlv_tree, tvb, offset, 2, "TLV Type: 32768 - TNA Information");
	    proto_tree_add_text(tlv_tree, tvb, offset+2, 2, "TLV Length: %u",
				tlv_length);
	    stlv_offset = offset + 4;

	    /
	    while (stlv_offset < tlv_end_offset) {
		stlv_type = tvb_get_ntohs(tvb, stlv_offset);
		stlv_len = tvb_get_ntohs(tvb, stlv_offset + 2);
		stlv_name = val_to_str(stlv_type, oif_stlv_str, "Unknown sub-TLV");
		switch (stlv_type) {

		case OIF_NODE_ID:
		    ti = proto_tree_add_text(tlv_tree, tvb, stlv_offset, stlv_len+4,
					     "%s: %s", stlv_name,
					     ip_to_str(tvb_get_ptr(tvb, stlv_offset + 4, 4)));
		    stlv_tree = proto_item_add_subtree(ti, ett_ospf_lsa_oif_tna_stlv);
		    proto_tree_add_text(stlv_tree, tvb, stlv_offset, 2,
					"TLV Type: %u: %s", stlv_type, stlv_name);
		    proto_tree_add_text(stlv_tree, tvb, stlv_offset+2, 2, "TLV Length: %u",
		    			stlv_len);
		    proto_tree_add_text(stlv_tree, tvb, stlv_offset+4, 4, "%s: %s", stlv_name,
					ip_to_str(tvb_get_ptr(tvb, stlv_offset + 4, 4)));
		    break;

		case OIF_TNA_IPv4_ADDRESS:
		    ti = proto_tree_add_text(tlv_tree, tvb, stlv_offset, stlv_len+4,
					     "%s (IPv4): %s", stlv_name,
					     ip_to_str(tvb_get_ptr(tvb, stlv_offset + 8, 4)));
		    stlv_tree = proto_item_add_subtree(ti, ett_ospf_lsa_oif_tna_stlv);
		    proto_tree_add_text(stlv_tree, tvb, stlv_offset, 2,
					"TLV Type: %u: %s (IPv4)", stlv_type, stlv_name);
		    proto_tree_add_text(stlv_tree, tvb, stlv_offset+2, 2, "TLV Length: %u", stlv_len);
		    proto_tree_add_text(stlv_tree, tvb, stlv_offset+4, 1, "Addr Length: %u",
					tvb_get_guint8 (tvb, stlv_offset+4));
		    proto_tree_add_text(stlv_tree, tvb, stlv_offset+8, stlv_len - 4, "TNA Addr: %s",
					ip_to_str(tvb_get_ptr(tvb, stlv_offset + 8, 4)));
		    break;

		case OIF_TNA_IPv6_ADDRESS:
		    ti = proto_tree_add_text(tlv_tree, tvb, stlv_offset, stlv_len+4,
					     "%s (IPv6): %s", stlv_name,
					     ip6_to_str((const struct e_in6_addr *)
							 tvb_get_ptr(tvb, stlv_offset + 8, 16)));
		    stlv_tree = proto_item_add_subtree(ti, ett_ospf_lsa_oif_tna_stlv);
		    proto_tree_add_text(stlv_tree, tvb, stlv_offset, 2,
					"TLV Type: %u: %s (IPv6)", stlv_type, stlv_name);
		    proto_tree_add_text(stlv_tree, tvb, stlv_offset+2, 2, "TLV Length: %u", stlv_len);
		    proto_tree_add_text(stlv_tree, tvb, stlv_offset+4, 1, "Addr Length: %u",
					tvb_get_guint8 (tvb, stlv_offset+4));
		    proto_tree_add_text(stlv_tree, tvb, stlv_offset+8, stlv_len - 4, "TNA Addr: %s",
					ip6_to_str((const struct e_in6_addr *)
						    tvb_get_ptr(tvb, stlv_offset + 8, 16)));
		    break;

		case OIF_TNA_NSAP_ADDRESS:
		    ti = proto_tree_add_text(tlv_tree, tvb, stlv_offset, stlv_len+4,
					     "%s (NSAP): %s", stlv_name,
					     tvb_bytes_to_str (tvb, stlv_offset + 8, stlv_len - 4));
		    stlv_tree = proto_item_add_subtree(ti, ett_ospf_lsa_oif_tna_stlv);
		    proto_tree_add_text(stlv_tree, tvb, stlv_offset, 2,
					"TLV Type: %u: %s (NSAP)", stlv_type, stlv_name);
		    proto_tree_add_text(stlv_tree, tvb, stlv_offset+2, 2, "TLV Length: %u", stlv_len);
		    proto_tree_add_text(stlv_tree, tvb, stlv_offset+4, 1, "Addr Length: %u",
					    tvb_get_guint8 (tvb, stlv_offset+4));
		    proto_tree_add_text(stlv_tree, tvb, stlv_offset+8, stlv_len - 4, "TNA Addr: %s",
					tvb_bytes_to_str(tvb, stlv_offset+8, stlv_len - 4));
		    break;

		default:
		    proto_tree_add_text(tlv_tree, tvb, stlv_offset, stlv_len+4,
					"Unknown Link sub-TLV: %u", stlv_type);
		    break;
		}
		stlv_offset += ((stlv_len+4+3)/4)*4;
	    }
	    break;
	default:
	    ti = proto_tree_add_text(mpls_tree, tvb, offset, tlv_length+4,
				     "Unknown LSA: %u", tlv_type);
	    tlv_tree = proto_item_add_subtree(ti, ett_ospf_lsa_mpls_link);
	    proto_tree_add_text(tlv_tree, tvb, offset, 2, "TLV Type: %u - Unknown",
				tlv_type);
	    proto_tree_add_text(tlv_tree, tvb, offset+2, 2, "TLV Length: %u",
				tlv_length);
	    proto_tree_add_text(tlv_tree, tvb, offset+4, tlv_length, "TLV Data");
	    break;
	}

	offset += tlv_length + 4;
	length -= tlv_length + 4;
    }
}
