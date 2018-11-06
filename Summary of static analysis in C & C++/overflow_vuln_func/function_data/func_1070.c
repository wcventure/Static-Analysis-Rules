void
proto_register_rdm(void)
{
	static hf_register_info hf[] = {
		{ &hf_rdm_sub_start_code,
			{ "Sub-start code", "rdm.ssc",
				FT_UINT8, BASE_HEX, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_message_length,
			{ "Message length", "rdm.len",
				FT_UINT8, BASE_DEC, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_dest_uid,
			{ "Destination UID", "rdm.dst",
				FT_BYTES, BASE_NONE, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_src_uid,
			{ "Source UID", "rdm.src",
				FT_BYTES, BASE_NONE, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_transaction_number,
			{ "Transaction number", "rdm.tn",
				FT_UINT8, BASE_DEC, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_port_id,
			{ "Port ID", "rdm.port_id",
				FT_UINT8, BASE_DEC, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_response_type,
			{ "Response type", "rdm.rt",
				FT_UINT8, BASE_HEX, VALS(rdm_rt_vals), 0x0,
				NULL, HFILL }},

		{ &hf_rdm_message_count,
			{ "Message count", "rdm.mc",
				FT_UINT8, BASE_DEC, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_sub_device,
			{ "Sub-device", "rdm.sd",
				FT_UINT16, BASE_DEC, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_mdb,
			{ "Message Data Block", "rdm.mdb",
				FT_NONE, BASE_NONE, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_command_class,
			{ "Command class", "rdm.cc",
				FT_UINT8, BASE_HEX, VALS(rdm_cc_vals), 0x0,
				NULL, HFILL }},

		{ &hf_rdm_parameter_id,
			{ "Parameter ID", "rdm.pid",
				FT_UINT16, BASE_HEX | BASE_EXT_STRING, &rdm_param_id_vals_ext, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_parameter_data_length,
			{ "Parameter data length", "rdm.pdl",
				FT_UINT8, BASE_DEC, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_parameter_data,
			{ "Parameter data", "rdm.pd",
				FT_NONE, BASE_NONE, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_parameter_data_raw,
			{ "Raw Data", "rdm.pd.raw",
				FT_BYTES, BASE_NONE, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_intron,
			{ "Intron", "rdm.intron",
				FT_BYTES, BASE_NONE, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_checksum,
			{ "Checksum", "rdm.checksum",
				FT_UINT16, BASE_HEX, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_checksum_status,
			{ "Checksum Status", "rdm.checksum.status",
				FT_UINT8, BASE_NONE, VALS(proto_checksum_vals), 0x0,
				NULL, HFILL }},

		{ &hf_rdm_trailer,
			{ "Trailer", "rdm.trailer",
				FT_BYTES, BASE_NONE, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_nack_reason,
			{ "NACK Reason", "rdm.pd.nack_reason",
				FT_UINT16, BASE_HEX, VALS(rdm_nr_vals), 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_queued_message_status,
			{ "Status", "rdm.pd.queued_message.status",
				FT_UINT8, BASE_HEX, VALS(rdm_status_vals), 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_sensor_nr,
			{ "Sensor Nr.", "rdm.pd.sensor.nr",
				FT_UINT8, BASE_DEC, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_sensor_value_pres,
			{ "Sensor Present Value", "rdm.pd.sensor.value.present",
				FT_INT16, BASE_DEC, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_sensor_value_low,
			{ "Sensor Lowest Value", "rdm.pd.sensor.value.lowest",
				FT_INT16, BASE_DEC, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_sensor_value_high,
			{ "Sensor Highest Value", "rdm.pd.sensor.value.highest",
				FT_INT16, BASE_DEC, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_sensor_value_rec,
			{ "Sensor Recorded Value", "rdm.pd.sensor.value.recorded",
				FT_INT16, BASE_DEC, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_sensor_range_min_value,
			{ "Sensor Range Min. Value", "rdm.pd.sensor.range.min_value",
				FT_INT16, BASE_DEC, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_sensor_range_max_value,
			{ "Sensor Range Max. Value", "rdm.pd.sensor.range.max_value",
				FT_INT16, BASE_DEC, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_sensor_normal_min_value,
			{ "Sensor Normal Min. Value", "rdm.pd.sensor.normal.min_value",
				FT_INT16, BASE_DEC, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_sensor_normal_max_value,
			{ "Sensor Normal Max. Value", "rdm.pd.sensor.normal.max_value",
				FT_INT16, BASE_DEC, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_sensor_recorded_value_support,
			{ "Sensor Recorded Value Support", "rdm.pd.sensor.recorded_value_support",
				FT_UINT8, BASE_HEX, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_sensor_type,
			{ "Sensor Type", "rdm.pd.sensor_type",
				FT_UINT8, BASE_HEX | BASE_EXT_STRING, &rdm_sensor_type_vals_ext, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_sensor_unit,
			{ "Sensor Unit", "rdm.pd.sensor_unit",
				FT_UINT8, BASE_HEX | BASE_EXT_STRING, &rdm_unit_vals_ext, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_sensor_prefix,
			{ "Sensor Prefix", "rdm.pd.sensor_prefix",
				FT_UINT8, BASE_HEX | BASE_EXT_STRING, &rdm_prefix_vals_ext, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_sensor_description,
			{ "Sensor Description", "rdm.pd.sensor.description",
				FT_STRING, BASE_NONE, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_manu_label,
			{ "Manufacturer Label", "rdm.pd.manu_label",
				FT_STRING, BASE_NONE, NULL, 0x0,
				NULL, HFILL }},


		{ &hf_rdm_pd_device_label,
			{ "Device Label", "rdm.pd.device_label",
				FT_STRING, BASE_NONE, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_dmx_start_address,
			{ "DMX Start Address", "rdm.pd.dmx_start_address",
				FT_UINT16, BASE_DEC, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_device_hours,
			{ "Device Hours", "rdm.pd.device_hours",
				FT_UINT32, BASE_DEC, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_lamp_hours,
			{ "Lamp Hours", "rdm.pd.lamp_hours",
				FT_UINT32, BASE_DEC, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_lamp_strikes,
			{ "Lamp Strikes", "rdm.pd.lamp_strikes",
				FT_UINT32, BASE_DEC, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_proto_vers,
			{ "RDM Protocol Version", "rdm.pd.proto_vers",
				FT_UINT16, BASE_HEX, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_device_model_id,
			{ "Device Model ID", "rdm.pd.device_model_id",
				FT_UINT16, BASE_HEX, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_product_cat,
			{ "Product Category", "rdm.pd.product_cat",
				FT_UINT16, BASE_HEX | BASE_EXT_STRING, &rdm_product_cat_vals_ext, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_software_vers_id,
			{ "Software Version ID", "rdm.pd.software_version_id",
				FT_UINT32, BASE_HEX, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_dmx_footprint,
			{ "DMX Footprint", "rdm.pd.dmx_footprint",
				FT_UINT16, BASE_DEC, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_dmx_pers_current,
			{ "Current DMX Personality", "rdm.pd.dmx_pers_current",
				FT_UINT8, BASE_DEC, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_dmx_pers_total,
			{ "Total nr. DMX Personalities", "rdm.pd.dmx_pers_total",
				FT_UINT8, BASE_DEC, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_sub_device_count,
			{ "Sub-Device Count", "rdm.pd.sub_device_count",
				FT_UINT16, BASE_DEC, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_sensor_count,
			{ "Sensor Count", "rdm.pd.sensor_count",
				FT_UINT8, BASE_DEC, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_device_model_description,
			{ "Device Model Description", "rdm.pd.device_model_description",
				FT_STRING, BASE_NONE, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_disc_unique_branch_lb_uid,
			{ "Lower Bound UID", "rdm.pd.disc_unique_branch.lb_uid",
				FT_BYTES, BASE_NONE, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_disc_unique_branch_ub_uid,
			{ "Upper Bound UID", "rdm.pd.disc_unique_branch.ub_uid",
				FT_BYTES, BASE_NONE, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_disc_mute_control_field,
			{ "Control Field", "rdm.pd.disc_mute.control_field",
				FT_UINT8, BASE_HEX, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_disc_mute_binding_uid,
			{ "Binding UID", "rdm.pd.disc_mute.binding_uid",
				FT_BYTES, BASE_NONE, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_disc_unmute_control_field,
			{ "Control Field", "rdm.pd.disc_unmute.control_field",
				FT_UINT8, BASE_HEX, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_disc_unmute_binding_uid,
			{ "Binding UID", "rdm.pd.disc_unmute.binding_uid",
				FT_BYTES, BASE_NONE, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_proxied_devices_uid,
			{ "UID", "rdm.pd.proxied_devices.uid",
				FT_BYTES, BASE_NONE, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_proxied_device_count,
			{ "Device Count", "rdm.pd.device_count",
				FT_UINT16, BASE_DEC, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_proxied_device_list_change,
			{ "List Change", "rdm.pd.list_change",
				FT_UINT8, BASE_DEC, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_real_time_clock_year,
			{ "Year", "rdm.pd.real_time_clock.year",
				FT_UINT16, BASE_DEC, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_real_time_clock_month,
			{ "Month", "rdm.pd.real_time_clock.month",
				FT_UINT8, BASE_DEC, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_real_time_clock_day,
			{ "Day", "rdm.pd.real_time_clock.day",
				FT_UINT8, BASE_DEC, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_real_time_clock_hour,
			{ "Hour", "rdm.pd.real_time_clock.hour",
				FT_UINT8, BASE_DEC, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_real_time_clock_minute,
			{ "Minute", "rdm.pd.real_time_clock.minute",
				FT_UINT8, BASE_DEC, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_real_time_clock_second,
			{ "Second", "rdm.pd.real_time_clock.second",
				FT_UINT8, BASE_DEC, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_lamp_state,
			{ "Lamp State", "rdm.pd.lamp_state",
				FT_UINT8, BASE_HEX, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_lamp_on_mode,
			{ "Lamp On Mode", "rdm.pd.lamp_on_mode",
				FT_UINT8, BASE_HEX, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_device_power_cycles,
			{ "Device Power Cycles", "rdm.pd.device_power_cycles",
				FT_UINT32, BASE_DEC, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_display_invert,
			{ "Display Invert", "rdm.pd.display_invert",
				FT_UINT8, BASE_HEX, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_display_level,
			{ "Display Level", "rdm.pd.display_level",
				FT_UINT8, BASE_DEC, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_pan_invert,
			{ "Pan Invert", "rdm.pd.pan_invert",
				FT_UINT8, BASE_HEX, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_tilt_invert,
			{ "Tilt Invert", "rdm.pd.tilt_invert",
				FT_UINT8, BASE_HEX, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_tilt_swap,
			{ "Tilt Swap", "rdm.pd.tilt_swap",
				FT_UINT8, BASE_HEX, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_selftest_nr,
			{ "Selftest Nr.", "rdm.pd.selftest.nr",
				FT_UINT8, BASE_DEC, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_selftest_state,
			{ "Selftest State", "rdm.pd.selftest.state",
				FT_UINT8, BASE_HEX, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_selftest_description,
			{ "Selftest Description", "rdm.pd.selftest.description",
				FT_STRING, BASE_NONE, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_language_code,
			{ "Language Code", "rdm.pd.language_code",
				FT_STRING, BASE_NONE, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_identify_device,
			{ "Identify Device", "rdm.pd.identify_device",
				FT_UINT8, BASE_HEX, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_identify_device_state,
			{ "Identify Device State", "rdm.pd.identify_device.state",
				FT_UINT8, BASE_HEX, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_reset_device,
			{ "Reset Device", "rdm.pd.reset_device",
				FT_UINT8, BASE_HEX, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_power_state,
			{ "Power State", "rdm.pd.power_state",
				FT_UINT8, BASE_HEX, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_capture_preset_scene_nr,
			{ "Scene Nr.", "rdm.pd.capture_preset.scene_nr",
				FT_UINT16, BASE_DEC, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_capture_preset_up_fade_time,
			{ "Up Fade Time", "rdm.pd.capture_preset.up_fade_time",
				FT_UINT16, BASE_DEC, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_capture_preset_down_fade_time,
			{ "Down Fade Time", "rdm.pd.capture_preset.down_fade_time",
				FT_UINT16, BASE_DEC, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_capture_preset_wait_time,
			{ "Wait Time", "rdm.pd.capture_preset.wait_time",
				FT_UINT16, BASE_DEC, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_preset_playback_mode,
			{ "Mode", "rdm.pd.preset_playback.mode",
				FT_UINT16, BASE_DEC, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_preset_playback_level,
			{ "Level", "rdm.pd.preset_playback.level",
				FT_UINT8, BASE_DEC, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_parameter_id,
			{ "ID", "rdm.pd.parameter.id",
				FT_UINT16, BASE_HEX, VALS(rdm_param_id_vals), 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_parameter_pdl_size,
			{ "PDL Size", "rdm.pd.parameter.pdl_size",
				FT_UINT8, BASE_DEC, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_parameter_data_type,
			{ "Data Type", "rdm.pd.parameter.data_type",
				FT_UINT8, BASE_DEC, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_parameter_cmd_class,
			{ "Command Class", "rdm.pd.parameter.cmd_class",
				FT_UINT8, BASE_DEC, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_parameter_type,
			{ "Type", "rdm.pd.parameter.type",
				FT_UINT8, BASE_DEC, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_parameter_unit,
			{ "Unit", "rdm.pd.parameter.unit",
				FT_UINT8, BASE_DEC, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_parameter_prefix,
			{ "Prefix", "rdm.pd.parameter.prefix",
				FT_UINT8, BASE_DEC, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_parameter_min_value,
			{ "Min. Value", "rdm.pd.parameter.min_value",
				FT_UINT32, BASE_DEC, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_parameter_max_value,
			{ "Max. Value", "rdm.pd.parameter.max_value",
				FT_UINT32, BASE_DEC, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_parameter_default_value,
			{ "Default Value", "rdm.pd.parameter.default_value",
				FT_UINT32, BASE_DEC, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_parameter_description,
			{ "Description", "rdm.pd.parameter.description",
				FT_STRING, BASE_NONE, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_software_version_label,
			{ "Version Label", "rdm.pd.software_version.label",
				FT_STRING, BASE_NONE, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_boot_software_version_id,
			{ "Version ID", "rdm.pd.boot_software_version.id",
				FT_UINT32, BASE_HEX, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_boot_software_version_label,
			{ "Version Label", "rdm.pd.boot_software_version.label",
				FT_STRING, BASE_NONE, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_comms_status_short_msg,
			{ "Short Msg", "rdm.pd.comms_status.short_msg",
				FT_UINT16, BASE_DEC, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_comms_status_len_mismatch,
			{ "Len Mismatch", "rdm.pd.comms_status.len_mismatch",
				FT_UINT16, BASE_DEC, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_comms_status_csum_fail,
			{ "Checksum Fail", "rdm.pd.comms_status.csum_fail",
				FT_UINT16, BASE_DEC, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_status_messages_type,
			{ "Type", "rdm.pd.status_messages.type",
				FT_UINT8, BASE_HEX, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_status_messages_sub_device_id,
			{ "Sub. Device ID", "rdm.pd.status_messages.sub_devices_id",
				FT_UINT16, BASE_HEX, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_status_messages_id,
			{ "ID", "rdm.pd.status_messages.id",
				FT_UINT16, BASE_HEX, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_status_messages_data_value_1,
			{ "Data Value 1", "rdm.pd.status_messages.data_value_1",
				FT_UINT16, BASE_HEX, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_status_messages_data_value_2,
			{ "Data Value 2", "rdm.pd.status_messages.data_value_2",
				FT_UINT16, BASE_HEX, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_status_id,
			{ "ID", "rdm.pd.status_id",
				FT_UINT16, BASE_DEC, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_status_id_description,
			{ "Description", "rdm.pd.status_id.description",
				FT_STRING, BASE_NONE, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_sub_device_status_report_threshold_status_type,
			{ "Status Type", "rdm.pd.sub_device_status_report_threshold.status_type",
				FT_UINT8, BASE_HEX, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_product_detail_id_list,
			{ "Sensor Count", "rdm.pd.product_detail_id_list",
				FT_UINT16, BASE_HEX, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_factory_defaults,
			{ "Factory Defaults", "rdm.pd.factory_defaults",
				FT_UINT8, BASE_DEC, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_dmx_pers_nr,
			{ "DMX Pers. Nr.", "rdm.pd.dmx_pers.nr",
				FT_UINT8, BASE_DEC, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_dmx_pers_count,
			{ "DMX Pers. Count", "rdm.pd.dmx_pers.count",
				FT_UINT8, BASE_DEC, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_dmx_pers_requested,
			{ "DMX Pers. Requested", "rdm.pd.dmx_pers.requested",
				FT_UINT8, BASE_DEC, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_dmx_pers_slots,
			{ "DMX Pers. Slots", "rdm.pd.dmx_pers.slots",
				FT_UINT16, BASE_DEC, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_dmx_pers_text,
			{ "DMX Pers. Text", "rdm.pd.dmx_pers.text",
				FT_STRING, BASE_NONE, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_slot_offset,
			{ "Slot Offset", "rdm.pd.slot_offset",
				FT_UINT16, BASE_DEC, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_slot_type,
			{ "Slot Type", "rdm.pd.slot_type",
				FT_UINT8, BASE_DEC, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_slot_label_id,
			{ "Slot Label ID", "rdm.pd.slot_label_id",
				FT_UINT16, BASE_HEX, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_slot_nr,
			{ "Slot Nr.", "rdm.pd.slot_nr",
				FT_UINT16, BASE_DEC, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_slot_description,
			{ "Slot Description", "rdm.pd.slot_description",
				FT_STRING, BASE_NONE, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_slot_value,
			{ "Slot Value", "rdm.pd.slot_value",
				FT_UINT8, BASE_DEC, NULL, 0x0,
				NULL, HFILL }},

		{ &hf_rdm_pd_rec_value_support,
			{ "Rec. Value Support", "rdm.pd.rec_value_support",
				FT_UINT8, BASE_HEX, NULL, 0x0,
				NULL, HFILL }}
	};

	static gint *ett[] = {
		&ett_rdm
	};

	static ei_register_info ei[] = {
		{ &ei_rdm_checksum, { "rdm.bad_checksum", PI_CHECKSUM, PI_ERROR, "Bad checksum", EXPFILL }},
	};

	expert_module_t* expert_rdm;

	proto_rdm = proto_register_protocol("Remote Device Management", "RDM", "rdm");
	proto_register_field_array(proto_rdm, hf, array_length(hf));
	proto_register_subtree_array(ett, array_length(ett));
	register_dissector("rdm", dissect_rdm, proto_rdm);
	expert_rdm = expert_register_protocol(proto_rdm);
	expert_register_field_array(expert_rdm, ei, array_length(ei));
}
