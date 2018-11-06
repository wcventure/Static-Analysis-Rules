static int dissect_gvcp(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, void *data _U_)
{
	gint offset = 0;
	proto_tree *gvcp_tree = NULL;
	proto_tree *gvcp_tree_flag = NULL;
	proto_tree *gvcp_telegram_tree = NULL;
	gint data_length = 0;
	gint command = -1;
	const gchar* command_string = NULL;
	gint status = 0;
	gint flags = -1;
	gint extendedblockids = -1;
	gint scheduledactioncommand = -1;
	gint ack_code = -1;
	const gchar* ack_string = NULL;
	gint request_id = 0;
	gchar key_code = 0;
	proto_item *ti = NULL;
	proto_item *item = NULL;
	conversation_t *conversation = 0;
	gvcp_conv_info_t *gvcp_info = 0;
	gvcp_transaction_t *gvcp_trans = 0;

	if (tvb_captured_length(tvb) <  GVCP_MIN_PACKET_SIZE)
	{
		return 0;
	}

	/
	key_code = (gchar) tvb_get_guint8(tvb, offset);
	ack_code = tvb_get_ntohs(tvb, offset+2);
	ack_string = try_val_to_str(ack_code, acknowledgenames);

	if ((key_code != 0x42) && !ack_string)
	{
		return 0;
	}

	/
	col_set_str(pinfo->cinfo, COL_PROTOCOL, "GVCP");

	/
	col_clear(pinfo->cinfo, COL_INFO);

	/
	/
	ti = proto_tree_add_item(tree, proto_gvcp, tvb, offset, -1, ENC_NA);
	gvcp_tree = proto_item_add_subtree(ti, ett_gvcp);

	/
	if (key_code == 0x42)
	{
		command = tvb_get_ntohs(tvb, offset+2);
		command_string = val_to_str(command, commandnames,"Unknown Command (0x%x)");

		/
		col_append_fstr(pinfo->cinfo, COL_INFO, "> %s ", command_string);

		item = proto_tree_add_text(gvcp_tree, tvb, offset, 8, "Command Header: %s", command_string);
		gvcp_tree = proto_item_add_subtree(item, ett_gvcp_cmd);

		/
		proto_tree_add_item(gvcp_tree, hf_gvcp_message_key_code, tvb, offset, 1, ENC_BIG_ENDIAN);
		offset++;

		/
		flags = (gchar) tvb_get_guint8(tvb, offset + 1);
		item = proto_tree_add_item(gvcp_tree, hf_gvcp_flag, tvb, offset, 1, ENC_BIG_ENDIAN);
		gvcp_tree_flag  = proto_item_add_subtree(item, ett_gvcp_flags);
		if (command == GVCP_ACTION_CMD)
		{
			proto_tree_add_item(gvcp_tree_flag, hf_gvcp_scheduledactioncommand_flag_v2_0, tvb, offset, 1, ENC_BIG_ENDIAN);
			scheduledactioncommand = (flags & 0x80);
		}
		if ((command == GVCP_EVENTDATA_CMD) ||
			(command == GVCP_EVENT_CMD) ||
			(command == GVCP_PACKETRESEND_CMD))
		{
			proto_tree_add_item(gvcp_tree_flag, hf_gvcp_64bitid_flag_v2_0, tvb, offset, 1, ENC_BIG_ENDIAN);
			extendedblockids = (flags & 0x10);
		}
		if ((command == GVCP_DISCOVERY_CMD) ||
			(command == GVCP_FORCEIP_CMD))
		{
			proto_tree_add_item(gvcp_tree_flag, hf_gvcp_allow_broadcast_acknowledge_flag, tvb, offset, 1, ENC_BIG_ENDIAN);
		}
		proto_tree_add_item(gvcp_tree_flag, hf_gvcp_acknowledge_required_flag, tvb, offset, 1, ENC_BIG_ENDIAN);

		offset++;

		/
		proto_tree_add_item(gvcp_tree, hf_gvcp_command, tvb, offset, 2, ENC_BIG_ENDIAN);
		offset += 2;
	}
	else /
	{
		status = tvb_get_ntohs(tvb, offset);
		col_append_fstr(pinfo->cinfo, COL_INFO, "< %s %s",
			ack_string, val_to_str(status, statusnames_short, "Unknown status (0x%04X)"));

		item = proto_tree_add_text(gvcp_tree, tvb, offset+2, tvb_captured_length(tvb)-2, "Acknowledge Header: %s", ack_string);
		gvcp_tree = proto_item_add_subtree(item, ett_gvcp_ack);

		/
		proto_tree_add_item(gvcp_tree, hf_gvcp_status, tvb, offset, 2, ENC_BIG_ENDIAN);
		offset += 2;

		/
		proto_tree_add_item(gvcp_tree, hf_gvcp_acknowledge, tvb, offset, 2, ENC_BIG_ENDIAN);

		offset += 2;
	}

	/
	proto_tree_add_item(gvcp_tree, hf_gvcp_length, tvb, offset, 2, ENC_BIG_ENDIAN);
	data_length = tvb_get_ntohs(tvb, offset);
	offset += 2;

	/
	proto_tree_add_item(gvcp_tree, hf_gvcp_request_id, tvb,    offset, 2, ENC_BIG_ENDIAN);
	request_id = tvb_get_ntohs(tvb, offset);
	offset += 2;

	conversation = find_or_create_conversation(pinfo);

	gvcp_info = (gvcp_conv_info_t*)conversation_get_proto_data(conversation, proto_gvcp);
	if (!gvcp_info)
	{
		gvcp_info = (gvcp_conv_info_t*)wmem_alloc(wmem_file_scope(), sizeof(gvcp_conv_info_t));
		gvcp_info->pdus = wmem_map_new(wmem_file_scope(), g_direct_hash, g_direct_equal);
		conversation_add_proto_data(conversation, proto_gvcp, gvcp_info);
	}

	if (!pinfo->fd->flags.visited)
	{
		if (key_code == 0x42)
		{
			/
			gvcp_trans = (gvcp_transaction_t*)wmem_alloc(wmem_packet_scope(), sizeof(gvcp_transaction_t));
			gvcp_trans->req_frame = pinfo->fd->num;
			gvcp_trans->rep_frame = 0;
			gvcp_trans->addr_list = 0;
			gvcp_trans->addr_count = 0;
		}
		else
		{
			if (ack_string)
			{
				/
				/
				gvcp_trans_array = (wmem_array_t*)wmem_map_lookup(gvcp_info->pdus, GUINT_TO_POINTER(request_id));
				if (gvcp_trans_array)
				{
					gint i;
					guint array_size = wmem_array_get_count(gvcp_trans_array);
					for (i = array_size-1; i >= 0; i--)
					{
						gvcp_trans = (gvcp_transaction_t*)wmem_array_index(gvcp_trans_array, i);

						if (gvcp_trans && (gvcp_trans->req_frame < pinfo->fd->num))
						{
							if (gvcp_trans->rep_frame != 0)
							{
								gvcp_trans = 0;
							}
							else
							{
								gvcp_trans->rep_frame = pinfo->fd->num;
							}

							break;
						}
						gvcp_trans = 0;
					}
				}
			}
		}
	}
	else
	{
		gvcp_trans = 0;
		gvcp_trans_array = (wmem_array_t*)wmem_map_lookup(gvcp_info->pdus, GUINT_TO_POINTER(request_id));

		if (gvcp_trans_array)
		{
			guint i;
			guint array_size = wmem_array_get_count(gvcp_trans_array);

			for (i = 0; i < array_size; ++i)
			{
				gvcp_trans = (gvcp_transaction_t*)wmem_array_index(gvcp_trans_array, i);
				if (gvcp_trans && (pinfo->fd->num == gvcp_trans->req_frame || pinfo->fd->num == gvcp_trans->rep_frame))
				{
					break;
				}

				gvcp_trans = 0;
			}
		}
	}

	if (!gvcp_trans)
	{
		gvcp_trans = (gvcp_transaction_t*)wmem_alloc(wmem_packet_scope(), sizeof(gvcp_transaction_t));
		gvcp_trans->req_frame = 0;
		gvcp_trans->rep_frame = 0;
		gvcp_trans->addr_list = 0;
		gvcp_trans->addr_count = 0;
	}

	/
	gvcp_telegram_tree = proto_item_add_subtree(gvcp_tree, ett_gvcp);

	/
	if (key_code == 0x42)
	{
		if (gvcp_telegram_tree != NULL)
		{
			if (gvcp_trans->rep_frame)
			{
				item = proto_tree_add_uint(gvcp_telegram_tree, hf_gvcp_response_in, tvb, 0, 0, gvcp_trans->rep_frame);
				PROTO_ITEM_SET_GENERATED(item);
			}
		}

		switch (command)
		{
		case GVCP_FORCEIP_CMD:
			dissect_forceip_cmd(gvcp_telegram_tree, tvb, pinfo, offset, data_length);
			break;

		case GVCP_PACKETRESEND_CMD:
			dissect_packetresend_cmd(gvcp_telegram_tree, tvb, pinfo, offset, data_length, extendedblockids);
			break;

		case GVCP_READREG_CMD:
			dissect_readreg_cmd(gvcp_telegram_tree, tvb, pinfo, offset, data_length, gvcp_trans);
			break;

		case GVCP_WRITEREG_CMD:
			dissect_writereg_cmd(gvcp_telegram_tree, tvb, pinfo, offset, data_length, gvcp_trans);
			break;

		case GVCP_READMEM_CMD:
			dissect_readmem_cmd(gvcp_telegram_tree, tvb, pinfo, offset);
			break;

		case GVCP_WRITEMEM_CMD:
			dissect_writemem_cmd(gvcp_telegram_tree, tvb, pinfo, offset, data_length, gvcp_trans);
			break;

		case GVCP_EVENT_CMD:
			dissect_event_cmd(gvcp_telegram_tree, tvb, pinfo, offset, data_length, extendedblockids);
			break;

		case GVCP_EVENTDATA_CMD:
			dissect_eventdata_cmd(gvcp_telegram_tree, tvb, pinfo, offset, extendedblockids);
			break;

		case GVCP_ACTION_CMD:
			dissect_action_cmd(gvcp_telegram_tree, tvb, pinfo, offset, scheduledactioncommand);
			break;

		case GVCP_DISCOVERY_CMD:
		default:
			break;
		}

		if (!pinfo->fd->flags.visited)
		{
			if (key_code == 0x42)
			{
				gvcp_trans_array = (wmem_array_t*)wmem_map_lookup(gvcp_info->pdus, GUINT_TO_POINTER(request_id));

				if(gvcp_trans_array)
				{
					wmem_array_append_one(gvcp_trans_array, gvcp_trans);
				}
				else
				{
					gvcp_trans_array = wmem_array_new(wmem_file_scope(), sizeof(gvcp_transaction_t));
					wmem_array_append_one(gvcp_trans_array, *gvcp_trans);
					wmem_map_insert(gvcp_info->pdus, GUINT_TO_POINTER(request_id), (void *)gvcp_trans_array);
				}
			}
		}
	}
	else
	{
		if (gvcp_telegram_tree != NULL)
		{
			if (gvcp_trans->req_frame)
			{
				item = proto_tree_add_uint(gvcp_telegram_tree, hf_gvcp_response_to, tvb, 0, 0, gvcp_trans->req_frame);
				PROTO_ITEM_SET_GENERATED(item);
			}
		}

		switch (ack_code)
		{
		case GVCP_DISCOVERY_ACK:
			dissect_discovery_ack(gvcp_telegram_tree, tvb, pinfo, offset, data_length);
			break;

		case GVCP_READREG_ACK:
			dissect_readreg_ack(gvcp_telegram_tree, tvb, pinfo, offset, data_length, gvcp_trans);
			break;

		case GVCP_WRITEREG_ACK:
			dissect_writereg_ack(gvcp_telegram_tree, tvb, pinfo, offset, gvcp_trans);
			break;

		case GVCP_READMEM_ACK:
			dissect_readmem_ack(gvcp_telegram_tree, tvb, pinfo, offset, data_length);
			break;

		case GVCP_WRITEMEM_ACK:
			dissect_writemem_ack(gvcp_telegram_tree, tvb, pinfo, offset, data_length, gvcp_trans);
			break;

		case GVCP_PENDING_ACK:
			dissect_pending_ack(gvcp_telegram_tree, tvb, pinfo, offset, data_length);
			break;

		case GVCP_FORCEIP_ACK:
			break;
		case GVCP_PACKETRESEND_ACK:
		case GVCP_EVENT_ACK:
		case GVCP_EVENTDATA_ACK:
		case GVCP_ACTION_ACK:
		default:
			proto_tree_add_item(gvcp_telegram_tree, hf_gvcp_payloaddata, tvb, offset, data_length, ENC_NA);
			break;
		}
	}
	return tvb_captured_length(tvb);
}
