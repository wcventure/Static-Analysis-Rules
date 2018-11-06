static guint
get_gnutella_pdu_len(tvbuff_t *tvb, int offset) {
	guint32 size;

	size = tvb_get_letohl(
		tvb,
		offset + GNUTELLA_HEADER_SIZE_OFFSET);
	if (size > 0x00FFFFFF) {
		/
		size = 0x00FFFFFF;
	}

	/
	return GNUTELLA_HEADER_LENGTH + size;
}
