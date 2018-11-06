static void 
bootp_init_protocol(void)
{
	gchar **optionstrings = NULL;
	gchar **optiondetail = NULL;
	gchar *type = NULL;
	guint i, ii;

	/
	for(i=0; i<BOOTP_OPT_NUM; i++)
	{
		bootp_opt[i].text = default_bootp_opt[i].text;
		bootp_opt[i].ftype = default_bootp_opt[i].ftype;
		bootp_opt[i].data = default_bootp_opt[i].data;
	}

	/
	optionstrings = ep_strsplit(pref_optionstring, ";", -1);
	for (i=0;optionstrings[i]!=NULL;i++)
	{
		/

		/
		optiondetail = ep_strsplit(optionstrings[i], ",",-1);
		/
		for(ii=0;(optiondetail[ii]!=NULL);ii++)
		{
			/
		}
		if (ii < 3) continue;                            /
		ii = atoi(optiondetail[0]);                      /
		if (ii==0 || ii>=BOOTP_OPT_NUM-1) continue;      /
		if (bootp_opt[ii].ftype == special) continue;    /
		bootp_opt[ii].text = se_strdup(optiondetail[1]); /
		type = optiondetail[2];                          /
		/
		if (g_ascii_strcasecmp(type,"string") == 0)
		{
			bootp_opt[ii].ftype = string;
		} else if (g_ascii_strcasecmp(type,"ipv4") == 0)
		{
			bootp_opt[ii].ftype = ipv4;
		} else if (g_ascii_strcasecmp(type,"bytes") == 0)
		{
			bootp_opt[ii].ftype = bytes;
		} else
		{
			bootp_opt[ii].ftype = opaque;
		}
	}
}
