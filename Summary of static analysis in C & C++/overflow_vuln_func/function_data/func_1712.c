static void
dissect_isup_cvr_response_ind_parameter(tvbuff_t *parameter_tvb, proto_tree *parameter_tree, proto_item *parameter_item)
{
	guint8 cvr_response_ind;
	
	cvr_response_ind = tvb_get_guint8(parameter_tvb, 0);
	proto_tree_add_uint(parameter_tree, hf_isup_cvr_rsp_ind, parameter_tvb, 0, CVR_RESP_IND_LENGTH, cvr_response_ind );
	proto_item_set_text(parameter_item, "Circuit Validation Test Response Indicator: 0x%x", cvr_response_ind );

}
