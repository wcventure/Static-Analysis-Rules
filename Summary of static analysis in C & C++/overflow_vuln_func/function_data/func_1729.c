static void
dissect_ansi_isup_optional_parameter(tvbuff_t *optional_parameters_tvb,packet_info *pinfo, proto_tree *isup_tree)
{ proto_item* parameter_item;
  proto_tree* parameter_tree;
  gint offset = 0;
  guint parameter_type, parameter_length, actual_length;
  tvbuff_t *parameter_tvb;
  guint8 octet;

  /
  parameter_type = 0xFF; /

  while ((tvb_length_remaining(optional_parameters_tvb, offset)  >= 1) && (parameter_type != PARAM_TYPE_END_OF_OPT_PARAMS)){
    parameter_type = tvb_get_guint8(optional_parameters_tvb, offset);

    if (parameter_type != PARAM_TYPE_END_OF_OPT_PARAMS){
      parameter_length = tvb_get_guint8(optional_parameters_tvb, offset + PARAMETER_TYPE_LENGTH);

      parameter_item = proto_tree_add_text(isup_tree, optional_parameters_tvb,
					   offset,
					   parameter_length  + PARAMETER_TYPE_LENGTH + PARAMETER_LENGTH_IND_LENGTH,
					   "Parameter: type %u",
					   parameter_type);
      parameter_tree = proto_item_add_subtree(parameter_item, ett_isup_parameter);
      proto_tree_add_uint_format(parameter_tree, hf_isup_parameter_type, optional_parameters_tvb, offset, PARAMETER_TYPE_LENGTH, parameter_type, "Optional Parameter: %u (%s)", parameter_type, val_to_str(parameter_type, ansi_isup_parameter_type_value,"unknown"));
      offset += PARAMETER_TYPE_LENGTH;

	  octet = tvb_get_guint8(optional_parameters_tvb,offset);
		
      proto_tree_add_uint_format(parameter_tree, hf_isup_parameter_length, optional_parameters_tvb, offset, PARAMETER_LENGTH_IND_LENGTH, parameter_length, "Parameter length: %u", parameter_length);
      offset += PARAMETER_LENGTH_IND_LENGTH;
	  if ( octet == 0 )
		  continue;
		
      actual_length = tvb_length_remaining(optional_parameters_tvb, offset);
      if (actual_length > 0){
		  parameter_tvb = tvb_new_subset(optional_parameters_tvb, offset, MIN(parameter_length, actual_length), parameter_length);
		  switch (parameter_type) {
			case PARAM_TYPE_CALL_REF:
				dissect_isup_call_reference_parameter(parameter_tvb, parameter_tree, parameter_item);
				break;
			case PARAM_TYPE_TRANSM_MEDIUM_REQU:
				dissect_isup_transmission_medium_requirement_parameter(parameter_tvb, parameter_tree, parameter_item);
				break;
			case PARAM_TYPE_ACC_TRANSP:
				dissect_isup_access_transport_parameter(parameter_tvb, parameter_tree, parameter_item, pinfo);
				break;
			case PARAM_TYPE_CALLED_PARTY_NR:
				dissect_isup_called_party_number_parameter(parameter_tvb, parameter_tree, parameter_item);
				break;
			case PARAM_TYPE_SUBSQT_NR:
				dissect_isup_subsequent_number_parameter(parameter_tvb, parameter_tree, parameter_item);
				break;
			case PARAM_TYPE_NATURE_OF_CONN_IND:
				dissect_isup_nature_of_connection_indicators_parameter(parameter_tvb, parameter_tree, parameter_item);
				break;
			case PARAM_TYPE_FORW_CALL_IND:
				dissect_isup_forward_call_indicators_parameter(parameter_tvb, parameter_tree, parameter_item);
				break;
			case PARAM_TYPE_OPT_FORW_CALL_IND:
				dissect_isup_optional_forward_call_indicators_parameter(parameter_tvb, parameter_tree, parameter_item);
				break;
			case PARAM_TYPE_CALLING_PRTY_CATEG:
				dissect_isup_calling_partys_category_parameter(parameter_tvb, parameter_tree, parameter_item);
				break;
			case PARAM_TYPE_CALLING_PARTY_NR:
				dissect_isup_calling_party_number_parameter(parameter_tvb, parameter_tree, parameter_item);
				break;
			case PARAM_TYPE_REDIRECTING_NR:
				dissect_isup_redirecting_number_parameter(parameter_tvb, parameter_tree, parameter_item);
				break;
			case PARAM_TYPE_REDIRECTION_NR:
				dissect_isup_redirection_number_parameter(parameter_tvb, parameter_tree, parameter_item);
				break;
			case PARAM_TYPE_CONNECTION_REQ:
				dissect_isup_connection_request_parameter(parameter_tvb, parameter_tree, parameter_item);
				break;
			case PARAM_TYPE_INFO_REQ_IND:
				dissect_isup_information_request_indicators_parameter(parameter_tvb, parameter_tree, parameter_item);
				break;
			case PARAM_TYPE_INFO_IND:
				dissect_isup_information_indicators_parameter(parameter_tvb, parameter_tree, parameter_item);
				break;
			case PARAM_TYPE_CONTINUITY_IND:
				dissect_isup_continuity_indicators_parameter(parameter_tvb, parameter_tree, parameter_item);
				break;
			case PARAM_TYPE_BACKW_CALL_IND:
				dissect_isup_backward_call_indicators_parameter(parameter_tvb, parameter_tree, parameter_item);
				break;
			case PARAM_TYPE_CAUSE_INDICATORS:
				dissect_ansi_isup_cause_indicators_parameter(parameter_tvb, parameter_tree, parameter_item);
				break;
			case PARAM_TYPE_REDIRECTION_INFO:
				dissect_isup_redirection_information_parameter(parameter_tvb, parameter_tree, parameter_item);
				break;
			case PARAM_TYPE_CIRC_GRP_SV_MSG_TYPE:
				dissect_isup_circuit_group_supervision_message_type_parameter(parameter_tvb, parameter_tree, parameter_item);
				break;
			case PARAM_TYPE_RANGE_AND_STATUS:
				dissect_isup_range_and_status_parameter(parameter_tvb, parameter_tree, parameter_item);
				break;
			case PARAM_TYPE_FACILITY_IND:
				dissect_isup_facility_ind_parameter(parameter_tvb, parameter_item);
				break;
			case PARAM_TYPE_CLSD_USR_GRP_ILOCK_CD:
				dissect_isup_closed_user_group_interlock_code_parameter(parameter_tvb, parameter_tree, parameter_item);
				break;
			case PARAM_TYPE_USER_SERVICE_INFO:
				dissect_isup_user_service_information_parameter(parameter_tvb, parameter_tree, parameter_item);
				break;
			case PARAM_TYPE_SIGNALLING_POINT_CODE:
				dissect_isup_signalling_point_code_parameter(parameter_tvb, parameter_tree, parameter_item);
				break;
			case PARAM_TYPE_USER_TO_USER_INFO:
				dissect_isup_user_to_user_information_parameter(parameter_tvb, parameter_tree, parameter_item);
				break;
			case PARAM_TYPE_CONNECTED_NR:
				dissect_isup_connected_number_parameter(parameter_tvb, parameter_tree, parameter_item);
				break;
			case PARAM_TYPE_SUSP_RESUME_IND:
				dissect_isup_suspend_resume_indicators_parameter(parameter_tvb, parameter_tree, parameter_item);
				break;
			case PARAM_TYPE_TRANSIT_NETW_SELECT:
				dissect_isup_transit_network_selection_parameter(parameter_tvb, parameter_tree, parameter_item);
				break;
			case PARAM_TYPE_EVENT_INFO:
				dissect_isup_event_information_parameter(parameter_tvb, parameter_tree, parameter_item);
				break;
			case PARAM_TYPE_CIRC_ASSIGN_MAP:
				dissect_isup_circuit_assignment_map_parameter(parameter_tvb, parameter_tree, parameter_item);
				break;
			case PARAM_TYPE_CIRC_STATE_IND:
				dissect_isup_circuit_state_ind_parameter(parameter_tvb, parameter_tree, parameter_item);
				break;
			case PARAM_TYPE_AUTO_CONG_LEVEL:
				dissect_isup_automatic_congestion_level_parameter(parameter_tvb, parameter_tree, parameter_item);
				break;
			case PARAM_TYPE_ORIG_CALLED_NR:
				dissect_isup_original_called_number_parameter(parameter_tvb, parameter_tree, parameter_item);
				break;
			case PARAM_TYPE_OPT_BACKW_CALL_IND:
				dissect_isup_optional_backward_call_indicators_parameter(parameter_tvb, parameter_tree, parameter_item);
				break;
			case PARAM_TYPE_USER_TO_USER_IND:
				dissect_isup_user_to_user_indicators_parameter(parameter_tvb, parameter_tree, parameter_item);
				break;
			case PARAM_TYPE_ORIG_ISC_POINT_CODE:
				dissect_isup_original_isc_point_code_parameter(parameter_tvb, parameter_tree, parameter_item);
				break;
			case PARAM_TYPE_GENERIC_NOTIF_IND:
				dissect_isup_generic_notification_indicator_parameter(parameter_tvb, parameter_tree, parameter_item);
				break;
			case PARAM_TYPE_CALL_HIST_INFO :
				dissect_isup_call_history_information_parameter(parameter_tvb, parameter_tree, parameter_item);
				break;
			case PARAM_TYPE_ACC_DELIV_INFO:
				dissect_isup_access_delivery_information_parameter(parameter_tvb, parameter_tree, parameter_item);
				break;
			case PARAM_TYPE_NETW_SPECIFIC_FACLTY:
				dissect_isup_network_specific_facility_parameter(parameter_tvb, parameter_tree, parameter_item);
				break;
			case PARAM_TYPE_USER_SERVICE_INFO_PR:
				dissect_isup_user_service_information_prime_parameter(parameter_tvb, parameter_tree, parameter_item);
				break;
			case PARAM_TYPE_PROPAG_DELAY_COUNTER:
				dissect_isup_propagation_delay_counter_parameter(parameter_tvb, parameter_tree, parameter_item);
				break;
			case PARAM_TYPE_REMOTE_OPERATIONS:
				dissect_isup_remote_operations_parameter(parameter_tvb, parameter_tree, parameter_item);
				break;
			case PARAM_TYPE_SERVICE_ACTIVATION:
				dissect_isup_service_activation_parameter(parameter_tvb, parameter_tree, parameter_item);
				break;
			case PARAM_TYPE_USER_TELESERV_INFO:
				dissect_isup_user_teleservice_information_parameter(parameter_tvb, parameter_tree, parameter_item);
				break;
			case PARAM_TYPE_TRANSM_MEDIUM_USED:
				dissect_isup_transmission_medium_used_parameter(parameter_tvb, parameter_tree, parameter_item);
				break;
			case PARAM_TYPE_CALL_DIV_INFO:
				dissect_isup_call_diversion_information_parameter(parameter_tvb, parameter_tree, parameter_item);
				break;
			case PARAM_TYPE_ECHO_CTRL_INFO:
				dissect_isup_echo_control_information_parameter(parameter_tvb, parameter_tree, parameter_item);
				break;
			case PARAM_TYPE_MSG_COMPAT_INFO:
				dissect_isup_message_compatibility_information_parameter(parameter_tvb, parameter_tree, parameter_item);
				break;
			case PARAM_TYPE_PARAM_COMPAT_INFO:
				dissect_isup_parameter_compatibility_information_parameter(parameter_tvb, parameter_tree, parameter_item);
				break;
			case PARAM_TYPE_MLPP_PRECEDENCE:
				dissect_isup_mlpp_precedence_parameter(parameter_tvb, parameter_tree, parameter_item);
				break;
			case PARAM_TYPE_MCID_REQ_IND:
				dissect_isup_mcid_request_indicators_parameter(parameter_tvb, parameter_tree, parameter_item);
				break;
			case PARAM_TYPE_MCID_RSP_IND:
				dissect_isup_mcid_response_indicators_parameter(parameter_tvb, parameter_tree, parameter_item);
				break;
			case PARAM_TYPE_HOP_COUNTER:
				dissect_isup_hop_counter_parameter(parameter_tvb, parameter_tree, parameter_item);
				break;
			case PARAM_TYPE_TRANSM_MEDIUM_RQUR_PR:
				dissect_isup_transmission_medium_requirement_prime_parameter(parameter_tvb, parameter_tree, parameter_item);
				break;
			case PARAM_TYPE_LOCATION_NR:
				dissect_isup_location_number_parameter(parameter_tvb, parameter_tree, parameter_item);
				break;
			case PARAM_TYPE_REDIR_NR_RSTRCT:
				dissect_isup_redirection_number_restriction_parameter(parameter_tvb, parameter_tree, parameter_item);
				break;
			case PARAM_TYPE_CALL_TRANS_REF:
				dissect_isup_call_transfer_reference_parameter(parameter_tvb, parameter_tree, parameter_item);
				break;
			case PARAM_TYPE_LOOP_PREV_IND:
				dissect_isup_loop_prevention_indicators_parameter(parameter_tvb, parameter_tree, parameter_item);
				break;
			case PARAM_TYPE_CALL_TRANS_NR:
				dissect_isup_call_transfer_number_parameter(parameter_tvb, parameter_tree, parameter_item);
				break;
			case PARAM_TYPE_CCSS:
				dissect_isup_ccss_parameter(parameter_tvb, parameter_tree, parameter_item);
				break;
			case PARAM_TYPE_FORW_GVNS:
				dissect_isup_forward_gvns_parameter(parameter_tvb, parameter_tree, parameter_item);
				break;
			case PARAM_TYPE_BACKW_GVNS:
				dissect_isup_backward_gvns_parameter(parameter_tvb, parameter_tree, parameter_item);
				break;
			case PARAM_TYPE_REDIRECT_CAPAB:
				dissect_isup_redirect_capability_parameter(parameter_tvb, parameter_tree, parameter_item);
				break;
			case PARAM_TYPE_NETW_MGMT_CTRL:
				dissect_isup_network_management_controls_parameter(parameter_tvb, parameter_tree, parameter_item);
				break;
			case PARAM_TYPE_CORRELATION_ID:
				dissect_isup_correlation_id_parameter(parameter_tvb, parameter_tree, parameter_item);
				break;
			case PARAM_TYPE_SCF_ID:
				dissect_isup_scf_id_parameter(parameter_tvb, parameter_tree, parameter_item);
				break;
			case PARAM_TYPE_CALL_DIV_TREAT_IND:
				dissect_isup_call_diversion_treatment_indicators_parameter(parameter_tvb, parameter_tree, parameter_item);
				break;
			case PARAM_TYPE_CALLED_IN_NR:
				dissect_isup_called_in_number_parameter(parameter_tvb, parameter_tree, parameter_item);
				break;
			case PARAM_TYPE_CALL_OFF_TREAT_IND:
				dissect_isup_call_offering_treatment_indicators_parameter(parameter_tvb, parameter_tree, parameter_item);
				break;
			case PARAM_TYPE_CHARGED_PARTY_IDENT:
				dissect_isup_charged_party_identification_parameter(parameter_tvb, parameter_tree, parameter_item);
				break;
			case PARAM_TYPE_CONF_TREAT_IND:
				dissect_isup_conference_treatment_indicators_parameter(parameter_tvb, parameter_tree, parameter_item);
				break;
			case PARAM_TYPE_DISPLAY_INFO:
				dissect_isup_display_information_parameter(parameter_tvb, parameter_tree, parameter_item);
				break;
			case PARAM_TYPE_UID_ACTION_IND:
				dissect_isup_uid_action_indicators_parameter(parameter_tvb, parameter_tree, parameter_item);
				break;
			case PARAM_TYPE_UID_CAPAB_IND:
				dissect_isup_uid_capability_indicators_parameter(parameter_tvb, parameter_tree, parameter_item);
				break;
			case PARAM_TYPE_REDIRECT_COUNTER:
				dissect_isup_redirect_counter_parameter(parameter_tvb, parameter_tree, parameter_item);
				break;
			case PARAM_TYPE_COLLECT_CALL_REQ:
				dissect_isup_collect_call_request_parameter(parameter_tvb, parameter_tree, parameter_item);
				break;
			case PARAM_TYPE_GENERIC_NR:
				dissect_isup_generic_number_parameter(parameter_tvb, parameter_tree, parameter_item);
				break;
			case PARAM_TYPE_GENERIC_DIGITS:
				dissect_isup_generic_digits_parameter(parameter_tvb, parameter_tree, parameter_item);
				break;
			case PARAM_TYPE_APPLICATON_TRANS:
				dissect_isup_application_transport_parameter(parameter_tvb, pinfo, parameter_tree, parameter_item);
				break;
	  
			default:
				dissect_isup_unknown_parameter(parameter_tvb, parameter_item);
				break;
			}
			
	
			offset += MIN(parameter_length, actual_length);
		}

	 }
    else {
		/
		proto_tree_add_uint_format(isup_tree, hf_isup_parameter_type, optional_parameters_tvb , offset, PARAMETER_TYPE_LENGTH, parameter_type, "End of optional parameters (%u)", parameter_type);
    }
  }
}
