static void
dissect_rsvp_diffserv_aware_te(proto_tree *ti, proto_tree *rsvp_object_tree,
			       tvbuff_t *tvb,
			       int offset, int obj_length,
			       int class _U_, int type)
{
    int offset2 = offset + 4;
    guint8 ct = 0;
    
    proto_tree_add_item_hidden(rsvp_object_tree,
			       rsvp_filter[RSVPF_DSTE],
			       tvb, offset, 8, FALSE);
    switch(type) {
    case 1:
	ct = tvb_get_guint8(tvb, offset2+3);
	proto_tree_add_text(rsvp_object_tree, tvb, offset+3, 1, "C-type: 1");
	proto_tree_add_item(rsvp_object_tree, 
			    rsvp_filter[RSVPF_DSTE_CLASSTYPE],
			    tvb, offset2+3, 1, FALSE);
	proto_item_set_text(ti, "CLASSTYPE: CT %u", ct);
	break;
    default:
	proto_item_set_text(ti, "CLASSTYPE: (Unknown C-type)");
	proto_tree_add_text(rsvp_object_tree, tvb, offset+3, 1,
			    "C-type: Unknown (%u)", type);
	proto_tree_add_text(rsvp_object_tree, tvb, offset2, obj_length - 4,
			    "Data (%d bytes)", obj_length - 4);
	break;
    }
}
