static void
dissect_isup_user_teleservice_information_parameter(tvbuff_t *parameter_tvb, proto_tree *parameter_tree, proto_item *parameter_item)
{ 
  guint length = tvb_length(parameter_tvb);
  proto_tree_add_text(parameter_tree, parameter_tvb, 0, length,
	  "User teleservice information (-> Q.931 High Layer Compatibility IE)");

  dissect_q931_high_layer_compat_ie(parameter_tvb, 0, length, parameter_tree);

  proto_item_set_text(parameter_item, 
	  "User teleservice information");
}
