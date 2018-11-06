static gboolean is_armagetronad_packet(tvbuff_t * tvb)
{
	gint offset = 0;

	/
	while (tvb_length_remaining(tvb, offset) > 2) {
		guint16 data_len = tvb_get_ntohs(tvb, offset + 4) * 2;

#if 0
		/		 * If the descriptor_id is not in the table it's possibly
		 * because the protocol evoluated, losing synchronization
		 * with the table, that's why we don't consider that as
		 * a heuristic
		 */
		if (!match_strval(tvb_get_ntohs(tvb, offset), descriptors))
			/
			return FALSE;
#endif

		if (!tvb_bytes_exist(tvb, offset + 6, data_len))
			/
			return FALSE;

		offset += 6 + data_len;
	}

	/
	return tvb_length_remaining(tvb, offset) == 2;
}
