static void
dissect_isup_parameter_compatibility_information_parameter(tvbuff_t *parameter_tvb, proto_tree *parameter_tree, proto_item *parameter_item)
{
  guint  length = tvb_length(parameter_tvb);
  guint  len = length;
  guint8 upgraded_parameter, upgraded_parameter_no;
  guint8 offset;
  guint8 instruction_indicators; 
  offset = 0;
  upgraded_parameter_no = 0;

  proto_item_set_text(parameter_item, "Parameter compatibility information (%u byte%s length)", length , plurality(length, "", "s"));
/

  while ( len > 0 ) {
  upgraded_parameter_no = upgraded_parameter_no + 1;
  upgraded_parameter = tvb_get_guint8(parameter_tvb, offset);

  proto_tree_add_text(parameter_tree, parameter_tvb, offset, 1,
	    "Upgraded parameter no: %u = %s", upgraded_parameter_no,
	    val_to_str(upgraded_parameter, isup_parameter_type_value, "unknown (%u)"));
  offset += 1;
  len -= 1;
  instruction_indicators = tvb_get_guint8(parameter_tvb, offset);

  proto_tree_add_text(parameter_tree, parameter_tvb, offset, 1,
		    "Instruction indicators: 0x%x ",
		    instruction_indicators);

 proto_tree_add_boolean(parameter_tree, hf_isup_transit_at_intermediate_exchange_ind, 
                parameter_tvb, offset, 1, instruction_indicators );

 proto_tree_add_boolean(parameter_tree, hf_isup_Release_call_ind, parameter_tvb, offset, 1, instruction_indicators );

 proto_tree_add_boolean(parameter_tree, hf_isup_Send_notification_ind, parameter_tvb, offset, 1, instruction_indicators );

 proto_tree_add_boolean(parameter_tree, hf_isup_Discard_message_ind_value, parameter_tvb, offset, 1, instruction_indicators );

 proto_tree_add_boolean(parameter_tree, hf_isup_Discard_parameter_ind, parameter_tvb, offset, 1, instruction_indicators );

 proto_tree_add_uint(parameter_tree, hf_isup_Pass_on_not_possible_indicator, parameter_tvb, offset, 1,instruction_indicators);

 proto_tree_add_boolean(parameter_tree, hf_isup_extension_ind , parameter_tvb, offset, 1, instruction_indicators );

  offset += 1;
  len -= 1;
  if (!(instruction_indicators & H_8BIT_MASK)) {
		if (len == 0)
			return;
                  instruction_indicators = tvb_get_guint8(parameter_tvb, offset);
		 proto_tree_add_uint(parameter_tree, hf_isup_Broadband_narrowband_interworking_ind, parameter_tvb, offset, 1,instruction_indicators);
                 offset += 1;
                  len -= 1;
                  }
   if (len == 0)
   return;
  ;
 }
/
 
}
