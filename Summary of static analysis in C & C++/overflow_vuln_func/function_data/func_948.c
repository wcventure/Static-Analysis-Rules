static void
dissect_rsvp_adspec (proto_tree *ti, tvbuff_t *tvb,
		     int offset, int obj_length,
		     int class, int type,
		     const char *type_str)
{
    int offset2 = offset + 4;
    proto_tree *rsvp_object_tree;
    int mylen, i;
    proto_tree *adspec_tree;

    rsvp_object_tree = proto_item_add_subtree(ti, TREE(TT_ADSPEC));
    proto_tree_add_text(rsvp_object_tree, tvb, offset, 2,
			"Length: %u", obj_length);
    proto_tree_add_text(rsvp_object_tree, tvb, offset+2, 1,
			"Class number: %u - %s",
			class, type_str);
    proto_tree_add_text(rsvp_object_tree, tvb, offset+3, 1,
			"C-type: %u", type);
    mylen = obj_length - 4;

    proto_tree_add_text(rsvp_object_tree, tvb, offset2, 1,
			"Message format version: %u",
			tvb_get_guint8(tvb, offset2)>>4);
    proto_tree_add_text(rsvp_object_tree, tvb, offset2+2, 2,
			"Data length: %u words, not including header",
			tvb_get_ntohs(tvb, offset2+2));
    mylen -= 4;
    offset2 += 4;
    while (mylen > 0) {
	guint8 service_num;
	guint8 break_bit;
	guint16 length;
	const char *str;

	service_num = tvb_get_guint8(tvb, offset2);
	str = val_to_str(service_num, intsrv_services_str, "Unknown");
	break_bit = tvb_get_guint8(tvb, offset2+1);
	length = tvb_get_ntohs(tvb, offset2+2);
	ti = proto_tree_add_text(rsvp_object_tree, tvb, offset2,
				 (length+1)*4,
				 str);
	adspec_tree = proto_item_add_subtree(ti,
					     TREE(TT_ADSPEC_SUBTREE));
	proto_tree_add_text(adspec_tree, tvb, offset2, 1,
			    "Service header %u - %s",
			    service_num, str);
	proto_tree_add_text(adspec_tree, tvb, offset2+1, 1,
			    (break_bit&0x80)?
			    "Break bit set":"Break bit not set");
	proto_tree_add_text(adspec_tree, tvb, offset2+2, 2,
			    "Data length: %u words, not including header",
			    length);
	mylen -= 4;
	offset2 += 4;
	i = length*4;
	while (i > 0) {
	    guint8 id;
	    guint16 phdr_length;

	    id = tvb_get_guint8(tvb, offset2);
	    phdr_length = tvb_get_ntohs(tvb, offset2+2);
	    str = match_strval(id, adspec_params);
	    if (str) {
		switch(id) {
		case 4:
		case 8:
		case 10:
		case 133:
		case 134:
		case 135:
		case 136:
		    /
		    proto_tree_add_text(adspec_tree, tvb, offset2,
					(phdr_length+1)<<2,
					"%s - %u (type %u, length %u)",
					str,
					tvb_get_ntohl(tvb, offset2+4),
					id, phdr_length);
		    break;

		case 6:
		    /
		    proto_tree_add_text(adspec_tree, tvb, offset2,
					(phdr_length+1)<<2,
					"%s - %.10g (type %u, length %u)",
					str,
					tvb_get_ntohieee_float(tvb, offset2+4),
					id, phdr_length);
		    break;
		default:
		    proto_tree_add_text(adspec_tree, tvb, offset2,
					(phdr_length+1)<<2,
					"%s (type %u, length %u)",
					str,
					id, phdr_length);
		}
	    } else {
		proto_tree_add_text(adspec_tree, tvb, offset2,
				    (phdr_length+1)<<2,
				    "Unknown (type %u, length %u)",
				    id, phdr_length);
	    }
	    offset2 += (phdr_length+1)<<2;
	    i -= (phdr_length+1)<<2;
	    mylen -= (phdr_length+1)<<2;
	}
    }
}
