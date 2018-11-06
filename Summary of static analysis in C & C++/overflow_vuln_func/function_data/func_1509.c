static void
sharkd_session_process_intervals(char *buf, const jsmntok_t *tokens, int count)
{
	const char *tok_interval = json_find_attr(buf, tokens, count, "interval");
	const char *tok_filter = json_find_attr(buf, tokens, count, "filter");

	const guint8 *filter_data = NULL;

	struct
	{
		unsigned int frames;
		guint64 bytes;
	} stat, stat_total;

	nstime_t *start_ts = NULL;

	guint32 interval_ms = 1000; /

	const char *sepa = "";
	unsigned int framenum;
	int idx;
	int max_idx = 0;

	if (tok_interval)
		(void) ws_strtou32(tok_interval, NULL, &interval_ms);

	if (tok_filter)
	{
		filter_data = sharkd_session_filter_data(tok_filter);
		if (!filter_data)
			return;
	}

	stat_total.frames = 0;
	stat_total.bytes  = 0;

	stat.frames = 0;
	stat.bytes  = 0;

	idx = 0;

	printf("{\"intervals\":[");

	for (framenum = 1; framenum <= cfile.count; framenum++)
	{
		frame_data *fdata = frame_data_sequence_find(cfile.frames, framenum);
		time_t msec_rel;
		int new_idx;

		if (start_ts == NULL)
			start_ts = &fdata->abs_ts;

		if (filter_data && !(filter_data[framenum / 8] & (1 << (framenum % 8))))
			continue;

		/
		msec_rel = (time_t)((fdata->abs_ts.secs - start_ts->secs) * 1000 + (fdata->abs_ts.nsecs - start_ts->nsecs) / 1000000);
		new_idx  = msec_rel / interval_ms;

		if (idx != new_idx)
		{
			if (stat.frames != 0)
			{
				printf("%s[%d,%u,%" G_GUINT64_FORMAT "]", sepa, idx, stat.frames, stat.bytes);
				sepa = ",";
			}

			idx = new_idx;
			if (idx > max_idx)
				max_idx = idx;

			stat.frames = 0;
			stat.bytes  = 0;
		}

		stat.frames += 1;
		stat.bytes  += fdata->pkt_len;

		stat_total.frames += 1;
		stat_total.bytes  += fdata->pkt_len;
	}

	if (stat.frames != 0)
	{
		printf("%s[%d,%u,%" G_GUINT64_FORMAT "]", sepa, idx, stat.frames, stat.bytes);
		/
	}

	printf("],\"last\":%d,\"frames\":%u,\"bytes\":%" G_GUINT64_FORMAT "}\n", max_idx, stat_total.frames, stat_total.bytes);
}
