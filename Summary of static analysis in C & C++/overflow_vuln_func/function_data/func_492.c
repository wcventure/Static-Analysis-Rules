static void
dissect_rsvp_ifid_tlv (proto_tree *ti, proto_tree *rsvp_object_tree, 
		       tvbuff_t *tvb, int offset, int obj_length, 
		       int subtree_type)
{
    int     tlv_off;
    guint16   tlv_type;
    guint     tlv_len;
    const char *ifindex_name;
    proto_tree *rsvp_ifid_subtree, *ti2;
    int       offset2 = offset + 4;

    for (tlv_off = 0; tlv_off < obj_length - 12; ) {
	tlv_type = tvb_get_ntohs(tvb, offset+tlv_off);
	tlv_len = tvb_get_ntohs(tvb, offset+tlv_off+2);

	if (tlv_len == 0) {
	    proto_tree_add_text(rsvp_object_tree, tvb, offset+tlv_off+2, 2,
		"Invalid length (0)");
	    return;
	}
	switch(tlv_type) {
	case 1:
	    ti2 = proto_tree_add_text(rsvp_object_tree, tvb,
				      offset+tlv_off, 8,
				      "IPv4 TLV - %s",
				      ip_to_str(tvb_get_ptr(tvb, offset+tlv_off+4, 4)));

	    rsvp_ifid_subtree = proto_item_add_subtree(ti2, subtree_type);
	    proto_tree_add_text(rsvp_ifid_subtree, tvb, offset+tlv_off, 2,
				"Type: 1 (IPv4)");
	    proto_tree_add_text(rsvp_ifid_subtree, tvb, offset+tlv_off+2, 2,
				"Length: %u",
				tvb_get_ntohs(tvb, offset+tlv_off+2));
	    proto_tree_add_text(rsvp_ifid_subtree, tvb, offset+tlv_off+4, 4,
				"IPv4 address: %s",
				ip_to_str(tvb_get_ptr(tvb, offset+tlv_off+4, 4)));
	    proto_item_append_text(ti, "Data IPv4: %s. ",
				   ip_to_str(tvb_get_ptr(tvb, offset+tlv_off+4, 4)));
	    break;
	    
	case 3:
	    ifindex_name = "";
	    goto ifid_ifindex;
	case 4:
	    ifindex_name = " Forward";
	    goto ifid_ifindex;
	case 5:
	    ifindex_name = " Reverse";
	ifid_ifindex:
	    ti2 = proto_tree_add_text(rsvp_object_tree, tvb,
				      offset+tlv_off, 12,
				      "Interface-Index%s TLV - %s, %d",
				      ifindex_name,
				      ip_to_str(tvb_get_ptr(tvb, offset+tlv_off+4, 4)),
				      tvb_get_ntohl(tvb, offset+tlv_off+8));
	    rsvp_ifid_subtree =	proto_item_add_subtree(ti2, subtree_type);
	    proto_tree_add_text(rsvp_ifid_subtree, tvb, offset+tlv_off, 2,
				"Type: %d (Interface Index%s)", tlv_type, ifindex_name);
	    proto_tree_add_text(rsvp_ifid_subtree, tvb, offset+tlv_off+2, 2,
				"Length: %u",
				tvb_get_ntohs(tvb, offset+tlv_off+2));
	    proto_tree_add_text(rsvp_ifid_subtree, tvb, offset+tlv_off+4, 4,
				"IPv4 address: %s",
				ip_to_str(tvb_get_ptr(tvb, offset+tlv_off+4, 4)));
	    proto_tree_add_text(rsvp_ifid_subtree, tvb, offset+tlv_off+8, 4,
				"Interface-ID: %d",
				tvb_get_ntohl(tvb, offset+tlv_off+8));
	    proto_item_append_text(ti, "Data If-Index%s: %s, %d. ", ifindex_name,
				   ip_to_str(tvb_get_ptr(tvb, offset+tlv_off+4, 4)),
				   tvb_get_ntohl(tvb, offset+tlv_off+8));
	    break;
	    
	default:
	    proto_tree_add_text(rsvp_object_tree, tvb, offset2+4, 4,
				"Logical interface: %u",
				tvb_get_ntohl(tvb, offset2+4));
	}
	tlv_off += tlv_len;
    }
}
