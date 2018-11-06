static guint
dissect_rdm_mdb(tvbuff_t *tvb, guint offset, proto_tree *tree)
{
	guint8	    cc;
	guint16	    param_id;
	guint8	    response_type = 0x00;
	guint8	    parameter_data_length;
	proto_tree *hi,*si, *mdb_tree;

	cc = tvb_get_guint8(tvb, offset + 4);

	switch (cc) {
	case RDM_CC_DISCOVERY_COMMAND:
	case RDM_CC_GET_COMMAND:
	case RDM_CC_SET_COMMAND:
		proto_tree_add_item(tree, hf_rdm_port_id, tvb,
			offset, 1, ENC_BIG_ENDIAN);
		offset += 1;
		break;

	case RDM_CC_DISCOVERY_COMMAND_RESPONSE:
	case RDM_CC_GET_COMMAND_RESPONSE:
	case RDM_CC_SET_COMMAND_RESPONSE:
		response_type = tvb_get_guint8(tvb, offset);
		proto_tree_add_item(tree, hf_rdm_response_type, tvb,
			offset, 1, ENC_BIG_ENDIAN);
		offset += 1;
		break;
	}

	proto_tree_add_item(tree, hf_rdm_message_count, tvb,
			offset, 1, ENC_BIG_ENDIAN);
	offset += 1;

	proto_tree_add_item(tree, hf_rdm_sub_device, tvb,
			offset, 2, ENC_BIG_ENDIAN);
	offset += 2;

	hi = proto_tree_add_item(tree, hf_rdm_mdb, tvb,
			offset, -1, ENC_NA);
	mdb_tree = proto_item_add_subtree(hi,ett_rdm);


	proto_tree_add_item(mdb_tree, hf_rdm_command_class, tvb,
			offset, 1, ENC_BIG_ENDIAN);
	offset += 1;

	param_id = tvb_get_ntohs(tvb, offset);
	proto_tree_add_item(mdb_tree, hf_rdm_parameter_id, tvb,
			offset, 2, ENC_BIG_ENDIAN);
	offset += 2;

	parameter_data_length = tvb_get_guint8(tvb, offset);
	proto_tree_add_item(mdb_tree, hf_rdm_parameter_data_length, tvb,
			offset, 1, ENC_BIG_ENDIAN);
	offset += 1;
	proto_item_set_len( mdb_tree,  parameter_data_length + 4);

	if (parameter_data_length > 0) {
		hi = proto_tree_add_item(mdb_tree, hf_rdm_parameter_data, tvb,
				offset, parameter_data_length, ENC_NA);
		si = proto_item_add_subtree(hi,ett_rdm);

		if (response_type == RDM_RESPONSE_TYPE_NACK_REASON) {
			offset = dissect_rdm_pd_nack_reason(tvb, offset, si, cc, parameter_data_length);

		} else {

			switch(param_id) {
			case RDM_PARAM_ID_SENSOR_VALUE:
				offset = dissect_rdm_pd_sensor_value(tvb, offset, si, cc, parameter_data_length);
				break;

			case RDM_PARAM_ID_QUEUED_MESSAGE:
				offset = dissect_rdm_pd_queued_message(tvb, offset, si, cc, parameter_data_length);
				break;

			case RDM_PARAM_ID_DMX_START_ADDRESS:
				offset = dissect_rdm_pd_dmx_start_address(tvb, offset, si, cc, parameter_data_length);
				break;

			case RDM_PARAM_ID_DEVICE_INFO:
				offset = dissect_rdm_pd_device_info(tvb, offset, si, cc, parameter_data_length);
				break;

			case RDM_PARAM_ID_DEVICE_MODEL_DESCRIPTION:
				offset = dissect_rdm_pd_device_model_description(tvb, offset, si, cc, parameter_data_length);
				break;

			case RDM_PARAM_ID_DEVICE_LABEL:
				offset = dissect_rdm_pd_device_label(tvb, offset, si, cc, parameter_data_length);
				break;

			case RDM_PARAM_ID_DEVICE_HOURS:
				offset = dissect_rdm_pd_device_hours(tvb, offset, si, cc, parameter_data_length);
				break;

			case RDM_PARAM_ID_LAMP_HOURS:
				offset = dissect_rdm_pd_lamp_hours(tvb, offset, si, cc, parameter_data_length);
				break;

			case RDM_PARAM_ID_LAMP_STRIKES:
				offset = dissect_rdm_pd_lamp_strikes(tvb, offset, si, cc, parameter_data_length);
				break;

			case RDM_PARAM_ID_SENSOR_DEFINITION:
				offset = dissect_rdm_pd_sensor_definition(tvb, offset, si, cc, parameter_data_length);
				break;

			case RDM_PARAM_ID_MANUFACTURER_LABEL:
				offset = dissect_rdm_pd_manufacturer_label(tvb, offset, si, cc, parameter_data_length);
				break;

			case RDM_PARAM_ID_DISC_UNIQUE_BRANCH:
				offset = dissect_rdm_pd_disc_unique_branch(tvb, offset, si, cc, parameter_data_length);
				break;

			case RDM_PARAM_ID_DISC_MUTE:
				offset = dissect_rdm_pd_disc_mute(tvb, offset, si, cc, parameter_data_length);
				break;

			case RDM_PARAM_ID_DISC_UN_MUTE:
				offset = dissect_rdm_pd_disc_un_mute(tvb, offset, si, cc, parameter_data_length);
				break;

			case RDM_PARAM_ID_PROXIED_DEVICES:
				offset = dissect_rdm_pd_proxied_devices(tvb, offset, si, cc, parameter_data_length);
				break;

			case RDM_PARAM_ID_PROXIED_DEVICE_COUNT:
				offset = dissect_rdm_pd_proxied_device_count(tvb, offset, si, cc, parameter_data_length);
				break;

			case RDM_PARAM_ID_COMMS_STATUS:
				offset = dissect_rdm_pd_comms_status(tvb, offset, si, cc, parameter_data_length);
				break;

			case RDM_PARAM_ID_STATUS_MESSAGES:
				offset = dissect_rdm_pd_status_messages(tvb, offset, si, cc, parameter_data_length);
				break;

			case RDM_PARAM_ID_STATUS_ID_DESCRIPTION:
				offset = dissect_rdm_pd_status_id_description(tvb, offset, si, cc, parameter_data_length);
				break;

			case RDM_PARAM_ID_CLEAR_STATUS_ID:
				offset = dissect_rdm_pd_clear_status_id(tvb, offset, si, cc, parameter_data_length);
				break;

			case RDM_PARAM_ID_SUB_DEVICE_STATUS_REPORT_THRESHOLD:
				offset = dissect_rdm_pd_sub_device_status_report_threshold(tvb, offset, si, cc, parameter_data_length);
				break;

			case RDM_PARAM_ID_SUPPORTED_PARAMETERS:
				offset = dissect_rdm_pd_supported_parameters(tvb, offset, si, cc, parameter_data_length);
				break;

			case RDM_PARAM_ID_PARAMETER_DESCRIPTION:
				offset = dissect_rdm_pd_parameter_description(tvb, offset, si, cc, parameter_data_length);
				break;

			case RDM_PARAM_ID_PRODUCT_DETAIL_ID_LIST:
				offset = dissect_rdm_pd_product_detail_id_list(tvb, offset, si, cc, parameter_data_length);
				break;

			case RDM_PARAM_ID_FACTORY_DEFAULTS:
				offset = dissect_rdm_pd_factory_defaults(tvb, offset, si, cc, parameter_data_length);
				break;

			case RDM_PARAM_ID_LANGUAGE_CAPABILITIES:
				offset = dissect_rdm_pd_language_capabilities(tvb, offset, si, cc, parameter_data_length);
				break;

			case RDM_PARAM_ID_LANGUAGE:
				offset = dissect_rdm_pd_language(tvb, offset, si, cc, parameter_data_length);
				break;

			case RDM_PARAM_ID_SOFTWARE_VERSION_LABEL:
				offset = dissect_rdm_pd_software_version_label(tvb, offset, si, cc, parameter_data_length);
				break;

			case RDM_PARAM_ID_BOOT_SOFTWARE_VERSION_ID:
				offset = dissect_rdm_pd_boot_software_version_id(tvb, offset, si, cc, parameter_data_length);
				break;

			case RDM_PARAM_ID_BOOT_SOFTWARE_VERSION_LABEL:
				offset = dissect_rdm_pd_boot_software_version_label(tvb, offset, si, cc, parameter_data_length);
				break;

			case RDM_PARAM_ID_DMX_PERSONALITY:
				offset = dissect_rdm_pd_dmx_personality(tvb, offset, si, cc, parameter_data_length);
				break;

			case RDM_PARAM_ID_DMX_PERSONALITY_DESCRIPTION:
				offset = dissect_rdm_pd_dmx_personality_description(tvb, offset, si, cc, parameter_data_length);
				break;

			case RDM_PARAM_ID_SLOT_INFO:
				offset = dissect_rdm_pd_slot_info(tvb, offset, si, cc, parameter_data_length);
				break;

			case RDM_PARAM_ID_SLOT_DESCRIPTION:
				offset = dissect_rdm_pd_slot_description(tvb, offset, si, cc, parameter_data_length);
				break;

			case RDM_PARAM_ID_DEFAULT_SLOT_VALUE:
				offset = dissect_rdm_pd_slot_value(tvb, offset, si, cc, parameter_data_length);
				break;

			case RDM_PARAM_ID_RECORD_SENSORS:
				offset = dissect_rdm_pd_record_sensors(tvb, offset, si, cc, parameter_data_length);
				break;

			case RDM_PARAM_ID_LAMP_STATE:
				offset = dissect_rdm_pd_lamp_state(tvb, offset, si, cc, parameter_data_length);
				break;

			case RDM_PARAM_ID_LAMP_ON_MODE:
				offset = dissect_rdm_pd_lamp_on_mode(tvb, offset, si, cc, parameter_data_length);
				break;

			case RDM_PARAM_ID_DEVICE_POWER_CYCLES:
				offset = dissect_rdm_pd_device_power_cycles(tvb, offset, si, cc, parameter_data_length);
				break;

			case RDM_PARAM_ID_DISPLAY_INVERT:
				offset = dissect_rdm_pd_display_invert(tvb, offset, si, cc, parameter_data_length);
				break;

			case RDM_PARAM_ID_DISPLAY_LEVEL:
				offset = dissect_rdm_pd_display_level(tvb, offset, si, cc, parameter_data_length);
				break;

			case RDM_PARAM_ID_PAN_INVERT:
				offset = dissect_rdm_pd_pan_invert(tvb, offset, si, cc, parameter_data_length);
				break;

			case RDM_PARAM_ID_TILT_INVERT:
				offset = dissect_rdm_pd_tilt_invert(tvb, offset, si, cc, parameter_data_length);
				break;

			case RDM_PARAM_ID_PAN_TILT_SWAP:
				offset = dissect_rdm_pd_pan_tilt_swap(tvb, offset, si, cc, parameter_data_length);
				break;

			case RDM_PARAM_ID_REAL_TIME_CLOCK:
				offset = dissect_rdm_pd_real_time_clock(tvb, offset, si, cc, parameter_data_length);
				break;

			case RDM_PARAM_ID_IDENTIFY_DEVICE:
				offset = dissect_rdm_pd_identify_device(tvb, offset, si, cc, parameter_data_length);
				break;

			case RDM_PARAM_ID_RESET_DEVICE:
				offset = dissect_rdm_pd_reset_device(tvb, offset, si, cc, parameter_data_length);
				break;

			case RDM_PARAM_ID_POWER_STATE:
				offset = dissect_rdm_pd_power_state(tvb, offset, si, cc, parameter_data_length);
				break;

			case RDM_PARAM_ID_PERFORM_SELFTEST:
				offset = dissect_rdm_pd_perform_selftest(tvb, offset, si, cc, parameter_data_length);
				break;

			case RDM_PARAM_ID_SELF_TEST_DESCRIPTION:
				offset = dissect_rdm_pd_self_test_description(tvb, offset, si, cc, parameter_data_length);
				break;

			case RDM_PARAM_ID_CAPTURE_PRESET:
				offset = dissect_rdm_pd_capture_preset(tvb, offset, si, cc, parameter_data_length);
				break;

			case RDM_PARAM_ID_PRESET_PLAYBACK:
				offset = dissect_rdm_pd_preset_playback(tvb, offset, si, cc, parameter_data_length);
				break;

			default:
				proto_tree_add_item(si, hf_rdm_parameter_data_raw, tvb,
					offset, parameter_data_length, ENC_NA);
				offset += parameter_data_length;
				break;
			}
		}
	}

	return offset;
}
