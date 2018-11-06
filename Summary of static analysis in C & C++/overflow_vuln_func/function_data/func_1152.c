static void
dissect_dtp(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree)
{
	proto_item *ti;
	proto_tree *dtp_tree;
	proto_tree *tlv_tree;
	int offset = 0;

	col_set_str(pinfo->cinfo, COL_PROTOCOL, "DTP");
	col_set_str(pinfo->cinfo, COL_INFO, "Dynamic Trunk Protocol");

	ti = proto_tree_add_item(tree, proto_dtp, tvb, offset, -1, ENC_NA);
	dtp_tree = proto_item_add_subtree(ti, ett_dtp);

	/
	proto_tree_add_item(dtp_tree, hf_dtp_version, tvb, offset, 1, ENC_BIG_ENDIAN);
	offset += 1;

	while (tvb_reported_length_remaining(tvb, offset) > 0) {
		int type, length, valuelength;
		proto_item * tlv_length_item;
		
		if (tvb_length_remaining(tvb, offset) < 4) {
			expert_add_info(pinfo, dtp_tree, &ei_dtp_truncated);
			break;
		}

		type = tvb_get_ntohs(tvb, offset);
		length = tvb_get_ntohs(tvb, offset + 2);
		valuelength = (length-4);

		ti = proto_tree_add_text(dtp_tree, tvb, offset, length, "%s",
					 val_to_str(type, dtp_tlv_type_vals, "Unknown TLV type: 0x%02x"));

		tlv_tree = proto_item_add_subtree(ti, ett_dtp_tlv);
		proto_tree_add_uint(tlv_tree, hf_dtp_tlvtype, tvb, offset, 2, type);
		offset+=2;

		tlv_length_item = proto_tree_add_uint(tlv_tree, hf_dtp_tlvlength, tvb, offset, 2, length);
		offset+=2;

		if (valuelength > tvb_length_remaining(tvb, offset)) {
			expert_add_info(pinfo, dtp_tree, &ei_dtp_truncated);
			break;
		}

		if (valuelength > 0) { /
			dissect_dtp_tlv(pinfo, tvb, offset, valuelength, tlv_tree, ti, tlv_length_item, (guint8) type);
		}
		else
			expert_add_info(pinfo, tlv_length_item, &ei_dtp_tlv_length_invalid);

		offset += valuelength;
	}
}
