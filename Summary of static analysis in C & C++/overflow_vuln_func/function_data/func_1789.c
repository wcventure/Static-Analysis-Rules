static void
Cmd_TokenizeString(const char* text)
{
	int start;

	cmd_argc = 0;

	start = 0;
	while (TRUE) {

		/
		while (*text && *text <= ' ' && *text != '\n') {
			text++;
			start++;
		}

		if (*text == '\n') {
			/
			text++;
			break;
		}

		if (!*text)
			return;

		text = COM_Parse (text);
		if (!text)
			return;

		if (cmd_argc < MAX_ARGS) {
			cmd_argv[cmd_argc] = wmem_strdup(wmem_packet_scope(), com_token);
			cmd_argv_start[cmd_argc] = start + com_token_start;
			cmd_argv_length[cmd_argc] = com_token_length;
			cmd_argc++;
		}

		start += com_token_start + com_token_length;
	}
}
