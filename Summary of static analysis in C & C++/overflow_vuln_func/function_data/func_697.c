static void
dissect_lwres(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree)
{
	guint16 version, flags, authtype, authlength ;
	guint32 length, opcode, result, recvlength, serial;
	guint32 message_type;

	proto_item* lwres_item;
	proto_tree* lwres_tree;

	col_set_str(pinfo->cinfo, COL_PROTOCOL, "lw_res");
	length = tvb_get_ntohl(tvb, LW_LENGTH_OFFSET);
	version = tvb_get_ntohs(tvb, LW_VERSION_OFFSET);
	flags = tvb_get_ntohs(tvb, LW_PKTFLASG_OFFSET);
	serial = tvb_get_ntohl(tvb, LW_SERIAL_OFFSET);
	opcode = tvb_get_ntohl(tvb,LW_OPCODE_OFFSET);
	result = tvb_get_ntohl(tvb, LW_RESULT_OFFSET);
	recvlength = tvb_get_ntohl(tvb, LW_RECVLEN_OFFSET);
	authtype = tvb_get_ntohs(tvb, LW_AUTHTYPE_OFFSET);
	authlength = tvb_get_ntohs(tvb, LW_AUTHLEN_OFFSET);

	message_type = (flags & LWRES_LWPACKETFLAG_RESPONSE) ? 2 : 1;

	if (check_col(pinfo->cinfo, COL_INFO)) {
       		col_clear(pinfo->cinfo, COL_INFO);

		if(flags & LWRES_LWPACKETFLAG_RESPONSE)
		{
       			col_add_fstr(pinfo->cinfo, COL_INFO,
               		"%s, opcode=%s, serial=0x%x, result=%s",
               			val_to_str((guint32)message_type,message_types_values,"unknown"),
               			val_to_str(opcode, opcode_values, "unknown"),
						serial,
						val_to_str(result,result_values,"unknown"));
       		}
		else
		{
       			col_add_fstr(pinfo->cinfo, COL_INFO,
               			"%s, opcode=%s, serial=0x%x",
               			val_to_str((guint32)message_type,message_types_values,"unknown"),
						val_to_str(opcode, opcode_values, "unknown"),
				serial);
		}
	}

	if(tree)
	{
		lwres_item = proto_tree_add_item(tree,proto_lwres, tvb,0, -1, FALSE);
		lwres_tree = proto_item_add_subtree(lwres_item, ett_lwres);
	}
	else return;


	proto_tree_add_uint(lwres_tree,
			hf_length,
			tvb,
			LW_LENGTH_OFFSET,
			sizeof(guint32),
			length);


	proto_tree_add_uint(lwres_tree,
				hf_version,
				tvb,
				LW_VERSION_OFFSET,
				sizeof(guint16),
				version);
		



	proto_tree_add_uint(lwres_tree,
				hf_flags,
				tvb,
				LW_PKTFLASG_OFFSET,
				sizeof(guint16),
				flags);

	proto_tree_add_uint(lwres_tree,
				hf_serial,
				tvb,
				LW_SERIAL_OFFSET,
				sizeof(guint32),
				serial);

	proto_tree_add_uint(lwres_tree,
				hf_opcode,
				tvb,
				LW_OPCODE_OFFSET,
				sizeof(guint32),
				opcode);

	proto_tree_add_uint(lwres_tree,
				hf_result,
				tvb,
				LW_RESULT_OFFSET,
				sizeof(guint32),
				result);

	proto_tree_add_uint(lwres_tree,
				hf_recvlen,
				tvb,
				LW_RECVLEN_OFFSET,
				sizeof(guint32),
				recvlength);

	proto_tree_add_uint(lwres_tree,
				hf_authtype,
				tvb,
				LW_AUTHTYPE_OFFSET,
				sizeof(guint16),
				authtype);

	proto_tree_add_uint(lwres_tree,
				hf_authlen,
				tvb,
				LW_AUTHLEN_OFFSET,
				sizeof(guint16),
				authlength);

	if(!result)
	{
		switch(opcode)
		{
			case LWRES_OPCODE_NOOP:
				dissect_noop(tvb, lwres_tree);
			break;

			case LWRES_OPCODE_GETADDRSBYNAME:
				dissect_getaddrsbyname(tvb, lwres_tree, message_type);
			break;

			case LWRES_OPCODE_GETNAMEBYADDR:
				dissect_getnamebyaddr(tvb, lwres_tree, message_type);
			break;

			case LWRES_OPCODE_GETRDATABYNAME:
				dissect_getrdatabyname(tvb, lwres_tree, message_type);
			break;
		}
	}

}
