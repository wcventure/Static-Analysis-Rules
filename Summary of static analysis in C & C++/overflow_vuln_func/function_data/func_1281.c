void
dissect_isup_message(tvbuff_t *message_tvb, packet_info *pinfo, proto_tree *isup_tree)
{ 
  tvbuff_t *parameter_tvb;
  tvbuff_t *optional_parameter_tvb;
  proto_item* pass_along_item;
  proto_tree* pass_along_tree;
  gint offset, bufferlength;
  guint8 message_type, opt_parameter_pointer;
  gint opt_part_possible = FALSE; /
  offset = 0;
  
  /
  message_type = tvb_get_guint8(message_tvb,0); 

  if (check_col(pinfo->cinfo, COL_INFO)){
    col_append_str(pinfo->cinfo, COL_INFO, val_to_str(message_type, isup_message_type_value, "reserved"));
    col_append_str(pinfo->cinfo, COL_INFO, " ");
  }
 
   proto_tree_add_uint_format(isup_tree, hf_isup_message_type, message_tvb, 0, MESSAGE_TYPE_LENGTH, message_type, "Message type: %s (%u)", val_to_str(message_type, isup_message_type_value, "reserved"), message_type);
   offset +=  MESSAGE_TYPE_LENGTH;

   bufferlength = tvb_length_remaining(message_tvb, offset);
   parameter_tvb = tvb_new_subset(message_tvb, offset, bufferlength, bufferlength);

   /
   switch (message_type) {
     case MESSAGE_TYPE_INITIAL_ADDR:
       offset += dissect_isup_initial_address_message(parameter_tvb, isup_tree);
       opt_part_possible = TRUE;
      break;
    case MESSAGE_TYPE_SUBSEQ_ADDR:
       offset += dissect_isup_subsequent_address_message(parameter_tvb, isup_tree);
       opt_part_possible = TRUE;
      break;
    case MESSAGE_TYPE_INFO_REQ:
       offset += dissect_isup_information_request_message(parameter_tvb, isup_tree);
       opt_part_possible = TRUE;
      break;
    case MESSAGE_TYPE_INFO:
       offset += dissect_isup_information_message(parameter_tvb, isup_tree);
       opt_part_possible = TRUE;
      break;
    case MESSAGE_TYPE_CONTINUITY:
       offset += dissect_isup_continuity_message(parameter_tvb, isup_tree);
      break;
    case MESSAGE_TYPE_ADDR_CMPL:
       offset += dissect_isup_address_complete_message(parameter_tvb, isup_tree);
       opt_part_possible = TRUE;
      break;
    case MESSAGE_TYPE_CONNECT:
       offset += dissect_isup_connect_message(parameter_tvb, isup_tree);
       opt_part_possible = TRUE;
      break;
    case MESSAGE_TYPE_FORW_TRANS:
      /
       opt_part_possible = TRUE;
      break;
    case MESSAGE_TYPE_ANSWER:
      /
       opt_part_possible = TRUE;
      break;
    case MESSAGE_TYPE_RELEASE:
       offset += dissect_isup_release_message(parameter_tvb, isup_tree);
       opt_part_possible = TRUE;
      break;
    case MESSAGE_TYPE_SUSPEND:
       offset += dissect_isup_suspend_resume_message(parameter_tvb, isup_tree);
       opt_part_possible = TRUE;
      break;
    case MESSAGE_TYPE_RESUME:
       offset += dissect_isup_suspend_resume_message(parameter_tvb, isup_tree);
       opt_part_possible = TRUE;
      break;
    case MESSAGE_TYPE_REL_CMPL:
      /
       opt_part_possible = TRUE;
      break;
    case MESSAGE_TYPE_CONT_CHECK_REQ:
      /
      break;
    case MESSAGE_TYPE_RESET_CIRCUIT:
      /
      break;
    case MESSAGE_TYPE_BLOCKING:
      /
      break;
    case MESSAGE_TYPE_UNBLOCKING:
      /
      break;
    case MESSAGE_TYPE_BLOCK_ACK:
      /
      break;
    case MESSAGE_TYPE_UNBLOCK_ACK:
      /
      break;
    case MESSAGE_TYPE_CIRC_GRP_RST:
       offset += dissect_isup_circuit_group_reset_query_message(parameter_tvb, isup_tree);
      break;
    case MESSAGE_TYPE_CIRC_GRP_BLCK:
       offset += dissect_isup_circuit_group_blocking_messages(parameter_tvb, isup_tree);
      break;
    case MESSAGE_TYPE_CIRC_GRP_UNBL:
       offset += dissect_isup_circuit_group_blocking_messages(parameter_tvb, isup_tree);
      break;
    case MESSAGE_TYPE_CIRC_GRP_BL_ACK:
       offset += dissect_isup_circuit_group_blocking_messages(parameter_tvb, isup_tree);
      break;
    case MESSAGE_TYPE_CIRC_GRP_UNBL_ACK:
       offset += dissect_isup_circuit_group_blocking_messages(parameter_tvb, isup_tree);
      break;
    case MESSAGE_TYPE_FACILITY_REQ:
       offset += dissect_isup_facility_request_accepted_message(parameter_tvb, isup_tree);
       opt_part_possible = TRUE;
      break;
    case MESSAGE_TYPE_FACILITY_ACC:
       offset += dissect_isup_facility_request_accepted_message(parameter_tvb, isup_tree);
       opt_part_possible = TRUE;
      break;
    case MESSAGE_TYPE_FACILITY_REJ:
       offset += dissect_isup_facility_reject_message(parameter_tvb, isup_tree);
       opt_part_possible = TRUE;
      break;
    case MESSAGE_TYPE_LOOP_BACK_ACK:
      /
      break;
    case MESSAGE_TYPE_PASS_ALONG:
      /
      {	guint8 pa_message_type;
	pa_message_type = tvb_get_guint8(parameter_tvb, 0);
	pass_along_item = proto_tree_add_text(isup_tree, parameter_tvb, offset, tvb_length_remaining(parameter_tvb, offset), "Pass-along: %s Message (%u)", val_to_str(pa_message_type, isup_message_type_value, "reserved"), pa_message_type);
	pass_along_tree = proto_item_add_subtree(pass_along_item, ett_isup_pass_along_message);
	dissect_isup_message(parameter_tvb, pinfo, pass_along_tree);
	break;
      }
    case MESSAGE_TYPE_CIRC_GRP_RST_ACK:
       offset += dissect_isup_circuit_group_reset_acknowledgement_message(parameter_tvb, isup_tree);
      break;
    case MESSAGE_TYPE_CIRC_GRP_QRY:
       offset += dissect_isup_circuit_group_reset_query_message(parameter_tvb, isup_tree);
      break;
    case MESSAGE_TYPE_CIRC_GRP_QRY_RSP:
       offset += dissect_isup_circuit_group_query_response_message(parameter_tvb, isup_tree);
      break;
    case MESSAGE_TYPE_CALL_PROGRSS:
       offset += dissect_isup_call_progress_message(parameter_tvb, isup_tree);
       opt_part_possible = TRUE;
      break;
    case MESSAGE_TYPE_USER2USER_INFO:
      offset += dissect_isup_user_to_user_information_message(parameter_tvb, isup_tree);
      opt_part_possible = TRUE;
      break;
    case MESSAGE_TYPE_UNEQUIPPED_CIC:
      /
      break;
    case MESSAGE_TYPE_CONFUSION:
      offset += dissect_isup_confusion_message(parameter_tvb, isup_tree);
       opt_part_possible = TRUE;
      break;
    case MESSAGE_TYPE_OVERLOAD:
      /
      break;
    case MESSAGE_TYPE_CHARGE_INFO:
      /
      proto_tree_add_text(isup_tree, parameter_tvb, 0, bufferlength, "Format is a national matter"); 
      break;
    case MESSAGE_TYPE_NETW_RESRC_MGMT:
      /
       opt_part_possible = TRUE;
      break;
    case MESSAGE_TYPE_FACILITY:
      /
       opt_part_possible = TRUE;
      break;
    case MESSAGE_TYPE_USER_PART_TEST:
      /
       opt_part_possible = TRUE;
      break;
    case MESSAGE_TYPE_USER_PART_AVAIL:
      /
       opt_part_possible = TRUE;
      break;
    case MESSAGE_TYPE_IDENT_REQ:
      /
       opt_part_possible = TRUE;
      break;
    case MESSAGE_TYPE_IDENT_RSP:
      /
       opt_part_possible = TRUE;
      break;
    case MESSAGE_TYPE_SEGMENTATION:
      /
       opt_part_possible = TRUE;
      break;
    case MESSAGE_TYPE_LOOP_PREVENTION:
      /
       opt_part_possible = TRUE;
      break;
    default:
      proto_tree_add_text(isup_tree, parameter_tvb, 0, bufferlength, "Unknown Message type (possibly reserved/used in former ISUP version)");
      break;
  }
   
   /
   if (opt_part_possible == TRUE){
     opt_parameter_pointer = tvb_get_guint8(message_tvb, offset);
     if (opt_parameter_pointer > 0){
       proto_tree_add_uint_format(isup_tree, hf_isup_pointer_to_start_of_optional_part, message_tvb, offset, PARAMETER_POINTER_LENGTH, opt_parameter_pointer, "Pointer to start of optional part: %u", opt_parameter_pointer);  
       offset += opt_parameter_pointer;
       optional_parameter_tvb = tvb_new_subset(message_tvb, offset, -1, -1 );
       dissect_isup_optional_parameter(optional_parameter_tvb, isup_tree);
     }
     else
       proto_tree_add_uint_format(isup_tree, hf_isup_pointer_to_start_of_optional_part, message_tvb, offset, PARAMETER_POINTER_LENGTH, opt_parameter_pointer, "No optional parameter present (Pointer: %u)", opt_parameter_pointer);  
   }
   else if (message_type !=MESSAGE_TYPE_CHARGE_INFO)
     proto_tree_add_text(isup_tree, message_tvb, 0, 0, "No optional parameters are possible with this message type");  
    
}
