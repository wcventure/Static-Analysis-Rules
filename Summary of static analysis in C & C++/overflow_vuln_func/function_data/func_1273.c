gint 
dissect_isup_circuit_group_blocking_messages(tvbuff_t *message_tvb, proto_tree *isup_tree)
{ proto_item* parameter_item;
  proto_tree* parameter_tree;
  tvbuff_t *parameter_tvb;
  gint offset = 0;
  gint parameter_type, parameter_pointer, parameter_length, actual_length;

   / 
  parameter_type = PARAM_TYPE_CIRC_GRP_SV_MSG_TYPE;
  parameter_item = proto_tree_add_text(isup_tree, message_tvb, offset,
				       CIRC_GRP_SV_MSG_TYPE_LENGTH,
				       "Circuit group supervision message type");
  parameter_tree = proto_item_add_subtree(parameter_item, ett_isup_parameter);
  proto_tree_add_uint_format(parameter_tree, hf_isup_parameter_type, message_tvb, 0, 0, parameter_type, "Mandatory Parameter: %u (%s)", parameter_type, val_to_str(parameter_type, isup_parameter_type_value,"unknown"));		
  actual_length = tvb_length_remaining(message_tvb, offset);
  parameter_tvb = tvb_new_subset(message_tvb, offset, MIN(CIRC_GRP_SV_MSG_TYPE_LENGTH, actual_length), CIRC_GRP_SV_MSG_TYPE_LENGTH);
  dissect_isup_circuit_group_supervision_message_type_parameter(parameter_tvb, parameter_tree, parameter_item);
  offset += CIRC_GRP_SV_MSG_TYPE_LENGTH;
  
  /
  parameter_type =  PARAM_TYPE_RANGE_AND_STATUS;

  parameter_pointer = tvb_get_guint8(message_tvb, offset);
  parameter_length = tvb_get_guint8(message_tvb, offset + parameter_pointer);

  parameter_item = proto_tree_add_text(isup_tree, message_tvb,
				       offset +  parameter_pointer,
				       parameter_length + PARAMETER_LENGTH_IND_LENGTH,
				       "Facility indicator");
  parameter_tree = proto_item_add_subtree(parameter_item, ett_isup_parameter);
  proto_tree_add_uint_format(parameter_tree, hf_isup_parameter_type, message_tvb, 0, 0, parameter_type, "Mandatory Parameter: %u (%s)", parameter_type, val_to_str(parameter_type, isup_parameter_type_value,"unknown"));	
  proto_tree_add_uint_format(parameter_tree, hf_isup_mandatory_variable_parameter_pointer, message_tvb, offset, PARAMETER_POINTER_LENGTH, parameter_pointer, "Pointer to Parameter: %u", parameter_pointer);  
  proto_tree_add_uint_format(parameter_tree, hf_isup_parameter_length, message_tvb, offset + parameter_pointer, PARAMETER_LENGTH_IND_LENGTH, parameter_length, "Parameter length: %u", parameter_length);
  actual_length = tvb_length_remaining(message_tvb, offset); 	       
  parameter_tvb = tvb_new_subset(message_tvb, offset + parameter_pointer + PARAMETER_LENGTH_IND_LENGTH, MIN(parameter_length, actual_length), parameter_length );
  dissect_isup_facility_ind_parameter(parameter_tvb, parameter_tree, parameter_item);
  offset += PARAMETER_POINTER_LENGTH;

  return offset;
}
