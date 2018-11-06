static int
decode_qos_umts(tvbuff_t *tvb, int offset, proto_tree *tree, const gchar* qos_str, guint8 type) {

	guint		length;
	guint8		al_ret_priority;
	guint8		delay, reliability, peak, precedence, mean, spare1, spare2, spare3;
	guint8		traf_class, del_order, del_err_sdu;
	guint8		max_sdu_size, max_ul, max_dl;
	guint8		res_ber, sdu_err_ratio;
	guint8		trans_delay, traf_handl_prio;
	guint8		guar_ul, guar_dl;
	proto_tree	*ext_tree_qos;
	proto_item	*te;
	int		mss, mu, md, gu, gd;

	/
	guint8      utf8_type = 1;

	/
	guint8      hyphen;

	/
	int		retval = 0;

	switch (type) {
		case 1:
			length = tvb_get_guint8 (tvb, offset);
			te = proto_tree_add_text (tree, tvb, offset, length + 1, "%s", qos_str);
			ext_tree_qos = proto_item_add_subtree (te, ett_gtp_qos);
			proto_tree_add_text (ext_tree_qos, tvb, offset, 1, "Length: %u", length);
			offset++;
			retval = length + 1;
			break;
		case 2:
			length = tvb_get_ntohs (tvb, offset + 1);
			te = proto_tree_add_text(tree, tvb, offset, length + 3, "%s", qos_str);
			ext_tree_qos = proto_item_add_subtree (te, ett_gtp_qos);
			proto_tree_add_text (ext_tree_qos, tvb, offset + 1, 2, "Length: %u", length);
			offset += 3;		/
			retval = length + 3;
			break;
		case 3:
			/
			utf8_type = 2;

			/
			length = tvb_length(tvb);
			te = proto_tree_add_text (tree, tvb, offset, length, "%s", qos_str);

			ext_tree_qos = proto_item_add_subtree (te, ett_gtp_qos);
			
			proto_tree_add_item (ext_tree_qos, hf_gtp_qos_version, tvb, offset, 2, FALSE);

			/
			hyphen = tvb_get_guint8(tvb, offset + 2);
			if (hyphen == ((guint8) '-'))
			{
				/
				proto_tree_add_text (ext_tree_qos, tvb, offset + 2, 1, "Hyphen separator: -");
				offset++; /
			}

			/
			offset++;
			
			length -= offset;
			length /=2;
			
			retval = length + 2;      /
			break;
		default:
			/
			length = 0;
			retval = 0;
			ext_tree_qos = NULL;
			break;
	}

	/
	al_ret_priority = tvb_get_guint8 (tvb, offset);

	/
	spare1 = wrapped_tvb_get_guint8(tvb, offset+(1 - 1) * utf8_type + 1, utf8_type) & 0xC0;
	delay = wrapped_tvb_get_guint8(tvb, offset+(1 - 1) * utf8_type + 1, utf8_type) & 0x38;
	reliability = wrapped_tvb_get_guint8(tvb, offset+(1 - 1) * utf8_type + 1, utf8_type) & 0x07;
	peak = wrapped_tvb_get_guint8(tvb, offset+(2 - 1) * utf8_type + 1, utf8_type) & 0xF0;
	spare2 = wrapped_tvb_get_guint8(tvb, offset+(2 - 1) * utf8_type + 1, utf8_type) & 0x08;
	precedence = wrapped_tvb_get_guint8(tvb, offset+(2 - 1) * utf8_type + 1, utf8_type) & 0x07;
	spare3 = wrapped_tvb_get_guint8(tvb, offset+(3 - 1) * utf8_type + 1, utf8_type) & 0xE0;
	mean = wrapped_tvb_get_guint8(tvb, offset+(3 - 1) * utf8_type + 1, utf8_type) & 0x1F;

	/
	if (type != 3)
		proto_tree_add_uint(ext_tree_qos, hf_gtp_qos_al_ret_priority, tvb, offset, 1, al_ret_priority);

	/
	proto_tree_add_uint(ext_tree_qos, hf_gtp_qos_spare1, tvb, offset+(1 - 1) * utf8_type + 1, utf8_type, spare1);
	proto_tree_add_uint(ext_tree_qos, hf_gtp_qos_delay, tvb, offset+(1 - 1) * utf8_type + 1, utf8_type, delay);
	proto_tree_add_uint(ext_tree_qos, hf_gtp_qos_reliability, tvb, offset+(1 - 1) * utf8_type + 1, utf8_type, reliability);
	proto_tree_add_uint(ext_tree_qos, hf_gtp_qos_peak, tvb, offset+(2 - 1) * utf8_type + 1, utf8_type, peak);
	proto_tree_add_uint(ext_tree_qos, hf_gtp_qos_spare2, tvb, offset+(2 - 1) * utf8_type + 1, utf8_type, spare2);
	proto_tree_add_uint(ext_tree_qos, hf_gtp_qos_precedence, tvb, offset+(2 - 1) * utf8_type + 1, utf8_type, precedence);
	proto_tree_add_uint(ext_tree_qos, hf_gtp_qos_spare3, tvb, offset+(3 - 1) * utf8_type + 1, utf8_type, spare3);
	proto_tree_add_uint(ext_tree_qos, hf_gtp_qos_mean, tvb, offset+(3 - 1) * utf8_type + 1, utf8_type, mean);

	if (length > 4) {

		/
		traf_class = wrapped_tvb_get_guint8(tvb, offset+(4 - 1) * utf8_type + 1, utf8_type) & 0xE0;
		del_order = wrapped_tvb_get_guint8(tvb, offset+(4 - 1) * utf8_type + 1, utf8_type) & 0x18;
		del_err_sdu = wrapped_tvb_get_guint8(tvb, offset+(4 - 1) * utf8_type + 1, utf8_type) & 0x07;
		max_sdu_size = wrapped_tvb_get_guint8(tvb, offset+(5 - 1) * utf8_type + 1, utf8_type);
		max_ul = wrapped_tvb_get_guint8(tvb, offset+(6 - 1) * utf8_type + 1, utf8_type);
		max_dl = wrapped_tvb_get_guint8(tvb, offset+(7 - 1) * utf8_type + 1, utf8_type);
		res_ber = wrapped_tvb_get_guint8(tvb, offset+(8 - 1) * utf8_type + 1, utf8_type) & 0xF0;
		sdu_err_ratio = wrapped_tvb_get_guint8(tvb, offset+(8 - 1) * utf8_type + 1, utf8_type) & 0x0F;
		trans_delay = wrapped_tvb_get_guint8(tvb, offset+(9 - 1) * utf8_type + 1, utf8_type) & 0xFC;
		traf_handl_prio = wrapped_tvb_get_guint8(tvb, offset+(9 - 1) * utf8_type + 1, utf8_type) & 0x03;
		guar_ul = wrapped_tvb_get_guint8(tvb, offset+(10 - 1) * utf8_type + 1, utf8_type);
		guar_dl = wrapped_tvb_get_guint8(tvb, offset+(11 - 1) * utf8_type + 1, utf8_type);

		/
		proto_tree_add_uint(ext_tree_qos, hf_gtp_qos_traf_class, tvb, offset+(4 - 1) * utf8_type + 1, utf8_type, traf_class);
		proto_tree_add_uint(ext_tree_qos, hf_gtp_qos_del_order, tvb, offset+(4 - 1) * utf8_type + 1, utf8_type, del_order);
		proto_tree_add_uint(ext_tree_qos, hf_gtp_qos_del_err_sdu, tvb, offset+(4 - 1) * utf8_type + 1, utf8_type, del_err_sdu);
		if (max_sdu_size == 0 || max_sdu_size > 150)
			proto_tree_add_uint(ext_tree_qos, hf_gtp_qos_max_sdu_size, tvb, offset+(5 - 1) * utf8_type + 1, utf8_type, max_sdu_size);
		if (max_sdu_size > 0 && max_sdu_size <= 150) {
			mss = max_sdu_size*10;
			proto_tree_add_uint_format(ext_tree_qos, hf_gtp_qos_max_sdu_size, tvb, offset+(5 - 1) * utf8_type + 1, utf8_type, mss, "Maximum SDU size : %u octets", mss);
		}

		if(max_ul == 0 || max_ul == 255)
			proto_tree_add_uint(ext_tree_qos, hf_gtp_qos_max_ul, tvb, offset+(6 - 1) * utf8_type + 1, utf8_type, max_ul);
		if(max_ul > 0 && max_ul <= 63)
			proto_tree_add_uint_format(ext_tree_qos, hf_gtp_qos_max_ul, tvb, offset+(6 - 1) * utf8_type + 1, utf8_type, max_ul, "Maximum bit rate for uplink : %u kbps", max_ul);
		if(max_ul > 63 && max_ul <=127) {
			mu = 64 + ( max_ul - 64 ) * 8;
			proto_tree_add_uint_format(ext_tree_qos, hf_gtp_qos_max_ul, tvb, offset+(6 - 1) * utf8_type + 1, utf8_type, mu, "Maximum bit rate for uplink : %u kbps", mu);
		}

		if(max_ul > 127 && max_ul <=254) {
			mu = 576 + ( max_ul - 128 ) * 64;
			proto_tree_add_uint_format(ext_tree_qos, hf_gtp_qos_max_ul, tvb, offset+(6 - 1) * utf8_type + 1, utf8_type, mu, "Maximum bit rate for uplink : %u kbps", mu);
		}

		if(max_dl == 0 || max_dl == 255)
			proto_tree_add_uint(ext_tree_qos, hf_gtp_qos_max_dl, tvb, offset+(7 - 1) * utf8_type + 1, utf8_type, max_dl);
		if(max_dl > 0 && max_dl <= 63)
			proto_tree_add_uint_format(ext_tree_qos, hf_gtp_qos_max_dl, tvb, offset+(7 - 1) * utf8_type + 1, utf8_type, max_dl, "Maximum bit rate for downlink : %u kbps", max_dl);
		if(max_dl > 63 && max_dl <=127) {
			md = 64 + ( max_dl - 64 ) * 8;
			proto_tree_add_uint_format(ext_tree_qos, hf_gtp_qos_max_dl, tvb, offset+(7 - 1) * utf8_type + 1, utf8_type, md, "Maximum bit rate for downlink : %u kbps", md);
		}
		if(max_dl > 127 && max_dl <=254) {
			md = 576 + ( max_dl - 128 ) * 64;
			proto_tree_add_uint_format(ext_tree_qos, hf_gtp_qos_max_dl, tvb, offset+(7 - 1) * utf8_type + 1, utf8_type, md, "Maximum bit rate for downlink : %u kbps", md);
		}

		proto_tree_add_uint(ext_tree_qos, hf_gtp_qos_res_ber, tvb, offset+(8 - 1) * utf8_type + 1, utf8_type, res_ber);
		proto_tree_add_uint(ext_tree_qos, hf_gtp_qos_sdu_err_ratio, tvb, offset+(8 - 1) * utf8_type + 1, utf8_type, sdu_err_ratio);
		proto_tree_add_uint(ext_tree_qos, hf_gtp_qos_trans_delay, tvb, offset+(9 - 1) * utf8_type + 1, utf8_type, trans_delay);
		proto_tree_add_uint(ext_tree_qos, hf_gtp_qos_traf_handl_prio, tvb, offset+(9 - 1) * utf8_type + 1, utf8_type, traf_handl_prio);

		if(guar_ul == 0 || guar_ul == 255)
			proto_tree_add_uint(ext_tree_qos, hf_gtp_qos_guar_ul, tvb, offset+(10 - 1) * utf8_type + 1, utf8_type, guar_ul);
		if(guar_ul > 0 && guar_ul <= 63)
			proto_tree_add_uint_format(ext_tree_qos, hf_gtp_qos_guar_ul, tvb, offset+(10 - 1) * utf8_type + 1, utf8_type, guar_ul, "Guaranteed bit rate for uplink : %u kbps", guar_ul);
		if(guar_ul > 63 && guar_ul <=127) {
			gu = 64 + ( guar_ul - 64 ) * 8;
			proto_tree_add_uint_format(ext_tree_qos, hf_gtp_qos_guar_ul, tvb, offset+(10 - 1) * utf8_type + 1, utf8_type, gu, "Guaranteed bit rate for uplink : %u kbps", gu);
		}
		if(guar_ul > 127 && guar_ul <=254) {
			gu = 576 + ( guar_ul - 128 ) * 64;
			proto_tree_add_uint_format(ext_tree_qos, hf_gtp_qos_guar_ul, tvb, offset+(10 - 1) * utf8_type + 1, utf8_type, gu, "Guaranteed bit rate for uplink : %u kbps", gu);
		}

		if(guar_dl == 0 || guar_dl == 255)
			proto_tree_add_uint(ext_tree_qos, hf_gtp_qos_guar_dl, tvb, offset+(11 - 1) * utf8_type + 1, utf8_type, guar_dl);
		if(guar_dl > 0 && guar_dl <= 63)
			proto_tree_add_uint_format(ext_tree_qos, hf_gtp_qos_guar_dl, tvb, offset+(11 - 1) * utf8_type + 1, utf8_type, guar_dl, "Guaranteed bit rate for downlink : %u kbps", guar_dl);
		if(guar_dl > 63 && guar_dl <=127) {
			gd = 64 + ( guar_dl - 64 ) * 8;
			proto_tree_add_uint_format(ext_tree_qos, hf_gtp_qos_guar_dl, tvb, offset+(11 - 1) * utf8_type + 1, utf8_type, gd, "Guaranteed bit rate for downlink : %u kbps", gd);
		}
		if(guar_dl > 127 && guar_dl <=254) {
			gd = 576 + ( guar_dl - 128 ) * 64;
			proto_tree_add_uint_format(ext_tree_qos, hf_gtp_qos_guar_dl, tvb, offset+(11 - 1) * utf8_type + 1, utf8_type, gd, "Guaranteed bit rate for downlink : %u kbps", gd);
		}

	}

	return retval;
}
