static void
dissect_quakeworld_ConnectionlessPacket(tvbuff_t *tvb, packet_info *pinfo,
	proto_tree *tree, int direction)
{
	proto_tree	*cl_tree = NULL;
	proto_item	*cl_item = NULL;
	proto_item	*text_item = NULL;
	proto_tree	*text_tree = NULL;
	guint8		text[2048];
	int		maxbufsize = 0;
	int		len;
	int		offset;
	guint32 marker;
	int command_len;
	char command[2048];
	int command_finished = FALSE;

	marker = tvb_get_ntohl(tvb, 0);
	if (tree) {
		cl_item = proto_tree_add_text(tree, tvb,
				0, -1, "Connectionless");
		if (cl_item)
			cl_tree = proto_item_add_subtree(
				cl_item, ett_quakeworld_connectionless);
	}

	if (cl_tree) {
		proto_tree_add_uint(cl_tree, hf_quakeworld_connectionless_marker,
				tvb, 0, 4, marker);
	}

	/
        offset = 4;

        maxbufsize = MIN((gint)sizeof(text), tvb_length_remaining(tvb, offset));
        len = tvb_get_nstringz0(tvb, offset, maxbufsize, text);
	/

        if (cl_tree) {
                text_item = proto_tree_add_string(cl_tree, hf_quakeworld_connectionless_text,
                        tvb, offset, len + 1, text);
		if (text_item) {
			text_tree = proto_item_add_subtree(
				text_item, ett_quakeworld_connectionless_text);
		}
        }

	if (direction == DIR_C2S) {
		/
		char *c;

		Cmd_TokenizeString(text);
		c = Cmd_Argv(0);
		
		/
		if (strcmp(c,"ping") == 0) {
			strcpy(command, "Ping");
			command_len = 4;
		} else if (strcmp(c,"status") == 0) {
			strcpy(command, "Status");
			command_len = 6;
		} else if (strcmp(c,"log") == 0) {
			strcpy(command, "Status");
			command_len = 3;
		} else if (strcmp(c,"connect") == 0) {
			int version;
			int qport;
			int challenge;
			char *infostring;
			proto_item *argument_item = NULL;
			proto_tree *argument_tree = NULL;
			proto_item *info_item = NULL;
			proto_tree *info_tree = NULL;
			strcpy(command, "Connect");
			command_len = Cmd_Argv_length(0);
			if (text_tree) {
				proto_tree_add_string(text_tree, hf_quakeworld_connectionless_command,
					tvb, offset, command_len, command);
				argument_item = proto_tree_add_string(text_tree,
					hf_quakeworld_connectionless_arguments,
					tvb, offset + Cmd_Argv_start(1), len + 1 - Cmd_Argv_start(1), 
					text + Cmd_Argv_start(1));
				if (argument_item) {
					argument_tree =
						proto_item_add_subtree(argument_item,	
							ett_quakeworld_connectionless_arguments);
				}
				command_finished=TRUE;
			}
			version = atoi(Cmd_Argv(1));
			qport = atoi(Cmd_Argv(2));
			challenge = atoi(Cmd_Argv(3));
			infostring = Cmd_Argv(4);
			if (argument_tree) {
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
				if (info_item)
					info_tree = proto_item_add_subtree(
						info_item, ett_quakeworld_connectionless_connect_infostring);
				dissect_id_infostring(tvb, info_tree, offset + Cmd_Argv_start(4),
					infostring,
					ett_quakeworld_connectionless_connect_infostring_key_value,
					hf_quakeworld_connectionless_connect_infostring_key_value,
					hf_quakeworld_connectionless_connect_infostring_key,
					hf_quakeworld_connectionless_connect_infostring_value);
			}
		} else if (strcmp(c,"getchallenge") == 0) {
			strcpy(command, "Get Challenge");
			command_len = Cmd_Argv_length(0);
		} else if (strcmp(c,"rcon") == 0) {
			char* password;
			int i;
			char remaining[1024];
			proto_item *argument_item = NULL;
			proto_tree *argument_tree = NULL;
			strcpy(command, "Remote Command");
			command_len = Cmd_Argv_length(0);
			if (text_tree) {
				proto_tree_add_string(text_tree, hf_quakeworld_connectionless_command,
					tvb, offset, command_len, command);
				argument_item = proto_tree_add_string(text_tree,
					hf_quakeworld_connectionless_arguments,
					tvb, offset + Cmd_Argv_start(1), len + 1 - Cmd_Argv_start(1), 
					text + Cmd_Argv_start(1));
				if (argument_item) {
					argument_tree =
						proto_item_add_subtree(argument_item,	
							ett_quakeworld_connectionless_arguments);
				}
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
			for (i=2; i<Cmd_Argc() ; i++) {
				remaining[0] = 0;
				strcat (remaining, Cmd_Argv(i) );
				strcat (remaining, " ");
			}
			if (text_tree) {
				proto_tree_add_string(argument_tree,
					hf_quakeworld_connectionless_rcon_command,
					tvb, offset + Cmd_Argv_start(2),
					Cmd_Argv_start(Cmd_Argc()-1) + Cmd_Argv_length(Cmd_Argc()-1) -
					Cmd_Argv_start(2),
					remaining);
			}
		} else if (c[0]==A2A_PING && ( c[1]==0 || c[1]=='\n')) {
			strcpy(command, "Ping");
			command_len = 1;
		} else if (c[0]==A2A_ACK && ( c[1]==0 || c[1]=='\n')) {
			strcpy(command, "Ack");
			command_len = 1;
		} else {
			strcpy(command, "Unknown");
			command_len = len;
		}
	}
	else {
		/
		if (text[0] == S2C_CONNECTION) {
			strcpy(command, "Connected");
			command_len = 1;
		} else if (text[0] == A2C_CLIENT_COMMAND) {
			strcpy(command, "Client Command");
			command_len = 1;
			/
		} else if (text[0] == A2C_PRINT) {
			strcpy(command, "Print");
			command_len = 1;
			/
		} else if (text[0] == A2A_PING) {
			strcpy(command, "Ping");
			command_len = 1;
		} else if (text[0] == S2C_CHALLENGE) {
			strcpy(command, "Challenge");
			command_len = 1;
			/
		} else {
			strcpy(command, "Unknown");
			command_len = len;
		}
	}
		
	if (check_col(pinfo->cinfo, COL_INFO)) {
		col_append_fstr(pinfo->cinfo, COL_INFO, " %s", command);
	}
		
	if (text_tree && !command_finished) {
		proto_tree_add_string(text_tree, hf_quakeworld_connectionless_command,
			tvb, offset, command_len, command);
	}
        offset += len + 1;
}
