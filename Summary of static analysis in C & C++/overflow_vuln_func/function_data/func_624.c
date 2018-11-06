static gint 
dissect_dsi_reply_get_status(tvbuff_t *tvb, proto_tree *tree, gint offset)
{
        proto_tree      *sub_tree;
	proto_item	*ti;

	guint16 ofs;
	guint16 flag;
	guint16 sign_ofs = 0;
	guint16 adr_ofs = 0;
	guint8	nbe;
	guint8  len;
	guint8  i;
		
	ti = proto_tree_add_text(tree, tvb, offset, -1, "Get Status");
	tree = proto_item_add_subtree(ti, ett_dsi_status);

	ofs = tvb_get_ntohs(tvb, offset +AFPSTATUS_MACHOFF);
	proto_tree_add_text(tree, tvb, offset +AFPSTATUS_MACHOFF, 2, "Machine offset: %d", ofs);

	ofs = tvb_get_ntohs(tvb, offset +AFPSTATUS_VERSOFF);
	proto_tree_add_text(tree, tvb, offset +AFPSTATUS_VERSOFF, 2, "Version offset: %d", ofs);

	ofs = tvb_get_ntohs(tvb, offset +AFPSTATUS_UAMSOFF);
	proto_tree_add_text(tree, tvb, offset +AFPSTATUS_UAMSOFF, 2, "UAMS offset: %d", ofs);

	ofs = tvb_get_ntohs(tvb, offset +AFPSTATUS_ICONOFF);
	proto_tree_add_text(tree, tvb, offset +AFPSTATUS_ICONOFF, 2, "Icon offset: %d", ofs);

	ofs = offset +AFPSTATUS_FLAGOFF;
	ti = proto_tree_add_item(tree, hf_dsi_server_flag, tvb, ofs, 2, FALSE);
	sub_tree = proto_item_add_subtree(ti, ett_dsi_status_server_flag);
	proto_tree_add_item(sub_tree, hf_dsi_server_flag_copyfile      , tvb, ofs, 2, FALSE);
	proto_tree_add_item(sub_tree, hf_dsi_server_flag_passwd        , tvb, ofs, 2, FALSE);
	proto_tree_add_item(sub_tree, hf_dsi_server_flag_no_save_passwd, tvb, ofs, 2, FALSE);
	proto_tree_add_item(sub_tree, hf_dsi_server_flag_srv_msg       , tvb, ofs, 2, FALSE);
	proto_tree_add_item(sub_tree, hf_dsi_server_flag_srv_sig       , tvb, ofs, 2, FALSE);
	proto_tree_add_item(sub_tree, hf_dsi_server_flag_tcpip         , tvb, ofs, 2, FALSE);
	proto_tree_add_item(sub_tree, hf_dsi_server_flag_notify        , tvb, ofs, 2, FALSE);
	proto_tree_add_item(sub_tree, hf_dsi_server_flag_fast_copy     , tvb, ofs, 2, FALSE);

	proto_tree_add_item(tree, hf_dsi_server_name, tvb, offset +AFPSTATUS_PRELEN, 1, FALSE);

	/
	flag = tvb_get_ntohs(tvb, ofs);
	if ((flag & AFPSRVRINFO_SRVSIGNATURE)) {
		ofs = offset +AFPSTATUS_PRELEN +tvb_get_guint8(tvb, offset +AFPSTATUS_PRELEN);
		if ((ofs & 1))
			ofs++;

		sign_ofs = tvb_get_ntohs(tvb, ofs);
		proto_tree_add_text(tree, tvb, ofs, 2, "Signature offset: %d", sign_ofs);
		sign_ofs += offset;

		if ((flag & AFPSRVRINFO_TCPIP)) {
			ofs += 2;
			adr_ofs =  tvb_get_ntohs(tvb, ofs);
			proto_tree_add_text(tree, tvb, ofs, 2, "Network address offset: %d", adr_ofs);
			adr_ofs += offset;
		}
	}
		
	ofs = offset +tvb_get_ntohs(tvb, offset +AFPSTATUS_MACHOFF);
	if (ofs)
		proto_tree_add_item(tree, hf_dsi_server_type, tvb, ofs, 1, FALSE);

	ofs = offset +tvb_get_ntohs(tvb, offset +AFPSTATUS_VERSOFF);
	if (ofs) {
		nbe = tvb_get_guint8(tvb, ofs);
		ti = proto_tree_add_text(tree, tvb, ofs, 1, "Version list: %d", nbe);
		ofs++;
		sub_tree = proto_item_add_subtree(ti, ett_dsi_vers);
		for (i = 0; i < nbe; i++) {
			len = tvb_get_guint8(tvb, ofs) +1;
			proto_tree_add_item(sub_tree, hf_dsi_server_vers, tvb, ofs, 1, FALSE);
			ofs += len;
		}
	}
	
	ofs = offset +tvb_get_ntohs(tvb, offset +AFPSTATUS_UAMSOFF);
	if (ofs) {
		nbe = tvb_get_guint8(tvb, ofs);
		ti = proto_tree_add_text(tree, tvb, ofs, 1, "UAMS list: %d", nbe);
		ofs++;
		sub_tree = proto_item_add_subtree(ti, ett_dsi_uams);
		for (i = 0; i < nbe; i++) {
			len = tvb_get_guint8(tvb, ofs) +1;
			proto_tree_add_item(sub_tree, hf_dsi_server_uams, tvb, ofs, 1, FALSE);
			ofs += len;
		}
	}

	ofs = offset +tvb_get_ntohs(tvb, offset +AFPSTATUS_ICONOFF);
	if (ofs)
		proto_tree_add_item(tree, hf_dsi_server_icon, tvb, ofs, 256, FALSE);

	if (sign_ofs) {
		proto_tree_add_item(tree, hf_dsi_server_signature, tvb, sign_ofs, 16, FALSE);
	}

	if (adr_ofs) {
		ofs = adr_ofs;
		nbe = tvb_get_guint8(tvb, ofs);
		ti = proto_tree_add_text(tree, tvb, ofs, 1, "Address list: %d", nbe);
		ofs++;
		sub_tree = proto_item_add_subtree(ti, ett_dsi_addr);
		for (i = 0; i < nbe; i++) {
			len = tvb_get_guint8(tvb, ofs) -2;
			proto_tree_add_item(sub_tree, hf_dsi_server_addr_len, tvb, ofs, 1, FALSE);
			ofs++;
			proto_tree_add_item(sub_tree, hf_dsi_server_addr_type, tvb, ofs, 1, FALSE); 
			ofs++;
			proto_tree_add_item(sub_tree, hf_dsi_server_addr_value,tvb, ofs, len, FALSE);
			ofs += len;
		}
	}
	return offset;
}
