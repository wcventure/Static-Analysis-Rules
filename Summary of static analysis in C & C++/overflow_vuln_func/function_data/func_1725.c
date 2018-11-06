static void
dissect_isup_echo_control_information_parameter(tvbuff_t *parameter_tvb, proto_tree *parameter_tree, proto_item *parameter_item)
{ guint8 indicator;
  gint offset = 0;
  indicator = tvb_get_guint8(parameter_tvb, 0);
  proto_tree_add_text(parameter_tree, parameter_tvb, 0, ECHO_CONTROL_INFO_LENGTH,
	  "Echo control information: 0x%x", indicator);

  proto_tree_add_uint(parameter_tree, hf_isup_OECD_inf_ind, 
                parameter_tvb, offset, 1, indicator );

  proto_tree_add_uint(parameter_tree, hf_isup_IECD_inf_ind, 
                parameter_tvb, offset, 1, indicator );

  proto_tree_add_uint(parameter_tree, hf_isup_OECD_req_ind, 
                parameter_tvb, offset, 1, indicator );

  proto_tree_add_uint(parameter_tree, hf_isup_IECD_req_ind, 
                parameter_tvb, offset, 1, indicator );

  proto_item_set_text(parameter_item, "Echo control information: 0x%x", indicator);
}
