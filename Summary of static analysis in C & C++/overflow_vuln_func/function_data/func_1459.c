static void
proto_mpeg_descriptor_dissect_logon_initialize(tvbuff_t *tvb, guint offset, guint8 len, proto_tree *tree)
{

	guint   end    = offset + len;
	guint8  flags;
	guint16 flags2;

	proto_tree_add_item(tree, hf_mpeg_descr_logon_initialize_group_id,                        tvb, offset, 1, ENC_BIG_ENDIAN);
	offset++;

	proto_tree_add_item(tree, hf_mpeg_descr_logon_initialize_logon_id,                        tvb, offset, 2, ENC_BIG_ENDIAN);
	offset += 2;

	proto_tree_add_item(tree, hf_mpeg_descr_logon_initialize_continuous_carrier_reserved,     tvb, offset, 1, ENC_BIG_ENDIAN);
	proto_tree_add_item(tree, hf_mpeg_descr_logon_initialize_continuous_carrier,              tvb, offset, 1, ENC_BIG_ENDIAN);
	proto_tree_add_item(tree, hf_mpeg_descr_logon_initialize_security_handshake_required,     tvb, offset, 1, ENC_BIG_ENDIAN);
	proto_tree_add_item(tree, hf_mpeg_descr_logon_initialize_prefix_flag,                     tvb, offset, 1, ENC_BIG_ENDIAN);
	proto_tree_add_item(tree, hf_mpeg_descr_logon_initialize_data_unit_labelling_flag,        tvb, offset, 1, ENC_BIG_ENDIAN);
	proto_tree_add_item(tree, hf_mpeg_descr_logon_initialize_mini_slot_flag,                  tvb, offset, 1, ENC_BIG_ENDIAN);
	proto_tree_add_item(tree, hf_mpeg_descr_logon_initialize_contention_based_mini_slot_flag, tvb, offset, 1, ENC_BIG_ENDIAN);
	offset++;

	flags = tvb_get_guint8(tvb, offset);

	proto_tree_add_item(tree, hf_mpeg_descr_logon_initialize_capacity_type_flag_reserved, tvb, offset, 1, ENC_BIG_ENDIAN);
	proto_tree_add_item(tree, hf_mpeg_descr_logon_initialize_capacity_type_flag,          tvb, offset, 1, ENC_BIG_ENDIAN);
	proto_tree_add_item(tree, hf_mpeg_descr_logon_initialize_traffic_burst_type,          tvb, offset, 1, ENC_BIG_ENDIAN);
	if (flags & MPEG_DESCR_LOGON_INITIALIZE_TRAFFIC_BURST_TYPE_MASK) {
		proto_tree_add_item(tree, hf_mpeg_descr_logon_initialize_connectivity, tvb, offset, 2, ENC_BIG_ENDIAN);
		flags2 = tvb_get_ntohs(tvb, offset);
		if (flags2 & MPEG_DESCR_LOGON_INITIALIZE_CONNECTIVITY_MASK) {
			proto_tree_add_item(tree, hf_mpeg_descr_logon_initialize_return_signalling_vpi_reserved, tvb, offset, 1, ENC_BIG_ENDIAN);
			proto_tree_add_item(tree, hf_mpeg_descr_logon_initialize_return_signalling_vpi, tvb, offset, 1, ENC_BIG_ENDIAN);
			offset++;

			proto_tree_add_item(tree, hf_mpeg_descr_logon_initialize_return_signalling_vci, tvb, offset, 2, ENC_BIG_ENDIAN);
			offset += 2;

			proto_tree_add_item(tree, hf_mpeg_descr_logon_initialize_forward_signalling_vpi_reserved, tvb, offset, 1, ENC_BIG_ENDIAN);
			proto_tree_add_item(tree, hf_mpeg_descr_logon_initialize_forward_signalling_vpi, tvb, offset, 1, ENC_BIG_ENDIAN);
			offset++;

			proto_tree_add_item(tree, hf_mpeg_descr_logon_initialize_forward_signalling_vci, tvb, offset, 2, ENC_BIG_ENDIAN);
			offset += 2;
		} else {
			proto_tree_add_item(tree, hf_mpeg_descr_logon_initialize_return_vpi_reserved, tvb, offset, 1, ENC_BIG_ENDIAN);
			proto_tree_add_item(tree, hf_mpeg_descr_logon_initialize_return_vpi, tvb, offset, 1, ENC_BIG_ENDIAN);
			offset++;

			proto_tree_add_item(tree, hf_mpeg_descr_logon_initialize_return_vci, tvb, offset, 2, ENC_BIG_ENDIAN);
			offset += 2;

		}
	} else {
		proto_tree_add_item(tree, hf_mpeg_descr_logon_initialize_return_trf_pid, tvb, offset, 2, ENC_BIG_ENDIAN);
		offset += 2;

		proto_tree_add_item(tree, hf_mpeg_descr_logon_initialize_return_ctrl_mngm_pid_reserved, tvb, offset, 2, ENC_BIG_ENDIAN);
		proto_tree_add_item(tree, hf_mpeg_descr_logon_initialize_return_ctrl_mngm_pid, tvb, offset, 2, ENC_BIG_ENDIAN);
		offset += 2;
	}

	if (offset < end && (flags & MPEG_DESCR_LOGON_INITIALIZE_CAPACITY_TYPE_FLAG_MASK)) {

		proto_tree_add_item(tree, hf_mpeg_descr_logon_initialize_cra_level,         tvb, offset, 3, ENC_BIG_ENDIAN);
		offset += 3;

		proto_tree_add_item(tree, hf_mpeg_descr_logon_initialize_vbdc_max_reserved, tvb, offset, 2, ENC_BIG_ENDIAN);
		proto_tree_add_item(tree, hf_mpeg_descr_logon_initialize_vbdc_max,          tvb, offset, 2, ENC_BIG_ENDIAN);
		offset += 2;

		proto_tree_add_item(tree, hf_mpeg_descr_logon_initialize_rbdc_max,          tvb, offset, 3, ENC_BIG_ENDIAN);
		offset += 3;

		proto_tree_add_item(tree, hf_mpeg_descr_logon_initialize_rbdc_timeout,      tvb, offset, 2, ENC_BIG_ENDIAN);
		offset += 2;
	}
}
