static void
dissect_isup_circuit_group_char_ind_parameter(tvbuff_t *parameter_tvb, proto_tree *parameter_tree, proto_item *parameter_item)
{
	guint8 cvr_cg_char_ind;

	cvr_cg_char_ind = tvb_get_guint8(parameter_tvb, 0);
	proto_tree_add_uint(parameter_tree, hf_isup_cvr_cg_car_ind, parameter_tvb, 0, CG_CHAR_IND_LENGTH, cvr_cg_char_ind );
	proto_tree_add_uint(parameter_tree, hf_isup_cvr_cg_double_seize, parameter_tvb, 0, CG_CHAR_IND_LENGTH, cvr_cg_char_ind );
	proto_tree_add_uint(parameter_tree, hf_isup_cvr_cg_alarm_car_ind, parameter_tvb, 0, CG_CHAR_IND_LENGTH, cvr_cg_char_ind );
	proto_tree_add_uint(parameter_tree, hf_isup_cvr_cont_chk_ind, parameter_tvb, 0, CG_CHAR_IND_LENGTH, cvr_cg_char_ind );
	
	proto_item_set_text(parameter_item, "Circuit Validation Test Response Circuit Group Characterstics: 0x%x", cvr_cg_char_ind );
}
