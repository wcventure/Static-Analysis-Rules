static void
dissect_rsvp_restart_cap (proto_tree *ti, proto_tree *rsvp_object_tree,
			  tvbuff_t *tvb,
			  int offset, int obj_length,
			  int class _U_, int type)
{
    int offset2 = offset + 4;

    proto_item_set_text(ti, "RESTART CAPABILITY: ");
    switch(type) {
    case 1:
	proto_tree_add_text(rsvp_object_tree, tvb, offset+3, 1,
			    "C-type: 1");
	proto_tree_add_text(rsvp_object_tree, tvb, offset2, 4,
			    "Restart Time: %d ms", 
			    tvb_get_ntohl(tvb, offset2));
	proto_tree_add_text(rsvp_object_tree, tvb, offset2+4, 4,
			    "Recovery Time: %d ms", 
			    tvb_get_ntohl(tvb, offset2+4));
	proto_item_append_text(ti, "Restart Time: %d ms. Recovery Time: %d ms.",
			    tvb_get_ntohl(tvb, offset2), tvb_get_ntohl(tvb, offset2+4));
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
