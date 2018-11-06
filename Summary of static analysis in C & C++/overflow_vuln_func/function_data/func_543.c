static void
dissect_mmse(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree)
{
    guint8	 pdut;
    guint	 offset;
    guint8	 field = 0;
    char	 strval[BUFSIZ];
    guint	 length;
    guint	 count;

    /
    proto_item *ti;
    proto_tree *mmse_tree;

    pdut = tvb_get_guint8(tvb, 1);
    /
    if (check_col(pinfo->cinfo, COL_PROTOCOL))
	col_set_str(pinfo->cinfo, COL_PROTOCOL, "MMSE");

    if (check_col(pinfo->cinfo, COL_INFO)) {
	col_clear(pinfo->cinfo, COL_INFO);
	col_add_fstr(pinfo->cinfo, COL_INFO, "MMS %s",
		     match_strval(pdut, vals_message_type));
    }

    /
    if (tree) {
	offset = 2;			/

	/
	ti = proto_tree_add_item(tree, proto_mmse, tvb, 0, -1, FALSE);
	mmse_tree = proto_item_add_subtree(ti, ett_mmse);

	/
	proto_tree_add_uint(mmse_tree, hf_mmse_message_type, tvb, 0, 2, pdut);
	/
	while ((offset < tvb_reported_length(tvb)) &&
	       (field = tvb_get_guint8(tvb, offset++)) != MM_CTYPE_HDR)
	{
	    switch (field)
	    {
		case MM_TID_HDR:		/
		    length = get_text_string(tvb, offset, strval);
		    proto_tree_add_string(mmse_tree, hf_mmse_transaction_id,
					  tvb, offset - 1, length + 1,strval);
		    offset += length;
		    break;
		case MM_VERSION_HDR:		/
		    field = tvb_get_guint8(tvb, offset++);
		    {
			guint8	 major, minor;

			major = (field & 0x70) >> 4;
			minor = field & 0x0F;
			if (minor == 0x0F)
			    sprintf(strval, "%d", major);
			else
			    sprintf(strval, "%d.%d", major, minor);
		    }
		    proto_tree_add_string(mmse_tree, hf_mmse_mms_version, tvb,
			    		  offset - 2, 2, strval);
		    break;
		case MM_BCC_HDR:		/
		    length = get_encoded_strval(tvb, offset, strval);
		    proto_tree_add_string(mmse_tree, hf_mmse_bcc, tvb,
			    		offset - 1, length + 1, strval);
		    offset += length;
		    break;
		case MM_CC_HDR:			/
		    length = get_encoded_strval(tvb, offset, strval);
		    proto_tree_add_string(mmse_tree, hf_mmse_cc, tvb,
			    		offset - 1, length + 1, strval);
		    offset += length;
		    break;
		case MM_CLOCATION_HDR:		/
		    length = get_text_string(tvb, offset, strval);
		    proto_tree_add_string(mmse_tree, hf_mmse_content_location,
					  tvb, offset - 1, length + 1,strval);
		    offset += length;
		    break;
		case MM_DATE_HDR:		/
		    {
			guint		 tval;
			nstime_t	 tmptime;

			tval = get_long_integer(tvb, offset, &count);
			tmptime.secs = tval;
			tmptime.nsecs = 0;
			proto_tree_add_time(mmse_tree, hf_mmse_date, tvb,
					    offset - 1, count + 1, &tmptime);
		    }
		    offset += count;
		    break;
		case MM_DREPORT_HDR:		/
		    field = tvb_get_guint8(tvb, offset++);
		    proto_tree_add_uint(mmse_tree, hf_mmse_delivery_report, tvb,
					offset - 2, 2, field);
		    break;
		case MM_DTIME_HDR:
		    /
		    length = get_value_length(tvb, offset, &count);
		    field = tvb_get_guint8(tvb, offset + count);
		    {
			guint		 tval;
			nstime_t	 tmptime;
			guint		 cnt;

			tval =  get_long_integer(tvb, offset + count + 1, &cnt);
			tmptime.secs = tval;
			tmptime.nsecs = 0;

			if (field == 0x80)
			    proto_tree_add_time(mmse_tree,
					        hf_mmse_delivery_time_abs,
						tvb, offset - 1,
						length + count + 1, &tmptime);
			else
			    proto_tree_add_time(mmse_tree,
						hf_mmse_delivery_time_rel,
						tvb, offset - 1,
						length + count + 1, &tmptime);
		    }
		    offset += length + count;
		    break;
		case MM_EXPIRY_HDR:
		    /
		    length = get_value_length(tvb, offset, &count);
		    field = tvb_get_guint8(tvb, offset + count);
		    {
			guint		 tval;
			nstime_t	 tmptime;
			guint		 cnt;

			tval = get_long_integer(tvb, offset + count + 1, &cnt);
			tmptime.secs = tval;
			tmptime.nsecs = 0;
			if (field == 0x80)
			    proto_tree_add_time(mmse_tree, hf_mmse_expiry_abs,
						tvb, offset - 1,
						length + count + 1, &tmptime);
			else
			    proto_tree_add_time(mmse_tree, hf_mmse_expiry_rel,
						tvb, offset - 1,
						length + count + 1, &tmptime);
		    }
		    offset += length + count;
		    break;
		case MM_FROM_HDR:
		    /
		    length = get_value_length(tvb, offset, &count);
		    field = tvb_get_guint8(tvb, offset + count);
		    if (field == 0x81) {
			strcpy(strval, "<insert address>");
		    } else {
			(void) get_encoded_strval(tvb, offset + count + 1,
						  strval);
		    }
		    proto_tree_add_string(mmse_tree, hf_mmse_from, tvb,
			    		  offset-1, length + count + 1, strval);
		    offset += length + count;
		    break;
		case MM_MCLASS_HDR:
		    /
		    field = tvb_get_guint8(tvb, offset);
		    if (field & 0x80) {
			offset++;
			proto_tree_add_uint(mmse_tree,
					    hf_mmse_message_class_id,
					    tvb, offset - 2, 2, field);
		    } else {
			length = get_text_string(tvb, offset, strval);
			proto_tree_add_string(mmse_tree,
					      hf_mmse_message_class_str,
					      tvb, offset - 1, length + 1,
					      strval);
			offset += length;
		    }
		    break;
		case MM_MID_HDR:		/
		    length = get_text_string(tvb, offset, strval);
		    proto_tree_add_string(mmse_tree, hf_mmse_message_id, tvb,
			    		  offset - 1, length + 1, strval);
		    offset += length;
		    break;
		case MM_MSIZE_HDR:		/
		    length = get_long_integer(tvb, offset, &count);
		    proto_tree_add_uint(mmse_tree, hf_mmse_message_size, tvb,
			    		offset - 1, count + 1, length);
		    offset += count;
		    break;
		case MM_PRIORITY_HDR:		/
		    field = tvb_get_guint8(tvb, offset++);
		    proto_tree_add_uint(mmse_tree, hf_mmse_priority, tvb,
					offset - 2, 2, field);
		    break;
		case MM_RREPLY_HDR:		/
		    field = tvb_get_guint8(tvb, offset++);
		    proto_tree_add_uint(mmse_tree, hf_mmse_read_reply, tvb,
					offset - 2, 2, field);
		    break;
		case MM_RALLOWED_HDR:		/
		    field = tvb_get_guint8(tvb, offset++);
		    proto_tree_add_uint(mmse_tree, hf_mmse_report_allowed, tvb,
					offset - 2, 2, field);
		    break;
		case MM_RSTATUS_HDR:
		    field = tvb_get_guint8(tvb, offset++);
		    proto_tree_add_uint(mmse_tree, hf_mmse_response_status, tvb,
					offset - 2, 2, field);
		    break;
		case MM_RTEXT_HDR:		/
		    length = get_encoded_strval(tvb, offset, strval);
		    proto_tree_add_string(mmse_tree, hf_mmse_response_text, tvb,
			    		offset - 1, length + 1, strval);
		    offset += length;
		    break;
		case MM_SVISIBILITY_HDR:	/
		    field = tvb_get_guint8(tvb, offset++);
		    proto_tree_add_uint(mmse_tree,hf_mmse_sender_visibility,
			    		tvb, offset - 2, 2, field);
		    break;
		case MM_STATUS_HDR:
		    field = tvb_get_guint8(tvb, offset++);
		    proto_tree_add_uint(mmse_tree, hf_mmse_status, tvb,
					offset - 2, 2, field);
		    break;
		case MM_SUBJECT_HDR:		/
		    length = get_encoded_strval(tvb, offset, strval);
		    proto_tree_add_string(mmse_tree, hf_mmse_subject, tvb,
			    		offset - 1, length + 1, strval);
		    offset += length;
		    break;
		case MM_TO_HDR:			/
		    length = get_encoded_strval(tvb, offset, strval);
		    proto_tree_add_string(mmse_tree, hf_mmse_to, tvb,
			    		offset - 1, length + 1, strval);
		    offset += length;
		    break;
		default:
		    if (field & 0x80) {
			proto_tree_add_text(mmse_tree, tvb, offset - 1, 1,
				"Unknown field (0x%02x)", field);
		    } else {
			guint	 length2;
			char	 strval2[BUFSIZ];

			--offset;
			length = get_text_string(tvb, offset, strval);
			length2= get_text_string(tvb, offset+length, strval2);

			proto_tree_add_string_format(mmse_tree,
						     hf_mmse_ffheader,
						     tvb, offset,
						     length + length2,
						     tvb_get_ptr(tvb,offset,length + length2),
						     "%s: %s",strval,strval2);
			offset += length + length2;
		    }
		    break;
	    }
	}
	if (field == MM_CTYPE_HDR) {
	    /
	    tvbuff_t	*tmp_tvb;
	    guint	 type;
	    const char	*type_str;

	    offset = add_content_type(mmse_tree, tvb, offset, &type, &type_str);
	    tmp_tvb = tvb_new_subset(tvb, offset, -1, -1);
	    add_post_data(mmse_tree, tmp_tvb, type, type_str);
	}
    }

    /
}
