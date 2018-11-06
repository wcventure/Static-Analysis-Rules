static void
dissect_packetcable_mta_cap(proto_tree *v_tree, tvbuff_t *tvb, int voff, int len)
{
	guint16 raw_val;
	unsigned long flow_val = 0;
	int off = PKT_MDC_TLV_OFF + voff;
	int subopt_off, max_len;
	guint tlv_len, i, mib_val;
	guint8 asc_val[3] = "  ", flow_val_str[5];
	char bit_fld[64];
	proto_item *ti, *mib_ti;
	proto_tree *subtree, *subtree2;

	tvb_memcpy (tvb, asc_val, off, 2);
	if (sscanf((gchar*)asc_val, "%x", &tlv_len) != 1 || tlv_len > 0xff) {
		proto_tree_add_text(v_tree, tvb, off, len - off,
			"Bogus length: %s", asc_val);
		return;
	} else {
		proto_tree_add_uint_format_value(v_tree, hf_bootp_pkt_mtacap_len, tvb, off, 2,
				tlv_len, "%d", tlv_len);
		off += 2;

		while (off - voff < len) {
			/
			raw_val = tvb_get_ntohs (tvb, off);

			/
			tvb_memcpy(tvb, asc_val, off + 2, 2);
			if (sscanf((gchar*)asc_val, "%x", &tlv_len) != 1 || tlv_len < 1) {
				proto_tree_add_text(v_tree, tvb, off, len - off,
							"[Bogus length: %s]", asc_val);
				return;
			} else {
				/

				ti = proto_tree_add_text(v_tree,
				    tvb, off, (tlv_len * 2) + 4,
				    "0x%s: %s = ",
				    tvb_format_text(tvb, off, 2),
				    val_to_str(raw_val, pkt_mdc_type_vals, "unknown"));
				switch (raw_val) {

				case PKT_MDC_VERSION:
					raw_val = tvb_get_ntohs(tvb, off + 4);
					proto_item_append_text(ti,
							       "%s (%s)",
							       val_to_str(raw_val, pkt_mdc_version_vals, "Reserved"),
							       tvb_format_stringzpad(tvb, off + 4, 2) );
					break;

				case PKT_MDC_TEL_END:
				case PKT_MDC_IF_INDEX:
					proto_item_append_text(ti,
							       "%s",
							       tvb_format_stringzpad(tvb, off + 4, 2) );
					break;

				case PKT_MDC_TGT:
				case PKT_MDC_HTTP_ACC:
				case PKT_MDC_SYSLOG:
				case PKT_MDC_NCS:
				case PKT_MDC_PRI_LINE:
				case PKT_MDC_NVRAM_STOR:
				case PKT_MDC_PROV_REP:
				case PKT_MDC_PROV_REP_LC:
				case PKT_MDC_SILENCE:
				case PKT_MDC_SILENCE_LC:
				case PKT_MDC_ECHO_CANCEL:
				case PKT_MDC_ECHO_CANCEL_LC:
				case PKT_MDC_RSVP:
				case PKT_MDC_RSVP_LC:
				case PKT_MDC_UGS_AD:
				case PKT_MDC_UGS_AD_LC:
				case PKT_MDC_FLOW_LOG:
				case PKT_MDC_RFC2833_DTMF:
				case PKT_MDC_VOICE_METRICS:
				case PKT_MDC_MGPI:
				case PKT_MDC_V152:
				case PKT_MDC_CBS:
				case PKT_MDC_CBS_LC:
					raw_val = tvb_get_ntohs(tvb, off + 4);
					proto_item_append_text(ti,
							       "%s (%s)",
							       val_to_str(raw_val, pkt_mdc_boolean_vals, "unknown"),
							       tvb_format_stringzpad(tvb, off + 4, 2) );
					break;

				case PKT_MDC_SUPP_CODECS:
				case PKT_MDC_SUPP_CODECS_LC:
					for (i = 0; i < tlv_len; i++) {
						raw_val = tvb_get_ntohs(tvb, off + 4 + (i * 2) );
						proto_item_append_text(ti,
								       "%s%s (%s)",
								       plurality(i + 1, "", ", "),
								       val_to_str(raw_val, pkt_mdc_codec_vals, "unknown"),
								       tvb_format_stringzpad(tvb, off + 4 + (i * 2), 2) );
					}
					break;

				case PKT_MDC_PROV_FLOWS:
					tvb_memcpy(tvb, flow_val_str, off + 4, 4);
					flow_val_str[4] = '\0';
					flow_val = strtoul((gchar*)flow_val_str, NULL, 16);
					proto_item_append_text(ti,
							       "0x%04lx", flow_val);
					break;

				case PKT_MDC_T38_VERSION:
					raw_val = tvb_get_ntohs(tvb, off + 4);
					proto_item_append_text(ti,
							       "%s (%s)",
							       val_to_str(raw_val, pkt_mdc_t38_version_vals, "unknown"),
							       tvb_format_stringzpad(tvb, off + 4, 2) );
					break;

				case PKT_MDC_T38_EC:
					raw_val = tvb_get_ntohs(tvb, off + 4);
					proto_item_append_text(ti,
							       "%s (%s)",
							       val_to_str(raw_val, pkt_mdc_t38_ec_vals, "unknown"),
							       tvb_format_stringzpad(tvb, off + 4, 2) );
					break;

				case PKT_MDC_MIBS:
					break;

				case PKT_MDC_VENDOR_TLV:
				default:
					proto_item_append_text(ti,
							       "%s",
							       tvb_format_stringzpad(tvb, off + 4, tlv_len * 2) );
					break;
				}
			}
			subtree = proto_item_add_subtree(ti, ett_bootp_option);
			if (raw_val == PKT_MDC_PROV_FLOWS) {
				for (i = 0 ; i < 3; i++) {
					if (flow_val & pkt_mdc_supp_flow_vals[i].value) {
						decode_bitfield_value(bit_fld, flow_val, pkt_mdc_supp_flow_vals[i].value, 16);
						proto_tree_add_text(subtree, tvb, off + 4, 4, "%s%s",
							bit_fld, pkt_mdc_supp_flow_vals[i].strptr);
					}
				}
			} else if (raw_val == PKT_MDC_MIBS) {
			/
				subopt_off = off + 4;
				max_len = subopt_off + (tlv_len * 2);
				while (subopt_off < max_len) {
					raw_val = tvb_get_ntohs(tvb, subopt_off);
					if (raw_val != 0x3032) { /
						tvb_memcpy(tvb, asc_val, subopt_off, 2);
						proto_tree_add_text(subtree, tvb, subopt_off, 2,
									"[Bogus length: %s]", asc_val);
						return;
					}

					subopt_off += 2;
					raw_val = tvb_get_ntohs(tvb, subopt_off);
					tvb_memcpy(tvb, asc_val, subopt_off, 2);

					mib_ti = proto_tree_add_text(subtree, tvb, subopt_off, 2, "%s (%s)",
						val_to_str(raw_val, pkt_mdc_mib_orgs, "Unknown"), asc_val);
					if (subopt_off > off + 4 + 2) {
						proto_item_append_text(ti, ", ");
					}
					proto_item_append_text(ti, "%s", val_to_str(raw_val, pkt_mdc_mib_orgs, "Unknown"));

					subopt_off += 2;
					tvb_memcpy(tvb, asc_val, subopt_off, 2);
					if (sscanf((gchar*)asc_val, "%x", &mib_val) != 1) {
						proto_tree_add_text(v_tree, tvb, subopt_off, 2,
									"[Bogus bitfield: %s]", asc_val);
						return;
					}
					switch (raw_val) {

					case PKT_MDC_MIB_CL:
						subtree2 = proto_item_add_subtree(mib_ti, ett_bootp_option);

						for (i = 0; i < 8; i++) {
							if (mib_val & pkt_mdc_cl_mib_vals[i].value) {
								decode_bitfield_value(bit_fld, mib_val, pkt_mdc_cl_mib_vals[i].value, 8);
								proto_tree_add_text(subtree2, tvb, subopt_off, 2,
										    "%s%s", bit_fld, pkt_mdc_cl_mib_vals[i].strptr);
							}
						}
						break;

					case PKT_MDC_MIB_IETF:
						subtree2 = proto_item_add_subtree(mib_ti, ett_bootp_option);

						for (i = 0; i < 8; i++) {
							if (mib_val & pkt_mdc_ietf_mib_vals[i].value) {
								decode_bitfield_value(bit_fld, mib_val, pkt_mdc_ietf_mib_vals[i].value, 8);
								proto_tree_add_text(subtree2, tvb, subopt_off, 2,
										    "%s%s", bit_fld, pkt_mdc_ietf_mib_vals[i].strptr);
							}
						}
						break;

					default:
						break;
					}
					subopt_off += 2;
				}

			}
			off += (tlv_len * 2) + 4;
		}
	}
}
