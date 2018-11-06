static void
dissect_eiss(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree)
{
	guint offset = 0, packet_length, sect_len;
	proto_item *ti = NULL;
	proto_item *pi = NULL;
	proto_tree *eiss_tree = NULL;
	proto_item *items[PACKET_MPEG_SECT_PI__SIZE];
	gboolean ssi;
	guint reserved;
	guint8 reserved2;
	guint8 sect_num, last_sect_num;

	guint16 eiss_application_type;
	guint8 platform_id_length;

	col_clear(pinfo->cinfo, COL_PROTOCOL);
	col_set_str(pinfo->cinfo, COL_PROTOCOL, "EISS");

	ti = proto_tree_add_item(tree, proto_eiss, tvb, offset, -1, ENC_NA);
	eiss_tree = proto_item_add_subtree(ti, ett_eiss);

	offset += packet_mpeg_sect_header_extra(tvb, offset, eiss_tree, &sect_len,
						&reserved, &ssi, items);

	packet_length = sect_len + 3 - 4; /

	if (FALSE != ssi) {
		proto_item *msg_error;
		msg_error = items[PACKET_MPEG_SECT_PI__SSI];

		PROTO_ITEM_SET_GENERATED(msg_error);
		expert_add_info_format(pinfo, msg_error, PI_MALFORMED, PI_ERROR,
					"Invalid section_syntax_indicator (should be 0)");
	}

	if (0 != reserved) {
		proto_item *msg_error;
		msg_error = items[PACKET_MPEG_SECT_PI__RESERVED];

		PROTO_ITEM_SET_GENERATED(msg_error);
		expert_add_info_format(pinfo, msg_error, PI_MALFORMED, PI_ERROR,
					"Invalid reserved1 bits (should all be 0)");
	}

	if (1021 < sect_len) {
		proto_item *msg_error;
		msg_error = items[PACKET_MPEG_SECT_PI__LENGTH];

		PROTO_ITEM_SET_GENERATED(msg_error);
		expert_add_info_format(pinfo, msg_error, PI_MALFORMED, PI_ERROR,
					"Invalid section_length (must not exceed 1021)");
	}

	reserved2 = tvb_get_guint8(tvb, offset);
	pi = proto_tree_add_item(eiss_tree, hf_eiss_reserved2, tvb, offset, 1, ENC_BIG_ENDIAN);
	if (0 != reserved2) {
		PROTO_ITEM_SET_GENERATED(pi);
		expert_add_info_format(pinfo, pi, PI_MALFORMED, PI_ERROR,
					"Invalid reserved2 bits (should all be 0)");
	}
	offset++;

	sect_num = tvb_get_guint8(tvb, offset);
	last_sect_num = tvb_get_guint8(tvb, offset + 1);
	pi = proto_tree_add_item(eiss_tree, hf_eiss_section_number, tvb, offset, 1, ENC_BIG_ENDIAN);
	if (last_sect_num < sect_num) {
		PROTO_ITEM_SET_GENERATED(pi);
		expert_add_info_format(pinfo, pi, PI_MALFORMED, PI_ERROR,
					"Invalid section_number (must be <= last_section_number)");
	}
	offset++;
	proto_tree_add_item(eiss_tree, hf_eiss_last_section_number, tvb, offset, 1, ENC_BIG_ENDIAN);
	offset++;
	proto_tree_add_item(eiss_tree, hf_eiss_protocol_version_major, tvb, offset, 1, ENC_BIG_ENDIAN);
	offset++;
	proto_tree_add_item(eiss_tree, hf_eiss_protocol_version_minor, tvb, offset, 1, ENC_BIG_ENDIAN);
	offset++;

	eiss_application_type = tvb_get_ntohs(tvb, offset);
	pi = proto_tree_add_item(eiss_tree, hf_eiss_application_type, tvb, offset, 2, ENC_BIG_ENDIAN);
	if (8 != eiss_application_type) {
		PROTO_ITEM_SET_GENERATED(pi);
		expert_add_info_format(pinfo, pi, PI_MALFORMED, PI_ERROR,
					"Invalid application_type (must be 0x0008)");
	}
	offset += 2;
	proto_tree_add_item(eiss_tree, hf_eiss_organisation_id, tvb, offset, 4, ENC_BIG_ENDIAN);
	offset += 4;
	proto_tree_add_item(eiss_tree, hf_eiss_application_id, tvb, offset, 2, ENC_BIG_ENDIAN);
	offset += 2;

	platform_id_length = tvb_get_guint8(tvb, offset);
	pi = proto_tree_add_item(eiss_tree, hf_eiss_platform_id_length, tvb, offset, 1, ENC_BIG_ENDIAN);
	if (0 != platform_id_length % 15) {
		PROTO_ITEM_SET_GENERATED(pi);
		expert_add_info_format(pinfo, pi, PI_MALFORMED, PI_ERROR,
		"Invalid platform_id_length (must be a multiple of sizeof(etv_bif_platform_ids) == 15)");
	}
	offset++;

	while (0 < platform_id_length) {
		guint tmp;

		tmp = dissect_etv_bif_platform_ids(tvb, eiss_tree, offset);
		offset += tmp;
		if (platform_id_length < tmp) {
			platform_id_length = 0;
			/
		} else {
			platform_id_length -= tmp;
		}
	}

	if (0 < packet_length) {
		proto_tree *eiss_desc_tree = NULL;
		pi = proto_tree_add_text(eiss_tree, tvb, offset,
					packet_length-offset,
					"%s", "EISS Descriptor(s)");
		eiss_desc_tree = proto_item_add_subtree(pi, ett_eiss_desc);
		while (offset < packet_length) {
			offset += dissect_eiss_descriptors(tvb, pinfo,
							eiss_desc_tree, offset);
		}
	}

	packet_mpeg_sect_crc(tvb, pinfo, eiss_tree, 0, sect_len - 1);
}
