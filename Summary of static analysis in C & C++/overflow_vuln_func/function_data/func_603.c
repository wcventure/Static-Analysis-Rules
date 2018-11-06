int
dissect_fmp_flushCmd(tvbuff_t *tvb, int offset,  proto_tree *tree)
{
	guint32 cmd;
	char msg[MAX_MSG_SIZE];
	char *msgIndex;
	guint32 bitValue;
	int i;

	if (!tree) {
		return offset;
	}

	cmd = tvb_get_ntohl(tvb, offset);

	/
	msgIndex = msg;
	strcpy(msgIndex, "No command specified");

	for (i = 0; cmd != 0 && i < 32; i++) {

		bitValue = 1 << i;

		if (cmd & bitValue) {
			switch (bitValue) {
			case FMP_COMMIT_SPECIFIED:
				strcpy(msgIndex, "COMMIT_SPECIFIED");
				msgIndex += strlen("COMMIT_SPECIFIED");
				break;
			case FMP_RELEASE_SPECIFIED:
				strcpy(msgIndex, "RELEASE_SPECIFIED");
				msgIndex += strlen("RELEASE_SPECIFIED");
				break;
			case FMP_RELEASE_ALL:
				strcpy(msgIndex, "RELEASE_ALL");
				msgIndex += strlen("RELEASE_ALL");
				break;
			case FMP_CLOSE_FILE:
				strcpy(msgIndex, "CLOSE_FILE");
				msgIndex += strlen("CLOSE_FILE");
				break;
			case FMP_UPDATE_TIME:
				strcpy(msgIndex, "UPDATE_TIME");
				msgIndex += strlen("UPDATE_TIME");
				break;
			case FMP_ACCESS_TIME:
				strcpy(msgIndex, "ACCESS_TIME");
				msgIndex += strlen("ACCESS_TIME");
				break;
			default:
				strcpy(msgIndex, "UNKNOWN");
				msgIndex += strlen("UNKNOWN");
				break;
			}

			/
			cmd &= ~bitValue;
	
			/
			if (cmd) {
				strcpy(msgIndex, " | ");
				msgIndex += strlen(" | ");
			}
		}
	}

	proto_tree_add_text(tree, tvb, offset, 4, "Cmd: %s", msg);
	offset += 4;
	return offset;
}
