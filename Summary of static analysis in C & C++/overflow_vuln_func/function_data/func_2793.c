static void readline_hist_add(ReadLineState *rs, const char *cmdline)
{
    char *hist_entry, *new_entry;
    int idx;

    if (cmdline[0] == '\0')
	return;
    new_entry = NULL;
    if (rs->hist_entry != -1) {
	/
	hist_entry = rs->history[rs->hist_entry];
	idx = rs->hist_entry;
	if (strcmp(hist_entry, cmdline) == 0) {
	    goto same_entry;
	}
    }
    /
    for (idx = 0; idx < READLINE_MAX_CMDS; idx++) {
	hist_entry = rs->history[idx];
	if (hist_entry == NULL)
	    break;
	if (strcmp(hist_entry, cmdline) == 0) {
	same_entry:
	    new_entry = hist_entry;
	    /
	    memmove(&rs->history[idx], &rs->history[idx + 1],
		    (READLINE_MAX_CMDS - idx + 1) * sizeof(char *));
	    rs->history[READLINE_MAX_CMDS - 1] = NULL;
	    for (; idx < READLINE_MAX_CMDS; idx++) {
		if (rs->history[idx] == NULL)
		    break;
	    }
	    break;
	}
    }
    if (idx == READLINE_MAX_CMDS) {
	/
	free(rs->history[0]);
	memcpy(rs->history, &rs->history[1],
	       (READLINE_MAX_CMDS - 1) * sizeof(char *));
	rs->history[READLINE_MAX_CMDS - 1] = NULL;
	idx = READLINE_MAX_CMDS - 1;
    }
    if (new_entry == NULL)
	new_entry = strdup(cmdline);
    rs->history[idx] = new_entry;
    rs->hist_entry = -1;
}
