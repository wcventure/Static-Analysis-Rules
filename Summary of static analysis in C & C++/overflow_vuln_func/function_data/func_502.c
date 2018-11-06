static void
dissect_rsvp_association (proto_tree *ti, proto_tree *rsvp_object_tree,
			  tvbuff_t *tvb,
			  int offset, int obj_length,
			  int class _U_, int type)
{
    guint16 association_type;
    guint16 association_id;
    static value_string association_type_vals[] = {
      {0, "Reserved"},
      {1, "Recovery"},
      { 0, NULL}
    };

    proto_item_set_text(ti, "ASSOCIATION ");
    association_type = tvb_get_ntohs (tvb, offset + 4);
    association_id = tvb_get_ntohs (tvb, offset + 6);
    switch(type) {
    case 1:
	proto_tree_add_text(rsvp_object_tree, tvb, offset+3, 1,
			    "C-type: 1 (IPv4)");
	proto_item_append_text(ti, "(IPv4): ");
	proto_tree_add_text(rsvp_object_tree, tvb, offset+4, 2,
			    "Association type: %s", 
			    val_to_str(association_type, association_type_vals, "Unknown (%u)"));
	proto_item_append_text(ti, "%s. ",
			       val_to_str(association_type, association_type_vals, "Unknown (%u)"));
	proto_tree_add_text(rsvp_object_tree, tvb, offset+6, 2,
			    "Association ID: %u", association_id);
	proto_item_append_text(ti, "ID: %u. ", association_id);
	proto_tree_add_text(rsvp_object_tree, tvb, offset+8, 4,
			    "Association source: %s", ip_to_str(tvb_get_ptr(tvb, offset+8, 4)));
	proto_item_append_text(ti, "Src: %s", ip_to_str(tvb_get_ptr(tvb, offset+8, 4)));
	break;

    case 2:
	proto_tree_add_text(rsvp_object_tree, tvb, offset+3, 1,
			    "C-type: 2 (IPv6)");
	proto_item_append_text(ti, "(IPv6): ");
	proto_tree_add_text(rsvp_object_tree, tvb, offset+4, 2,
			    "Association type: %s", 
			    val_to_str(association_type, association_type_vals, "Unknown (%u)"));
	proto_item_append_text(ti, "%s. ",
			       val_to_str(association_type, association_type_vals, "Unknown (%u)"));
	proto_tree_add_text(rsvp_object_tree, tvb, offset+6, 2,
			    "Association ID: %u", association_id);
	proto_item_append_text(ti, "ID: %u. ", association_id);
	proto_tree_add_text(rsvp_object_tree, tvb, offset+8, 16,
			    "Association source: %s", ip6_to_str((const struct e_in6_addr *)
								tvb_get_ptr(tvb, offset+8, 16)));
	proto_item_append_text(ti, "Src: %s", ip6_to_str((const struct e_in6_addr *)
							tvb_get_ptr(tvb, offset+8, 16)));
	break;

    default:
	proto_tree_add_text(rsvp_object_tree, tvb, offset+3, 1,
			    "C-type: Unknown (%u)", type);
	proto_tree_add_text(rsvp_object_tree, tvb, offset+4, obj_length - 4,
			    "Data (%d bytes)", obj_length - 4);
	break;
    }
}
