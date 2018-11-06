static void
dissect_ipdc_common(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree)
{
	proto_item *ti;
	proto_tree *ipdc_tree;
	proto_item *ipdc_tag;
	proto_tree *tag_tree;
	tvbuff_t *q931_tvb;

	const char *des;
	const char *enum_val = "";
	char *tmp_str;
	char tmp_tag_text[IPDC_STR_LEN + 1];
	const value_string *val_ptr;
	guint32	type;
	guint len;
	guint i;
	guint status;
	gshort tag;
	guint32 tmp_tag;

	gshort nr = tvb_get_guint8(tvb,0);
	gshort ns = tvb_get_guint8(tvb,1);
        guint payload_len = get_ipdc_pdu_len(tvb,0);

        gshort protocol_id;
        gshort trans_id_size;
        guint32 trans_id;
        guint16 message_code;
        guint16 offset;

        /
        if (check_col(pinfo->cinfo, COL_PROTOCOL))
                col_set_str(pinfo->cinfo, COL_PROTOCOL, "IPDC");

	/
	if (payload_len < 4)
		return;

	/
	if (check_col(pinfo->cinfo, COL_INFO)) {
		if (new_packet == TRUE) {
			col_clear(pinfo->cinfo, COL_INFO);
			new_packet = FALSE;
		}
		col_append_fstr(pinfo->cinfo, COL_INFO, "r=%u s=%u ",
		nr, ns);
	}

	if (payload_len == 4) {
		if (!tree)
			return;

	        ti = proto_tree_add_item(tree, proto_ipdc, tvb, 0, -1, FALSE);
       		ipdc_tree = proto_item_add_subtree(ti, ett_ipdc);
		proto_tree_add_item(ipdc_tree, hf_ipdc_nr, tvb, 0, 1, nr);
        	proto_tree_add_item(ipdc_tree, hf_ipdc_ns, tvb, 1, 1, ns);
        	proto_tree_add_uint(ipdc_tree, hf_ipdc_payload_len, tvb, 2, 2,
			payload_len);

		return;
	}

	/
	protocol_id = tvb_get_guint8(tvb,4);
        trans_id_size = TRANS_ID_SIZE_IPDC; /
       	trans_id = tvb_get_ntohl(tvb,6);
       	message_code = tvb_get_ntohs(tvb,6+trans_id_size);
       	offset = 6 + trans_id_size + 2; /

	if (check_col(pinfo->cinfo, COL_INFO))
       		col_append_fstr(pinfo->cinfo, COL_INFO,
			"TID=%x %s ",
                        trans_id,
                        val_to_str(message_code, message_code_vals,
                        TEXT_UNDEFINED));

	if (!tree)
		return;

	ti = proto_tree_add_item(tree, proto_ipdc, tvb, 0, -1, FALSE);
	ipdc_tree = proto_item_add_subtree(ti, ett_ipdc);

	proto_tree_add_item(ipdc_tree, hf_ipdc_nr, tvb, 0, 1, nr);
	proto_tree_add_item(ipdc_tree, hf_ipdc_ns, tvb, 1, 1, ns);
	proto_tree_add_uint(ipdc_tree, hf_ipdc_payload_len, tvb,
		2, 2, payload_len);

	proto_tree_add_item(ipdc_tree, hf_ipdc_protocol_id, tvb,
		4, 1, protocol_id);
	proto_tree_add_item(ipdc_tree, hf_ipdc_trans_id_size, tvb,
		5, 1, trans_id_size);
	proto_tree_add_item(ipdc_tree, hf_ipdc_trans_id, tvb,
		6, trans_id_size, trans_id);
	proto_tree_add_item(ipdc_tree, hf_ipdc_message_code, tvb,
		6 + trans_id_size + 1, 1, message_code);

	ipdc_tag = proto_tree_add_text(ipdc_tree, tvb, offset,
	payload_len - offset, "IPDC tags");
	tag_tree = proto_item_add_subtree(ipdc_tag, ett_ipdc_tag);

	/
	for (;;) {
		tag = tvb_get_guint8(tvb, offset);

		if (tag == 0x0) {
			if (offset == payload_len - 1) {
				proto_tree_add_text(tag_tree, tvb,
				offset, 1, "end of tags");
			} else {
				proto_tree_add_text(tag_tree, tvb,
				offset, 1, "data trailing end of tags");
			}

			break;
		}

		len = tvb_get_guint8(tvb,offset+1);
		des = val_to_str(tag, tag_description, TEXT_UNDEFINED);
		/
		for (i = 0; (ipdc_tag_types[i].tag != tag &&
			ipdc_tag_types[i].type != IPDC_UNKNOWN); i++)
		;
		type = ipdc_tag_types[i].type;

		tmp_tag = 0;

		switch (type) {
			/
			case IPDC_ASCII:
				tmp_str = tvb_memdup(tvb, offset + 2, len);
				strncpy(tmp_tag_text, tmp_str, len);
				tmp_tag_text[len] = 0;
				free(tmp_str);
				proto_tree_add_text(tag_tree, tvb, offset,
				len + 2, "0x%2.2x: %s: %s", tag, des,
				tmp_tag_text);
			break;

			/
			case IPDC_UINT:
			case IPDC_BYTE:
				for (i = 0; i < len; i++) 
					tmp_tag += tvb_get_guint8(tvb,
						offset + 2 + i) * (guint32)
							pow(256, len - (i + 1));

				if (len == 1)
					enum_val =
						val_to_str(IPDC_TAG(tag) +
						tmp_tag,
						tag_enum_type, TEXT_UNDEFINED);

				if (len == 1 &&
					strcmp(enum_val, TEXT_UNDEFINED) != 0) {
					proto_tree_add_text(tag_tree, tvb,
						offset, len + 2,
						"0x%2.2x: %s: %s",
						tag, des, enum_val);
				} else {
					proto_tree_add_text(tag_tree, tvb,
						offset, len + 2,
						"0x%2.2x: %s: %u",
						tag, des, tmp_tag);
				}
			break;

			/
			case IPDC_IPA:
				switch (len) {
					case 4:
						g_snprintf(tmp_tag_text,
						IPDC_STR_LEN,
						"%u.%u.%u.%u",
						tvb_get_guint8(tvb, offset + 2),
						tvb_get_guint8(tvb, offset + 3),
						tvb_get_guint8(tvb, offset + 4),
						tvb_get_guint8(tvb, offset + 5)
						);
					break;

					case 6:
						g_snprintf(tmp_tag_text,
						IPDC_STR_LEN,
						"%u.%u.%u.%u:%u",
						tvb_get_guint8(tvb, offset + 2),
						tvb_get_guint8(tvb, offset + 3),
						tvb_get_guint8(tvb, offset + 4),
						tvb_get_guint8(tvb, offset + 5),
						tvb_get_ntohs(tvb, offset + 6));
					break;

					default:
						g_snprintf(tmp_tag_text,
						IPDC_STR_LEN,
						"Invalid IP address length %u",
                                       		 len);
				}

				proto_tree_add_text(tag_tree, tvb,
					offset, len + 2,
					"0x%2.2x: %s: %s",
					tag, des, tmp_tag_text);
			break;

			/
			case IPDC_LINESTATUS:
			case IPDC_CHANNELSTATUS:
				proto_tree_add_text(tag_tree, tvb, offset,
				len + 2, "0x%2.2x: %s", tag, des);
				val_ptr = (type == IPDC_LINESTATUS) ?
					line_status_vals : channel_status_vals;

				for (i = 0; i < len; i++) {
					status = tvb_get_guint8(tvb,offset+2+i);

					proto_tree_add_text(tag_tree, tvb,
						offset + 2 + i, 1, 
						" %.2u: %.2x (%s)",
						i + 1, status,
						val_to_str(status,
						val_ptr,
						TEXT_UNDEFINED));
				}
			break;

			case IPDC_Q931:
				q931_tvb =
					tvb_new_subset(tvb, offset+2, len, len);
				call_dissector(q931_handle,q931_tvb,pinfo,tree);
			break;

			case IPDC_ENCTYPE:
				proto_tree_add_text(tag_tree, tvb,
					offset, len + 2,
					"0x%2.2x: %s: %s",
					tag, des, val_to_str(
					tvb_get_guint8(tvb,offset+2),
					encoding_type_vals,
					TEXT_UNDEFINED));

				if (len == 2) {
					proto_tree_add_text(tag_tree, tvb,
						offset, len + 2,
						"0x%2.2x: %s: %u",
						tag, des,
						tvb_get_guint8(tvb,offset+3));
				}
			break;
					
			/
			default:
				proto_tree_add_text(tag_tree, tvb, offset,
				len + 2, "0x%2.2x: %s", tag, des);
		} /

		offset += len + 2;
	}
}
