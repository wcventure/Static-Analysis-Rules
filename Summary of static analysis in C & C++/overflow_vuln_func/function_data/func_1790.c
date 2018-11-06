static void
dissect_quakeworld_ConnectionlessPacket(tvbuff_t *tvb, packet_info *pinfo,
	proto_tree *tree, int direction)
{
	proto_tree	*cl_tree;
	proto_tree	*text_tree = NULL;
	proto_item	*pi = NULL;
	guint8		*text;
	int		len;
	int		offset;
	guint32		marker;
	int		command_len;
	const char	*command = "";
	gboolean	command_finished = FALSE;

	marker = tvb_get_ntohl(tvb, 0);
	cl_tree = proto_tree_add_subtree(tree, tvb, 0, -1, ett_quakeworld_connectionless, NULL, "Connectionless");

	proto_tree_add_uint(cl_tree, hf_quakeworld_connectionless_marker,
				tvb, 0, 4, marker);

	/
	offset = 4;

	text = tvb_get_stringz_enc(wmem_packet_scope(), tvb, offset, &len, ENC_ASCII|ENC_NA);
	/

	if (cl_tree) {
		proto_item *text_item;
		text_item = proto_tree_add_string(cl_tree, hf_quakeworld_connectionless_text,
						  tvb, offset, len, text);
		text_tree = proto_item_add_subtree(text_item, ett_quakeworld_connectionless_text);
	}

	if (direction == DIR_C2S) {
		/
		const char *c;

		Cmd_TokenizeString(text);
		c = Cmd_Argv(0);

		/
		if (strcmp(c,"ping") == 0) {
			command = "Ping";
			command_len = 4;
		} else if (strcmp(c,"status") == 0) {
			command = "Status";
			command_len = 6;
		} else if (strcmp(c,"log") == 0) {
			command = "Log";
			command_len = 3;
		} else if (strcmp(c,"connect") == 0) {
			guint32 version = 0;
			guint16 qport = 0;
			guint32 challenge = 0;
			gboolean version_valid = TRUE;
			gboolean qport_valid = TRUE;
			gboolean challenge_valid = TRUE;
			const char *infostring;
			proto_tree *argument_tree = NULL;
			command = "Connect";
			command_len = Cmd_Argv_length(0);
			if (text_tree) {
				proto_item *argument_item;
				pi = proto_tree_add_string(text_tree, hf_quakeworld_connectionless_command,
					tvb, offset, command_len, command);
				argument_item = proto_tree_add_string(text_tree,
					hf_quakeworld_connectionless_arguments,
					tvb, offset + Cmd_Argv_start(1), len + 1 - Cmd_Argv_start(1),
					text + Cmd_Argv_start(1));
				argument_tree = proto_item_add_subtree(argument_item,
								       ett_quakeworld_connectionless_arguments);
				command_finished=TRUE;
			}
			version_valid = ws_strtou32(Cmd_Argv(1), NULL, &version);
			qport_valid = ws_strtou16(Cmd_Argv(2), NULL, &qport);
			challenge_valid = ws_strtou32(Cmd_Argv(3), NULL, &challenge);
			infostring = Cmd_Argv(4);

			if (text_tree && (!version_valid || !qport_valid || !challenge_valid))
				expert_add_info(pinfo, pi, &ei_quakeworld_connectionless_command_invalid);

			if (argument_tree) {
				proto_item *info_item;
				proto_tree *info_tree;
				proto_tree_add_uint(argument_tree,
					hf_quakeworld_connectionless_connect_version,
					tvb,
					offset + Cmd_Argv_start(1),
					Cmd_Argv_length(1), version);
				proto_tree_add_uint(argument_tree,
					hf_quakeworld_connectionless_connect_qport,
					tvb,
					offset + Cmd_Argv_start(2),
					Cmd_Argv_length(2), qport);
				proto_tree_add_int(argument_tree,
					hf_quakeworld_connectionless_connect_challenge,
					tvb,
					offset + Cmd_Argv_start(3),
					Cmd_Argv_length(3), challenge);
				info_item = proto_tree_add_string(argument_tree,
					hf_quakeworld_connectionless_connect_infostring,
					tvb,
					offset + Cmd_Argv_start(4),
					Cmd_Argv_length(4), infostring);
				info_tree = proto_item_add_subtree(
					info_item, ett_quakeworld_connectionless_connect_infostring);
				dissect_id_infostring(tvb, info_tree, offset + Cmd_Argv_start(4),
					wmem_strdup(wmem_packet_scope(), infostring),
					ett_quakeworld_connectionless_connect_infostring_key_value,
					hf_quakeworld_connectionless_connect_infostring_key_value,
					hf_quakeworld_connectionless_connect_infostring_key,
					hf_quakeworld_connectionless_connect_infostring_value);
			}
		} else if (strcmp(c,"getchallenge") == 0) {
			command = "Get Challenge";
			command_len = Cmd_Argv_length(0);
		} else if (strcmp(c,"rcon") == 0) {
			const char* password;
			int i;
			char remaining[MAX_TEXT_SIZE+1];
			proto_tree *argument_tree = NULL;
			command = "Remote Command";
			command_len = Cmd_Argv_length(0);
			if (text_tree) {
				proto_item *argument_item;
				proto_tree_add_string(text_tree, hf_quakeworld_connectionless_command,
					tvb, offset, command_len, command);
				argument_item = proto_tree_add_string(text_tree,
					hf_quakeworld_connectionless_arguments,
					tvb, offset + Cmd_Argv_start(1), len - Cmd_Argv_start(1),
					text + Cmd_Argv_start(1));
				argument_tree =	proto_item_add_subtree(argument_item,
								       ett_quakeworld_connectionless_arguments);
				command_finished=TRUE;
			}
			password = Cmd_Argv(1);
			if (argument_tree) {
				proto_tree_add_string(argument_tree,
					hf_quakeworld_connectionless_rcon_password,
					tvb,
					offset + Cmd_Argv_start(1),
					Cmd_Argv_length(1), password);
			}
			remaining[0] = '\0';
			for (i=2; i<Cmd_Argc() ; i++) {
				g_strlcat (remaining, Cmd_Argv(i), MAX_TEXT_SIZE+1);
				g_strlcat (remaining, " ", MAX_TEXT_SIZE+1);
			}
			if (text_tree) {
				proto_tree_add_string(argument_tree,
					hf_quakeworld_connectionless_rcon_command,
					tvb, offset + Cmd_Argv_start(2),
					Cmd_Argv_start(Cmd_Argc()-1) + Cmd_Argv_length(Cmd_Argc()-1) -
					Cmd_Argv_start(2),
					remaining);
			}
		} else if (c[0]==A2A_PING && ( c[1]=='\0' || c[1]=='\n')) {
			command = "Ping";
			command_len = 1;
		} else if (c[0]==A2A_ACK && ( c[1]=='\0' || c[1]=='\n')) {
			command = "Ack";
			command_len = 1;
		} else {
			command = "Unknown";
			command_len = len - 1;
		}
	}
	else {
		/
		if (text[0] == S2C_CONNECTION) {
			command = "Connected";
			command_len = 1;
		} else if (text[0] == A2C_CLIENT_COMMAND) {
			command = "Client Command";
			command_len = 1;
			/
		} else if (text[0] == A2C_PRINT) {
			command = "Print";
			command_len = 1;
			/
		} else if (text[0] == A2A_PING) {
			command = "Ping";
			command_len = 1;
		} else if (text[0] == S2C_CHALLENGE) {
			command = "Challenge";
			command_len = 1;
			/
		} else {
			command = "Unknown";
			command_len = len - 1;
		}
	}

	col_append_fstr(pinfo->cinfo, COL_INFO, " %s", command);

	if (!command_finished) {
		proto_tree_add_string(text_tree, hf_quakeworld_connectionless_command,
			tvb, offset, command_len, command);
	}
	/
}
