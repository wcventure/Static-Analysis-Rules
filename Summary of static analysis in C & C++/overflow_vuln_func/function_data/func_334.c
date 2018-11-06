static gboolean
dissect_rlc_heur(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree)
{
	gint                 offset = 0;
	fp_info              *fpi;
	rlc_info             *rlci;
	tvbuff_t             *rlc_tvb;
	guint8               tag = 0;
	guint                channelType = UMTS_CHANNEL_TYPE_UNSPECIFIED;
	gboolean             fpInfoAlreadySet = FALSE;
	gboolean             rlcInfoAlreadySet = FALSE;
	gboolean             channelTypePresent = FALSE;
	gboolean             rlcModePresent = FALSE;
	proto_item           *ti = NULL;
	proto_tree           *subtree = NULL;

	/
	if (!global_rlc_heur) {
		return FALSE;
	}

	/

	/
	if ((size_t)tvb_length_remaining(tvb, offset) < (strlen(RLC_START_STRING)+2+2)) {
		return FALSE;
	}

	/
	if (tvb_strneql(tvb, offset, RLC_START_STRING, (gint)strlen(RLC_START_STRING)) != 0) {
		return FALSE;
	}
	offset += (gint)strlen(RLC_START_STRING);

	/
	fpi = p_get_proto_data(pinfo->fd, proto_fp);
	if (fpi == NULL) {
		/
		fpi = se_alloc0(sizeof(fp_info));
	} else {
		fpInfoAlreadySet = TRUE;
	}
	rlci = p_get_proto_data(pinfo->fd, proto_rlc);
	if (rlci == NULL) {
		/
		rlci = se_alloc0(sizeof(rlc_info));
	} else {
		rlcInfoAlreadySet = TRUE;
	}

	/
	while (tag != RLC_PAYLOAD_TAG) {
		/
		tag = tvb_get_guint8(tvb, offset++);
		switch (tag) {
			case RLC_CHANNEL_TYPE_TAG:
				channelType = tvb_get_guint8(tvb, offset);
				offset++;
				channelTypePresent = TRUE;
				break;
			case RLC_MODE_TAG:
				rlci->mode[fpi->cur_tb] = tvb_get_guint8(tvb, offset);
				offset++;
				rlcModePresent = TRUE;
				break;
			case RLC_DIRECTION_TAG:
				fpi->is_uplink = (tvb_get_guint8(tvb, offset) == DIRECTION_UPLINK) ? TRUE : FALSE;
				offset++;
				break;
			case RLC_URNTI_TAG:
				rlci->urnti[fpi->cur_tb] = tvb_get_ntohl(tvb, offset);
				offset += 4;
				break;
			case RLC_RADIO_BEARER_ID_TAG:
				rlci->rbid[fpi->cur_tb] = tvb_get_guint8(tvb, offset);
				offset++;
				break;
			case RLC_LI_SIZE_TAG:
				rlci->li_size[fpi->cur_tb] = (enum rlc_li_size) tvb_get_guint8(tvb, offset);
				offset++;
				break;
			case RLC_PAYLOAD_TAG:
				/
				continue;
			default:
				/
				return FALSE;
		}
	}

	if ((channelTypePresent == FALSE) && (rlcModePresent == FALSE)) {
		/
		return FALSE;
	}

	/
	if (!fpInfoAlreadySet) {
		p_add_proto_data(pinfo->fd, proto_fp, fpi);
	}
	if (!rlcInfoAlreadySet) {
		p_add_proto_data(pinfo->fd, proto_rlc, rlci);
	}

    /
    /

    /
    rlc_tvb = tvb_new_subset(tvb, offset, -1, tvb_reported_length(tvb)-offset);
	switch (channelType) {
		case UMTS_CHANNEL_TYPE_UNSPECIFIED:
			/
			col_set_str(pinfo->cinfo, COL_PROTOCOL, "RLC");
			col_clear(pinfo->cinfo, COL_INFO);

			if (tree) {
				ti = proto_tree_add_item(tree, proto_rlc, rlc_tvb, 0, -1, ENC_NA);
				subtree = proto_item_add_subtree(ti, ett_rlc);
			}

			if (rlci->mode[fpi->cur_tb] == RLC_AM) {
				proto_item_append_text(ti, " AM");
				dissect_rlc_am(RLC_UNKNOWN_CH, rlc_tvb, pinfo, tree, subtree);
			} else if (rlci->mode[fpi->cur_tb] == RLC_UM) {
				proto_item_append_text(ti, " UM");
				dissect_rlc_um(RLC_UNKNOWN_CH, rlc_tvb, pinfo, tree, subtree);
			} else {
				proto_item_append_text(ti, " TM");
				dissect_rlc_tm(RLC_UNKNOWN_CH, rlc_tvb, pinfo, tree, subtree);
			}
			break;
		case UMTS_CHANNEL_TYPE_PCCH:
			dissect_rlc_pcch(rlc_tvb, pinfo, tree);
			break;
		case UMTS_CHANNEL_TYPE_CCCH:
			dissect_rlc_ccch(rlc_tvb, pinfo, tree);
			break;
		case UMTS_CHANNEL_TYPE_DCCH:
			dissect_rlc_dcch(rlc_tvb, pinfo, tree);
			break;
		case UMTS_CHANNEL_TYPE_PS_DTCH:
			dissect_rlc_ps_dtch(rlc_tvb, pinfo, tree);
			break;
		case UMTS_CHANNEL_TYPE_CTCH:
			dissect_rlc_ctch(rlc_tvb, pinfo, tree);
			break;

		default:
			/
			return FALSE;
	}

    return TRUE;
}
