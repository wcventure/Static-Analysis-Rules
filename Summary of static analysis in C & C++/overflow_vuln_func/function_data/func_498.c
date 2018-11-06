static void
dissect_rsvp_ero_rro_subobjects (proto_tree *ti, proto_tree *rsvp_object_tree, 
				 tvbuff_t *tvb,
				 int offset, int obj_length, int class)
{
    int i, j, k, l, flags;
    proto_tree *ti2, *rsvp_ro_subtree, *rsvp_rro_flags_subtree;
    int tree_type;

    switch(class) {
    case RSVP_CLASS_EXPLICIT_ROUTE:
	tree_type = TREE(TT_EXPLICIT_ROUTE_SUBOBJ); 
	break;
    case RSVP_CLASS_RECORD_ROUTE:
	tree_type = TREE(TT_RECORD_ROUTE_SUBOBJ); 
	break;
    default:
	/
	return;
    }

    for (i=1, l = 0; l < obj_length - 4; i++) {
	j = tvb_get_guint8(tvb, offset+l) & 0x7f;
	switch(j) {
	case 1: /
	    k = tvb_get_guint8(tvb, offset+l) & 0x80;
	    ti2 = proto_tree_add_text(rsvp_object_tree, tvb,
				      offset+l, 8,
				      "IPv4 Subobject - %s%s",
				      ip_to_str(tvb_get_ptr(tvb, offset+l+2, 4)),
				      class == RSVP_CLASS_EXPLICIT_ROUTE ? 
				      (k ? ", Loose" : ", Strict") : "");
	    rsvp_ro_subtree =
		proto_item_add_subtree(ti2, tree_type);
	    if (class == RSVP_CLASS_EXPLICIT_ROUTE) 
		proto_tree_add_text(rsvp_ro_subtree, tvb, offset+l, 1,
				    k ? "Loose Hop " : "Strict Hop");
	    proto_tree_add_text(rsvp_ro_subtree, tvb, offset+l, 1,
				"Type: 1 (IPv4)");
	    proto_tree_add_text(rsvp_ro_subtree, tvb, offset+l+1, 1,
				"Length: %u",
				tvb_get_guint8(tvb, offset+l+1));
	    proto_tree_add_text(rsvp_ro_subtree, tvb, offset+l+2, 4,
				"IPv4 hop: %s",
				ip_to_str(tvb_get_ptr(tvb, offset+l+2, 4)));
	    proto_tree_add_text(rsvp_ro_subtree, tvb, offset+l+6, 1,
				"Prefix length: %u",
				tvb_get_guint8(tvb, offset+l+6));
	    if (i < 4) {
		proto_item_append_text(ti, "IPv4 %s%s",
				       ip_to_str(tvb_get_ptr(tvb, offset+l+2, 4)),
				       k ? " [L]" : "");
	    }
	    if (class == RSVP_CLASS_RECORD_ROUTE) {
		flags = tvb_get_guint8(tvb, offset+l+7);
		if (flags&0x10) {
		    proto_item_append_text(ti,  " (Node-id)");
		    proto_item_append_text(ti2, " (Node-id)");
		}
		if (flags&0x01) proto_item_append_text(ti2, ", Local Protection Available");
		if (flags&0x02) proto_item_append_text(ti2, ", Local Protection In Use");
		if (flags&0x04) proto_item_append_text(ti2, ", Backup BW Avail");
		if (flags&0x08) proto_item_append_text(ti2, ", Backup is Next-Next-Hop");
		ti2 = proto_tree_add_text(rsvp_ro_subtree, tvb, offset+l+7, 1,
					  "Flags: 0x%02x", flags);
		rsvp_rro_flags_subtree = 
		    proto_item_add_subtree(ti2, TREE(TT_RECORD_ROUTE_SUBOBJ_FLAGS)); 
		proto_tree_add_text(rsvp_rro_flags_subtree, tvb, offset+l+7, 1,
				    decode_boolean_bitfield(flags, 0x01, 8, 
							    "Local Protection Available",
							    "Local Protection Not Available"));
		proto_tree_add_text(rsvp_rro_flags_subtree, tvb, offset+l+7, 1,
				    decode_boolean_bitfield(flags, 0x02, 8, 
							    "Local Protection In Use",
							    "Local Protection Not In Use"));
		proto_tree_add_text(rsvp_rro_flags_subtree, tvb, offset+l+7, 1,
				    decode_boolean_bitfield(flags, 0x04, 8, 
							    "Bandwidth Protection Available",
							    "Bandwidth Protection Not Available"));
		proto_tree_add_text(rsvp_rro_flags_subtree, tvb, offset+l+7, 1,
				    decode_boolean_bitfield(flags, 0x08, 8, 
							    "Node Protection Available",
							    "Node Protection Not Available"));
		proto_tree_add_text(rsvp_rro_flags_subtree, tvb, offset+l+7, 1,
				    decode_boolean_bitfield(flags, 0x10, 8,
							    "Address Specifies a Node-id Address",
							    "Address Doesn't Specify a Node-id Address"));
	    }

	    break;

	case 2: /
	    ti2 = proto_tree_add_text(rsvp_object_tree, tvb,
				      offset+l, 20,
				      "IPv6 Subobject");
	    rsvp_ro_subtree =
		proto_item_add_subtree(ti2, tree_type);
	    k = tvb_get_guint8(tvb, offset+l) & 0x80;
	    if (class == RSVP_CLASS_EXPLICIT_ROUTE) 
		proto_tree_add_text(rsvp_ro_subtree, tvb, offset+l, 1,
				    k ? "Loose Hop " : "Strict Hop");
	    proto_tree_add_text(rsvp_ro_subtree, tvb, offset+l, 1,
				"Type: 2 (IPv6)");
	    proto_tree_add_text(rsvp_ro_subtree, tvb, offset+l+1, 1,
				"Length: %u",
				tvb_get_guint8(tvb, offset+l+1));
	    proto_tree_add_text(rsvp_ro_subtree, tvb, offset+l+2, 16,
				"IPv6 hop: %s",
				ip6_to_str((const struct e_in6_addr *)tvb_get_ptr(tvb, offset+l+2, 16)));
	    proto_tree_add_text(rsvp_ro_subtree, tvb, offset+l+18, 1,
				"Prefix length: %u",
				tvb_get_guint8(tvb, offset+l+18));
	    if (i < 4) {
		proto_item_append_text(ti, "IPv6 [...]%s", k ? " [L]":"");
	    }
	    if (class == RSVP_CLASS_RECORD_ROUTE) {
		flags = tvb_get_guint8(tvb, offset+l+19);
		if (flags&0x10) {
		    proto_item_append_text(ti,  " (Node-id)");
		    proto_item_append_text(ti2, " (Node-id)");
		}
		if (flags&0x01) proto_item_append_text(ti2, ", Local Protection Available");
		if (flags&0x02) proto_item_append_text(ti2, ", Local Protection In Use");
		if (flags&0x04) proto_item_append_text(ti2, ", Backup BW Avail");
		if (flags&0x08) proto_item_append_text(ti2, ", Backup is Next-Next-Hop");
		ti2 = proto_tree_add_text(rsvp_ro_subtree, tvb, offset+l+19, 1,
					  "Flags: 0x%02x", flags);
		rsvp_rro_flags_subtree = 
		    proto_item_add_subtree(ti2, TREE(TT_RECORD_ROUTE_SUBOBJ_FLAGS)); 
		proto_tree_add_text(rsvp_rro_flags_subtree, tvb, offset+l+19, 1,
				    decode_boolean_bitfield(flags, 0x01, 8, 
							    "Local Protection Available",
							    "Local Protection Not Available"));
		proto_tree_add_text(rsvp_rro_flags_subtree, tvb, offset+l+19, 1,
				    decode_boolean_bitfield(flags, 0x02, 8, 
							    "Local Protection In Use",
							    "Local Protection Not In Use"));
		proto_tree_add_text(rsvp_rro_flags_subtree, tvb, offset+l+19, 1,
				    decode_boolean_bitfield(flags, 0x04, 8, 
							    "Backup Tunnel Has Bandwidth",
							    "Backup Tunnel Does Not Have Bandwidth"));
		proto_tree_add_text(rsvp_rro_flags_subtree, tvb, offset+l+19, 1,
				    decode_boolean_bitfield(flags, 0x08, 8, 
							    "Backup Tunnel Goes To Next-Next-Hop",
							    "Backup Tunnel Goes To Next-Hop"));
		proto_tree_add_text(rsvp_rro_flags_subtree, tvb, offset+l+19, 1,
				    decode_boolean_bitfield(flags, 0x10, 8,
							    "Address Specifies a Node-id Address",
							    "Address Doesn't Specify a Node-id Address"));
	    }

	    break;

	case 3: /
	    k = tvb_get_guint8(tvb, offset+l) & 0x80;
	    ti2 = proto_tree_add_text(rsvp_object_tree, tvb,
				      offset+l, 8,
				      "Label Subobject - %d, %s",
				      tvb_get_ntohl(tvb, offset+l+4),
				      class == RSVP_CLASS_EXPLICIT_ROUTE ? 
				      (k ? "Loose" : "Strict") : "");
	    rsvp_ro_subtree =
		proto_item_add_subtree(ti2, tree_type);
	    if (class == RSVP_CLASS_EXPLICIT_ROUTE) 
		proto_tree_add_text(rsvp_ro_subtree, tvb, offset+l, 1,
				    k ? "Loose Hop " : "Strict Hop");
	    proto_tree_add_text(rsvp_ro_subtree, tvb, offset+l, 1,
				"Type: 3 (Label)");
	    proto_tree_add_text(rsvp_ro_subtree, tvb, offset+l+1, 1,
				"Length: %u",
				tvb_get_guint8(tvb, offset+l+1));
	    if (class == RSVP_CLASS_RECORD_ROUTE) {
		flags = tvb_get_guint8(tvb, offset+l+2);
		if (flags&0x01) proto_item_append_text(ti2, ", Local Protection Available");
		if (flags&0x02) proto_item_append_text(ti2, ", Local Protection In Use");
		if (flags&0x04) proto_item_append_text(ti2, ", Backup BW Avail");
		if (flags&0x08) proto_item_append_text(ti2, ", Backup is Next-Next-Hop");
		ti2 = proto_tree_add_text(rsvp_ro_subtree, tvb, offset+l+2, 1,
					  "Flags: 0x%02x", flags);
		rsvp_rro_flags_subtree = 
		    proto_item_add_subtree(ti2, TREE(TT_RECORD_ROUTE_SUBOBJ_FLAGS)); 
		proto_tree_add_text(rsvp_rro_flags_subtree, tvb, offset+l+2, 1,
				    decode_boolean_bitfield(flags, 0x01, 8, 
							    "Local Protection Available",
							    "Local Protection Not Available"));
		proto_tree_add_text(rsvp_rro_flags_subtree, tvb, offset+l+2, 1,
				    decode_boolean_bitfield(flags, 0x02, 8, 
							    "Local Protection In Use",
							    "Local Protection Not In Use"));
		proto_tree_add_text(rsvp_rro_flags_subtree, tvb, offset+l+2, 1,
				    decode_boolean_bitfield(flags, 0x04, 8, 
							    "Backup Tunnel Has Bandwidth",
							    "Backup Tunnel Does Not Have Bandwidth"));
		proto_tree_add_text(rsvp_rro_flags_subtree, tvb, offset+l+2, 1,
				    decode_boolean_bitfield(flags, 0x08, 8, 
							    "Backup Tunnel Goes To Next-Next-Hop",
							    "Backup Tunnel Goes To Next-Hop"));
	    }
	    proto_tree_add_text(rsvp_ro_subtree, tvb, offset+l+3, 1,
				"C-Type: %u",
				tvb_get_guint8(tvb, offset+l+3));
	    proto_tree_add_text(rsvp_ro_subtree, tvb, offset+l+4, 4,
				"Label: %d",
				tvb_get_ntohl(tvb, offset+l+4));
	    if (i < 4) {
		proto_item_append_text(ti, "Label %d%s",
				       tvb_get_ntohl(tvb, offset+l+4),
				       k ? " [L]":"");
	    }
	    break;

	case 4: /
	    k = tvb_get_guint8(tvb, offset+l) & 0x80;
	    ti2 = proto_tree_add_text(rsvp_object_tree, tvb,
				      offset+l, 8,
				      "Unnumbered Interface-ID - %s, %d, %s",
				      ip_to_str(tvb_get_ptr(tvb, offset+l+4, 4)),
				      tvb_get_ntohl(tvb, offset+l+8),
				      class == RSVP_CLASS_EXPLICIT_ROUTE ? 
				      (k ? "Loose" : "Strict") : "");
	    rsvp_ro_subtree =
		proto_item_add_subtree(ti2, tree_type);
	    if (class == RSVP_CLASS_EXPLICIT_ROUTE) 
		proto_tree_add_text(rsvp_ro_subtree, tvb, offset+l, 1,
				    k ? "Loose Hop " : "Strict Hop");
	    proto_tree_add_text(rsvp_ro_subtree, tvb, offset+l, 1,
				"Type: 4 (Unnumbered Interface-ID)");
	    proto_tree_add_text(rsvp_ro_subtree, tvb, offset+l+1, 1,
				"Length: %u",
				tvb_get_guint8(tvb, offset+l+1));
	    if (class == RSVP_CLASS_RECORD_ROUTE) {
		flags = tvb_get_guint8(tvb, offset+l+2);
		if (flags&0x01) proto_item_append_text(ti2, ", Local Protection Available");
		if (flags&0x02) proto_item_append_text(ti2, ", Local Protection In Use");
		if (flags&0x04) proto_item_append_text(ti2, ", Backup BW Avail");
		if (flags&0x08) proto_item_append_text(ti2, ", Backup is Next-Next-Hop");
		ti2 = proto_tree_add_text(rsvp_ro_subtree, tvb, offset+l+2, 1,
					  "Flags: 0x%02x", flags);
		rsvp_rro_flags_subtree = 
		    proto_item_add_subtree(ti2, TREE(TT_RECORD_ROUTE_SUBOBJ_FLAGS)); 
		proto_tree_add_text(rsvp_rro_flags_subtree, tvb, offset+l+2, 1,
				    decode_boolean_bitfield(flags, 0x01, 8, 
							    "Local Protection Available",
							    "Local Protection Not Available"));
		proto_tree_add_text(rsvp_rro_flags_subtree, tvb, offset+l+2, 1,
				    decode_boolean_bitfield(flags, 0x02, 8, 
							    "Local Protection In Use",
							    "Local Protection Not In Use"));
		proto_tree_add_text(rsvp_rro_flags_subtree, tvb, offset+l+2, 1,
				    decode_boolean_bitfield(flags, 0x04, 8, 
							    "Backup Tunnel Has Bandwidth",
							    "Backup Tunnel Does Not Have Bandwidth"));
		proto_tree_add_text(rsvp_rro_flags_subtree, tvb, offset+l+2, 1,
				    decode_boolean_bitfield(flags, 0x08, 8, 
							    "Backup Tunnel Goes To Next-Next-Hop",
							    "Backup Tunnel Goes To Next-Hop"));
	    }
	    proto_tree_add_text(rsvp_ro_subtree, tvb, offset+l+4, 4,
				"Router-ID: %s",
				ip_to_str(tvb_get_ptr(tvb, offset+l+4, 4)));
	    proto_tree_add_text(rsvp_ro_subtree, tvb, offset+l+8, 4,
				"Interface-ID: %d",
				tvb_get_ntohl(tvb, offset+l+8));
	    if (i < 4) {
		proto_item_append_text(ti, "Unnum %s/%d%s",
				       ip_to_str(tvb_get_ptr(tvb, offset+l+4, 4)),
				       tvb_get_ntohl(tvb, offset+l+8),
				       k ? " [L]":"");
	    }

	    break;

	case 32: /
	    if (class == RSVP_CLASS_RECORD_ROUTE) goto defaultsub;
	    k = tvb_get_ntohs(tvb, offset+l+2);
	    ti2 = proto_tree_add_text(rsvp_object_tree, tvb,
				      offset+l, 4,
				      "Autonomous System %u",
				      k);
	    rsvp_ro_subtree =
		proto_item_add_subtree(ti2, tree_type);
	    proto_tree_add_text(rsvp_ro_subtree, tvb, offset+l, 1,
				"Type: 32 (Autonomous System Number)");
	    proto_tree_add_text(rsvp_ro_subtree, tvb, offset+l+1, 1,
				"Length: %u",
				tvb_get_guint8(tvb, offset+l+1));
	    proto_tree_add_text(rsvp_ro_subtree, tvb, offset+l+2, 2,
				"Autonomous System %u", k);
	    if (i < 4) {
		proto_item_append_text(ti, "AS %d",
				       tvb_get_ntohs(tvb, offset+l+2));
	    }

	    break;

	default: /
	defaultsub:
	    k = tvb_get_guint8(tvb, offset+l) & 0x80;
	    ti2 = proto_tree_add_text(rsvp_object_tree, tvb,
				      offset+l,
				      tvb_get_guint8(tvb, offset+l+1),
				      "Unknown subobject: %d", j);
	    rsvp_ro_subtree =
		proto_item_add_subtree(ti2, tree_type);
	    proto_tree_add_text(rsvp_ro_subtree, tvb, offset+l, 1,
				k ? "Loose Hop " : "Strict Hop");
	    proto_tree_add_text(rsvp_ro_subtree, tvb, offset+l, 1,
				"Type: %u (Unknown)", j);
	    proto_tree_add_text(rsvp_ro_subtree, tvb, offset+l+1, 1,
				"Length: %u",
				tvb_get_guint8(tvb, offset+l+1));

	}

	if (tvb_get_guint8(tvb, offset+l+1) < 1) {
	    proto_tree_add_text(rsvp_ro_subtree, tvb, offset+l+1, 1,
		"Invalid length: %u", tvb_get_guint8(tvb, offset+l+1));
	    return;
	}
	l += tvb_get_guint8(tvb, offset+l+1);
	if (l < obj_length - 4) {
	    if (i < 4)
		proto_item_append_text(ti, ", ");
	    else if (i==4)
		proto_item_append_text(ti, "...");
	}
    }
}
