static void
dissect_isup_user_to_user_indicators_parameter(tvbuff_t *parameter_tvb, 
											   proto_tree *parameter_tree, 
											   proto_item *parameter_item)
{
  guint8 indicators;

  indicators = tvb_get_guint8(parameter_tvb, 0);
  proto_tree_add_boolean(parameter_tree, hf_isup_UUI_type, parameter_tvb, 0, 1, indicators);
  if ( (indicators & 0x01) == 0 ){	
	  /
	  proto_tree_add_uint(parameter_tree, hf_isup_UUI_req_service1, parameter_tvb, 0, 1, indicators);
	  proto_tree_add_uint(parameter_tree, hf_isup_UUI_req_service2, parameter_tvb, 0, 1, indicators);
	  proto_tree_add_uint(parameter_tree, hf_isup_UUI_req_service3, parameter_tvb, 0, 1, indicators);
  }
  else {
	  /
  	  proto_tree_add_uint(parameter_tree, hf_isup_UUI_res_service1, parameter_tvb, 0, 1, indicators);
	  proto_tree_add_uint(parameter_tree, hf_isup_UUI_res_service2, parameter_tvb, 0, 1, indicators);
	  proto_tree_add_uint(parameter_tree, hf_isup_UUI_res_service3, parameter_tvb, 0, 1, indicators);
	  proto_tree_add_boolean(parameter_tree, hf_isup_UUI_network_discard_ind, parameter_tvb, 0, 1, indicators);
	
  }  
  proto_item_set_text(parameter_item,"User-to-user indicators: 0x%x", indicators );
}
