static int
dissect_gsm_map_PDP_Type(gboolean implicit_tag _U_, tvbuff_t *tvb, int offset, packet_info *pinfo _U_, proto_tree *tree, int hf_index _U_) {
#line 470 "gsmmap.cnf"
	guint8 pdp_type_org;
	tvbuff_t	*parameter_tvb;


  offset = dissect_ber_octet_string(implicit_tag, pinfo, tree, tvb, offset, hf_index,
                                       &parameter_tvb);


 if (!parameter_tvb)
	return offset;
	proto_tree_add_item(tree, hf_gsm_map_pdp_type_org, parameter_tvb, 0,1,FALSE);
	pdp_type_org = tvb_get_guint8(parameter_tvb,1);
	switch (pdp_type_org){
		case 0: /
			proto_tree_add_item(tree, hf_gsm_map_etsi_pdp_type_number, parameter_tvb, 0,1,FALSE);
			break;
		case 1: /
			proto_tree_add_item(tree, hf_gsm_map_ietf_pdp_type_number, parameter_tvb, 0,1,FALSE);
			break;
		default:
		break;
	}



  return offset;
}
