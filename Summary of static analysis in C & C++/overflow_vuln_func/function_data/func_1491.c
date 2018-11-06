static int
decode_gtp_ranap_cause(tvbuff_t *tvb, int offset, packet_info *pinfo _U_, proto_tree *tree) {

	guint8		ranap;

	ranap = tvb_get_guint8(tvb, offset+1);

	if(ranap > 0 && ranap <=64)
		proto_tree_add_uint_format(tree, hf_gtp_ranap_cause, tvb, offset, 2, 
			ranap, "%s (Radio Network Layer Cause) : %s (%u)", 
			val_to_str(GTP_EXT_RANAP_CAUSE, gtp_val, "Unknown"), 
			val_to_str(ranap, ranap_cause_type, "Unknown RANAP Cause"), ranap);

	if(ranap > 64 && ranap <=80)
		proto_tree_add_uint_format(tree, hf_gtp_ranap_cause, tvb, offset, 2, 
			ranap, "%s (Transport Layer Cause) : %s (%u)", 
			val_to_str(GTP_EXT_RANAP_CAUSE, gtp_val, "Unknown"), 
			val_to_str(ranap, ranap_cause_type, "Unknown RANAP Cause"), ranap);

	if(ranap > 80 && ranap <=96)
		proto_tree_add_uint_format(tree, hf_gtp_ranap_cause, tvb, offset, 2, 
			ranap, "%s (NAS Cause) : %s (%u)", 
			val_to_str(GTP_EXT_RANAP_CAUSE, gtp_val, "Unknown"), 
			val_to_str(ranap, ranap_cause_type, "Unknown RANAP Cause"), ranap);

	if(ranap > 96 && ranap <=112)
		proto_tree_add_uint_format(tree, hf_gtp_ranap_cause, tvb, offset, 2, ranap, 
			"%s (Protocol Cause) : %s (%u)", 
			val_to_str(GTP_EXT_RANAP_CAUSE, gtp_val, "Unknown"), 
			val_to_str(ranap, ranap_cause_type, "Unknown RANAP Cause"), ranap);

	if(ranap > 112 && ranap <=128)
		proto_tree_add_uint_format(tree, hf_gtp_ranap_cause, tvb, offset, 2, ranap, 
			"%s (Miscellaneous Cause) : %s (%u)", 
			val_to_str(GTP_EXT_RANAP_CAUSE, gtp_val, "Unknown"), 
			val_to_str(ranap, ranap_cause_type, "Unknown RANAP Cause"), ranap);

	if(ranap > 128 /)
		proto_tree_add_uint_format(tree, hf_gtp_ranap_cause, tvb, offset, 2, ranap, 
			"%s (Non-standard Cause) : %s (%u)", 
			val_to_str(GTP_EXT_RANAP_CAUSE, gtp_val, "Unknown"), 
			val_to_str(ranap, ranap_cause_type, "Unknown RANAP Cause"), ranap);

	return 2;
}
