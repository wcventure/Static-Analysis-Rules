static void
dissect_t30_partial_page_request(tvbuff_t *tvb, int offset, packet_info *pinfo, int len, proto_tree *tree)
{
	int frame_count = 0;
	int frame;
	gchar *buf = ep_alloc(10*1 + 90*2 + 156*3 + 256*2 + 1); /
	gchar *buf_top = buf;
	
	if (len != 32) {
		proto_tree_add_text(tree, tvb, offset, tvb_reported_length_remaining(tvb, offset), "[MALFORMED OR SHORT PACKET: PPR length must be 32 bytes]");
		expert_add_info_format(pinfo, NULL, PI_MALFORMED, PI_ERROR, "T30 PPR length must be 32 bytes");
		col_append_str(pinfo->cinfo, COL_INFO, " [MALFORMED OR SHORT PACKET]");
		return;
	}
	
	for (frame=0; frame < 255; ) {
		guint8 octet = tvb_get_guint8(tvb, offset);
		guint8 bit = 1<<7;
		
		for (;bit;) {
			if (octet & bit) {
				++frame_count;
				buf_top += g_sprintf(buf_top, "%u, ", frame);
			}
			bit >>= 1;
			++frame;
		}
		++offset;
	}
	proto_tree_add_uint(tree, hf_t30_partial_page_request_frame_count, tvb, offset, 1, frame_count);
	if (buf_top > buf+1) {
		buf_top[-2] = '\0';
		proto_tree_add_string_format(tree, hf_t30_partial_page_request_frames, tvb, offset, (gint)(buf_top-buf), buf, "Frames: %s", buf);
	}

    if (check_col(pinfo->cinfo, COL_INFO))
        col_append_fstr(pinfo->cinfo, COL_INFO, " - %d frames", frame_count);

}
