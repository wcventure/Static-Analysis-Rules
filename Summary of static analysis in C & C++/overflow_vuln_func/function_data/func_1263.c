gint
dissect_isup_initial_address_message(tvbuff_t *message_tvb, proto_tree *isup_tree)
{ proto_item* parameter_item;
  proto_tree* parameter_tree;
  tvbuff_t *parameter_tvb;
  gint offset = 0;
  gint parameter_type, parameter_pointer, parameter_length, actual_length;
  
  /
  parameter_type = PARAM_TYPE_NATURE_OF_CONN_IND;
  parameter_item = proto_tree_add_text(isup_tree, message_tvb, offset,
				       NATURE_OF_CONNECTION_IND_LENGTH,
				       "Nature of Connection Indicators");
  parameter_tree = proto_item_add_subtree(parameter_item, ett_isup_parameter);
  proto_tree_add_uint_format(parameter_tree, hf_isup_parameter_type, message_tvb, 0, 0, parameter_type, "Mandatory Parameter: %u (%s)", parameter_type, val_to_str(parameter_type, isup_parameter_type_value,"unknown"));		
  actual_length = tvb_length_remaining(message_tvb, offset);
  parameter_tvb = tvb_new_subset(message_tvb, offset, MIN(NATURE_OF_CONNECTION_IND_LENGTH, actual_length), NATURE_OF_CONNECTION_IND_LENGTH);
  dissect_isup_nature_of_connection_indicators_parameter(parameter_tvb, parameter_tree, parameter_item);
  offset += NATURE_OF_CONNECTION_IND_LENGTH;

  /
  parameter_type =  PARAM_TYPE_FORW_CALL_IND;
  parameter_item = proto_tree_add_text(isup_tree, message_tvb, offset,
				       FORWARD_CALL_IND_LENGTH,
				       "Forward Call Indicators");
  parameter_tree = proto_item_add_subtree(parameter_item, ett_isup_parameter);
  proto_tree_add_uint_format(parameter_tree, hf_isup_parameter_type, message_tvb, 0, 0, parameter_type, "Mandatory Parameter: %u (%s)", parameter_type, val_to_str(parameter_type, isup_parameter_type_value,"unknown"));		
  actual_length = tvb_length_remaining(message_tvb, offset); 
  parameter_tvb = tvb_new_subset(message_tvb, offset, MIN(FORWARD_CALL_IND_LENGTH, actual_length), FORWARD_CALL_IND_LENGTH );
  dissect_isup_forward_call_indicators_parameter(parameter_tvb, parameter_tree, parameter_item);
  offset +=  FORWARD_CALL_IND_LENGTH;

  /
  parameter_type = PARAM_TYPE_CALLING_PRTY_CATEG;
  parameter_item = proto_tree_add_text(isup_tree, message_tvb, offset,
				       CALLING_PRTYS_CATEGORY_LENGTH,
				       "Calling Party's category");
  parameter_tree = proto_item_add_subtree(parameter_item, ett_isup_parameter);
  proto_tree_add_uint_format(parameter_tree, hf_isup_parameter_type, message_tvb, 0, 0, parameter_type, "Mandatory Parameter: %u (%s)", parameter_type, val_to_str(parameter_type, isup_parameter_type_value,"unknown"));		
  actual_length = tvb_length_remaining(message_tvb, offset); 
  parameter_tvb = tvb_new_subset(message_tvb, offset, MIN(CALLING_PRTYS_CATEGORY_LENGTH, actual_length),CALLING_PRTYS_CATEGORY_LENGTH );
  dissect_isup_calling_partys_category_parameter(parameter_tvb, parameter_tree, parameter_item);
  offset += CALLING_PRTYS_CATEGORY_LENGTH;

  /
  parameter_type = PARAM_TYPE_TRANSM_MEDIUM_REQU;
  parameter_item = proto_tree_add_text(isup_tree, message_tvb, offset,
				       TRANSMISSION_MEDIUM_REQUIREMENT_LENGTH,
				       "Transmission medium requirement");
  parameter_tree = proto_item_add_subtree(parameter_item, ett_isup_parameter);
  proto_tree_add_uint_format(parameter_tree, hf_isup_parameter_type, message_tvb, 0, 0, parameter_type, "Mandatory Parameter: %u (%s)", parameter_type, val_to_str(parameter_type, isup_parameter_type_value,"unknown"));		       
  actual_length = tvb_length_remaining(message_tvb, offset); 
  parameter_tvb = tvb_new_subset(message_tvb, offset, MIN(TRANSMISSION_MEDIUM_REQUIREMENT_LENGTH, actual_length), TRANSMISSION_MEDIUM_REQUIREMENT_LENGTH);
  dissect_isup_transmission_medium_requirement_parameter(parameter_tvb, parameter_tree, parameter_item);
  offset += TRANSMISSION_MEDIUM_REQUIREMENT_LENGTH;


  /
  parameter_type = PARAM_TYPE_CALLED_PARTY_NR;
  parameter_pointer = tvb_get_guint8(message_tvb, offset);
  parameter_length = tvb_get_guint8(message_tvb, offset + parameter_pointer);

  parameter_item = proto_tree_add_text(isup_tree, message_tvb,
				       offset +  parameter_pointer,
				       parameter_length + PARAMETER_LENGTH_IND_LENGTH,
				       "Called Party Number");
  parameter_tree = proto_item_add_subtree(parameter_item, ett_isup_parameter);
  proto_tree_add_uint_format(parameter_tree, hf_isup_parameter_type, message_tvb, 0, 0, parameter_type, "Mandatory Parameter: %u (%s)", parameter_type, val_to_str(parameter_type, isup_parameter_type_value,"unknown"));	
  proto_tree_add_uint_format(parameter_tree, hf_isup_mandatory_variable_parameter_pointer, message_tvb, offset, PARAMETER_POINTER_LENGTH, parameter_pointer, "Pointer to Parameter: %u", parameter_pointer);  
  proto_tree_add_uint_format(parameter_tree, hf_isup_parameter_length, message_tvb, offset + parameter_pointer, PARAMETER_LENGTH_IND_LENGTH, parameter_length, "Parameter length: %u", parameter_length);
  actual_length = tvb_length_remaining(message_tvb, offset); 	       
  parameter_tvb = tvb_new_subset(message_tvb, offset + parameter_pointer + PARAMETER_LENGTH_IND_LENGTH, MIN(parameter_length, actual_length), parameter_length );
  dissect_isup_called_party_number_parameter(parameter_tvb, parameter_tree, parameter_item);
  offset += PARAMETER_POINTER_LENGTH;

  return offset;
}
