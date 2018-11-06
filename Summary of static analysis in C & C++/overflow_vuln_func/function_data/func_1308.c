static int
dissect_ospf_v2_lsa(tvbuff_t *tvb, int offset, proto_tree *tree,
		 gboolean disassemble_body)
{
    proto_tree *ospf_lsa_tree;
    proto_item *ti;

    guint8		 ls_type;
    guint16		 ls_length;
    int			 end_offset;
    guint16		 nr_links;
    guint16		 nr_tos;

    /
    guint8		 link_type;
    guint16 		 link_counter;
    guint8 		 tos_counter;
    const char 		*link_type_str;
    const char 		*link_type_short_str;
    const char 		*link_id;

    /
    guint8		 options;

    /
    guint8		 ls_id_type;

    ls_type = tvb_get_guint8(tvb, offset + 3);
    ls_length = tvb_get_ntohs(tvb, offset + 18);
    end_offset = offset + ls_length;

    if (disassemble_body) {
	ti = proto_tree_add_text(tree, tvb, offset, ls_length,
				 "LS Type: %s",
				 val_to_str(ls_type, ls_type_vals, "Unknown (%d)"));
    } else {
	ti = proto_tree_add_text(tree, tvb, offset, OSPF_LSA_HEADER_LENGTH,
				 "LSA Header");
    }
    ospf_lsa_tree = proto_item_add_subtree(ti, ett_ospf_lsa);

    proto_tree_add_text(ospf_lsa_tree, tvb, offset, 2, "LS Age: %u seconds",
			tvb_get_ntohs(tvb, offset));
    dissect_ospf_bitfield(ospf_lsa_tree, tvb, offset + 2, &bfinfo_v2_options);
    proto_tree_add_item(ospf_lsa_tree, ospf_filter[OSPFF_LS_TYPE], tvb,
			offset + 3, 1, FALSE);
    proto_tree_add_item_hidden(ospf_lsa_tree,
			       ospf_filter[ospf_ls_type_to_filter(ls_type)], tvb,
			       offset + 3, 1, FALSE);

    if (is_opaque(ls_type)) {
    	ls_id_type = tvb_get_guint8(tvb, offset + 4);
	proto_tree_add_uint(ospf_lsa_tree, ospf_filter[OSPFF_LS_OPAQUE_TYPE],
			    tvb, offset + 4, 1, ls_id_type);

	switch (ls_id_type) {

	case OSPF_LSA_MPLS_TE:
	    proto_tree_add_text(ospf_lsa_tree, tvb, offset + 5, 1, "Link State ID TE-LSA Reserved: %u",
				tvb_get_guint8(tvb, offset + 5));
	    proto_tree_add_item(ospf_lsa_tree, ospf_filter[OSPFF_LS_MPLS_TE_INSTANCE],
	    			tvb, offset + 6, 2, FALSE);
	    break;

	default:
	    proto_tree_add_text(ospf_lsa_tree, tvb, offset + 5, 3, "Link State ID Opaque ID: %u",
				tvb_get_ntoh24(tvb, offset + 5));
	    break;
	}
    } else {
	ls_id_type = 0;
	proto_tree_add_text(ospf_lsa_tree, tvb, offset + 4, 4, "Link State ID: %s",
			    ip_to_str(tvb_get_ptr(tvb, offset + 4, 4)));
    }

    proto_tree_add_item(ospf_lsa_tree, ospf_filter[OSPFF_ADV_ROUTER],
			tvb, offset + 8, 4, FALSE);
    proto_tree_add_text(ospf_lsa_tree, tvb, offset + 12, 4, "LS Sequence Number: 0x%08x",
			tvb_get_ntohl(tvb, offset + 12));
    proto_tree_add_text(ospf_lsa_tree, tvb, offset + 16, 2, "LS Checksum: %04x",
			tvb_get_ntohs(tvb, offset + 16));

    proto_tree_add_text(ospf_lsa_tree, tvb, offset + 18, 2, "Length: %u",
			ls_length);

    /
    offset += OSPF_LSA_HEADER_LENGTH;
    if (ls_length <= OSPF_LSA_HEADER_LENGTH)
	return offset;	/
    ls_length -= OSPF_LSA_HEADER_LENGTH;

    if (!disassemble_body)
	return offset;

    switch (ls_type){

    case OSPF_LSTYPE_ROUTER:
	/
	dissect_ospf_bitfield(ospf_lsa_tree, tvb, offset, &bfinfo_v2_router_lsa_flags);

	nr_links = tvb_get_ntohs(tvb, offset + 2);
	proto_tree_add_text(ospf_lsa_tree, tvb, offset + 2, 2, "Number of Links: %u",
			    nr_links);
	offset += 4;
	/
	for (link_counter = 1; link_counter <= nr_links; link_counter++) {
            proto_tree *ospf_lsa_router_link_tree;
            proto_item *ti_local;


	    /
	    link_type = tvb_get_guint8(tvb, offset + 8);
	    switch (link_type) {

	    case OSPF_LINK_PTP:
                link_type_str="Point-to-point connection to another router";
                link_type_short_str="PTP";
		link_id="Neighboring router's Router ID";
		break;

	    case OSPF_LINK_TRANSIT:
		link_type_str="Connection to a transit network";
                link_type_short_str="Transit";
		link_id="IP address of Designated Router";
		break;

	    case OSPF_LINK_STUB:
		link_type_str="Connection to a stub network";
                link_type_short_str="Stub";
		link_id="IP network/subnet number";
		break;

	    case OSPF_LINK_VIRTUAL:
		link_type_str="Virtual link";
                link_type_short_str="Virtual";
		link_id="Neighboring router's Router ID";
		break;

	    default:
		link_type_str="Unknown link type";
                link_type_short_str="Unknown";
		link_id="Unknown link ID";
		break;
	    }

	    nr_tos = tvb_get_guint8(tvb, offset + 9);

            
            ti_local = proto_tree_add_text(ospf_lsa_tree, tvb, offset, 12 + 4 * nr_tos,
                                     "Type: %-8s ID: %-15s Data: %-15s Metric: %d",
                                     link_type_short_str, 
                                     ip_to_str(tvb_get_ptr(tvb, offset, 4)),
                                     ip_to_str(tvb_get_ptr(tvb, offset + 4, 4)),
                                     tvb_get_ntohs(tvb, offset + 10));

            ospf_lsa_router_link_tree = proto_item_add_subtree(ti_local, ett_ospf_lsa_router_link);

	    proto_tree_add_text(ospf_lsa_router_link_tree, tvb, offset, 4, "%s: %s", link_id,
				ip_to_str(tvb_get_ptr(tvb, offset, 4)));

	    /
	    proto_tree_add_text(ospf_lsa_router_link_tree, tvb, offset + 4, 4, "Link Data: %s",
				ip_to_str(tvb_get_ptr(tvb, offset + 4, 4)));

	    proto_tree_add_text(ospf_lsa_router_link_tree, tvb, offset + 8, 1, "Link Type: %u - %s",
				link_type, link_type_str);
	    proto_tree_add_text(ospf_lsa_router_link_tree, tvb, offset + 9, 1, "Number of TOS metrics: %u",
				nr_tos);
	    proto_tree_add_text(ospf_lsa_router_link_tree, tvb, offset + 10, 2, "TOS 0 metric: %u",
				tvb_get_ntohs(tvb, offset + 10));

	    offset += 12;

	    /
	    for (tos_counter = 1; tos_counter <= nr_tos; tos_counter++) {
		proto_tree_add_text(ospf_lsa_router_link_tree, tvb, offset, 4, "TOS: %u, Metric: %u",
				    tvb_get_guint8(tvb, offset),
				    tvb_get_ntohs(tvb, offset + 2));
		offset += 4;
	    }
	}
	break;

    case OSPF_LSTYPE_NETWORK:
	proto_tree_add_text(ospf_lsa_tree, tvb, offset, 4, "Netmask: %s",
				ip_to_str(tvb_get_ptr(tvb, offset, 4)));
	offset += 4;

	while (offset < end_offset) {
	    proto_tree_add_text(ospf_lsa_tree, tvb, offset, 4, "Attached Router: %s",
				ip_to_str(tvb_get_ptr(tvb, offset, 4)));
	    offset += 4;
	}
	break;

    case OSPF_LSTYPE_SUMMERY:
    /
    case OSPF_LSTYPE_ASBR:
	proto_tree_add_text(ospf_lsa_tree, tvb, offset, 4, "Netmask: %s",
			    ip_to_str(tvb_get_ptr(tvb, offset, 4)));
	offset += 4;

	proto_tree_add_text(ospf_lsa_tree, tvb, offset, 4, "Metric: %u",
			    tvb_get_ntoh24(tvb, offset + 1));
	offset += 4;

	/
	while (offset < end_offset) {
	    proto_tree_add_text(ospf_lsa_tree, tvb, offset, 4, "TOS: %u, Metric: %u",
				tvb_get_guint8(tvb, offset),
				tvb_get_ntoh24(tvb, offset + 1));
	    offset += 4;
	}
	break;

    case OSPF_LSTYPE_ASEXT:
    case OSPF_LSTYPE_ASEXT7:
	proto_tree_add_text(ospf_lsa_tree, tvb, offset, 4, "Netmask: %s",
			    ip_to_str(tvb_get_ptr(tvb, offset, 4)));
	offset += 4;

	options = tvb_get_guint8(tvb, offset);
	if (options & 0x80) { /
	    proto_tree_add_text(ospf_lsa_tree, tvb, offset, 1,
		    "External Type: Type 2 (metric is larger than any other link state path)");
	} else {
	    proto_tree_add_text(ospf_lsa_tree, tvb, offset, 1,
		    "External Type: Type 1 (metric is specified in the same units as interface cost)");
	}
	/
	proto_tree_add_text(ospf_lsa_tree, tvb, offset + 1, 3, "Metric: %u",
			    tvb_get_ntoh24(tvb, offset + 1));
	offset += 4;

	proto_tree_add_text(ospf_lsa_tree, tvb, offset, 4, "Forwarding Address: %s",
			    ip_to_str(tvb_get_ptr(tvb, offset, 4)));
	offset += 4;

	proto_tree_add_text(ospf_lsa_tree, tvb, offset, 4, "External Route Tag: %u",
			    tvb_get_ntohl(tvb, offset));
	offset += 4;

	/
	while (offset < end_offset) {
	    options = tvb_get_guint8(tvb, offset);
	    if (options & 0x80) { /
		proto_tree_add_text(ospf_lsa_tree, tvb, offset, 1,
			"External Type: Type 2 (metric is larger than any other link state path)");
	    } else {
		proto_tree_add_text(ospf_lsa_tree, tvb, offset, 1,
			"External Type: Type 1 (metric is specified in the same units as interface cost)");
	    }
	    proto_tree_add_text(ospf_lsa_tree, tvb, offset, 4, "TOS: %u, Metric: %u",
				options & 0x7F,
				tvb_get_ntoh24(tvb, offset + 1));
	    offset += 4;

	    proto_tree_add_text(ospf_lsa_tree, tvb, offset, 4, "Forwarding Address: %s",
				ip_to_str(tvb_get_ptr(tvb, offset, 4)));
	    offset += 4;

	    proto_tree_add_text(ospf_lsa_tree, tvb, offset, 4, "External Route Tag: %u",
				tvb_get_ntohl(tvb, offset));
	    offset += 4;
	}
	break;

    case OSPF_LSTYPE_OP_LINKLOCAL:
    case OSPF_LSTYPE_OP_AREALOCAL:
    case OSPF_LSTYPE_OP_ASWIDE:
	/
	dissect_ospf_lsa_opaque(tvb, offset, ospf_lsa_tree, ls_id_type,
				ls_length);
	offset += ls_length;
	break;

    default:
	/
	proto_tree_add_text(ospf_lsa_tree, tvb, offset, ls_length,
			    "Unknown LSA Type");
	offset += ls_length;
	break;
    }
    /
    return offset;
}
