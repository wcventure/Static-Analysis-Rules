static void
decode_rtp_stream(rtp_stream_info_t *rsi, gpointer ptr)
{
	GString *key_str = NULL;
	rtp_channel_info_t *rci;
	gboolean first = TRUE;
	GList*  rtp_packet_list;
	rtp_packet_t *rp;

	size_t i;
	gint32 j;
	double rtp_time;
	double rtp_time_prev;
	double arrive_time;
	double arrive_time_prev;
	double start_time;
	double start_rtp_time = 0;
	double diff;
	double pack_period;
	gint32 silence_frames;
	int seq;
#ifdef DEBUG /
	double total_time;
	double mean_delay;
	double variation;
#endif
	char *src_addr, *dst_addr;

	size_t decoded_bytes;
	size_t decoded_bytes_prev;
	int jitter_buff;
	SAMPLE *out_buff = NULL;
	sample_t silence;
	sample_t sample;
	nstime_t sample_delta;
	guint8 status;
	guint32 start_timestamp;
	GHashTable *decoders_hash = NULL;

	guint32 progbar_nextstep;
	int progbar_quantum;
	gfloat progbar_val;
	data_info *info = (data_info *) ptr;

	silence.val = 0;
	silence.status = S_NORMAL;

	/
	if (rsi->decode == FALSE) {
		return;
	}

	/
	jitter_buff = (int) gtk_spin_button_get_value((GtkSpinButton * )jitter_spinner);

	/
	key_str = g_string_new("");
	src_addr = address_to_display(NULL, &(rsi->src_addr));
	dst_addr = address_to_display(NULL, &(rsi->dest_addr));
	g_string_printf(key_str, "%s:%d %s:%d %d %u", src_addr,
		rsi->src_port, dst_addr,
		rsi->dest_port, rsi->call_num, info->current_channel);
	wmem_free(NULL, src_addr);
	wmem_free(NULL, dst_addr);
	/

	/
	if (!rtp_channels_hash) {
		rtp_channels_hash = g_hash_table_new_full( g_str_hash, g_str_equal, rtp_key_destroy, rtp_channel_value_destroy);
	}

	/
	rci = (rtp_channel_info_t *)g_hash_table_lookup( rtp_channels_hash, key_str->str);

	/
	if (rci == NULL) {
		rci = g_new0(rtp_channel_info_t, 1);
		rci->call_num = rsi->call_num;
		rci->start_time_abs = rsi->start_fd->abs_ts;
		rci->stop_time_abs = rsi->start_fd->abs_ts;
		rci->samples = g_array_new (FALSE, FALSE, sizeof(sample_t));
		rci->first_stream = rsi;
		rci->num_packets = rsi->packet_count;
		g_hash_table_insert(rtp_channels_hash, g_strdup(key_str->str), rci);
	} else {
		/
		silence_frames = (gint32)((nstime_to_msec(&rsi->start_fd->abs_ts) - nstime_to_msec(&rci->stop_time_abs)) * sample_rate);
		for (j = 0; j < silence_frames; j++) {
			g_array_append_val(rci->samples, silence);
		}
		rci->num_packets += rsi->packet_count;
	}

	/
	first = TRUE;
	rtp_time_prev = 0;
	decoded_bytes_prev = 0;
	start_time = 0;
	arrive_time_prev = 0;
	pack_period = 0;
#ifdef DEBUG /
	total_time = 0;
	mean_delay = 0;
	variation = 0;
#endif
	seq = 0;
	start_timestamp = 0;
	decoders_hash = rtp_decoder_hash_table_new();

	/

	/
	progbar_nextstep = 0;
	/
	progbar_quantum = total_packets/100;

	status = S_NORMAL;

	rtp_packet_list = g_list_first(rsi->rtp_packet_list);
	while (rtp_packet_list)
	{

		if (progbar_count >= progbar_nextstep) {
			g_assert(total_packets > 0);

			progbar_val = (gfloat) progbar_count / total_packets;

			update_progress_bar(progbar_val);

			progbar_nextstep += progbar_quantum;
		}


		rp = (rtp_packet_t *)rtp_packet_list->data;
		if (first == TRUE) {
/
/
			start_timestamp = rp->info->info_timestamp;
			start_rtp_time = 0;
			rtp_time_prev = start_rtp_time;
			first = FALSE;
			seq = rp->info->info_seq_num - 1;
		}

		decoded_bytes = decode_rtp_packet(rp, &out_buff, decoders_hash, &channels, &sample_rate);
		if (decoded_bytes == 0) {
			seq = rp->info->info_seq_num;
		}

		rtp_time = (double)(rp->info->info_timestamp-start_timestamp)/sample_rate - start_rtp_time;

		if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(cb_use_rtp_timestamp))) {
			arrive_time = rtp_time;
		} else {
			arrive_time = (double)rp->arrive_offset/1000 - start_time;
		}

		if (rp->info->info_seq_num != seq+1){
			rci->out_of_seq++;
			status = S_WRONG_SEQ;
		}
		seq = rp->info->info_seq_num;

		diff = arrive_time - rtp_time;

		if (diff<0) diff = -diff;

#ifdef DEBUG
		total_time = (double)rp->arrive_offset/1000;
		printf("seq = %d arr = %f abs_diff = %f index = %d tim = %f ji=%d jb=%f\n",rp->info->info_seq_num,
			total_time, diff, rci->samples->len, ((double)rci->samples->len / sample_rate - total_time) * 1000, 0,
				(mean_delay + 4 * variation) * 1000);
		fflush(stdout);
#endif
		/
		if ( diff*1000 > jitter_buff && !gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(cb_use_uninterrupted_mode))) {
#ifdef DEBUG
			printf("Packet drop by jitter buffer exceeded\n");
#endif
			rci->drop_by_jitter_buff++;
			status = S_DROP_BY_JITT;

			/
			if ( (rtp_time - rtp_time_prev) > pack_period*2 ){
#ifdef DEBUG
				printf("Resync...\n");
#endif
				silence_frames = (gint32)((arrive_time - arrive_time_prev)*sample_rate - decoded_bytes_prev / sizeof(SAMPLE));
				/
				if (silence_frames > MAX_SILENCE_FRAMES)
					silence_frames = MAX_SILENCE_FRAMES;

				for (j = 0; j < silence_frames; j++) {
					silence.status = status;
					g_array_append_val(rci->samples, silence);

					/
					status = S_NORMAL;
				}

				decoded_bytes_prev = 0;
/
/
				start_timestamp = rp->info->info_timestamp;
				start_rtp_time = 0;
				start_time = (double)rp->arrive_offset/1000;
				rtp_time_prev = 0;
			}
		} else {
			/

			if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(cb_use_uninterrupted_mode))) {
				silence_frames = 0;
			} else {
				silence_frames = (gint32)((rtp_time - rtp_time_prev)*sample_rate - decoded_bytes_prev / sizeof(SAMPLE));
			}

			if (silence_frames != 0) {
				rci->wrong_timestamp++;
				status = S_WRONG_TIMESTAMP;
			}

			/
			if (silence_frames > MAX_SILENCE_FRAMES)
				silence_frames = MAX_SILENCE_FRAMES;

			for (j = 0; j < silence_frames; j++) {
				silence.status = status;
				g_array_append_val(rci->samples, silence);

				/
				status = S_NORMAL;
			}

			if (silence_frames > 0) {
				silence_frames = 0;
			}

			/
			for (i = -silence_frames + ((info->current_channel) ? 1 : 0); i < decoded_bytes / (int) sizeof(SAMPLE); i += channels) {
				if(out_buff)
					sample.val = out_buff[i];
				sample.status = status;
				g_array_append_val(rci->samples, sample);
				status = S_NORMAL;
			}
			rtp_time_prev = rtp_time;
			pack_period = (double) decoded_bytes / sizeof(SAMPLE) / sample_rate;
			decoded_bytes_prev = decoded_bytes;
			arrive_time_prev = arrive_time;
		}

		if (out_buff) {
			g_free(out_buff);
			out_buff = NULL;
		}
		rtp_packet_list = g_list_next (rtp_packet_list);
		progbar_count++;
	}
	rci->max_frame_index = rci->samples->len;
	sample_delta.secs = rci->samples->len / sample_rate;
	sample_delta.nsecs = (rci->samples->len % sample_rate) * 1000000000;
	nstime_sum(&rci->stop_time_abs, &rci->start_time_abs, &sample_delta);

	g_string_free(key_str, TRUE);
	g_hash_table_destroy(decoders_hash);
}
