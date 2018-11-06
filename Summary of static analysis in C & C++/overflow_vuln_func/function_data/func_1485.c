guint
packet_mpeg_sect_crc(tvbuff_t *tvb, packet_info *pinfo,
		     proto_tree *tree, guint start, guint end)
{
	guint32     crc, calculated_crc;
	const char *label;

	crc = tvb_get_ntohl(tvb, end);

	calculated_crc = crc;
	label = "Unverified";
	if (mpeg_sect_check_crc) {
		label = "Verified";
		calculated_crc = crc32_mpeg2_tvb_offset(tvb, start, end);
	}

	if (calculated_crc == crc) {
		proto_tree_add_uint_format( tree, hf_mpeg_sect_crc, tvb,
			end, 4, crc, "CRC: 0x%08x [%s]", crc, label);
	} else {
		proto_item *msg_error = NULL;

		msg_error = proto_tree_add_uint_format( tree, hf_mpeg_sect_crc, tvb,
							end, 4, crc,
							"CRC: 0x%08x [Failed Verification (Calculated: 0x%08x)]",
							crc, calculated_crc );
		PROTO_ITEM_SET_GENERATED(msg_error);
		expert_add_info_format( pinfo, msg_error, PI_MALFORMED,
					PI_ERROR, "Invalid CRC" );
	}

	return 4;
}
