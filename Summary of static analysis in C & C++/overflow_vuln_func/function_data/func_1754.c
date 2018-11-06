int
dissect_gsm_map_QoS_Subscribed(gboolean implicit_tag _U_, tvbuff_t *tvb, int offset, packet_info *pinfo _U_, proto_tree *tree, int hf_index _U_) {
#line 496 "gsmmap.cnf"

	tvbuff_t	*parameter_tvb;

  offset = dissect_ber_octet_string(implicit_tag, pinfo, tree, tvb, offset, hf_index,
                                       &parameter_tvb);


	 if (!parameter_tvb)
		return offset;
	de_sm_qos(parameter_tvb, tree, 0, 3, NULL,0);



  return offset;
}
