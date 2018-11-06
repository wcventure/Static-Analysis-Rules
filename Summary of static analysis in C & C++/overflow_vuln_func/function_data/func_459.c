static int
dissect_negprot_response(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, int offset, proto_tree *smb_tree _U_)
{
	smb_info_t *si = pinfo->private_data;
	guint8 wc;
	guint16 dialect;
	const char *dn;
	int dn_len;
	guint16 bc;
	guint16 ekl=0;
	guint32 caps=0;
	gint16 tz;
	struct negprot_dialects *dialects = NULL;
	const char *dialect_name = NULL;

	DISSECTOR_ASSERT(si);

	WORD_COUNT;

	/
	dialect = tvb_get_letohs(tvb, offset);

	if (si->sip && si->sip->extra_info_type==SMB_EI_DIALECTS) {
		dialects = si->sip->extra_info;
		if (dialect <= dialects->num) {
			dialect_name = dialects->name[dialect];
		}
	}
	if (!dialect_name) {
		dialect_name = "unknown";
	}

	switch(wc){
	case 1:
		if(dialect==0xffff){
			proto_tree_add_uint_format(tree, hf_smb_dialect_index,
				tvb, offset, 2, dialect,
				"Selected Index: -1, PC NETWORK PROGRAM 1.0 choosen");
		} else {
			proto_tree_add_uint(tree, hf_smb_dialect_index,
				tvb, offset, 2, dialect);
		}
		break;
	case 13:
		proto_tree_add_uint_format(tree, hf_smb_dialect_index,
			tvb, offset, 2, dialect,
			"Dialect Index: %u, Greater than CORE PROTOCOL and up to LANMAN2.1", dialect);
		break;
	case 17:
		proto_tree_add_uint_format(tree, hf_smb_dialect_index,
			tvb, offset, 2, dialect,
			"Dialect Index: %u: %s", dialect, dialect_name);
		break;
	default:
		tvb_ensure_bytes_exist(tvb, offset, wc*2);
		proto_tree_add_text(tree, tvb, offset, wc*2,
			"Words for unknown response format");
		offset += wc*2;
		goto bytecount;
	}
	offset += 2;

	switch(wc){
	case 13:
		/
		offset = dissect_negprot_security_mode(tvb, tree, offset, wc);

		/
		proto_tree_add_item(tree, hf_smb_max_trans_buf_size,
			tvb, offset, 2, TRUE);
		offset += 2;

		/
		proto_tree_add_item(tree, hf_smb_max_mpx_count,
			tvb, offset, 2, TRUE);
		offset += 2;

		/
		proto_tree_add_item(tree, hf_smb_max_vcs_num,
			tvb, offset, 2, TRUE);
		offset += 2;

		/
		offset = dissect_negprot_rawmode(tvb, tree, offset);

		/
		proto_tree_add_item(tree, hf_smb_session_key,
			tvb, offset, 4, TRUE);
		offset += 4;

		/
		offset = dissect_smb_datetime(tvb, tree, offset, hf_smb_server_date_time, hf_smb_server_smb_date, hf_smb_server_smb_time,
		    TRUE);

		/
		tz = tvb_get_letohs(tvb, offset);
		proto_tree_add_int_format(tree, hf_smb_server_timezone, tvb, offset, 2, tz, "Server Time Zone: %d min from UTC", tz);
		offset += 2;

		/
		ekl = tvb_get_letohs(tvb, offset);
		proto_tree_add_uint(tree, hf_smb_encryption_key_length, tvb, offset, 2, ekl);
		offset += 2;

		/
		proto_tree_add_item(tree, hf_smb_reserved, tvb, offset, 2, TRUE);
		offset += 2;

		break;

	case 17:
		/
		offset = dissect_negprot_security_mode(tvb, tree, offset, wc);

		/
		proto_tree_add_item(tree, hf_smb_max_mpx_count,
			tvb, offset, 2, TRUE);
		offset += 2;

		/
		proto_tree_add_item(tree, hf_smb_max_vcs_num,
			tvb, offset, 2, TRUE);
		offset += 2;

		/
		proto_tree_add_item(tree, hf_smb_max_trans_buf_size,
			tvb, offset, 4, TRUE);
		offset += 4;

		/
		proto_tree_add_item(tree, hf_smb_max_raw_buf_size,
			tvb, offset, 4, TRUE);
		offset += 4;

		/
		proto_tree_add_item(tree, hf_smb_session_key,
			tvb, offset, 4, TRUE);
		offset += 4;

		/
		caps = dissect_negprot_capabilities(tvb, tree, offset);
		offset += 4;

		/
		offset = dissect_nt_64bit_time(tvb, tree, offset,
			       	hf_smb_system_time);

		/
		tz = tvb_get_letohs(tvb, offset);
		proto_tree_add_int_format(tree, hf_smb_server_timezone,
			tvb, offset, 2, tz,
			"Server Time Zone: %d min from UTC", tz);
		offset += 2;

		/
		ekl = tvb_get_guint8(tvb, offset);
		proto_tree_add_uint(tree, hf_smb_encryption_key_length,
			tvb, offset, 1, ekl);
		offset += 1;

		break;
	}

	BYTE_COUNT;

	switch(wc){
	case 13:
		/
		if(ekl){
			CHECK_BYTE_COUNT(ekl);
			proto_tree_add_item(tree, hf_smb_encryption_key, tvb, offset, ekl, TRUE);
			COUNT_BYTES(ekl);
		}

		/
		dn = get_unicode_or_ascii_string(tvb, &offset,
			si->unicode, &dn_len, FALSE, FALSE, &bc);
		if (dn == NULL)
			goto endofcommand;
		proto_tree_add_string(tree, hf_smb_primary_domain, tvb,
			offset, dn_len,dn);
		COUNT_BYTES(dn_len);
		break;

	case 17:
		if(!(caps&SERVER_CAP_EXTENDED_SECURITY)){
			/
			/
			if(ekl){
				CHECK_BYTE_COUNT(ekl);
				proto_tree_add_item(tree, hf_smb_encryption_key,
					tvb, offset, ekl, TRUE);
				COUNT_BYTES(ekl);
			}

			/
			/
			/
			/
			si->unicode = (caps&SERVER_CAP_UNICODE) ||
			  si->unicode;

			dn = get_unicode_or_ascii_string(tvb,
				&offset, si->unicode, &dn_len, TRUE, FALSE,
				&bc);
			if (dn == NULL)
				goto endofcommand;
			proto_tree_add_string(tree, hf_smb_primary_domain,
				tvb, offset, dn_len, dn);
			COUNT_BYTES(dn_len);

			/
			dn = get_unicode_or_ascii_string(tvb,
				&offset, si->unicode, &dn_len, TRUE, FALSE,
				&bc);
			if (dn == NULL)
				goto endofcommand;
			proto_tree_add_string(tree, hf_smb_server,
				tvb, offset, dn_len, dn);
			COUNT_BYTES(dn_len);

		} else {
			proto_item *blob_item;
			guint16 sbloblen;

			/
			/
			CHECK_BYTE_COUNT(16);
			proto_tree_add_item(tree, hf_smb_server_guid,
				tvb, offset, 16, TRUE);
			COUNT_BYTES(16);

			/
			/
			sbloblen=bc;
			if(sbloblen>tvb_length_remaining(tvb, offset)){
				sbloblen=tvb_length_remaining(tvb,offset);
			}
			blob_item = proto_tree_add_item(
				tree, hf_smb_security_blob,
				tvb, offset, sbloblen, TRUE);

			/

			if(bc){
				tvbuff_t *gssapi_tvb;
				proto_tree *gssapi_tree;

				gssapi_tree = proto_item_add_subtree(
					blob_item, ett_smb_secblob);

				/
				gssapi_tvb = tvb_new_subset(
					tvb, offset, sbloblen, bc);

				call_dissector(
					gssapi_handle, gssapi_tvb, pinfo,
					gssapi_tree);

				if (si->ct)
				  si->ct->raw_ntlmssp = 0;

				COUNT_BYTES(bc);
			}
			else {

			  /

			  if (si->ct)
			    si->ct->raw_ntlmssp = 1;

			}
		}
		break;
	}

	END_OF_SMB

	return offset;
}
