void
proto_register_isup(void)
{
/
	static hf_register_info hf[] = {
		{ &hf_isup_cic,
			{ "CIC",           "isup.cic",
			FT_UINT16, BASE_HEX, NULL, 0x0,
			  "", HFILL }},

		{ &hf_isup_message_type,
			{ "Message Type",  "isup.message_type",
			FT_UINT8, BASE_DEC, NULL, 0x0,
			"", HFILL }},

		{ &hf_isup_parameter_type,
			{ "Parameter Type",  "isup.parameter_type",
			FT_UINT8, BASE_DEC, NULL, 0x0,
			"", HFILL }},

		{ &hf_isup_parameter_length,
			{ "Parameter Length",  "isup.parameter_length",
			FT_UINT8, BASE_DEC, NULL, 0x0,
			"", HFILL }},

		{ &hf_isup_mandatory_variable_parameter_pointer,
			{ "Pointer to Parameter",  "isup.mandatory_variable_parameter_pointer",
			FT_UINT8, BASE_DEC, NULL, 0x0,
			"", HFILL }},

		{ &hf_isup_pointer_to_start_of_optional_part,
			{ "Pointer to optional parameter part",  "isup.optional_parameter_part_pointer",
			FT_UINT8, BASE_DEC, NULL, 0x0,
			"", HFILL }},

		{ &hf_isup_satellite_indicator,
			{ "Satellite Indicator",  "isup.satellite_indicator",
			FT_UINT8, BASE_HEX, VALS(isup_satellite_ind_value), BA_8BIT_MASK,
			"", HFILL }},

		{ &hf_isup_continuity_check_indicator,
			{ "Continuity Check Indicator",  "isup.continuity_check_indicator",
			FT_UINT8, BASE_HEX, VALS(isup_continuity_check_ind_value), DC_8BIT_MASK,
			"", HFILL }},

		{ &hf_isup_echo_control_device_indicator,
			{ "Echo Control Device Indicator",  "isup.echo_control_device_indicator",
			FT_BOOLEAN, 8, TFS(&isup_echo_control_device_ind_value),E_8BIT_MASK,
			"", HFILL }},

		{ &hf_isup_forw_call_natnl_inatnl_call_indicator,
			{ "National/international call indicator",  "isup.forw_call_natnl_inatnl_call_indicator",
			FT_BOOLEAN, 16, TFS(&isup_natnl_inatnl_call_ind_value),A_16BIT_MASK,
			"", HFILL }},

		{ &hf_isup_forw_call_end_to_end_method_indicator,
			{ "End-to-end method indicator",  "isup.forw_call_end_to_end_method_indicator",
			FT_UINT16, BASE_HEX, VALS(isup_end_to_end_method_ind_value), CB_16BIT_MASK,
			"", HFILL }},

		{ &hf_isup_forw_call_interworking_indicator,
			{ "Interworking indicator",  "isup.forw_call_interworking_indicator",
			FT_BOOLEAN, 16, TFS(&isup_interworking_ind_value), D_16BIT_MASK,
			"", HFILL }},

		{ &hf_isup_forw_call_end_to_end_info_indicator,
			{ "End-to-end information indicator",  "isup.forw_call_end_to_end_information_indicator",
			FT_BOOLEAN, 16, TFS(&isup_end_to_end_info_ind_value), E_16BIT_MASK,
			"", HFILL }},

		{ &hf_isup_forw_call_isdn_user_part_indicator,
			{ "ISDN user part indicator",  "isup.forw_call_isdn_user_part_indicator",
			FT_BOOLEAN, 16, TFS(&isup_ISDN_user_part_ind_value), F_16BIT_MASK,
			"", HFILL }},

		{ &hf_isup_forw_call_preferences_indicator,
			{ "ISDN user part preference indicator",  "isup.forw_call_preferences_indicator",
			FT_UINT16, BASE_HEX, VALS(isup_preferences_ind_value), HG_16BIT_MASK,
			"", HFILL }},

		{ &hf_isup_forw_call_isdn_access_indicator,
			{ "ISDN access indicator",  "isup.forw_call_isdn_access_indicator",
			FT_BOOLEAN, 16, TFS(&isup_ISDN_originating_access_ind_value), I_16BIT_MASK,
			"", HFILL }},

		{ &hf_isup_forw_call_sccp_method_indicator,
			{ "SCCP method indicator",  "isup.forw_call_sccp_method_indicator",
			FT_UINT16, BASE_HEX, VALS(isup_SCCP_method_ind_value), KJ_16BIT_MASK,
			"", HFILL }},

		{ &hf_isup_calling_partys_category,
			{ "Calling Party's category",  "isup.calling_partys_category",
			FT_UINT8, BASE_HEX, VALS(isup_calling_partys_category_value), 0x0,
			"", HFILL }},

		{ &hf_isup_transmission_medium_requirement,
			{ "Transmission medium requirement",  "isup.transmission_medium_requirement",
			FT_UINT8, BASE_DEC, VALS(isup_transmission_medium_requirement_value), 0x0,
			"", HFILL }},

		{ &hf_isup_odd_even_indicator,
			{ "Odd/even indicator",  "isup.isdn_odd_even_indicator",
			FT_BOOLEAN, 8, TFS(&isup_odd_even_ind_value), ISUP_ODD_EVEN_MASK,
			"", HFILL }},

		{ &hf_isup_called_party_nature_of_address_indicator,
			{ "Nature of address indicator",  "isup.called_party_nature_of_address_indicator",
			FT_UINT8, BASE_DEC, VALS(isup_called_party_nature_of_address_ind_value), ISUP_NATURE_OF_ADDRESS_IND_MASK,
			"", HFILL }},

		{ &hf_isup_calling_party_nature_of_address_indicator,
			{ "Nature of address indicator",  "isup.calling_party_nature_of_address_indicator",
			FT_UINT8, BASE_DEC, VALS(isup_calling_party_nature_of_address_ind_value), ISUP_NATURE_OF_ADDRESS_IND_MASK,
			"", HFILL }},

		{ &hf_isup_inn_indicator,
			{ "INN indicator",  "isup.inn_indicator",
			FT_BOOLEAN, 8, TFS(&isup_INN_ind_value), ISUP_INN_MASK,
			"", HFILL }},

		{ &hf_isup_ni_indicator,
			{ "NI indicator",  "isup.ni_indicator",
			FT_BOOLEAN, 8, TFS(&isup_NI_ind_value), ISUP_NI_MASK,
			"", HFILL }},

		{ &hf_isup_numbering_plan_indicator,
			{ "Numbering plan indicator",  "isup.numbering_plan_indicator",
			FT_UINT8, BASE_DEC, VALS(isup_numbering_plan_ind_value), ISUP_NUMBERING_PLAN_IND_MASK,
			"", HFILL }},

		{ &hf_isup_address_presentation_restricted_indicator,
			{ "Address presentation restricted indicator",  "isup.address_presentation_restricted_indicator",
			FT_UINT8, BASE_DEC, VALS(isup_address_presentation_restricted_ind_value), ISUP_ADDRESS_PRESENTATION_RESTR_IND_MASK,
			"", HFILL }},

		{ &hf_isup_screening_indicator,
			{ "Screening indicator",  "isup.screening_indicator",
			FT_UINT8, BASE_DEC, VALS(isup_screening_ind_value), ISUP_SCREENING_IND_MASK,
			"", HFILL }},

		{ &hf_isup_screening_indicator_enhanced,
			{ "Screening indicator",  "isup.screening_indicator_enhanced",
			FT_UINT8, BASE_DEC, VALS(isup_screening_ind_enhanced_value), ISUP_SCREENING_IND_MASK,
			"", HFILL }},

		{ &hf_isup_called_party_odd_address_signal_digit,
			{ "Address signal digit",  "isup.called_party_odd_address_signal_digit",
			FT_UINT8, BASE_DEC, VALS(isup_called_party_address_digit_value), ISUP_ODD_ADDRESS_SIGNAL_DIGIT_MASK,
			"", HFILL }},

		{ &hf_isup_calling_party_odd_address_signal_digit,
			{ "Address signal digit",  "isup.calling_party_odd_address_signal_digit",
			FT_UINT8, BASE_DEC, VALS(isup_calling_party_address_digit_value), ISUP_ODD_ADDRESS_SIGNAL_DIGIT_MASK,
			"", HFILL }},

		{ &hf_isup_called_party_even_address_signal_digit,
			{ "Address signal digit",  "isup.called_party_even_address_signal_digit",
			FT_UINT8, BASE_DEC, VALS(isup_called_party_address_digit_value), ISUP_EVEN_ADDRESS_SIGNAL_DIGIT_MASK,
			"", HFILL }},

		{ &hf_isup_calling_party_even_address_signal_digit,
			{ "Address signal digit",  "isup.calling_party_even_address_signal_digit",
			FT_UINT8, BASE_DEC, VALS(isup_calling_party_address_digit_value), ISUP_EVEN_ADDRESS_SIGNAL_DIGIT_MASK,
			"", HFILL }},

		{ &hf_isup_calling_party_address_request_indicator,
			{ "Calling party address request indicator",  "isup.calling_party_address_request_indicator",
			FT_BOOLEAN, 16, TFS(&isup_calling_party_address_request_ind_value), A_16BIT_MASK,
			"", HFILL }},

		{ &hf_isup_info_req_holding_indicator,
			{ "Holding indicator",  "isup.info_req_holding_indicator",
			FT_BOOLEAN, 16, TFS(&isup_holding_ind_value), B_16BIT_MASK,
			"", HFILL }},

		{ &hf_isup_calling_partys_category_request_indicator,
			{ "Calling party's category request indicator",  "isup.calling_partys_category_request_indicator",
			FT_BOOLEAN, 16, TFS(&isup_calling_partys_category_request_ind_value), D_16BIT_MASK,
			"", HFILL }},

		{ &hf_isup_charge_information_request_indicator,
			{ "Charge information request indicator",  "isup.charge_information_request_indicator",
			FT_BOOLEAN, 16, TFS(&isup_charge_information_request_ind_value), E_16BIT_MASK,
			"", HFILL }},

		{ &hf_isup_malicious_call_identification_request_indicator,
			{ "Malicious call identification request indicator (ISUP'88)",  "isup.malicious_call_ident_request_indicator",
			FT_BOOLEAN, 16, TFS(&isup_malicious_call_identification_request_ind_value), H_16BIT_MASK,
			"", HFILL }},

		{ &hf_isup_calling_party_address_response_indicator,
			{ "Calling party address response indicator",  "isup.calling_party_address_response_indicator",
			FT_UINT16, BASE_HEX, VALS(isup_calling_party_address_response_ind_value), BA_16BIT_MASK,
			"", HFILL }},

		{ &hf_isup_OECD_inf_ind,
			{ "OECD information indicator",  "isup.OECD_inf_ind_vals",
			FT_UINT8, BASE_HEX, VALS(OECD_inf_ind_vals), BA_8BIT_MASK,
			"", HFILL }},

		{ &hf_isup_IECD_inf_ind,
			{ "IECD information indicator",  "isup.IECD_inf_ind_vals",
			FT_UINT8, BASE_HEX, VALS(IECD_inf_ind_vals), DC_8BIT_MASK,
			"", HFILL }},

		{ &hf_isup_OECD_req_ind,
			{ "OECD request indicator",  "isup.OECD_req_ind_vals",
			FT_UINT8, BASE_HEX, VALS(OECD_req_ind_vals), FE_8BIT_MASK,
			"", HFILL }},

		{ &hf_isup_IECD_req_ind,
			{ "IECD request indicator",  "isup.IECD_req_ind_vals",
			FT_UINT8, BASE_HEX, VALS(IECD_req_ind_vals), HG_8BIT_MASK,
			"", HFILL }},

		{ &hf_isup_hold_provided_indicator,
			{ "Hold provided indicator",  "isup.hold_provided_indicator",
			FT_BOOLEAN, 16, TFS(&isup_hold_provided_ind_value), C_16BIT_MASK,
			"", HFILL }},

		{ &hf_isup_calling_partys_category_response_indicator,
			{ "Calling party's category response indicator",  "isup.calling_partys_category_response_indicator",
			FT_BOOLEAN, 16, TFS(&isup_calling_partys_category_response_ind_value), F_16BIT_MASK,
			"", HFILL }},

		{ &hf_isup_charge_information_response_indicator,
			{ "Charge information response indicator",  "isup.charge_information_response_indicator",
			FT_BOOLEAN, 16, TFS(&isup_charge_information_response_ind_value), G_16BIT_MASK,
			"", HFILL }},

		{ &hf_isup_solicited_indicator,
			{ "Solicited indicator",  "isup.solicided_indicator",
			FT_BOOLEAN, 16, TFS(&isup_solicited_information_ind_value), H_16BIT_MASK,
			"", HFILL }},

		{ &hf_isup_continuity_indicator,
			{ "Continuity indicator",  "isup.continuity_indicator",
			FT_BOOLEAN, 8, TFS(&isup_continuity_ind_value), A_8BIT_MASK,
			"", HFILL }},

		{ &hf_isup_backw_call_charge_ind,
			{ "Charge indicator",  "isup.charge_indicator",
			FT_UINT16, BASE_HEX, VALS(isup_charge_ind_value), BA_16BIT_MASK,
			"", HFILL }},

		{ &hf_isup_backw_call_called_partys_status_ind,
			{ "Called party's status indicator",  "isup.called_partys_status_indicator",
			FT_UINT16, BASE_HEX, VALS(isup_called_partys_status_ind_value), DC_16BIT_MASK,
			"", HFILL }},

		{ &hf_isup_backw_call_called_partys_category_ind,
			{ "Called party's category indicator",  "isup.called_partys_category_indicator",
			FT_UINT16, BASE_HEX, VALS(isup_called_partys_category_ind_value), FE_16BIT_MASK,
			"", HFILL }},

		{ &hf_isup_backw_call_end_to_end_method_ind,
			{ "End-to-end method indicator",  "isup.backw_call_end_to_end_method_indicator",
			FT_UINT16, BASE_HEX, VALS(isup_end_to_end_method_ind_value), HG_16BIT_MASK,
			"", HFILL }},

		{ &hf_isup_backw_call_interworking_ind,
			{ "Interworking indicator",  "isup.backw_call_interworking_indicator",
			FT_BOOLEAN, 16, TFS(&isup_interworking_ind_value), I_16BIT_MASK,
			"", HFILL }},

		{ &hf_isup_backw_call_end_to_end_info_ind,
			{ "End-to-end information indicator",  "isup.backw_call_end_to_end_information_indicator",
			FT_BOOLEAN, 16, TFS(&isup_end_to_end_info_ind_value), J_16BIT_MASK,
			"", HFILL }},

		{ &hf_isup_backw_call_isdn_user_part_ind,
			{ "ISDN user part indicator",  "isup.backw_call_isdn_user_part_indicator",
			FT_BOOLEAN, 16, TFS(&isup_ISDN_user_part_ind_value), K_16BIT_MASK,
			"", HFILL }},

		{ &hf_isup_backw_call_holding_ind,
			{ "Holding indicator",  "isup.backw_call_holding_indicator",
			FT_BOOLEAN, 16, TFS(&isup_holding_ind_value), L_16BIT_MASK,
			"", HFILL }},

		{ &hf_isup_backw_call_isdn_access_ind,
			{ "ISDN access indicator",  "isup.backw_call_isdn_access_indicator",
			FT_BOOLEAN, 16, TFS(&isup_ISDN_terminating_access_ind_value), M_16BIT_MASK,
			"", HFILL }},

		{ &hf_isup_backw_call_echo_control_device_ind,
			{ "Echo Control Device Indicator",  "isup.backw_call_echo_control_device_indicator",
			FT_BOOLEAN, 16, TFS(&isup_echo_control_device_ind_value), N_16BIT_MASK,
			"", HFILL }},

		{ &hf_isup_backw_call_sccp_method_ind,
			{ "SCCP method indicator",  "isup.backw_call_sccp_method_indicator",
			FT_UINT16, BASE_HEX, VALS(isup_SCCP_method_ind_value), PO_16BIT_MASK,
			"", HFILL }},

		{ &hf_isup_cause_indicator,
			{ "Cause indicator",  "isup.cause_indicator",
			FT_UINT8, BASE_DEC, VALS(q850_cause_code_vals), 0x7f,
			"", HFILL }},

		{ &hf_ansi_isup_cause_indicator,
			{ "Cause indicator",  "ansi_isup.cause_indicator",
			FT_UINT8, BASE_DEC, VALS(ansi_isup_cause_code_vals), 0x7f,
			"", HFILL }},

		{ &hf_isup_suspend_resume_indicator,
			{ "Suspend/Resume indicator",  "isup.suspend_resume_indicator",
			FT_BOOLEAN, 8, TFS(&isup_suspend_resume_ind_value), A_8BIT_MASK,
			"", HFILL }},

		{ &hf_isup_range_indicator,
		        { "Range indicator",  "isup.range_indicator",
			  FT_UINT8, BASE_DEC, NULL , 0x0,
			  "", HFILL }},

		{ &hf_isup_cgs_message_type,
			{ "Circuit group supervision message type",  "isup.cgs_message_type",
			FT_UINT8, BASE_DEC, VALS(isup_cgs_message_type_value), BA_8BIT_MASK,
			"", HFILL }},

		{ &hf_isup_mtc_blocking_state1,
			{ "Maintenance blocking state",  "isup.mtc_blocking_state",
			FT_UINT8, BASE_DEC, VALS(isup_mtc_blocking_state_DC00_value), BA_8BIT_MASK,
			"", HFILL }},

		{ &hf_isup_mtc_blocking_state2,
			{ "Maintenance blocking state",  "isup.mtc_blocking_state",
			FT_UINT8, BASE_DEC, VALS(isup_mtc_blocking_state_DCnot00_value), BA_8BIT_MASK,
			"", HFILL }},

		{ &hf_isup_call_proc_state,
			{ "Call processing state",  "isup.call_processing_state",
			FT_UINT8, BASE_DEC, VALS(isup_call_processing_state_value), DC_8BIT_MASK,
			"", HFILL }},

		{ &hf_isup_hw_blocking_state,
			{ "HW blocking state",  "isup.hw_blocking_state",
			FT_UINT8, BASE_DEC, VALS(isup_HW_blocking_state_value), FE_8BIT_MASK,
			"", HFILL }},

		{ &hf_isup_event_ind,
			{ "Event indicator",  "isup.event_ind",
			  FT_UINT8, 8, VALS(isup_event_ind_value), GFEDCBA_8BIT_MASK,
			"", HFILL }},

		{ &hf_isup_event_presentation_restricted_ind,
			{ "Event presentation restricted indicator",  "isup.event_presentatiation_restr_ind",
			FT_BOOLEAN, 8, TFS(&isup_event_presentation_restricted_ind_value), H_8BIT_MASK,
			"", HFILL }},

		{ &hf_isup_cug_call_ind,
		        { "Closed user group call indicator",  "isup.clg_call_ind",
			FT_UINT8, BASE_DEC, VALS(isup_CUG_call_ind_value), BA_8BIT_MASK,
			"", HFILL }},

		{ &hf_isup_simple_segmentation_ind,
			{ "Simple segmentation indicator",  "isup.simple_segmentation_ind",
			FT_BOOLEAN, 8, TFS(&isup_simple_segmentation_ind_value), C_8BIT_MASK,
			"", HFILL }},

		{ &hf_isup_connected_line_identity_request_ind,
			{ "Connected line identity request indicator",  "isup.connected_line_identity_request_ind",
			FT_BOOLEAN, 8, TFS(&isup_connected_line_identity_request_ind_value), H_8BIT_MASK,
			"", HFILL }},

		{ &hf_isup_redirecting_ind,
		        { "Redirection indicator",  "isup.redirecting_ind",
			FT_UINT16, BASE_DEC, VALS(isup_redirecting_ind_value), CBA_16BIT_MASK,
			"", HFILL }},

		{ &hf_isup_original_redirection_reason,
		        { "Original redirection reason",  "isup.original_redirection_reason",
			FT_UINT16, BASE_DEC, VALS(isup_original_redirection_reason_value), HGFE_16BIT_MASK,
			"", HFILL }},

		{ &hf_isup_redirection_counter,
		        { "Redirection counter",  "isup.redirection_counter",
			FT_UINT16, BASE_DEC, NULL, KJI_16BIT_MASK,
			"", HFILL }},

		{ &hf_isup_redirection_reason,
		        { "Redirection reason",  "isup.redirection_reason",
			FT_UINT16, BASE_DEC, VALS(isup_redirection_reason_value), PONM_16BIT_MASK,
			"", HFILL }},

		{ &hf_isup_type_of_network_identification,
		        { "Type of network identification",  "isup.type_of_network_identification",
			FT_UINT8, BASE_DEC, VALS(isup_type_of_network_identification_value), GFE_8BIT_MASK,
			"", HFILL }},

		{ &hf_isup_network_identification_plan,
		        { "Network identification plan",  "isup.network_identification_plan",
			FT_UINT8, BASE_DEC, VALS(isup_network_identification_plan_value), DCBA_8BIT_MASK,
			"", HFILL }},

		{ &hf_isup_map_type,
		        { "Map Type",  "isup.map_type",
			FT_UINT8, BASE_DEC, VALS(isup_map_type_value), FEDCBA_8BIT_MASK,
			"", HFILL }},

		{ &hf_isup_automatic_congestion_level,
		        { "Automatic congestion level",  "isup.automatic_congestion_level",
			FT_UINT8, BASE_DEC, VALS(isup_auto_congestion_level_value), 0x0,
			"", HFILL }},

		{ &hf_isup_inband_information_ind,
			{ "In-band information indicator",  "isup.inband_information_ind",
			FT_BOOLEAN, 8, TFS(&isup_inband_information_ind_value), A_8BIT_MASK,
			"", HFILL }},

		{ &hf_isup_call_diversion_may_occur_ind,
			{ "Call diversion may occur indicator",  "isup.call_diversion_may_occur_ind",
			FT_BOOLEAN, 8, TFS(&isup_call_diversion_may_occur_ind_value), B_8BIT_MASK,
			"", HFILL }},

		{ &hf_isup_mlpp_user_ind,
			{ "MLPP user indicator",  "isup.mlpp_user",
			FT_BOOLEAN, 8, TFS(&isup_MLPP_user_ind_value), D_8BIT_MASK,
			"", HFILL }},

		{ &hf_isup_UUI_type,
			{ "User-to-User indicator type",  "isup.UUI_type",
			FT_BOOLEAN, 8, TFS(&isup_UUI_type_value), A_8BIT_MASK,
			"", HFILL }},

		{ &hf_isup_UUI_req_service1,
			{ "User-to-User indicator request service 1",  "isup.UUI_req_service1",
			FT_UINT8, BASE_DEC, VALS(isup_UUI_request_service_values), CB_8BIT_MASK,
			"", HFILL }},

		{ &hf_isup_UUI_req_service2,
			{ "User-to-User indicator request service 2",  "isup.UUI_req_service2",
			FT_UINT8, BASE_DEC, VALS(isup_UUI_request_service_values), ED_8BIT_MASK,
			"", HFILL }},

		{ &hf_isup_UUI_req_service3,
			{ "User-to-User indicator request service 3",  "isup.UUI_req_service3",
			FT_UINT8, BASE_DEC, VALS(isup_UUI_request_service_values), GF_8BIT_MASK,
			"", HFILL }},

		{ &hf_isup_UUI_res_service1,
			{ "User-to-User indicator response service 1",  "isup.UUI_res_service1",
			FT_UINT8, BASE_DEC, VALS(isup_UUI_response_service_values), CB_8BIT_MASK,
			"", HFILL }},

		{ &hf_isup_UUI_res_service2,
			{ "User-to-User indicator response service 2",  "isup.UUI_res_service2",
			FT_UINT8, BASE_DEC, VALS(isup_UUI_response_service_values), ED_8BIT_MASK,
			"", HFILL }},

		{ &hf_isup_UUI_res_service3,
			{ "User-to-User response service 3",  "isup.UUI_res_service3",
			FT_UINT8, BASE_DEC, VALS(isup_UUI_response_service_values), GF_8BIT_MASK,
			"", HFILL }},

		{ &hf_isup_UUI_network_discard_ind,
			{ "User-to-User indicator network discard indicator",  "isup.UUI_network_discard_ind",
			FT_BOOLEAN, 8, TFS(&isup_UUI_network_discard_ind_value), H_8BIT_MASK,
			"", HFILL }},

		{ &hf_isup_access_delivery_ind,
			{ "Access delivery indicator",  "isup.access_delivery_ind",
			FT_BOOLEAN, 8, TFS(&isup_access_delivery_ind_value), A_8BIT_MASK,
			"", HFILL }},

		{ &hf_isup_transmission_medium_requirement_prime,
			{ "Transmission medium requirement prime",  "isup.transmission_medium_requirement_prime",
			FT_UINT8, BASE_DEC, VALS(isup_transmission_medium_requirement_prime_value), 0x0,
			"", HFILL }},

		{ &hf_isup_loop_prevention_response_ind,
			{ "Response indicator",  "isup.loop_prevention_response_ind",
			FT_UINT8, BASE_DEC, VALS(isup_loop_prevention_response_ind_value), CB_8BIT_MASK,
			"", HFILL }},

		{ &hf_isup_temporary_alternative_routing_ind,
			{ "Temporary alternative routing indicator",  "isup.temporary_alternative_routing_ind",
			FT_BOOLEAN, 8, TFS(&isup_temporary_alternative_routing_ind_value), A_8BIT_MASK,
			"", HFILL }},

		{ &hf_isup_extension_ind,
			{ "Extension indicator",  "isup.extension_ind",
			FT_BOOLEAN, 8, TFS(&isup_extension_ind_value), H_8BIT_MASK,
			"", HFILL }},

		{ &hf_isup_call_to_be_diverted_ind,
			{ "Call to be diverted indicator",  "isup.call_to_be_diverted_ind",
			FT_UINT8, BASE_DEC, VALS(isup_call_to_be_diverted_ind_value), BA_8BIT_MASK,
			"", HFILL }},

		{ &hf_isup_call_to_be_offered_ind,
			{ "Call to be offered indicator",  "isup.call_to_be_offered_ind",
			FT_UINT8, BASE_DEC, VALS(isup_call_to_be_offered_ind_value), BA_8BIT_MASK,
			"", HFILL }},

		{ &hf_isup_conference_acceptance_ind,
			{ "Conference acceptance indicator",  "isup.conference_acceptance_ind",
			FT_UINT8, BASE_DEC, VALS(isup_conference_acceptance_ind_value), BA_8BIT_MASK,
			"", HFILL }},

		{ &hf_isup_transit_at_intermediate_exchange_ind,
			{ "Transit at intermediate exchange indicator", "isup.transit_at_intermediate_exchange_ind",
			FT_BOOLEAN, 8, TFS(&isup_transit_at_intermediate_exchange_ind_value), A_8BIT_MASK,
			"", HFILL }},

		{ &hf_isup_Release_call_ind,
			{ "Release call indicator", "isup.Release_call_ind",
			FT_BOOLEAN, 8, TFS(&isup_Release_call_indicator_value), B_8BIT_MASK,
			"", HFILL }},

		{ &hf_isup_Send_notification_ind,
			{ "Send notification indicator", "isup.Send_notification_ind",
			FT_BOOLEAN, 8, TFS(&isup_Send_notification_ind_value),C_8BIT_MASK,
			"", HFILL }},

		{ &hf_isup_Discard_message_ind_value,
			{ "Discard message indicator","isup.Discard_message_ind_value",
			FT_BOOLEAN, 8, TFS(&isup_Discard_message_ind_value), D_8BIT_MASK,
			"", HFILL }},

		{ &hf_isup_Discard_parameter_ind,		
			{ "Discard parameter indicator","isup.Discard_parameter_ind",
			FT_BOOLEAN, 8, TFS(&isup_Discard_parameter_ind_value), E_8BIT_MASK,
			"", HFILL }},

		{ &hf_isup_Pass_on_not_possible_indicator,
			{ "Pass on not possible indicator",  "isup_Pass_on_not_possible_ind",
			FT_UINT8, BASE_HEX, VALS(isup_Pass_on_not_possible_indicator_vals),GF_8BIT_MASK,
			"", HFILL }},

		{ &hf_isup_pass_on_not_possible_indicator2,
			{ "Pass on not possible indicator",  "isup_Pass_on_not_possible_val",
			FT_BOOLEAN, 8, TFS(&isup_pass_on_not_possible_indicator_value),E_8BIT_MASK,
			"", HFILL }},

		{ &hf_isup_Broadband_narrowband_interworking_ind,
			{ "Broadband narrowband interworking indicator Bits JF",  "isup_broadband-narrowband_interworking_ind",
			FT_UINT8, BASE_HEX, VALS(ISUP_Broadband_narrowband_interworking_indicator_vals),BA_8BIT_MASK,
			"", HFILL }},

		{ &hf_isup_Broadband_narrowband_interworking_ind2,
			{ "Broadband narrowband interworking indicator Bits GF",  "isup_broadband-narrowband_interworking_ind2",
			FT_UINT8, BASE_HEX, VALS(ISUP_Broadband_narrowband_interworking_indicator_vals),GF_8BIT_MASK,
			"", HFILL }},

		{ &hf_isup_app_cont_ident,
			{ "Application context identifier",  "isup.app_context_identifier",
			FT_UINT16, BASE_DEC, VALS(isup_application_transport_parameter_value),GFEDCBA_8BIT_MASK,
			"", HFILL }},

		{ &hf_isup_app_Release_call_ind, 
			{ "Release call indicator (RCI)",  "isup.app_Release_call_indicator",
			FT_BOOLEAN, 8, TFS(&isup_Release_call_indicator_value), A_8BIT_MASK,
			"", HFILL }},

		{ &hf_isup_app_Send_notification_ind, 
			{ "Send notification indicator (SNI)",  "isup.app_Send_notification_ind",
			FT_BOOLEAN, 8, TFS(&isup_Send_notification_ind_value), B_8BIT_MASK,
			"", HFILL }},

		{ &hf_isup_apm_segmentation_ind,
		        { "APM segmentation indicator",  "isup.apm_segmentation_ind",
			FT_UINT8, BASE_DEC, VALS(isup_APM_segmentation_ind_value), FEDCBA_8BIT_MASK,
			"", HFILL }},

		{ &hf_isup_apm_si_ind, 
			{ "Sequence indicator (SI)",  "isup.APM_Sequence_ind",
			FT_BOOLEAN, 8, TFS(&isup_Sequence_ind_value), G_8BIT_MASK,
			"", HFILL }},
		
		{ &hf_isup_orig_addr_len, 
			{ "Originating Address length",  "isup.orig_addr_len",
			FT_UINT8, BASE_DEC, NULL, 0x0,
			"Originating Address length", HFILL }},

		{ &hf_isup_dest_addr_len, 
			{ "Destination Address length",  "isup.orig_addr_len",
			FT_UINT8, BASE_DEC, NULL, 0x0,
			"Destination Address length", HFILL }},

		{ &hf_isup_apm_slr, 
			{ "Segmentation local reference (SLR)",  "isup.APM_slr",
			FT_UINT8, BASE_DEC, NULL,GFEDCBA_8BIT_MASK,
			"", HFILL }},
		{ &hf_isup_cause_location,
			{ "Cause location", "isup.cause_location",
			FT_UINT8, BASE_DEC, VALS(q931_cause_location_vals), 0x0f,
			"", HFILL }},

		{ &hf_ansi_isup_coding_standard,
		  { "Coding standard", "ansi_isup.coding_standard", FT_UINT8, BASE_HEX,
			 VALS(ansi_isup_coding_standard_vals), 0x60,"", HFILL }},

		{ &hf_bat_ase_identifier,
			{ "BAT ASE Identifiers",  "bicc.bat_ase_identifier",
			FT_UINT8, BASE_HEX, VALS(bat_ase_list_of_Identifiers_vals),0x0,	
			"", HFILL }},
  
		{ &hf_length_indicator,
			{ "BAT ASE Element length indicator",  "bicc.bat_ase_length_indicator",
			FT_UINT16, BASE_DEC, NULL,0x0,
			"", HFILL }},

		{ &hf_Action_Indicator,
			{ "BAT ASE action indicator field",  "bicc.bat_ase_bat_ase_action_indicator_field",
			FT_UINT8, BASE_HEX, VALS(bat_ase_action_indicator_field_vals),0x00,	
			"", HFILL }},

		{ &hf_Instruction_ind_for_general_action,
			{ "BAT ASE Instruction indicator for general action",  "bicc.bat_ase_Instruction_ind_for_general_action",
			FT_UINT8, BASE_HEX, VALS(Instruction_indicator_for_general_action_vals),0x03,	
			"", HFILL }},

		{ &hf_Send_notification_ind_for_general_action, 
			{ "Send notification indicator for general action",  "bicc.bat_ase_Send_notification_ind_for_general_action",
			FT_BOOLEAN, 8, TFS(&isup_Send_notification_ind_value), 0x04,
			"", HFILL }},

		{ &hf_Instruction_ind_for_pass_on_not_possible, 
			{ "Instruction ind for pass-on not possible",  "bicc.bat_ase_Instruction_ind_for_pass_on_not_possible",
			FT_UINT8, BASE_HEX, VALS(Instruction_indicator_for_pass_on_not_possible_vals),0x30,	
			"", HFILL }},

		{ &hf_Send_notification_ind_for_pass_on_not_possible, 
			{ "Send notification indication for pass-on not possible",  "bicc.bat_ase_Send_notification_ind_for_pass_on_not_possible",
			FT_BOOLEAN, 8, TFS(&isup_Send_notification_ind_value), 0x40,
			"", HFILL }},

		{ &hf_BCTP_Version_Indicator,
			{ "BCTP Version Indicator",  "bicc.bat_ase_BCTP_Version_Indicator",
			FT_UINT8, BASE_DEC, NULL,0x1f,
			"", HFILL }},
	
		{ &hf_BVEI,
			{ "BVEI",  "bicc.bat_ase_BCTP_BVEI",
			FT_BOOLEAN, 8, TFS(&BCTP_BVEI_value), 0x40,
			"", HFILL }},

		{ &hf_Tunnelled_Protocol_Indicator,
			{ "Tunnelled Protocol Indicator",  "bicc.bat_ase_BCTP_Tunnelled_Protocol_Indicator",
			FT_UINT8, BASE_DEC, VALS(BCTP_Tunnelled_Protocol_Indicator_vals),0x3f,	
			"", HFILL }},

		{ &hf_TPEI,
			{ "TPEI",  "bicc.bat_ase_BCTP_tpei",
			FT_BOOLEAN, 8, TFS(&BCTP_TPEI_value), 0x40,
			"", HFILL }},

		{ &hf_bncid,
			{ "Backbone Network Connection Identifier (BNCId)", "bat_ase.bncid",
			FT_UINT32, BASE_HEX, NULL, 0x0,
			  "", HFILL }},

		{ &hf_bat_ase_biwfa,
			{ "Interworking Function Address( X.213 NSAP encoded)", "bat_ase_biwfa",
			FT_BYTES, BASE_HEX, NULL, 0x0,
			  "", HFILL }},

		{ &hf_afi,
			{ "X.213 Address Format Information ( AFI )",  "x213.afi",
			FT_UINT8, BASE_HEX, VALS(x213_afi_value),0x0,	
			"", HFILL }},
		
		{ &hf_bicc_nsap_dsp,
			{ "X.213 Address Format Information ( DSP )",  "x213.dsp",
			FT_BYTES, BASE_HEX, NULL,0x0,	
			"", HFILL }},
		{ &hf_characteristics,
			{ "Backbone network connection characteristics", "bat_ase.char",
			FT_UINT8, BASE_HEX, VALS(bearer_network_connection_characteristics_vals),0x0,	
			  "", HFILL }},

		{ &hf_Organization_Identifier,
			{ "Organization identifier subfield ",  "bat_ase.organization_identifier_subfield",
			FT_UINT8, BASE_DEC, VALS(bat_ase_organization_identifier_subfield_vals),0x0,
			"", HFILL }},

		{ &hf_codec_type,
			{ "ITU-T codec type subfield",  "bat_ase.ITU_T_codec_type_subfield",
			FT_UINT8, BASE_HEX, VALS(ITU_T_codec_type_subfield_vals),0x0,
			"", HFILL }},

		{ &hf_etsi_codec_type,
			{ "ETSI codec type subfield",  "bat_ase.ETSI_codec_type_subfield",
			FT_UINT8, BASE_HEX, VALS(ETSI_codec_type_subfield_vals),0x0,
			"", HFILL }},

		{ &hf_active_code_set,
			{ "Active Code Set",  "bat_ase.acs",
			FT_UINT8, BASE_HEX, NULL,0x0,
			"", HFILL }},

		{ &hf_active_code_set_12_2,
			{ "12.2 kbps rate",  "bat_ase.acs.12_2",
			FT_UINT8, BASE_HEX, NULL,0x80,
			"", HFILL }},

		{ &hf_active_code_set_10_2,
			{ "10.2 kbps rate",  "bat_ase.acs.10_2",
			FT_UINT8, BASE_HEX, NULL,0x40,
			"", HFILL }},
		
		{ &hf_active_code_set_7_95,
			{ "7.95 kbps rate",  "bat_ase.acs.7_95",
			FT_UINT8, BASE_HEX, NULL,0x20,
			"", HFILL }},
		
		{ &hf_active_code_set_7_40,
			{ "7.40 kbps rate",  "bat_ase.acs.7_40",
			FT_UINT8, BASE_HEX, NULL,0x10,
			"", HFILL }},
	
		{ &hf_active_code_set_6_70,
			{ "6.70 kbps rate",  "bat_ase.acs.6_70",
			FT_UINT8, BASE_HEX, NULL,0x08,
			"", HFILL }},
		
		{ &hf_active_code_set_5_90,
			{ "5.90 kbps rate",  "bat_ase.acs.5_90",
			FT_UINT8, BASE_HEX, NULL,0x04,
			"", HFILL }},

		{ &hf_active_code_set_5_15,
			{ "5.15 kbps rate",  "bat_ase.acs.5_15",
			FT_UINT8, BASE_HEX, NULL,0x02,
			"", HFILL }},
		
		{ &hf_active_code_set_4_75,
			{ "4.75 kbps rate",  "bat_ase.acs.4_75",
			FT_UINT8, BASE_HEX, NULL,0x01,
			"", HFILL }},

		{ &hf_supported_code_set,
			{ "Supported Code Set",  "bat_ase.scs",
			FT_UINT8, BASE_HEX, NULL,0x0,
			"", HFILL }},
		
		{ &hf_supported_code_set_12_2,
			{ "12.2 kbps rate",  "bat_ase.scs.12_2",
			FT_UINT8, BASE_HEX, NULL,0x80,
			"", HFILL }},
	
		{ &hf_supported_code_set_10_2,
			{ "10.2 kbps rate",  "bat_ase.scs.10_2",
			FT_UINT8, BASE_HEX, NULL,0x40,
			"", HFILL }},
		
		{ &hf_supported_code_set_7_95,
			{ "7.95 kbps rate",  "bat_ase.scs.7_95",
			FT_UINT8, BASE_HEX, NULL,0x20,
			"", HFILL }},
		
		{ &hf_supported_code_set_7_40,
			{ "7.40 kbps rate",  "bat_ase.scs.7_40",
			FT_UINT8, BASE_HEX, NULL,0x10,
			"", HFILL }},
		
		{ &hf_supported_code_set_6_70,
			{ "6.70 kbps rate",  "bat_ase.scs.6_70",
			FT_UINT8, BASE_HEX, NULL,0x08,
			"", HFILL }},
		
		{ &hf_supported_code_set_5_90,
			{ "5.90 kbps rate",  "bat_ase.scs.5_90",
			FT_UINT8, BASE_HEX, NULL,0x04,
			"", HFILL }},
		
		{ &hf_supported_code_set_5_15,
			{ "5.15 kbps rate",  "bat_ase.scs.5_15",
			FT_UINT8, BASE_HEX, NULL,0x02,
			"", HFILL }},
		
		{ &hf_supported_code_set_4_75,
			{ "4.75 kbps rate",  "bat_ase.scs.4_75",
			FT_UINT8, BASE_HEX, NULL,0x01,
			"", HFILL }},
		
		{ &hf_optimisation_mode,
			{ "Optimisation Mode for ACS , OM",  "bat_ase.optimisation_mode",
			FT_UINT8, BASE_HEX, VALS(optimisation_mode_vals),0x8,
			"Optimisation Mode for ACS , OM", HFILL }},
		
		{ &hf_max_codec_modes,
			{ "Maximal number of Codec Modes, MACS",  "bat_ase.macs",
			FT_UINT8, BASE_DEC, NULL,0x07,
			"Maximal number of Codec Modes, MACS", HFILL }},
		
		
		{ &hf_bearer_control_tunneling,
			{ "Bearer control tunneling",  "bat_ase.bearer_control_tunneling",
			FT_BOOLEAN, 8, TFS(&Bearer_Control_Tunnelling_ind_value),0x01,
			"", HFILL }},

		{ &hf_BAT_ASE_Comp_Report_Reason,
			{ "Compabillity report reason",  "bat_ase.Comp_Report_Reason",
			FT_UINT8, BASE_HEX, VALS(BAT_ASE_Report_Reason_vals),0x0,
			"", HFILL }},


		{ &hf_BAT_ASE_Comp_Report_ident,
			{ "Bearer control tunneling",  "bat_ase.bearer_control_tunneling",
			FT_UINT8, BASE_HEX, VALS(bat_ase_list_of_Identifiers_vals),0x0,
			"", HFILL }},

		{ &hf_BAT_ASE_Comp_Report_diagnostic,
			{ "Diagnostics",  "bat_ase.Comp_Report_diagnostic",
			FT_UINT16, BASE_HEX, NULL,0x0,
			"", HFILL }},

		{ &hf_Local_BCU_ID,
			{ "Local BCU ID",  "bat_ase.Local_BCU_ID",
			FT_UINT32, BASE_HEX, NULL, 0x0,
			  "", HFILL }},

		{ &hf_late_cut_trough_cap_ind,
			{ "Late Cut-through capability indicator",  "bat_ase.late_cut_trough_cap_ind",
			FT_BOOLEAN, 8, TFS(&late_cut_trough_cap_ind_value),0x01,
			"", HFILL }},

		{ &hf_bat_ase_signal,
			{ "Q.765.5 - Signal Type",  "bat_ase.signal_type",
			FT_UINT8, BASE_HEX, VALS(BAt_ASE_Signal_Type_vals),0x0,
			"", HFILL }},

		{ &hf_bat_ase_duration,
			{ "Duration in ms",  "bat_ase.signal_type",
			FT_UINT16, BASE_DEC, NULL,0x0,
			"", HFILL }},

		{ &hf_bat_ase_bearer_redir_ind,
			{ "Redirection Indicator",  "bat_ase.bearer_redir_ind",
			FT_UINT8, BASE_HEX, VALS(Bearer_Redirection_Indicator_vals),0x0,
			"", HFILL }},

		{ &hf_nsap_ipv4_addr,
			{ "IWFA IPv4 Address", "nsap.ipv4_addr", 
			FT_IPv4, BASE_NONE, NULL, 0x0,
			"IPv4 address", HFILL }},

		{ &hf_nsap_ipv6_addr,
			{ "IWFA IPv6 Address", "nsap.ipv6_addr",
			FT_IPv6, BASE_NONE, NULL, 0x0,
			"IPv6 address", HFILL}},

		{ &hf_iana_icp,
			{ "IANA ICP",  "nsap.iana_icp",
			FT_UINT16, BASE_HEX, VALS(iana_icp_values),0x0,
                      "", HFILL }},

              { &hf_isup_called,
                      { "ISUP Called Number",  "isup.called",
                      FT_STRING, BASE_NONE, NULL,0x0,
                      "", HFILL }},

              { &hf_isup_calling,
                      { "ISUP Calling Number",  "isup.calling",
                      FT_STRING, BASE_NONE, NULL,0x0,
                      "", HFILL }},

              { &hf_isup_redirecting,
                      { "ISUP Redirecting Number",  "isup.redirecting",
                      FT_STRING, BASE_NONE, NULL,0x0,
			"", HFILL }},
		{&hf_isup_apm_msg_fragments,
			{"Message fragments", "isup_apm.msg.fragments",
			FT_NONE, BASE_NONE, NULL, 0x00,	NULL, HFILL } 
		},
		{&hf_isup_apm_msg_fragment,
			{"Message fragment", "isup_apm.msg.fragment",
			FT_FRAMENUM, BASE_NONE, NULL, 0x00, NULL, HFILL } 
		},
		{&hf_isup_apm_msg_fragment_overlap,
			{"Message fragment overlap", "isup_apm.msg.fragment.overlap",
			FT_BOOLEAN, BASE_NONE, NULL, 0x00, NULL, HFILL } 
		},
		{&hf_isup_apm_msg_fragment_overlap_conflicts,
			{"Message fragment overlapping with conflicting data","isup_apm.msg.fragment.overlap.conflicts",
			FT_BOOLEAN, BASE_NONE, NULL, 0x00, NULL, HFILL } 
		},
		{&hf_isup_apm_msg_fragment_multiple_tails,
			{"Message has multiple tail fragments", "isup_apm.msg.fragment.multiple_tails", 
			FT_BOOLEAN, BASE_NONE, NULL, 0x00, NULL, HFILL } 
		},
		{&hf_isup_apm_msg_fragment_too_long_fragment,
			{"Message fragment too long", "isup_apm.msg.fragment.too_long_fragment",
			FT_BOOLEAN, BASE_NONE, NULL, 0x00, NULL, HFILL } 
		},
		{&hf_isup_apm_msg_fragment_error,
			{"Message defragmentation error", "isup_apm.msg.fragment.error",
			FT_FRAMENUM, BASE_NONE, NULL, 0x00, NULL, HFILL } 
		},
		{&hf_isup_apm_msg_reassembled_in,
			{"Reassembled in", "isup_apm.msg.reassembled.in",
			FT_FRAMENUM, BASE_NONE, NULL, 0x00, NULL, HFILL } 
		},
		{&hf_isup_cvr_rsp_ind,
		 {"CVR Response Ind", "conn_rsp_ind",
		  FT_UINT8, BASE_DEC, VALS(isup_cvr_rsp_ind_value), BA_8BIT_MASK,
		  "", HFILL }},
		{&hf_isup_cvr_cg_car_ind,
		 {"CVR Circuit Group Carrier","cg_carrier_ind",
		 FT_UINT8, BASE_HEX, VALS(isup_cvr_cg_car_ind_value), BA_8BIT_MASK,
		  "", HFILL }},
		{&hf_isup_cvr_cg_double_seize,
		 {"Doube Seize Control", "cg_char_ind.doubleSeize",
		  FT_UINT8, BASE_HEX, VALS(isup_cvr_cg_double_seize_value), DC_8BIT_MASK,
		  "", HFILL }},
		{&hf_isup_cvr_cg_alarm_car_ind,
		 {"Alarm Carrier Indicator", "cg_alarm_car_ind",
		  FT_UINT8, BASE_HEX, VALS(isup_cvr_alarm_car_ind_value), FE_8BIT_MASK,
		  "", HFILL }},
		{&hf_isup_cvr_cont_chk_ind,
		 {"Continuity Check Indicator","cg_alarm_cnt_chk",
		  FT_UINT8, BASE_HEX, VALS(isup_cvr_cont_chk_ind_value), HG_8BIT_MASK,
		  "",HFILL }}
	};

/
	static gint *ett[] = {
		&ett_isup,
		&ett_isup_parameter,
		&ett_isup_address_digits,
		&ett_isup_pass_along_message,
		&ett_isup_circuit_state_ind,
		&ett_bat_ase,
		&ett_bat_ase_element,
		&ett_bat_ase_iwfa,
		&ett_scs,
		&ett_acs,
		&ett_isup_apm_msg_fragment,
		&ett_isup_apm_msg_fragments,
	};

/
	proto_isup = proto_register_protocol("ISDN User Part",
	    "ISUP", "isup");

	register_dissector("isup", dissect_isup, proto_isup);

/
	proto_register_field_array(proto_isup, hf, array_length(hf));
	proto_register_subtree_array(ett, array_length(ett));

	isup_tap = register_tap("isup");

	isup_module = prefs_register_protocol(proto_isup, NULL);


	prefs_register_bool_preference(isup_module, "show_cic_in_info", "Show CIC in Info column",
				 "Show the CIC value (in addition to the message type) in the Info column",
				 (gint *)&isup_show_cic_in_info);

	prefs_register_bool_preference(isup_module, "defragment_apm",
		"Reassemble APM messages",
		"Whether APM messages datagrams should be reassembled",
		&isup_apm_desegment);	

	/
	stats_tree_register("isup","isup_msg","ISUP Messages", msg_stats_tree_packet, msg_stats_tree_init, NULL );
}
