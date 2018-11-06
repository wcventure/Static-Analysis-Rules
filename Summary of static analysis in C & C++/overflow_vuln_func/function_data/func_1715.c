static void
dissect_isup_user_to_user_information_parameter(tvbuff_t *parameter_tvb, proto_tree *parameter_tree, proto_item *parameter_item)
{ guint length = tvb_reported_length(parameter_tvb);
  proto_tree_add_text(parameter_tree, parameter_tvb, 0, -1, 
	  "User-to-user info (-> Q.931)");
  dissect_q931_user_user_ie(parameter_tvb, 0, length,
    parameter_tree );
  proto_item_set_text(parameter_item, "User-to-user information,(%u byte%s length)",
	length , plurality(length, "", "s"));
}
