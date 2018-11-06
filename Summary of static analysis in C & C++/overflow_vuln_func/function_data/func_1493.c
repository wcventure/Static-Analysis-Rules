static int
decode_gtp_mm_cntxt(tvbuff_t *tvb, int offset, packet_info *pinfo _U_, proto_tree *tree) {

	guint16		length, quint_len, con_len;
	guint8		cksn, count, sec_mode, len;
	proto_tree	*ext_tree_mm;
	proto_item	*te;
    proto_item  *tf = NULL;
    proto_tree  *tf_tree = NULL;
	tvbuff_t	*l3_tvb;


	te = proto_tree_add_text(tree, tvb, offset, 1, val_to_str(GTP_EXT_MM_CNTXT, gtp_val, "Unknown message"));
	ext_tree_mm = proto_item_add_subtree(te, ett_gtp_mm);

	/
	length = tvb_get_ntohs(tvb, offset+1);
	if (length < 1) return 3;

	/
	cksn = tvb_get_guint8(tvb, offset+3) & 0x07;
	/
	sec_mode = (tvb_get_guint8(tvb, offset+4) >> 6) & 0x03;
	count = (tvb_get_guint8(tvb, offset+4) >> 3) & 0x07;

	proto_tree_add_text(ext_tree_mm, tvb, offset+1, 2, "Length: %x", length);
	if (gtp_version == 0)
		sec_mode = 1;
	

	switch (sec_mode) {
		case 0:				/
			proto_tree_add_item(ext_tree_mm, hf_gtp_cksn_ksi, tvb, offset+3, 1, FALSE);
			proto_tree_add_item(ext_tree_mm, hf_gtp_security_mode, tvb, offset+4, 1, FALSE);
			proto_tree_add_item(ext_tree_mm, hf_gtp_no_of_vectors, tvb, offset+4, 1, FALSE);
			proto_tree_add_item(ext_tree_mm, hf_gtp_cipher_algorithm, tvb, offset+4, 1, FALSE);
			proto_tree_add_text(ext_tree_mm, tvb, offset+5, 16, "Ciphering key CK: %s", tvb_bytes_to_str(tvb, offset+5, 16));
			proto_tree_add_text(ext_tree_mm, tvb, offset+21, 16, "Integrity key IK: %s", tvb_bytes_to_str(tvb, offset+21, 16));
			quint_len = tvb_get_ntohs(tvb, offset+37);
			proto_tree_add_text(ext_tree_mm, tvb, offset+37, 2, "Quintuplets length: 0x%x (%u)", quint_len, quint_len);

			offset = offset + decode_quintuplet(tvb, offset+39, ext_tree_mm, count) + 39;


			break;
		case 1:				/
			proto_tree_add_item(ext_tree_mm, hf_gtp_cksn, tvb, offset+3, 1, FALSE);
			if (gtp_version != 0) 
				proto_tree_add_item(ext_tree_mm, hf_gtp_security_mode, tvb, offset+4, 1, FALSE);

			proto_tree_add_item(ext_tree_mm, hf_gtp_no_of_vectors, tvb, offset+4, 1, FALSE);
			proto_tree_add_item(ext_tree_mm, hf_gtp_cipher_algorithm, tvb, offset+4, 1, FALSE);
			proto_tree_add_text(ext_tree_mm, tvb, offset+5, 8, "Ciphering key Kc: %s", tvb_bytes_to_str(tvb, offset+5, 8));

			offset = offset + decode_triplet(tvb, offset+13, ext_tree_mm, count) + 14;

			break;
		case 2:				/
			proto_tree_add_item(ext_tree_mm, hf_gtp_ksi, tvb, offset+3, 1, FALSE);
			proto_tree_add_item(ext_tree_mm, hf_gtp_security_mode, tvb, offset+4, 1, FALSE);
			proto_tree_add_item(ext_tree_mm, hf_gtp_no_of_vectors, tvb, offset+4, 1, FALSE);
			proto_tree_add_text(ext_tree_mm, tvb, offset+5, 16, "Ciphering key CK: %s", tvb_bytes_to_str(tvb, offset+5, 16));
			proto_tree_add_text(ext_tree_mm, tvb, offset+21, 16, "Integrity key IK: %s", tvb_bytes_to_str(tvb, offset+21, 16));
			quint_len = tvb_get_ntohs(tvb, offset+37);
			proto_tree_add_text(ext_tree_mm, tvb, offset+37, 2, "Quintuplets length: 0x%x (%u)", quint_len, quint_len);

			offset = offset + decode_quintuplet(tvb, offset+39, ext_tree_mm, count) + 39;

			break;
		case 3:				/
			proto_tree_add_item(ext_tree_mm, hf_gtp_cksn, tvb, offset+3, 1, FALSE);
			proto_tree_add_item(ext_tree_mm, hf_gtp_security_mode, tvb, offset+4, 1, FALSE);
			proto_tree_add_item(ext_tree_mm, hf_gtp_no_of_vectors, tvb, offset+4, 1, FALSE);
			proto_tree_add_item(ext_tree_mm, hf_gtp_cipher_algorithm, tvb, offset+4, 1, FALSE);
			proto_tree_add_text(ext_tree_mm, tvb, offset+5, 8, "Ciphering key Kc: %s", tvb_bytes_to_str(tvb, offset+5, 8));
			quint_len = tvb_get_ntohs(tvb, offset+13);
			proto_tree_add_text(ext_tree_mm, tvb, offset+13, 2, "Quintuplets length: 0x%x (%u)", quint_len, quint_len);

			offset = offset + decode_quintuplet(tvb, offset+15, ext_tree_mm, count) + 15;

			break;
		default:
			break;
	}

/
	de_gmm_drx_param(tvb, ext_tree_mm, offset, 2, NULL, 0);
	offset = offset +2;

	len	= tvb_get_guint8(tvb, offset);
    tf = proto_tree_add_text(ext_tree_mm,
    	tvb, offset, len+1,
    	"MS Network Capability");

    tf_tree = proto_item_add_subtree(tf, ett_gtp_net_cap);

	proto_tree_add_text(tf_tree, tvb, offset, 1, "Length of MS network capability contents: %u", len);

	offset++;
/
	de_gmm_ms_net_cap(tvb, tf_tree, offset, len, NULL, 0);
	offset = offset +len;

/

	con_len = tvb_get_ntohs(tvb, offset);
	proto_tree_add_text(ext_tree_mm, tvb, offset, 2, "Container length: %u", con_len);
	offset = offset + 2;

	if (con_len > 0) {
		
		l3_tvb = tvb_new_subset(tvb, offset,con_len, con_len );
		if  (!dissector_try_port(bssap_pdu_type_table,BSSAP_PDU_TYPE_DTAP, l3_tvb, pinfo, ext_tree_mm))
		   		call_dissector(data_handle, l3_tvb, pinfo, ext_tree_mm);
	}

	return 3+length;
}
