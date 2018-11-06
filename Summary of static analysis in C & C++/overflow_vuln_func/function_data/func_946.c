static void
dissect_rsvp_tspec (proto_tree *ti, tvbuff_t *tvb,
		    int offset, int obj_length,
		    int class, int type,
		    const char *type_str)
{
    int offset2 = offset + 4;
    proto_tree *rsvp_object_tree;
    int mylen;
    proto_tree *tspec_tree, *ti2;
    guint8 signal_type;

    rsvp_object_tree = proto_item_add_subtree(ti, TREE(TT_TSPEC));
    proto_tree_add_text(rsvp_object_tree, tvb, offset, 2,
			"Length: %u", obj_length);
    proto_tree_add_text(rsvp_object_tree, tvb, offset+2, 1,
			"Class number: %u - %s",
			class, type_str);
    mylen = obj_length - 4;

    switch(type) {
    case 2:
	proto_tree_add_text(rsvp_object_tree, tvb, offset+3, 1,
			    "C-type: 1 - Integrated Services");
	proto_tree_add_text(rsvp_object_tree, tvb, offset2, 1,
			    "Message format version: %u",
			    tvb_get_guint8(tvb, offset2)>>4);
	proto_tree_add_text(rsvp_object_tree, tvb, offset2+2, 2,
			    "Data length: %u words, not including header",
			    tvb_get_ntohs(tvb, offset2+2));

	mylen -= 4;
	offset2 += 4;

	proto_item_set_text(ti, "SENDER TSPEC: IntServ: ");

	while (mylen > 0) {
	    guint8 service_num;
	    guint8 param_id;
	    guint16 param_len;
	    guint16 param_len_processed;
	    guint16 length;

	    service_num = tvb_get_guint8(tvb, offset2);
	    proto_tree_add_text(rsvp_object_tree, tvb, offset2, 1,
				"Service header: %u - %s",
				service_num,
				val_to_str(service_num, qos_vals, "Unknown"));
	    length = tvb_get_ntohs(tvb, offset2+2);
	    proto_tree_add_text(rsvp_object_tree, tvb, offset2+2, 2,
				"Length of service %u data: %u words, "
				"not including header",
				service_num, length);

	    mylen -= 4;
	    offset2 += 4;

	    /
	    param_len_processed = 0;
	    while (param_len_processed < length) {
		param_id = tvb_get_guint8(tvb, offset2);
		param_len = tvb_get_ntohs(tvb, offset2+2) + 1;
		if (param_len < 1)
		    THROW(ReportedBoundsError);
		switch(param_id) {
		case 127:
		    /
		    ti2 = proto_tree_add_text(rsvp_object_tree, tvb,
					      offset2, param_len*4,
					      "Token Bucket TSpec: ");
		    tspec_tree = proto_item_add_subtree(ti2, TREE(TT_TSPEC_SUBTREE));

		    proto_tree_add_text(tspec_tree, tvb, offset2, 1,
					"Parameter %u - %s",
					param_id,
					val_to_str(param_id, svc_vals, "Unknown"));
		    proto_tree_add_text(tspec_tree, tvb, offset2+1, 1,
					"Parameter %u flags: 0x%02x",
					param_id,
					tvb_get_guint8(tvb, offset2+1));
		    proto_tree_add_text(tspec_tree, tvb, offset2+2, 2,
					"Parameter %u data length: %u words, "
					"not including header",
					param_id,
					tvb_get_ntohs(tvb, offset2+2));
		    proto_tree_add_text(tspec_tree, tvb, offset2+4, 4,
					"Token bucket rate: %.10g",
					tvb_get_ntohieee_float(tvb, offset2+4));
		    proto_tree_add_text(tspec_tree, tvb, offset2+8, 4,
					"Token bucket size: %.10g",
					tvb_get_ntohieee_float(tvb, offset2+8));
		    proto_tree_add_text(tspec_tree, tvb, offset2+12, 4,
					"Peak data rate: %.10g",
					tvb_get_ntohieee_float(tvb, offset2+12));
		    proto_tree_add_text(tspec_tree, tvb, offset2+16, 4,
					"Minimum policed unit [m]: %u",
					tvb_get_ntohl(tvb, offset2+16));
		    proto_tree_add_text(tspec_tree, tvb, offset2+20, 4,
					"Maximum packet size [M]: %u",
					tvb_get_ntohl(tvb, offset2+20));
		    proto_item_append_text(ti, "Token Bucket, %.10g bytes/sec. ",
					   tvb_get_ntohieee_float(tvb, offset2+4));
		    proto_item_append_text(ti2, "Rate=%.10g Burst=%.10g Peak=%.10g m=%u M=%u",
					   tvb_get_ntohieee_float(tvb, offset2+4),
					   tvb_get_ntohieee_float(tvb, offset2+8),
					   tvb_get_ntohieee_float(tvb, offset2+12),
					   tvb_get_ntohl(tvb, offset2+16),
					   tvb_get_ntohl(tvb, offset2+20));
		    break;

		case 128:
		    /
		    ti2 = proto_tree_add_text(rsvp_object_tree, tvb,
					      offset2, param_len*4,
					      "Null Service TSpec: ");
		    tspec_tree = proto_item_add_subtree(ti2, TREE(TT_TSPEC_SUBTREE));

		    proto_tree_add_text(tspec_tree, tvb, offset2, 1,
					"Parameter %u - %s",
					param_id,
					val_to_str(param_id, svc_vals, "Unknown"));
		    proto_tree_add_text(tspec_tree, tvb, offset2+1, 1,
					"Parameter %u flags: %x",
					param_id,
					tvb_get_guint8(tvb, offset2+1));
		    proto_tree_add_text(tspec_tree, tvb, offset2+2, 2,
					"Parameter %u data length: %u words, "
					"not including header",
					param_id,
					tvb_get_ntohs(tvb, offset2+2));
		    proto_tree_add_text(tspec_tree, tvb, offset2+4, 4,
					"Maximum packet size [M]: %u",
					tvb_get_ntohl(tvb, offset2+4));
		    proto_item_append_text(ti, "Null Service. M=%u",
					   tvb_get_ntohl(tvb, offset2+4));
		    proto_item_append_text(ti2, "Max pkt size=%u",
					   tvb_get_ntohl(tvb, offset2+4));
		    break;

		case 126:
		    /
		    ti2 = proto_tree_add_text(rsvp_object_tree, tvb,
					      offset2, param_len*4,
					      "Compression Hint: ");
		    tspec_tree = proto_item_add_subtree(ti2, TREE(TT_TSPEC_SUBTREE));

		    proto_tree_add_text(tspec_tree, tvb, offset2, 1,
					"Parameter %u - %s",
					param_id,
					val_to_str(param_id, svc_vals, "Unknown"));
		    proto_tree_add_text(tspec_tree, tvb, offset2+1, 1,
					"Parameter %u flags: %x",
					param_id,
					tvb_get_guint8(tvb, offset2+1));
		    proto_tree_add_text(tspec_tree, tvb, offset2+2, 2,
					"Parameter %u data length: %u words, "
					"not including header",
					param_id,
					tvb_get_ntohs(tvb, offset2+2));
		    proto_tree_add_text(tspec_tree, tvb, offset2+4, 4,
					"Hint: %u",
					tvb_get_ntohl(tvb, offset2+4));
		    proto_tree_add_text(tspec_tree, tvb, offset2+4, 4,
					"Compression Factor: %u",
					tvb_get_ntohl(tvb, offset2+8));
		    proto_item_append_text(ti, "Compression Hint. Hint=%u, Factor=%u",
					   tvb_get_ntohl(tvb, offset2+4),
					   tvb_get_ntohl(tvb, offset2+8));
		    proto_item_append_text(ti2, "Hint=%u, Factor=%u",
					   tvb_get_ntohl(tvb, offset2+4),
					   tvb_get_ntohl(tvb, offset2+8));
		    break;

		default:
		    proto_tree_add_text(rsvp_object_tree, tvb, offset2, param_len*4,
					"Unknown parameter %d, %d words",
					param_id, param_len);
		    break;
		}
		param_len_processed += param_len;
		offset2 += param_len*4;
	    }
	    mylen -= length*4;
	}
	break;

    case 4: /
	proto_item_set_text(ti, "SENDER TSPEC: SONET/SDH: ");

	proto_tree_add_text(rsvp_object_tree, tvb, offset+3, 1,
			    "C-type: 4 - SONET/SDH");
	signal_type = tvb_get_guint8(tvb, offset2);
	proto_tree_add_text(rsvp_object_tree, tvb, offset2, 1,
			    "Signal Type: %d - %s", signal_type,
			    val_to_str(signal_type,
				       gmpls_sonet_signal_type_str, "Unknown"));
	ti2 = proto_tree_add_text(rsvp_object_tree, tvb, offset2+1, 1,
			    "Requested Concatenation (RCC): %d", tvb_get_guint8(tvb, offset2+1));
	tspec_tree = proto_item_add_subtree(ti2, TREE(TT_TSPEC_SUBTREE));
	proto_tree_add_text(tspec_tree, tvb, offset2+1, 1,
			    decode_boolean_bitfield(tvb_get_guint8(tvb, offset2+1), 0x01, 8,
						    "Standard contiguous concatenation",
						    "No standard contiguous concatenation"));
	proto_tree_add_text(tspec_tree, tvb, offset2+1, 1,
			    decode_boolean_bitfield(tvb_get_guint8(tvb, offset2+1), 0x02, 8,
						    "Arbitrary contiguous concatenation",
						    "No arbitrary contiguous concatenation"));
	proto_tree_add_text(rsvp_object_tree, tvb, offset2+2, 2,
			    "Number of Contiguous Components (NCC): %d", tvb_get_ntohs(tvb, offset2+2));
	proto_tree_add_text(rsvp_object_tree, tvb, offset2+4, 2,
			    "Number of Virtual Components (NVC): %d", tvb_get_ntohs(tvb, offset2+4));
	proto_tree_add_text(rsvp_object_tree, tvb, offset2+6, 2,
			    "Multiplier (MT): %d", tvb_get_ntohs(tvb, offset2+6));
	ti2 = proto_tree_add_text(rsvp_object_tree, tvb, offset2+8, 4,
				  "Transparency (T): 0x%0x", tvb_get_ntohl(tvb, offset2+8));
	tspec_tree = proto_item_add_subtree(ti2, TREE(TT_TSPEC_SUBTREE));
	proto_tree_add_text(tspec_tree, tvb, offset2+8, 4,
			    decode_boolean_bitfield(tvb_get_ntohl(tvb, offset2+8), 0x0001, 32,
						    "Section/Regenerator Section layer transparency",
						    "No Section/Regenerator Section layer transparency"));
	proto_tree_add_text(tspec_tree, tvb, offset2+8, 4,
			    decode_boolean_bitfield(tvb_get_guint8(tvb, offset2+8), 0x0002, 32,
						    "Line/Multiplex Section layer transparency",
						    "No Line/Multiplex Section layer transparency"));
	proto_tree_add_text(tspec_tree, tvb, offset2+8, 4,
			    decode_boolean_bitfield(tvb_get_guint8(tvb, offset2+8), 0x0004, 32,
						    "J0 transparency",
						    "No J0 transparency"));
	proto_tree_add_text(tspec_tree, tvb, offset2+8, 4,
			    decode_boolean_bitfield(tvb_get_guint8(tvb, offset2+8), 0x0008, 32,
						    "SOH/RSOH DCC transparency",
						    "No SOH/RSOH DCC transparency"));
	proto_tree_add_text(tspec_tree, tvb, offset2+8, 4,
			    decode_boolean_bitfield(tvb_get_guint8(tvb, offset2+8), 0x0010, 32,
						    "LOH/MSOH DCC transparency",
						    "No LOH/MSOH DCC transparency"));
	proto_tree_add_text(tspec_tree, tvb, offset2+8, 4,
			    decode_boolean_bitfield(tvb_get_guint8(tvb, offset2+8), 0x0020, 32,
						    "LOH/MSOH Extended DCC transparency",
						    "No LOH/MSOH Extended DCC transparency"));
	proto_tree_add_text(tspec_tree, tvb, offset2+8, 4,
			    decode_boolean_bitfield(tvb_get_guint8(tvb, offset2+8), 0x0040, 32,
						    "K1/K2 transparency",
						    "No K1/K2 transparency"));
	proto_tree_add_text(tspec_tree, tvb, offset2+8, 4,
			    decode_boolean_bitfield(tvb_get_guint8(tvb, offset2+8), 0x0080, 32,
						    "E1 transparency",
						    "No E1 transparency"));
	proto_tree_add_text(tspec_tree, tvb, offset2+8, 4,
			    decode_boolean_bitfield(tvb_get_guint8(tvb, offset2+8), 0x0100, 32,
						    "F1 transparency",
						    "No F1 transparency"));
	proto_tree_add_text(tspec_tree, tvb, offset2+8, 4,
			    decode_boolean_bitfield(tvb_get_guint8(tvb, offset2+8), 0x0200, 32,
						    "E2 transparency",
						    "No E2 transparency"));
	proto_tree_add_text(tspec_tree, tvb, offset2+8, 4,
			    decode_boolean_bitfield(tvb_get_guint8(tvb, offset2+8), 0x0400, 32,
						    "B1 transparency",
						    "No B1 transparency"));
	proto_tree_add_text(tspec_tree, tvb, offset2+8, 4,
			    decode_boolean_bitfield(tvb_get_guint8(tvb, offset2+8), 0x0800, 32,
						    "B2 transparency",
						    "No B2 transparency"));
	proto_tree_add_text(tspec_tree, tvb, offset2+8, 4,
			    decode_boolean_bitfield(tvb_get_guint8(tvb, offset2+8), 0x1000, 32,
						    "M0 transparency",
						    "No M0 transparency"));
	proto_tree_add_text(tspec_tree, tvb, offset2+8, 4,
			    decode_boolean_bitfield(tvb_get_guint8(tvb, offset2+8), 0x2000, 32,
						    "M1 transparency",
						    "No M1 transparency"));
	proto_tree_add_text(rsvp_object_tree, tvb, offset2+12, 4,
			    "Profile (P): %d", tvb_get_ntohl(tvb, offset2+12));

	proto_item_append_text(ti, "Signal [%s], RCC %d, NCC %d, NVC %d, MT %d, Transparency %d, Profile %d",
			       val_to_str(signal_type, gmpls_sonet_signal_type_str, "Unknown"),
			       tvb_get_guint8(tvb, offset2+1), tvb_get_ntohs(tvb, offset2+2),
			       tvb_get_ntohs(tvb, offset2+4), tvb_get_ntohs(tvb, offset2+6),
			       tvb_get_ntohl(tvb, offset2+8), tvb_get_ntohl(tvb, offset2+12));
	break;

    default: /
	proto_tree_add_text(rsvp_object_tree, tvb, offset+3, 1,
			    "C-type: Unknown (%u)",
			    type);
	proto_tree_add_text(rsvp_object_tree, tvb, offset2, obj_length - 4,
			    "Data (%d bytes)", obj_length - 4);
	break;

    }
}
