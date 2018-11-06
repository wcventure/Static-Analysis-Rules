static gint
dissect_ansi_isup_circuit_validation_test_resp_message(tvbuff_t *message_tvb, proto_tree *isup_tree)
{
  proto_item* parameter_item;
  proto_tree* parameter_tree;
  tvbuff_t *parameter_tvb;
  gint offset = 0;
  gint parameter_type,actual_length;

  /
  parameter_type = ANSI_ISUP_PARAM_TYPE_CVR_RESP_IND;
  parameter_item = proto_tree_add_text(isup_tree, message_tvb, offset, CVR_RESP_IND_LENGTH, "CVR Reponse Indicator");
  
  parameter_tree = proto_item_add_subtree(parameter_item, ett_isup_parameter);
  proto_tree_add_uint_format(parameter_tree, hf_isup_parameter_type, message_tvb, 0, 0, parameter_type, "Mandatory Parameter: %u (%s)", parameter_type, val_to_str(parameter_type, isup_parameter_type_value,"CVR Reponse Indicator"));
  
  actual_length = tvb_ensure_length_remaining(message_tvb, offset);
  
  parameter_tvb = tvb_new_subset(message_tvb, offset, MIN(CVR_RESP_IND_LENGTH, actual_length), CVR_RESP_IND_LENGTH);
  dissect_isup_cvr_response_ind_parameter(parameter_tvb, parameter_tree, parameter_item);
  offset += CVR_RESP_IND_LENGTH;

  /
  parameter_type = ANSI_ISUP_PARAM_TYPE_CG_CHAR_IND;
  parameter_item = proto_tree_add_text(isup_tree, message_tvb, offset,
									   CG_CHAR_IND_LENGTH,
								       "Circuit Group Characterstics Indicators");
  parameter_tree = proto_item_add_subtree(parameter_item, ett_isup_parameter);
  proto_tree_add_uint_format(parameter_tree, hf_isup_parameter_type, message_tvb, 0, 0, parameter_type, "Mandatory Parameter: %u (%s)", parameter_type, val_to_str(parameter_type, isup_parameter_type_value,"Circuit Group Characters"));
  actual_length = tvb_ensure_length_remaining(message_tvb, offset);
  parameter_tvb = tvb_new_subset(message_tvb, offset, MIN(CG_CHAR_IND_LENGTH, actual_length), CG_CHAR_IND_LENGTH);
  dissect_isup_circuit_group_char_ind_parameter(parameter_tvb, parameter_tree, parameter_item);
  offset += CG_CHAR_IND_LENGTH;

  return offset;
}
