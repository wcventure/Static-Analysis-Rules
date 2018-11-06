static void
dissect_rsvp_label_set (proto_item *ti, proto_tree *rsvp_object_tree,
			tvbuff_t *tvb,
		        int offset, int obj_length,
		        int class _U_, int type)
{
    int offset2 = offset + 8;
    guint8 label_type;
    int len, i;

    static value_string action_type_vals[] = {
      {0, "Inclusive list"},
      {1, "Exclusive list"},
      {2, "Inclusive range"},
      {3, "Exclusive range"},
      {0, NULL}
   };

    len = obj_length - 8;
    proto_tree_add_text(rsvp_object_tree, tvb, offset+3, 1, "C-type: %u", type);
    proto_tree_add_text(rsvp_object_tree, tvb, offset+4, 1, "Action: %s", 
			val_to_str(tvb_get_guint8(tvb, offset+4),
			action_type_vals, "Unknown (%u)"));
    proto_item_append_text(ti, ": %s",
			   val_to_str(tvb_get_guint8(tvb, offset+4),
			   action_type_vals, "Unknown (%u)"));
    label_type = tvb_get_guint8 (tvb, offset+7);
    proto_tree_add_text(rsvp_object_tree, tvb, offset+7, 1, "Label type: %s",
			label_type==1 ? "Packet Label" : "Generalized Label");
    proto_item_append_text(ti, ", %s",
			label_type==1 ? "Packet Label: " : "Generalized Label: ");

    for (i = 0; i < len/4; i++) {
	proto_tree_add_text(rsvp_object_tree, tvb, offset2+i*4, 4,
			    "Subchannel %u: %u", i+1,
			    tvb_get_ntohl(tvb, offset2+i*4));

	if (i<5) {
		if (i!=0)
		  proto_item_append_text(ti, ", ");

		proto_item_append_text(ti, "%u",
					tvb_get_ntohl(tvb, offset2+i*4));
	}
    }
}
