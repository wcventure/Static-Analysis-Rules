int
dissect_igap(tvbuff_t *tvb, packet_info *pinfo, proto_tree *parent_tree, int offset)
{
    proto_tree *tree;
    proto_item *item;
    guint8 type, tsecs, subtype, asize, msize, account[17], message[65];

    if (!proto_is_protocol_enabled(find_protocol_by_id(proto_igap))) {
	/
	return offset + tvb_length_remaining(tvb, offset);
    }

    item = proto_tree_add_item(parent_tree, proto_igap, tvb, offset, -1, FALSE);
    tree = proto_item_add_subtree(item, ett_igap);

    if (check_col(pinfo->cinfo, COL_PROTOCOL)) {
	col_set_str(pinfo->cinfo, COL_PROTOCOL, "IGAP");
    }
    if (check_col(pinfo->cinfo, COL_INFO)) {
	col_clear(pinfo->cinfo, COL_INFO);
    }

    type = tvb_get_guint8(tvb, offset);
    if (check_col(pinfo->cinfo, COL_INFO)) {
	col_add_fstr(pinfo->cinfo, COL_INFO, "%s",
		     val_to_str(type, igap_types, "Unknown Type: 0x%02x"));
    }
    proto_tree_add_uint(tree, hf_type, tvb, offset, 1, type);
    offset += 1;

    tsecs = tvb_get_guint8(tvb, offset);
    proto_tree_add_uint_format(tree, hf_max_resp, tvb, offset, 1, tsecs,
	"Max Response Time: %.1f sec (0x%02x)", tsecs * 0.1, tsecs);
    offset += 1;

    igmp_checksum(tree, tvb, hf_checksum, hf_checksum_bad, pinfo, 0);
    offset += 2;

    proto_tree_add_item(tree, hf_maddr, tvb, offset, 4, FALSE);
    offset += 4;

    proto_tree_add_uint(tree, hf_version, tvb, offset, 1, 
	tvb_get_guint8(tvb, offset));
    offset += 1;

    subtype = tvb_get_guint8(tvb, offset);
    proto_tree_add_uint(tree, hf_subtype, tvb, offset, 1, subtype);
    offset += 2;

    proto_tree_add_uint(tree, hf_challengeid, tvb, offset, 1,
	tvb_get_guint8(tvb, offset));
    offset += 1;

    asize = tvb_get_guint8(tvb, offset);
    proto_tree_add_uint(tree, hf_asize, tvb, offset, 1, asize);
    offset += 1;

    msize = tvb_get_guint8(tvb, offset);
    proto_tree_add_uint(tree, hf_msize, tvb, offset, 1, msize);
    offset += 3;

    if (asize > 0) {
	tvb_memcpy(tvb, account, offset, asize);
	account[asize] = '\0';
	proto_tree_add_string(tree, hf_account, tvb, offset, asize, account);
    }
    offset += 16;

    if (msize > 0) {
	tvb_memcpy(tvb, message, offset, msize);
	switch (subtype) {
	case IGAP_SUBTYPE_PASSWORD_JOIN:
	case IGAP_SUBTYPE_PASSWORD_LEAVE:
	    /
	    message[msize] = '\0';
	    proto_tree_add_text(tree, tvb, offset, msize,
				"User password: %s", message);
	    break;
	case IGAP_SUBTYPE_CHALLENGE_RESPONSE_JOIN:
	case IGAP_SUBTYPE_CHALLENGE_RESPONSE_LEAVE:
	    /
	    proto_tree_add_text(tree, tvb, offset, msize,
				"Result of MD5 calculation: 0x%s",
				bytes_to_str(message, msize));
	    break;
	case IGAP_SUBTYPE_CHALLENGE:
	    /
	    proto_tree_add_text(tree, tvb, offset, msize,
				"Challenge: 0x%s",
				bytes_to_str(message, msize));
	    break;
	case IGAP_SUBTYPE_AUTH_MESSAGE:
	    /
	    proto_tree_add_text(tree, tvb, offset, msize,
				"Authentication result: %s (0x%x)",
				val_to_str(message[0], igap_auth_result,
				"Unknown"), message[0]);
	    break;
	case IGAP_SUBTYPE_ACCOUNTING_MESSAGE:
	    /
	    proto_tree_add_text(tree, tvb, offset, msize,
				"Accounting status: %s (0x%x)",
				val_to_str(message[0], igap_account_status,
				"Unknown"), message[0]);
	    break;
	default:
	    proto_tree_add_text(tree, tvb, offset, msize,
				"Message: (Unknown)");
	}
    }

    offset += 64;
    if (item) proto_item_set_len(item, offset);
    return offset;
}
