static gboolean
dissect_dsmcc_ts(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree_in, void *data _U_)
{
	proto_item *pi;
	proto_tree *tree;
	guint crc_len;
	guint8 tid;
	guint16 sect_len;
	guint32 crc, calculated_crc;
	const char *label;
	tvbuff_t *sub_tvb;
	guint16 ssi;
	guint offset = 0;

	pi = proto_tree_add_item(tree_in, proto_dsmcc, tvb, 0, -1, ENC_NA);
	tree = proto_item_add_subtree(pi, ett_dsmcc);

	tid = tvb_get_guint8(tvb, offset);
	proto_tree_add_item(tree, hf_dsmcc_table_id, tvb,
		offset, 1, ENC_BIG_ENDIAN);
	offset++;
	ssi = tvb_get_ntohs(tvb, offset);
	ssi &= DSMCC_SSI_MASK;
	proto_tree_add_item(tree, hf_dsmcc_section_syntax_indicator, tvb,
		offset, 2, ENC_BIG_ENDIAN);
	proto_tree_add_item(tree, hf_dsmcc_private_indicator, tvb,
		offset, 2, ENC_BIG_ENDIAN);
	proto_tree_add_item(tree, hf_dsmcc_reserved, tvb,
		offset, 2, ENC_BIG_ENDIAN);
	proto_tree_add_item(tree, hf_dsmcc_section_length, tvb,
		offset, 2, ENC_BIG_ENDIAN);
	sect_len = tvb_get_ntohs(tvb, offset);
	sect_len &= DSMCC_LENGTH_MASK;
	offset += 2;

	proto_tree_add_item(tree, hf_dsmcc_table_id_extension, tvb,
		offset, 2, ENC_BIG_ENDIAN);
	offset += 2;
	proto_tree_add_item(tree, hf_dsmcc_reserved2, tvb,
		offset, 1, ENC_BIG_ENDIAN);
	proto_tree_add_item(tree, hf_dsmcc_version_number, tvb,
		offset, 1, ENC_BIG_ENDIAN);
	proto_tree_add_item(tree, hf_dsmcc_current_next_indicator, tvb,
		offset, 1, ENC_BIG_ENDIAN);
	offset++;
	proto_tree_add_item(tree, hf_dsmcc_section_number, tvb,
		offset, 1, ENC_BIG_ENDIAN);
	offset++;
	proto_tree_add_item(tree, hf_dsmcc_last_section_number, tvb,
		offset, 1, ENC_BIG_ENDIAN);
	offset++;

	sub_tvb = tvb_new_subset(tvb, offset, sect_len-9, sect_len-9);
	switch (tid) {
		case DSMCC_TID_LLCSNAP:
			/
			break;
		case DSMCC_TID_UN_MSG:
		case DSMCC_TID_DD_MSG:
			dissect_dsmcc_un(sub_tvb, pinfo, tree, tree_in);
			break;
		case DSMCC_TID_DESC_LIST:
			/
			break;
		case DSMCC_TID_PRIVATE:
			/
			break;
		default:
			break;
	}

	crc_len = 3 + sect_len - 4;	/
	if (ssi) {
		crc = tvb_get_ntohl(tvb, crc_len);

		calculated_crc = crc;
		label = "Unverified";
		if (dsmcc_sect_check_crc) {
			label = "Verified";
			calculated_crc = crc32_mpeg2_tvb_offset(tvb, 0, crc_len);
		}

		if (calculated_crc == crc) {
			proto_tree_add_uint_format( tree, hf_dsmcc_crc, tvb,
				crc_len, 4, crc, "CRC: 0x%08x [%s]", crc, label);
		} else {
			proto_item *msg_error = NULL;

			msg_error = proto_tree_add_uint_format( tree, hf_dsmcc_crc, tvb,
								crc_len, 4, crc,
								"CRC: 0x%08x [Failed Verification (Calculated: 0x%08x)]",
								crc, calculated_crc );
			PROTO_ITEM_SET_GENERATED(msg_error);
			expert_add_info_format( pinfo, msg_error, PI_MALFORMED,
						PI_ERROR, "Invalid CRC" );
		}
	} else {
		/
		proto_tree_add_item(tree, hf_dsmcc_checksum, tvb,
			crc_len, 4, ENC_BIG_ENDIAN);
	}

	return TRUE;
}
