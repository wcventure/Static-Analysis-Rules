static void
dissect_rsvp_error (proto_item *ti, proto_tree *rsvp_object_tree,
		    tvbuff_t *tvb,
		    int offset, int obj_length,
		    int class _U_, int type)
{
    int offset2 = offset + 4;
    int offset3;
    guint8 error_flags;
    guint8 error_code;
    guint16 error_val;
    proto_tree *ti2, *rsvp_error_subtree;

    switch(type) {
    case 1: {
	proto_tree_add_text(rsvp_object_tree, tvb, offset+3, 1,
			    "C-type: 1 - IPv4");
	proto_tree_add_text(rsvp_object_tree, tvb, offset2, 4,
			    "Error node: %s",
			    ip_to_str(tvb_get_ptr(tvb, offset2, 4)));
	offset3 = offset2+4;
	break;
    }

    case 2: {
	proto_tree_add_text(rsvp_object_tree, tvb, offset+3, 1,
			    "C-type: 2 - IPv6");
	proto_tree_add_text(rsvp_object_tree, tvb, offset2, 16,
			    "Error node: %s",
			    ip6_to_str((const struct e_in6_addr *)tvb_get_ptr(tvb, offset2, 16)));
	offset3 = offset2+16;
	break;
    }

    case 3: {
	proto_tree_add_text(rsvp_object_tree, tvb, offset+3, 1,
			    "C-type: 3 - IPv4 IF-ID");
	proto_tree_add_text(rsvp_object_tree, tvb, offset2, 4,
			    "Error node: %s",
			    ip_to_str(tvb_get_ptr(tvb, offset2, 4)));
	offset3 = offset2+4;
	break;
    }

    default:
	proto_tree_add_text(rsvp_object_tree, tvb, offset+3, 1,
			    "C-type: Unknown (%u)",
			    type);
	proto_tree_add_text(rsvp_object_tree, tvb, offset2, obj_length - 4,
			    "Data (%d bytes)", obj_length - 4);
	return;
    }

    error_flags = tvb_get_guint8(tvb, offset3);
    ti2 = proto_tree_add_text(rsvp_object_tree, tvb, offset3, 1,
			      "Flags: 0x%02x", error_flags);
    rsvp_error_subtree = proto_item_add_subtree(ti2, TREE(TT_ERROR_FLAGS));
    proto_tree_add_text(rsvp_error_subtree, tvb, offset3, 1,
		    decode_boolean_bitfield(error_flags, 0x04, 8,
					    "Path State Removed",
					    ""));
    proto_tree_add_text(rsvp_error_subtree, tvb, offset3, 1,
		    decode_boolean_bitfield(error_flags, 0x02, 8,
					    "NotGuilty",
					    ""));
    proto_tree_add_text(rsvp_error_subtree, tvb, offset3, 1,
		    decode_boolean_bitfield(error_flags, 0x01, 8,
					    "InPlace",
					    ""));
    proto_item_append_text(ti2, " %s %s %s",
			   (error_flags & (1<<2))  ? "Path-State-Removed" : "",
			   (error_flags & (1<<1))  ? "NotGuilty" : "",
			   (error_flags & (1<<0))  ? "InPlace" : "");
    error_code = tvb_get_guint8(tvb, offset3+1);
    proto_tree_add_text(rsvp_object_tree, tvb, offset3+1, 1,
			"Error code: %u - %s", error_code,
			val_to_str(error_code, rsvp_error_codes, "Unknown (%d)"));
    error_val = dissect_rsvp_error_value(rsvp_object_tree, tvb, offset3+2, error_code);

    switch (type) {
    case 1:
	proto_item_set_text(ti, "ERROR: IPv4, Error code: %s, Value: %d, Error Node: %s",
			    val_to_str(error_code, rsvp_error_codes, "Unknown (%d)"),
			    error_val, ip_to_str(tvb_get_ptr(tvb, offset2, 4)));
	break;
    case 3:
	proto_item_set_text(ti, "ERROR: IPv4 IF-ID, Error code: %s, Value: %d, Control Node: %s. ",
			    val_to_str(error_code, rsvp_error_codes, "Unknown (%d)"),
			    error_val, ip_to_str(tvb_get_ptr(tvb, offset2, 4)));
	dissect_rsvp_ifid_tlv(ti, rsvp_object_tree, tvb, offset+12, obj_length, 
			      TREE(TT_ERROR_SUBOBJ));
	break;
    }
}
