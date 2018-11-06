static int
dissect_vendor_cl_suboption(proto_tree *v_tree, tvbuff_t *tvb,
    int optoff, int optend)
{
	int suboptoff = optoff;
	guint8 subopt, val;
	guint8 subopt_len;
	proto_item *ti;
	proto_tree *subtree;
	int i;

	static struct opt_info o125_cl_opt[]= {
		/ {"nop", special, NULL},	/
		/ {"Option Request = ", val_u_byte, NULL},
		/ {"TFTP Server Addresses : ", ipv4_list, NULL},
		/ {"eRouter Container Option : ", bytes, NULL},
		/ {"MIB Environment Indicator Option = ", special, NULL},
		/ {"Modem Capabilities : ", special, NULL},
	};

	static const value_string pkt_mib_env_ind_opt_vals[] = {
		{ 0x00, "Reserved" },
		{ 0x01, "CableLabs" },
		{ 0x02, "IETF" },
		{ 0x03, "EuroCableLabs" },
		{ 0, NULL } 
	};

	subopt = tvb_get_guint8(tvb, suboptoff);
	suboptoff++;

	if (suboptoff >= optend) {
		proto_tree_add_text(v_tree, tvb, optoff, 1,
			"Suboption %d: no room left in option for suboption length",
	 		subopt);
	 	return (optend);
	}
	subopt_len = tvb_get_guint8(tvb, suboptoff);
	suboptoff++;

	if (suboptoff+subopt_len > optend) {
		proto_tree_add_text(v_tree, tvb, optoff, optend-optoff,
			"Suboption %d: no room left in option for suboption value",
	 		subopt);
	 	return (optend);
	}

	if ((subopt < 1) || (subopt >= array_length(o125_cl_opt))) {
		proto_tree_add_text(v_tree, tvb, optoff, subopt_len+2,
			"Unknown suboption %d (%d byte%s)", subopt, subopt_len,
			plurality(subopt_len, "", "s"));
	} else {
		switch (o125_cl_opt[subopt].ftype) {

		case bytes:
			proto_tree_add_text(v_tree, tvb, optoff, subopt_len+2,
				"Suboption %d: %s%s (%d byte%s)", subopt,
				o125_cl_opt[subopt].text,
				tvb_bytes_to_str(tvb, suboptoff, subopt_len),
				subopt_len, plurality(subopt_len, "", "s"));
			break;

		case ipv4_list: 
			ti = proto_tree_add_text(v_tree, tvb, optoff, 2,
					"Suboption %d %s", subopt, o125_cl_opt[subopt].text);

			if ((subopt_len % 4) != 0) {
				proto_item_append_text(ti, 
					"Invalid length for suboption %d (%d byte%s)", subopt, subopt_len,
					plurality(subopt_len, "", "s"));
			} else {
				subtree = proto_item_add_subtree(ti, ett_bootp_option);
				for (i = 0; i < subopt_len; i+=4) {
						proto_tree_add_text(subtree, tvb, suboptoff+i, 4, "IP Address: %s",
							ip_to_str(tvb_get_ptr(tvb, (suboptoff+i), 4)));
				}
			}
			break;

		case special:
			if (subopt == 4) {
			  val = tvb_get_guint8(tvb, suboptoff);
				proto_tree_add_text(v_tree, tvb, optoff, subopt_len+2,
					"Suboption %d: %s%s", subopt,
					o125_cl_opt[subopt].text,
					val_to_str(val, pkt_mib_env_ind_opt_vals, "unknown"));
			}
			else {
				proto_tree_add_text(v_tree, tvb, optoff, subopt_len+2,
					"Suboption %d: %s%s (%d byte%s)",
		 			subopt, o125_cl_opt[subopt].text,
					tvb_bytes_to_str(tvb, suboptoff, subopt_len), 
					subopt_len, plurality(subopt_len, "", "s"));
			}
			break;

		case string:
			proto_tree_add_text(v_tree, tvb, optoff, subopt_len+2,
				"Suboption %d: %s\"%s\"", subopt,
				o125_cl_opt[subopt].text,
				tvb_format_stringzpad(tvb, suboptoff, subopt_len));
			break;

		case val_u_byte:
			val = tvb_get_guint8(tvb, suboptoff);
			proto_tree_add_text(v_tree, tvb, optoff, subopt_len+2,
				"Suboption %d: %s\"%s\"", subopt,
				o125_cl_opt[subopt].text,
				tvb_bytes_to_str(tvb, suboptoff, subopt_len));
			break;

		case val_u_short:
			proto_tree_add_text(v_tree, tvb, optoff, subopt_len+2,
				"Suboption %d: %s%d", subopt,
				o125_cl_opt[subopt].text,
				tvb_get_ntohs(tvb, suboptoff));
			break;

		default:
			proto_tree_add_text(v_tree, tvb, optoff, subopt_len+2,"ERROR, please report: Unknown subopt type handler %d", subopt);
			break;
		}
	}
	optoff += (subopt_len + 2);
	return optoff;
}
