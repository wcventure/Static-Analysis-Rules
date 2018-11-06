static void
dissect_rsvp_session_attribute (proto_item *ti, proto_tree *rsvp_object_tree,
				tvbuff_t *tvb,
				int offset, int obj_length,
				int class _U_, int type)
{
    int offset2 = offset + 4;
    guint8 flags;
    guint8 name_len;
    proto_tree *ti2, *rsvp_sa_flags_tree;

    switch(type) {
    case 1:
    case 7:

	proto_tree_add_text(rsvp_object_tree, tvb, offset+3, 1,
			    "C-type: %u - IPv4 LSP (%sResource Affinities)",
                            type, (type == 1) ? "" : "No ");
        
        if (type == 1) {
            proto_tree_add_text(rsvp_object_tree, tvb, offset2, 4, 
			    "Exclude-Any: 0x%0x", tvb_get_ntohl(tvb, offset2));
            proto_tree_add_text(rsvp_object_tree, tvb, offset2+4, 4, 
			    "Include-Any: 0x%0x", tvb_get_ntohl(tvb, offset2+4));
            proto_tree_add_text(rsvp_object_tree, tvb, offset2+8, 4, 
			    "Include-All: 0x%0x", tvb_get_ntohl(tvb, offset2+8));
            offset2 = offset2+12;
        }

	proto_tree_add_text(rsvp_object_tree, tvb, offset2, 1,
			    "Setup priority: %u",
			    tvb_get_guint8(tvb, offset2));
	proto_tree_add_text(rsvp_object_tree, tvb, offset2+1, 1,
			    "Hold priority: %u",
			    tvb_get_guint8(tvb, offset2+1));
	flags = tvb_get_guint8(tvb, offset2+2);
	ti2 = proto_tree_add_text(rsvp_object_tree, tvb, offset2+2, 1,
				  "Flags: 0x%02x", flags);
	rsvp_sa_flags_tree = proto_item_add_subtree(ti2,
						    TREE(TT_SESSION_ATTRIBUTE_FLAGS));
	proto_tree_add_text(rsvp_sa_flags_tree, tvb, offset2+2, 1,
			    decode_boolean_bitfield(flags, 0x01, 8,
						    "Local protection desired",
						    "Local protection not desired"));
	proto_tree_add_text(rsvp_sa_flags_tree, tvb, offset2+2, 1,
			    decode_boolean_bitfield(flags, 0x02, 8,
						    "Label recording desired",
						    "Label recording not desired"));
	proto_tree_add_text(rsvp_sa_flags_tree, tvb, offset2+2, 1,
			    decode_boolean_bitfield(flags, 0x04, 8,
						    "SE style desired",
						    "SE style not desired"));
	proto_tree_add_text(rsvp_sa_flags_tree, tvb, offset2+2, 1,
			    decode_boolean_bitfield(flags, 0x08, 8,
						    "Bandwidth protection desired",
						    "Bandwidth protection not desired"));
	proto_tree_add_text(rsvp_sa_flags_tree, tvb, offset2+2, 1,
			    decode_boolean_bitfield(flags, 0x10, 8,
						    "Node protection desired",
						    "Node protection not desired"));

	name_len = tvb_get_guint8(tvb, offset2+3);
	proto_tree_add_text(rsvp_object_tree, tvb, offset2+3, 1,
			    "Name length: %u", name_len);
	proto_tree_add_text(rsvp_object_tree, tvb, offset2+4, name_len,
			    "Name: %s",
			    tvb_format_text(tvb, offset2+4, name_len));

	proto_item_set_text(ti, "SESSION ATTRIBUTE: SetupPrio %d, HoldPrio %d, %s%s%s%s%s [%s]",
			    tvb_get_guint8(tvb, offset2),
			    tvb_get_guint8(tvb, offset2+1),
			    flags &0x01 ? "Local Protection, " : "",
			    flags &0x02 ? "Label Recording, " : "",
			    flags &0x04 ? "SE Style, " : "",
			    flags &0x08 ? "Bandwidth Protection, " : "",
			    flags &0x10 ? "Node Protection, " : "",
			    name_len ? tvb_format_text(tvb, offset2+4, name_len) : "");
	break;

    default:
	proto_tree_add_text(rsvp_object_tree, tvb, offset+3, 1,
			    "C-type: Unknown (%u)",
			    type);
	proto_tree_add_text(rsvp_object_tree, tvb, offset2, obj_length - 4,
			    "Data (%d bytes)", obj_length - 4);
	break;
    }
}
