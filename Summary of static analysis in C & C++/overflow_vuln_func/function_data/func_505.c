static void
dissect_rsvp_fast_reroute (proto_tree *ti, proto_tree *rsvp_object_tree,
			   tvbuff_t *tvb,
			   int offset, int obj_length,
			   int class _U_, int type)
{
    guint8 flags;
    proto_tree *ti2, *rsvp_frr_flags_tree;

    proto_item_set_text(ti, "FAST_REROUTE: ");
    switch(type) {
    case 1:
    case 7:
	if ((type==1 && obj_length!=24) || (type==7 && obj_length!=20)) {
	    proto_tree_add_text(rsvp_object_tree, tvb, offset, obj_length, 
				"<<<Invalid length: cannot decode>>>");
	    proto_item_append_text(ti, "Invalid length");
	    break;
	}
	proto_tree_add_text(rsvp_object_tree, tvb, offset+3, 1,
			    "C-type: %u", type);
	proto_tree_add_text(rsvp_object_tree, tvb, offset+4, 1, 
			    "Setup Priority: %d", tvb_get_guint8(tvb, offset+4));
	proto_tree_add_text(rsvp_object_tree, tvb, offset+5, 1, 
			    "Hold Priority: %d", tvb_get_guint8(tvb, offset+5));
	proto_tree_add_text(rsvp_object_tree, tvb, offset+6, 1, 
			    "Hop Limit: %d", tvb_get_guint8(tvb, offset+6));

        flags = tvb_get_guint8(tvb, offset+7);
	ti2 = proto_tree_add_text(rsvp_object_tree, tvb, offset+7, 1, 
                                  "Flags: 0x%02x", flags);
        rsvp_frr_flags_tree = proto_item_add_subtree(ti2,
                                                     TREE(TT_FAST_REROUTE_FLAGS));
	proto_tree_add_text(rsvp_frr_flags_tree, tvb, offset+7, 1,
			    decode_boolean_bitfield(flags, 0x01, 8,
						    "One-to-One Backup desired",
						    "One-to-One Backup not desired"));
	proto_tree_add_text(rsvp_frr_flags_tree, tvb, offset+7, 1,
			    decode_boolean_bitfield(flags, 0x02, 8,
						    "Facility Backup desired",
						    "Facility Backup not desired"));
	proto_tree_add_text(rsvp_object_tree, tvb, offset+8, 4, 
			    "Bandwidth: %.10g", tvb_get_ntohieee_float(tvb, offset+8));
	proto_tree_add_text(rsvp_object_tree, tvb, offset+12, 4, 
			    "Include-Any: 0x%0x", tvb_get_ntohl(tvb, offset+12));
	proto_tree_add_text(rsvp_object_tree, tvb, offset+16, 4, 
			    "Exclude-Any: 0x%0x", tvb_get_ntohl(tvb, offset+16));
	if (type==1) {
	    proto_tree_add_text(rsvp_object_tree, tvb, offset+20, 4, 
				"Include-All: 0x%0x", tvb_get_ntohl(tvb, offset+20));
	}

        proto_item_append_text(ti, "%s%s",
                               flags &0x01 ? "One-to-One Backup, " : "",
                               flags &0x02 ? "Facility Backup" : "");
	break;

    default:
	proto_tree_add_text(rsvp_object_tree, tvb, offset+3, 1,
			    "C-type: Unknown (%u)",
			    type);
	proto_tree_add_text(rsvp_object_tree, tvb, offset+4, obj_length - 4,
			    "Data (%d bytes)", obj_length - 4);
	break;
    }
}
