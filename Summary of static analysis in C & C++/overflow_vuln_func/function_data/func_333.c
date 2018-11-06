static void
dissect_rlc_um(enum rlc_channel_type channel, tvbuff_t *tvb, packet_info *pinfo,
	       proto_tree *top_level, proto_tree *tree)
{
#define MAX_LI 16
	struct rlc_li li[MAX_LI];
	fp_info *fpinf;
	rlc_info *rlcinf;
	guint32 orig_num;
	guint8 seq;
	guint8 next_byte, offs = 0;
	gint16 pos, num_li = 0;
	gboolean is_truncated, li_is_on_2_bytes;
	proto_item *truncated_ti;

	next_byte = tvb_get_guint8(tvb, offs++);
	seq = next_byte >> 1;

	/
	if (tree) {
		proto_tree_add_bits_item(tree, hf_rlc_seq, tvb, 0, 7, ENC_BIG_ENDIAN);
		proto_tree_add_bits_item(tree, hf_rlc_ext, tvb, 7, 1, ENC_BIG_ENDIAN);
	}

	fpinf = p_get_proto_data(pinfo->fd, proto_fp);
	rlcinf = p_get_proto_data(pinfo->fd, proto_rlc);
	if (!fpinf || !rlcinf) {
		proto_tree_add_text(tree, tvb, 0, -1,
			"Cannot dissect RLC frame because per-frame info is missing");
		return;
	}
	pos = fpinf->cur_tb;
	if (rlcinf->ciphered[pos] == TRUE && rlcinf->deciphered[pos] == FALSE) {
		proto_tree_add_text(tree, tvb, 0, -1,
			"Cannot dissect RLC frame because it is ciphered");
		col_append_str(pinfo->cinfo, COL_INFO, "[Ciphered Data]");
		return;
	}

	if (rlcinf->li_size[pos] == RLC_LI_VARIABLE) {
		li_is_on_2_bytes = (tvb_length(tvb) > 125) ? TRUE : FALSE;
	} else {
		li_is_on_2_bytes = (rlcinf->li_size[pos] == RLC_LI_15BITS) ? TRUE : FALSE;
	}

	num_li = rlc_decode_li(RLC_UM, tvb, pinfo, tree, li, MAX_LI, li_is_on_2_bytes);
	if (num_li == -1) return; /
	offs += ((li_is_on_2_bytes) ? 2 : 1) * num_li;

	if (global_rlc_headers_expected) {
		/
		is_truncated = (tvb_length_remaining(tvb, offs) == 0);
		truncated_ti = proto_tree_add_boolean(tree, hf_rlc_header_only, tvb, 0, 0,
		                                      is_truncated);
		if (is_truncated) {
			PROTO_ITEM_SET_GENERATED(truncated_ti);
			expert_add_info_format(pinfo, truncated_ti, PI_SEQUENCE, PI_NOTE,
			                       "RLC PDU SDUs have been omitted");
			return;
		} else {
			PROTO_ITEM_SET_HIDDEN(truncated_ti);
		}
	}

	/
	if (pinfo->fd->num == 0) return;
	/
	if (rlc_is_duplicate(RLC_UM, pinfo, seq, &orig_num) == TRUE) {
		col_add_fstr(pinfo->cinfo, COL_INFO, "[RLC UM Fragment] [Duplicate]  SN=%u", seq);
		proto_tree_add_uint(tree, hf_rlc_duplicate_of, tvb, 0, 0, orig_num);
		return;
	}
	rlc_um_reassemble(tvb, offs, pinfo, tree, top_level, channel, seq, li, num_li, li_is_on_2_bytes);
}
