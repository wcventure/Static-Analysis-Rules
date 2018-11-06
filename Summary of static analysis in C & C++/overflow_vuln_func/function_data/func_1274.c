gint
dissect_isup_facility_request_accepted_message(tvbuff_t *message_tvb, proto_tree *isup_tree)
{ proto_item* parameter_item;
  proto_tree* parameter_tree;
  tvbuff_t *parameter_tvb;
  gint offset = 0;
  gint parameter_type, actual_length;
 
  / 
  parameter_type = PARAM_TYPE_FACILITY_IND;
  parameter_item = proto_tree_add_text(isup_tree, message_tvb, offset,
				       FACILITY_IND_LENGTH,
				       "Facility indicator");
  parameter_tree = proto_item_add_subtree(parameter_item, ett_isup_parameter);
  proto_tree_add_uint_format(parameter_tree, hf_isup_parameter_type, message_tvb, 0, 0, parameter_type, "Mandatory Parameter: %u (%s)", parameter_type, val_to_str(parameter_type, isup_parameter_type_value,"unknown"));		
  actual_length = tvb_length_remaining(message_tvb, offset);
  parameter_tvb = tvb_new_subset(message_tvb, offset, MIN(FACILITY_IND_LENGTH, actual_length), FACILITY_IND_LENGTH);
  dissect_isup_facility_ind_parameter(parameter_tvb, parameter_tree, parameter_item);
  offset += FACILITY_IND_LENGTH;
  return offset;
}
