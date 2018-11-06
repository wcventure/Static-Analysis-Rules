static void
dissect_rsvp_call_id (proto_tree *ti, proto_tree *rsvp_object_tree,
		      tvbuff_t *tvb,
		      int offset, int obj_length,
		      int class _U_, int c_type)
{
    int type;
    char *str;
    int offset2 = offset + 4;
    int offset3, offset4, len;

    static value_string address_type_vals[] = {
      {1, "1 (IPv4)"},
      {2, "2 (IPv6)"},
      {3, "3 (NSAP)"},
      {4, "4 (MAC)"},
      {0x7f, "0x7f (Vendor-defined)"},
      {0, NULL}
    };

    proto_item_set_text(ti, "CALL-ID: ");
    type = tvb_get_guint8 (tvb, offset2);
    switch(c_type) {
    case 0:
	  proto_item_append_text(ti,"Empty");
	  proto_tree_add_text(rsvp_object_tree, tvb, offset+3, 1,
			      "C-type: Empty (%u)", type);
	  proto_tree_add_text(rsvp_object_tree, tvb, offset2, obj_length-4,
			      "Data (%d bytes)", obj_length-4);
	  break;
    case 1:
    case 2:
	if (c_type == 1) {
	  offset3 = offset2 + 4;
	  len = obj_length - 16;
	  proto_tree_add_text(rsvp_object_tree, tvb, offset+3, 1,
			    "C-type: 1 (operator specific)");
	  proto_tree_add_text(rsvp_object_tree, tvb, offset2, 1, "Address type: %s",
			      val_to_str(type, address_type_vals, "Unknown (%u)"));
	  proto_tree_add_text(rsvp_object_tree, tvb, offset2+1, 3, "Reserved: %u",
			      tvb_get_ntoh24(tvb, offset2+1));
	  proto_item_append_text(ti, "Operator-Specific. Addr Type: %s. ", 
				 val_to_str(type, address_type_vals, "Unknown (%u)"));
	}
	else {
	  offset3 = offset2 + 16;
	  len = obj_length - 28;
	  proto_tree_add_text(rsvp_object_tree, tvb, offset+3, 1,
			    "C-type: 2 (globally unique)");
	  proto_tree_add_text(rsvp_object_tree, tvb, offset2, 1, "Address type: %s",
			      val_to_str(type, address_type_vals, "Unknown (%u)"));
	  str = tvb_get_ephemeral_string (tvb, offset2 + 1, 3);  
	  proto_tree_add_text(rsvp_object_tree, tvb, offset2 + 1, 3,
			      "International Segment: %s", str); 
	  proto_item_append_text(ti, "Globally-Unique. Addr Type: %s. Intl Segment: %s. ", 
				 val_to_str(type, address_type_vals, "Unknown (%u)"), str);
	  str = tvb_get_ephemeral_string (tvb, offset2 + 4, 12);  
	  proto_tree_add_text(rsvp_object_tree, tvb, offset2 + 4, 12,
			      "National Segment: %s", str); 
	  proto_item_append_text(ti, "Natl Segment: %s. ", str);
	}

	switch(type) {
	case 1:
	  offset4 = offset3 + 4;
	  proto_tree_add_text(rsvp_object_tree, tvb, offset3, 4, "Source Transport Network addr: %s",
			      ip_to_str(tvb_get_ptr(tvb, offset3, 4)));
	  proto_item_append_text(ti, "Src: %s. ", ip_to_str(tvb_get_ptr(tvb, offset3, 4)));
	  break;
	  
	case 2:
	  offset4 = offset3 + 16;
	  proto_tree_add_text(rsvp_object_tree, tvb, offset3, 16, "Source Transport Network addr: %s",
			      ip6_to_str((const struct e_in6_addr *) tvb_get_ptr(tvb, offset3, 16)));
	  proto_item_append_text(ti, "Src: %s. ", 
				 ip6_to_str((const struct e_in6_addr *) tvb_get_ptr(tvb, offset3, 16)));
	  break;
	  
	case 3:
	  offset4 = offset3 + 20;
	  proto_tree_add_text(rsvp_object_tree, tvb, offset3, 20, "Source Transport Network addr: %s",
			      tvb_bytes_to_str(tvb, offset3, 20));
	  proto_item_append_text(ti, "Src: %s. ", tvb_bytes_to_str(tvb, offset3, 20));
	  break;
	  
	case 4:
	  offset4 = offset3 + 6;
	  proto_tree_add_text(rsvp_object_tree, tvb, offset3, 6, "Source Transport Network addr: %s",
			      tvb_bytes_to_str(tvb, offset3, 6));
	  proto_item_append_text(ti, "Src: %s. ", tvb_bytes_to_str(tvb, offset3, 6));
	  break;
	  
	case 0x7F:
	  offset4 = offset3 + len;
	  proto_tree_add_text(rsvp_object_tree, tvb, offset3, len, "Source Transport Network addr: %s",
			      tvb_bytes_to_str(tvb, offset3, len));
	  proto_item_append_text(ti, "Src: %s. ", tvb_bytes_to_str(tvb, offset3, len));
	  break;

	default:
	  offset4 = offset3 + len;
	  proto_tree_add_text(rsvp_object_tree, tvb, offset3, len, "Unknown Transport Network type: %d",
			      type);
	}

	proto_tree_add_text(rsvp_object_tree, tvb, offset4, 8, "Local Identifier: %s",
			    tvb_bytes_to_str(tvb, offset4, 8));
	proto_item_append_text(ti, "Local ID: %s. ", tvb_bytes_to_str(tvb, offset4, 8));
	break;

    default:
	proto_item_append_text(ti, " Unknown");
	proto_tree_add_text(rsvp_object_tree, tvb, offset+3, 1,
			    "C-type: Unknown (%u)", type);
	proto_tree_add_text(rsvp_object_tree, tvb, offset2, obj_length - 4,
			    "Data (%d bytes)", obj_length - 4);
	break;
    }
}
