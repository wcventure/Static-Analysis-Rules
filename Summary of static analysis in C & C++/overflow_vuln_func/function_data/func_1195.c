static int
dissect_ansi_tcap_TransactionID_U(gboolean implicit_tag _U_, tvbuff_t *tvb _U_, int offset _U_, asn1_ctx_t *actx _U_, proto_tree *tree _U_, int hf_index _U_) {
#line 115 "ansi_tcap.cnf"

tvbuff_t *next_tvb;
guint8 len;

  offset = dissect_ber_octet_string(implicit_tag, actx, tree, tvb, offset, hf_index,
                                       &next_tvb);


if(next_tvb) {
	if(tvb_length(next_tvb) !=0)
		ansi_tcap_private.TransactionID_str = tvb_bytes_to_str(next_tvb, 0,tvb_length(next_tvb));
	len = tvb_length_remaining(next_tvb, 0);
	switch(len) {
	case 1:
		gp_tcapsrt_info->src_tid=tvb_get_guint8(next_tvb, 0);
		break;
	case 2:
		gp_tcapsrt_info->src_tid=tvb_get_ntohs(next_tvb, 0);
		break;
	case 4:
		gp_tcapsrt_info->src_tid=tvb_get_ntohl(next_tvb, 0);
		break;
	default:
		gp_tcapsrt_info->src_tid=0;
		break;
	}
}



  return offset;
}
