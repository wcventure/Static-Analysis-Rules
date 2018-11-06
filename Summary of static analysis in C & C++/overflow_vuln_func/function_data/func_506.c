static void
dissect_rsvp_detour (proto_tree *ti, proto_tree *rsvp_object_tree,
		     tvbuff_t *tvb,
		     int offset, int obj_length,
		     int class _U_, int type)
{
    int remaining_length, count;
    int iter;

    proto_item_set_text(ti, "DETOUR: ");
    switch(type) {
    case 7:
        iter = 0;
        proto_tree_add_text(rsvp_object_tree, tvb, offset+3, 1,
			    "C-type: %u", type);
	for (remaining_length = obj_length - 4, count = 1;
	     remaining_length > 0; remaining_length -= 8, count++) {
	    if (remaining_length < 8) {
		proto_tree_add_text(rsvp_object_tree, tvb, offset+remaining_length, 
				    obj_length-remaining_length, 
				    "<<<Invalid length: cannot decode>>>");
		proto_item_append_text(ti, "Invalid length");
		break;
	    }
	    iter++;
	    proto_tree_add_text(rsvp_object_tree, tvb, offset+(4*iter), 4,
				"PLR ID %d: %s", count, 
				ip_to_str(tvb_get_ptr(tvb, offset+(4*iter), 4)));
	    iter++;
	    proto_tree_add_text(rsvp_object_tree, tvb, offset+(4*iter), 4,
				"Avoid Node ID %d: %s", count, 
				ip_to_str(tvb_get_ptr(tvb, offset+(4*iter), 4)));
	}
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
