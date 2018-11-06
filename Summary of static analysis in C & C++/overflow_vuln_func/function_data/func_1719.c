static int
dissect_codec(tvbuff_t *parameter_tvb, proto_tree *bat_ase_element_tree, gint length_indicator, gint offset,gint identifier)
{
/
guint8 compatibility_info;

	proto_tree_add_uint(bat_ase_element_tree , hf_bat_ase_identifier , parameter_tvb, offset - 1, 1, identifier );
	proto_tree_add_uint(bat_ase_element_tree , hf_length_indicator  , parameter_tvb, offset, 1, length_indicator );
	offset = offset + 1;	
	compatibility_info = tvb_get_guint8(parameter_tvb, offset);
	proto_tree_add_uint(bat_ase_element_tree, hf_Instruction_ind_for_general_action , parameter_tvb, offset, 1, compatibility_info );
	proto_tree_add_boolean(bat_ase_element_tree, hf_Send_notification_ind_for_general_action , parameter_tvb, offset, 1, compatibility_info );
	proto_tree_add_uint(bat_ase_element_tree, hf_Instruction_ind_for_pass_on_not_possible , parameter_tvb, offset, 1, compatibility_info );
	proto_tree_add_boolean(bat_ase_element_tree, hf_Send_notification_ind_for_pass_on_not_possible , parameter_tvb, offset, 1, compatibility_info );
	proto_tree_add_boolean(bat_ase_element_tree, hf_isup_extension_ind , parameter_tvb, offset, 1, compatibility_info );

	offset = dissect_codec_mode(bat_ase_element_tree, parameter_tvb, offset+1,length_indicator-1);
return offset;
}
