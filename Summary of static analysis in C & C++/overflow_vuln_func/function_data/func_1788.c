static const char *
COM_Parse (const char *data)
{
	int	c;
	int	len;

	len = 0;
	com_token[0] = '\0';
	com_token_start = 0;
	com_token_length = 0;

	if (data == NULL)
		return NULL;

	/
skipwhite:
	while (TRUE) {
		c = *data;
		if (c == '\0')
			return NULL;	/
		if ((c != ' ') && (!g_ascii_iscntrl(c)))
		    break;
		data++;
		com_token_start++;
	}

	/
	if ((c=='/') && (data[1]=='/')) {
		while (*data && *data != '\n'){
			data++;
			com_token_start++;
		}
		goto skipwhite;
	}

	/
	if (c == '\"') {
		data++;
		com_token_start++;
		while (TRUE) {
			c = *data++;
			if ((c=='\"') || (c=='\0')) {
				com_token[len] = '\0';
				return data;
			}
			com_token[len] = c;
			len++;
			com_token_length++;
		}
	}

	/
	do {
		com_token[len] = c;
		data++;
		len++;
		com_token_length++;
		c = *data;
	} while (( c != ' ') && (!g_ascii_iscntrl(c)));

	com_token[len] = '\0';
	return data;
}
