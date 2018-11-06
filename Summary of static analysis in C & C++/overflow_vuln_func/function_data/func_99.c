void 
dissect_bfd_control(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree)
{
    gint bfd_version = -1;
    gint bfd_diag = -1;
    gint bfd_sta = -1;
    gint bfd_flags = -1;
    gint bfd_flags_h = -1;
    gint bfd_flags_p = -1;
    gint bfd_flags_f = -1;
    gint bfd_flags_c = -1;
    gint bfd_flags_a = -1;
    gint bfd_flags_d = -1;
    gint bfd_flags_m = -1;
    gint bfd_flags_d_v0 = -1;
    gint bfd_flags_p_v0 = -1;
    gint bfd_flags_f_v0 = -1;
    gint bfd_detect_time_multiplier = -1;
    gint bfd_length = -1;
    gint bfd_my_discriminator = -1;
    gint bfd_your_discriminator = -1;
    gint bfd_desired_min_tx_interval = -1;
    gint bfd_required_min_rx_interval = -1;
    gint bfd_required_min_echo_interval = -1;

    proto_item *ti;
    proto_tree *bfd_tree;
    proto_tree *bfd_flags_tree;

    const char *sep;

    col_set_str(pinfo->cinfo, COL_PROTOCOL, "BFD Control");
    col_clear(pinfo->cinfo, COL_INFO);

    bfd_version = ((tvb_get_guint8(tvb, 0) & 0xe0) >> 5);
    bfd_diag = (tvb_get_guint8(tvb, 0) & 0x1f);
    switch (bfd_version) {
        case 0:
            bfd_flags = tvb_get_guint8(tvb, 1 );
            bfd_flags_h = (tvb_get_guint8(tvb, 1) & 0x80);
            bfd_flags_d_v0 = (tvb_get_guint8(tvb, 1) & 0x40);
            bfd_flags_p_v0 = (tvb_get_guint8(tvb, 1) & 0x20);
            bfd_flags_f_v0 = (tvb_get_guint8(tvb, 1) & 0x10);
            break;
        case 1:
        default:
            bfd_sta = (tvb_get_guint8(tvb, 1) & 0xc0);
            bfd_flags = (tvb_get_guint8(tvb, 1) & 0x3e);
            bfd_flags_p = (tvb_get_guint8(tvb, 1) & 0x20);
            bfd_flags_f = (tvb_get_guint8(tvb, 1) & 0x10);
            bfd_flags_c = (tvb_get_guint8(tvb, 1) & 0x08);
            bfd_flags_a = (tvb_get_guint8(tvb, 1) & 0x04);
            bfd_flags_d = (tvb_get_guint8(tvb, 1) & 0x02);
            bfd_flags_m = (tvb_get_guint8(tvb, 1) & 0x01);
            break;
    }
    bfd_detect_time_multiplier = tvb_get_guint8(tvb, 2);
    bfd_length = tvb_get_guint8(tvb, 3);
    bfd_my_discriminator = tvb_get_ntohl(tvb, 4);
    bfd_your_discriminator = tvb_get_ntohl(tvb, 8);
    bfd_desired_min_tx_interval = tvb_get_ntohl(tvb, 12);
    bfd_required_min_rx_interval = tvb_get_ntohl(tvb, 16);
    bfd_required_min_echo_interval = tvb_get_ntohl(tvb, 20);

	switch (bfd_version) {
            case 0:
                col_add_fstr(pinfo->cinfo, COL_INFO, "Diag: %s, Flags: 0x%02x",
                             val_to_str(bfd_diag, bfd_control_v0_diag_values, "UNKNOWN"),
                             bfd_flags);
                break;
            case 1:
            default:
                col_add_fstr(pinfo->cinfo, COL_INFO, "Diag: %s, State: %s, Flags: 0x%02x",
                             val_to_str(bfd_diag, bfd_control_v1_diag_values, "UNKNOWN"),
                             val_to_str(bfd_sta >> 6 , bfd_control_sta_values, "UNKNOWN"),
                             bfd_flags);
                break;
    }

    if (tree) {
        ti = proto_tree_add_protocol_format(tree, proto_bfd, tvb, 0, bfd_length,
                                            "BFD Control message");

        bfd_tree = proto_item_add_subtree(ti, ett_bfd);

        proto_tree_add_uint(bfd_tree, hf_bfd_version, tvb, 0,
                                 1, bfd_version << 5);

        proto_tree_add_uint(bfd_tree, hf_bfd_diag, tvb, 0,
                                 1, bfd_diag);

	switch (bfd_version) {
            case 0:
                break;
            case 1:
            default:
                proto_tree_add_uint(bfd_tree, hf_bfd_sta, tvb, 1,
                                    1, bfd_sta);

                break;
	}
	switch (bfd_version) {
            case 0:
                ti = proto_tree_add_text ( bfd_tree, tvb, 1, 1, "Message Flags: 0x%02x",
                                           bfd_flags);
                bfd_flags_tree = proto_item_add_subtree(ti, ett_bfd_flags);
                proto_tree_add_boolean(bfd_flags_tree, hf_bfd_flags_h, tvb, 1, 1, bfd_flags_h);
                proto_tree_add_boolean(bfd_flags_tree, hf_bfd_flags_d_v0, tvb, 1, 1, bfd_flags_d_v0);
                proto_tree_add_boolean(bfd_flags_tree, hf_bfd_flags_p_v0, tvb, 1, 1, bfd_flags_p_v0);
                proto_tree_add_boolean(bfd_flags_tree, hf_bfd_flags_f_v0, tvb, 1, 1, bfd_flags_f_v0);

                sep = initial_sep;
                APPEND_BOOLEAN_FLAG(bfd_flags_h, ti, "%sH");
                APPEND_BOOLEAN_FLAG(bfd_flags_d_v0, ti, "%sD");
                APPEND_BOOLEAN_FLAG(bfd_flags_p_v0, ti, "%sP");
                APPEND_BOOLEAN_FLAG(bfd_flags_f_v0, ti, "%sF");
                if (sep != initial_sep) {
                    proto_item_append_text (ti, ")");
                }
                break;
            case 1:
            default:
                ti = proto_tree_add_text ( bfd_tree, tvb, 1, 1, "Message Flags: 0x%02x",
                                           bfd_flags);
                bfd_flags_tree = proto_item_add_subtree(ti, ett_bfd_flags);
                proto_tree_add_boolean(bfd_flags_tree, hf_bfd_flags_p, tvb, 1, 1, bfd_flags_p);
                proto_tree_add_boolean(bfd_flags_tree, hf_bfd_flags_f, tvb, 1, 1, bfd_flags_f);
                proto_tree_add_boolean(bfd_flags_tree, hf_bfd_flags_c, tvb, 1, 1, bfd_flags_c);
                proto_tree_add_boolean(bfd_flags_tree, hf_bfd_flags_a, tvb, 1, 1, bfd_flags_a);
                proto_tree_add_boolean(bfd_flags_tree, hf_bfd_flags_d, tvb, 1, 1, bfd_flags_d);
                proto_tree_add_boolean(bfd_flags_tree, hf_bfd_flags_m, tvb, 1, 1, bfd_flags_m);

                sep = initial_sep;
                APPEND_BOOLEAN_FLAG(bfd_flags_p, ti, "%sP");
                APPEND_BOOLEAN_FLAG(bfd_flags_f, ti, "%sF");
                APPEND_BOOLEAN_FLAG(bfd_flags_c, ti, "%sC");
                APPEND_BOOLEAN_FLAG(bfd_flags_a, ti, "%sA");
                APPEND_BOOLEAN_FLAG(bfd_flags_d, ti, "%sD");
                APPEND_BOOLEAN_FLAG(bfd_flags_m, ti, "%sM");
                if (sep != initial_sep) {
                    proto_item_append_text (ti, ")");
                }
                break;
	}

        proto_tree_add_uint_format_value(bfd_tree, hf_bfd_detect_time_multiplier, tvb, 2,
                                              1, bfd_detect_time_multiplier,
                                              "%u (= %u ms Detection time)",
                                              bfd_detect_time_multiplier,
                                              bfd_detect_time_multiplier * bfd_desired_min_tx_interval/1000);

	proto_tree_add_uint_format_value(bfd_tree, hf_bfd_message_length, tvb, 3, 1, bfd_length,
		"%u bytes", bfd_length);

        proto_tree_add_uint(bfd_tree, hf_bfd_my_discriminator, tvb, 4,
                                 4, bfd_my_discriminator);

        proto_tree_add_uint(bfd_tree, hf_bfd_your_discriminator, tvb, 8,
                                 4, bfd_your_discriminator);

        proto_tree_add_uint_format_value(bfd_tree, hf_bfd_desired_min_tx_interval, tvb, 12,
                                              4, bfd_desired_min_tx_interval,
                                              "%4u ms (%u us)",
                                              bfd_desired_min_tx_interval/1000,
					      bfd_desired_min_tx_interval);

        proto_tree_add_uint_format_value(bfd_tree, hf_bfd_required_min_rx_interval, tvb, 16,
                                              4, bfd_required_min_rx_interval,
                                              "%4u ms (%u us)",
                                              bfd_required_min_rx_interval/1000,
					      bfd_required_min_rx_interval);

        proto_tree_add_uint_format_value(bfd_tree, hf_bfd_required_min_echo_interval, tvb, 20,
                                              4, bfd_required_min_echo_interval,
                                              "%4u ms (%u us)",
                                              bfd_required_min_echo_interval/1000,
					      bfd_required_min_echo_interval);

	/
	if (bfd_version && bfd_flags_a) {
	    if (bfd_length >= 28) {
		dissect_bfd_authentication(tvb, pinfo, bfd_tree);
	    } else {
		ti = proto_tree_add_text(bfd_tree, tvb, 24, bfd_length,
			"Authentication: Length of the BFD frame is invalid (%d)", bfd_length);
		expert_add_info_format(pinfo, ti, PI_MALFORMED, PI_WARN,
			"Authentication flag is set in a BFD packet, but no authentication data is present");
	    }
	}
    }
    return;
}
