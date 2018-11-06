int
dissect_gsm_map_Ext_QoS_Subscribed(gboolean implicit_tag _U_, tvbuff_t *tvb, int offset, packet_info *pinfo _U_, proto_tree *tree, int hf_index _U_) {
#line 510 "gsmmap.cnf"

	tvbuff_t	*parameter_tvb;

  offset = dissect_ber_octet_string(implicit_tag, pinfo, tree, tvb, offset, hf_index,
                                       &parameter_tvb);


	if (!parameter_tvb)
		return offset;
	dissect_gsm_map_ext_qos_subscribed(tvb, pinfo, tree);



  return offset;
}
