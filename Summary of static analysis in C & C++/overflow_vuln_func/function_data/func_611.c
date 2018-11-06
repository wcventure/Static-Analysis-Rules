static gint
dissect_asp_reply_get_status(tvbuff_t *tvb, packet_info *pinfo _U_, proto_tree *tree, gint offset)
{
        proto_tree      *sub_tree;
	proto_item	*ti;

	guint16 ofs;
	guint16 flag;
	guint16 machine_ofs;
	guint16 sign_ofs = 0;
	guint16 adr_ofs = 0;
	guint16 dir_ofs = 0;
	guint16 utf_ofs = 0;
	guint8	nbe;
	guint8  len;
	guint8  i;

	if (!tree)
		return offset;

	ti = proto_tree_add_text(tree, tvb, offset, -1, "Get Status");
	tree = proto_item_add_subtree(ti, ett_asp_status);

	machine_ofs = tvb_get_ntohs(tvb, offset +AFPSTATUS_MACHOFF);
	proto_tree_add_text(tree, tvb, offset +AFPSTATUS_MACHOFF, 2, "Machine offset: %u", machine_ofs);
	if (machine_ofs)
		machine_ofs += offset;

	ofs = tvb_get_ntohs(tvb, offset +AFPSTATUS_VERSOFF);
	proto_tree_add_text(tree, tvb, offset +AFPSTATUS_VERSOFF, 2, "Version offset: %u", ofs);

	ofs = tvb_get_ntohs(tvb, offset +AFPSTATUS_UAMSOFF);
	proto_tree_add_text(tree, tvb, offset +AFPSTATUS_UAMSOFF, 2, "UAMS offset: %u", ofs);

	ofs = tvb_get_ntohs(tvb, offset +AFPSTATUS_ICONOFF);
	proto_tree_add_text(tree, tvb, offset +AFPSTATUS_ICONOFF, 2, "Icon offset: %u", ofs);

	ofs = offset +AFPSTATUS_FLAGOFF;
	ti = proto_tree_add_item(tree, hf_asp_server_flag, tvb, ofs, 2, FALSE);
	sub_tree = proto_item_add_subtree(ti, ett_asp_status_server_flag);
	proto_tree_add_item(sub_tree, hf_asp_server_flag_copyfile      , tvb, ofs, 2, FALSE);
	proto_tree_add_item(sub_tree, hf_asp_server_flag_passwd        , tvb, ofs, 2, FALSE);
	proto_tree_add_item(sub_tree, hf_asp_server_flag_no_save_passwd, tvb, ofs, 2, FALSE);
	proto_tree_add_item(sub_tree, hf_asp_server_flag_srv_msg       , tvb, ofs, 2, FALSE);
	proto_tree_add_item(sub_tree, hf_asp_server_flag_srv_sig       , tvb, ofs, 2, FALSE);
	proto_tree_add_item(sub_tree, hf_asp_server_flag_tcpip         , tvb, ofs, 2, FALSE);
	proto_tree_add_item(sub_tree, hf_asp_server_flag_notify        , tvb, ofs, 2, FALSE);
	proto_tree_add_item(sub_tree, hf_asp_server_flag_reconnect     , tvb, ofs, 2, FALSE);
	proto_tree_add_item(sub_tree, hf_asp_server_flag_directory     , tvb, ofs, 2, FALSE);
	proto_tree_add_item(sub_tree, hf_asp_server_flag_utf8_name     , tvb, ofs, 2, FALSE);
	proto_tree_add_item(sub_tree, hf_asp_server_flag_fast_copy     , tvb, ofs, 2, FALSE);

	proto_tree_add_item(tree, hf_asp_server_name, tvb, offset +AFPSTATUS_PRELEN, 1, FALSE);

	flag = tvb_get_ntohs(tvb, ofs);
	if ((flag & AFPSRVRINFO_SRVSIGNATURE)) {
		ofs = offset +AFPSTATUS_PRELEN +tvb_get_guint8(tvb, offset +AFPSTATUS_PRELEN) +1;
		if ((ofs & 1))
			ofs++;

		sign_ofs = tvb_get_ntohs(tvb, ofs);
		proto_tree_add_text(tree, tvb, ofs, 2, "Signature offset: %u", sign_ofs);
		sign_ofs += offset;
		ofs += 2;

		if ((flag & AFPSRVRINFO_TCPIP) && ofs < machine_ofs ) {
			adr_ofs =  tvb_get_ntohs(tvb, ofs);
			proto_tree_add_text(tree, tvb, ofs, 2, "Network address offset: %u", adr_ofs);
			adr_ofs += offset;
			ofs += 2;
		}

		if ((flag & AFPSRVRINFO_SRVDIRECTORY) && ofs < machine_ofs) {
			dir_ofs =  tvb_get_ntohs(tvb, ofs);
			proto_tree_add_text(tree, tvb, ofs, 2, "Directory services offset: %u", dir_ofs);
			dir_ofs += offset;
			ofs += 2;
		}

		if ((flag & AFPSRVRINFO_SRVUTF8) && ofs < machine_ofs) {
			utf_ofs =  tvb_get_ntohs(tvb, ofs);
			proto_tree_add_text(tree, tvb, ofs, 2, "UTF-8 Server name offset: %u", utf_ofs);
			utf_ofs += offset;
		}
	}

	if (machine_ofs)
		proto_tree_add_item(tree, hf_asp_server_type, tvb, machine_ofs, 1, FALSE);

	ofs = offset +tvb_get_ntohs(tvb, offset +AFPSTATUS_VERSOFF);
	if (ofs) {
		nbe = tvb_get_guint8(tvb, ofs);
		ti = proto_tree_add_text(tree, tvb, ofs, 1, "Version list: %u", nbe);
		ofs++;
		sub_tree = proto_item_add_subtree(ti, ett_asp_vers);
		for (i = 0; i < nbe; i++) {
			len = tvb_get_guint8(tvb, ofs) +1;
			proto_tree_add_item(sub_tree, hf_asp_server_vers, tvb, ofs, 1, FALSE);
			ofs += len;
		}
	}

	ofs = offset +tvb_get_ntohs(tvb, offset +AFPSTATUS_UAMSOFF);
	if (ofs) {
		nbe = tvb_get_guint8(tvb, ofs);
		ti = proto_tree_add_text(tree, tvb, ofs, 1, "UAMS list: %u", nbe);
		ofs++;
		sub_tree = proto_item_add_subtree(ti, ett_asp_uams);
		for (i = 0; i < nbe; i++) {
			len = tvb_get_guint8(tvb, ofs) +1;
			proto_tree_add_item(sub_tree, hf_asp_server_uams, tvb, ofs, 1, FALSE);
			ofs += len;
		}
	}

	ofs = offset +tvb_get_ntohs(tvb, offset +AFPSTATUS_ICONOFF);
	if (ofs)
		proto_tree_add_item(tree, hf_asp_server_icon, tvb, ofs, 256, FALSE);

	if (sign_ofs) {
		proto_tree_add_item(tree, hf_asp_server_signature, tvb, sign_ofs, 16, FALSE);
	}

	if (adr_ofs) {
        	proto_tree *adr_tree;
		char *tmp;
        	const guint8 *ip;
		guint16 net;
		guint8  node;
        	guint16 port;

		ofs = adr_ofs;
		nbe = tvb_get_guint8(tvb, ofs);
		ti = proto_tree_add_text(tree, tvb, ofs, 1, "Address list: %u", nbe);
		ofs++;
		adr_tree = proto_item_add_subtree(ti, ett_asp_addr);
		for (i = 0; i < nbe; i++) {
			guint8 type;

			len = tvb_get_guint8(tvb, ofs);
			type =  tvb_get_guint8(tvb, ofs +1);
			switch (type) {
			case 1:	/
				ip = tvb_get_ptr(tvb, ofs+2, 4);
				ti = proto_tree_add_text(adr_tree, tvb, ofs, len, "ip %s", ip_to_str(ip));
				break;
			case 2: /
				ip = tvb_get_ptr(tvb, ofs+2, 4);
				port = tvb_get_ntohs(tvb, ofs+6);
				ti = proto_tree_add_text(adr_tree, tvb, ofs, len, "ip %s:%u",ip_to_str(ip),port);
				break;
			case 3: /
				net  = tvb_get_ntohs(tvb, ofs+2);
				node = tvb_get_guint8(tvb, ofs +4);
				port = tvb_get_guint8(tvb, ofs +5);
				ti = proto_tree_add_text(adr_tree, tvb, ofs, len, "ddp %u.%u:%u",
					net, node, port);
				break;
			case 5: /
				ip = tvb_get_ptr(tvb, ofs+2, 4);
				port = tvb_get_ntohs(tvb, ofs+6);
				ti = proto_tree_add_text(adr_tree, tvb, ofs, len, "ip (ssh tunnel) %s:%u",ip_to_str(ip),port);
				break;
			case 4: /
				if (len > 2) {
					tmp = g_malloc( len -1);
					tvb_memcpy(tvb, tmp, ofs +2, len -2);
					tmp[len -2] = 0;
					ti = proto_tree_add_text(adr_tree, tvb, ofs, len, "dns %s", tmp);
					g_free(tmp);
					break;
				}
				/
			default:
				ti = proto_tree_add_text(adr_tree, tvb, ofs, len,"Unknow type : %u", type);
				break;
			}
			len -= 2;
			sub_tree = proto_item_add_subtree(ti,ett_asp_addr_line);
			proto_tree_add_item(sub_tree, hf_asp_server_addr_len, tvb, ofs, 1, FALSE);
			ofs++;
			proto_tree_add_item(sub_tree, hf_asp_server_addr_type, tvb, ofs, 1, FALSE);
			ofs++;
			proto_tree_add_item(sub_tree, hf_asp_server_addr_value,tvb, ofs, len, FALSE);
			ofs += len;
		}
	}

	if (dir_ofs) {
		ofs = dir_ofs;
		nbe = tvb_get_guint8(tvb, ofs);
		ti = proto_tree_add_text(tree, tvb, ofs, 1, "Directory services list: %u", nbe);
		ofs++;
		sub_tree = proto_item_add_subtree(ti, ett_asp_directory);
		for (i = 0; i < nbe; i++) {
			len = tvb_get_guint8(tvb, ofs) +1;
			proto_tree_add_item(sub_tree, hf_asp_server_directory, tvb, ofs, 1, FALSE);
			ofs += len;
		}
	}
	if (utf_ofs) {
		ofs = utf_ofs;
		len  = (guint8) tvb_get_ntohs(tvb, ofs);
		proto_tree_add_item(tree, hf_asp_server_utf8_name_len, tvb, ofs, 2, FALSE);
		ofs += 2;		
		proto_tree_add_item(tree, hf_asp_server_utf8_name, tvb, ofs, len, FALSE);
		ofs += len;
	}
	/
	return offset;
}
