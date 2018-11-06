static void
dissect_isup_message_compatibility_information_parameter(tvbuff_t *parameter_tvb, proto_tree *parameter_tree, proto_item *parameter_item)
{
 guint length = tvb_length(parameter_tvb);
 guint instruction_indicators;
 gint offset = 0;
 instruction_indicators = tvb_get_guint8(parameter_tvb, offset);
 
 proto_tree_add_text(parameter_tree, parameter_tvb, 0, length, 
	  "Message compatibility information");

 proto_tree_add_boolean(parameter_tree, hf_isup_transit_at_intermediate_exchange_ind, 
                parameter_tvb, offset, 1, instruction_indicators );

 proto_tree_add_boolean(parameter_tree, hf_isup_Release_call_ind, 
	 parameter_tvb, offset, 1, instruction_indicators );

 proto_tree_add_boolean(parameter_tree, hf_isup_Send_notification_ind, 
	 parameter_tvb, offset, 1, instruction_indicators );

 proto_tree_add_boolean(parameter_tree, hf_isup_Discard_message_ind_value, 
	 parameter_tvb, offset, 1, instruction_indicators );

 proto_tree_add_boolean(parameter_tree, hf_isup_pass_on_not_possible_indicator2, 
	 parameter_tvb, offset, 1,instruction_indicators);

 proto_tree_add_uint(parameter_tree, hf_isup_Broadband_narrowband_interworking_ind2,
	 parameter_tvb, offset, 1,instruction_indicators);

 proto_tree_add_boolean(parameter_tree, hf_isup_extension_ind , 
	 parameter_tvb, offset, 1, instruction_indicators );

  proto_item_set_text(parameter_item, "Message compatibility information (%u byte%s length)",
	  length , plurality(length, "", "s"));
}
