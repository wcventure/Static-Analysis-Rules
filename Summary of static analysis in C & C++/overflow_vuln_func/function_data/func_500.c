static void
dissect_rsvp_record_route (proto_item *ti, proto_tree *rsvp_object_tree,
			   tvbuff_t *tvb,
			   int offset, int obj_length,
			   int class, int type)
{
    /
    /
    /

    proto_item_set_text(ti, "RECORD ROUTE: ");
    /
    switch(type) {
    case 1: 
	proto_tree_add_text(rsvp_object_tree, tvb, offset+3, 1,
			    "C-type: 1");

	dissect_rsvp_ero_rro_subobjects(ti, rsvp_object_tree, tvb,
					offset + 4, obj_length, class);
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
