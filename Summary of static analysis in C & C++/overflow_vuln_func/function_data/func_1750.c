int
dissect_gsm_map_GSN_Address(gboolean implicit_tag _U_, tvbuff_t *tvb, int offset, packet_info *pinfo _U_, proto_tree *tree, int hf_index _U_) {
#line 524 "gsmmap.cnf"

	tvbuff_t	*parameter_tvb;
	guint8		octet;
	proto_item *item;
	proto_tree *subtree;

  offset = dissect_ber_octet_string(implicit_tag, pinfo, tree, tvb, offset, hf_index,
                                       &parameter_tvb);


	if (!parameter_tvb)
		return offset;
	item = get_ber_last_created_item();
	subtree = proto_item_add_subtree(item, ett_gsm_map_pdptypenumber);

	octet = tvb_get_guint8(parameter_tvb,0);
	switch(octet){
	case 0x04: /
		proto_tree_add_item(subtree, hf_gsm_map_GSNAddress_IPv4, parameter_tvb, 1, tvb_length_remaining(parameter_tvb, 1), FALSE);
		break;
	case 0x50: /
		proto_tree_add_item(subtree, hf_gsm_map_GSNAddress_IPv4, parameter_tvb, 1, tvb_length_remaining(parameter_tvb, 1), FALSE);
		break;
	default:
		break;
	}



  return offset;
}
