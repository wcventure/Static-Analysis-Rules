int
dissect_gsm_map_RAIdentity(gboolean implicit_tag _U_, tvbuff_t *tvb, int offset, packet_info *pinfo _U_, proto_tree *tree, int hf_index _U_) {
#line 554 "gsmmap.cnf"

	tvbuff_t	*parameter_tvb;
	proto_item *item;
	proto_tree *subtree;

  offset = dissect_ber_octet_string(implicit_tag, pinfo, tree, tvb, offset, hf_index,
                                       &parameter_tvb);


	 if (!parameter_tvb)
		return offset;
	item = get_ber_last_created_item();
	subtree = proto_item_add_subtree(item, ett_gsm_map_RAIdentity);
	de_gmm_rai(parameter_tvb, subtree, 0, 3, NULL,0);



  return offset;
}
