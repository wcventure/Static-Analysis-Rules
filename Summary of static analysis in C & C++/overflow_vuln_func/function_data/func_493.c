static void
dissect_rsvp_hop (proto_item *ti, proto_tree *rsvp_object_tree,
		  tvbuff_t *tvb,
		  int offset, int obj_length,
		  int class _U_, int type)
{
    int offset2 = offset + 4;

    switch(type) {
    case 1:
	proto_tree_add_text(rsvp_object_tree, tvb, offset+3, 1,
			    "C-type: 1 - IPv4");
	proto_tree_add_text(rsvp_object_tree, tvb, offset2, 4,
			    "Neighbor address: %s",
			    ip_to_str(tvb_get_ptr(tvb, offset2, 4)));
	proto_tree_add_text(rsvp_object_tree, tvb, offset2+4, 4,
			    "Logical interface: %u",
			    tvb_get_ntohl(tvb, offset2+4));
	proto_item_set_text(ti, "HOP: IPv4, %s",
			    ip_to_str(tvb_get_ptr(tvb, offset2, 4)));
	break;

    case 2:
	proto_tree_add_text(rsvp_object_tree, tvb, offset+3, 1,
			    "C-type: 2 - IPv6");
	proto_tree_add_text(rsvp_object_tree, tvb, offset2, 16,
			    "Neighbor address: %s",
			    ip6_to_str((const struct e_in6_addr *)tvb_get_ptr(tvb, offset2, 16)));
	proto_tree_add_text(rsvp_object_tree, tvb, offset2+16, 4,
			    "Logical interface: 0x%08x",
			    tvb_get_ntohl(tvb, offset2+16));
	break;

    case 3:
	proto_tree_add_text(rsvp_object_tree, tvb, offset+3, 1,
			    "C-type: 3 - IPv4 IF-ID");
	proto_tree_add_text(rsvp_object_tree, tvb, offset2, 4,
			    "Neighbor address: %s",
			    ip_to_str(tvb_get_ptr(tvb, offset2, 4)));
	proto_tree_add_text(rsvp_object_tree, tvb, offset2+4, 4,
			    "Logical interface: %u",
			    tvb_get_ntohl(tvb, offset2+4));

	proto_item_set_text(ti, "HOP: IPv4 IF-ID. Control IPv4: %s. ",
			    ip_to_str(tvb_get_ptr(tvb, offset2, 4)));

	dissect_rsvp_ifid_tlv(ti, rsvp_object_tree, tvb, offset+12, obj_length, 
			      TREE(TT_HOP_SUBOBJ));
			      
	break;

    default:
	proto_tree_add_text(rsvp_object_tree, tvb, offset+3, 1,
			    "C-type: Unknown (%u)",
			    type);
	proto_tree_add_text(rsvp_object_tree, tvb, offset2, obj_length - 4,
			    "Data (%d bytes)", obj_length - 4);
    }
}
