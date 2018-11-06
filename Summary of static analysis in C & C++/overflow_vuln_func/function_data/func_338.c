static int
bootp_dhcp_decode_agent_info(proto_tree *v_tree, tvbuff_t *tvb, int optoff,
    int optend)
{
	int suboptoff = optoff;
	guint8 subopt, vs_opt, vs_len;
	int subopt_len, datalen;
	guint32 enterprise;
	proto_item *vti;
	proto_tree *subtree = 0;
	guint8 tag, tag_len;

	subopt = tvb_get_guint8(tvb, optoff);
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
	switch (subopt) {

	case 1: /
		proto_tree_add_text(v_tree, tvb, optoff, subopt_len + 2,
				    "Agent Circuit ID: %s",
				    tvb_bytes_to_str(tvb, suboptoff, subopt_len));
		break;

	case 2: /
		proto_tree_add_text(v_tree, tvb, optoff, subopt_len + 2,
				    "Agent Remote ID: %s",
				    tvb_bytes_to_str(tvb, suboptoff, subopt_len));
		break;

	case 3:
		proto_tree_add_text(v_tree, tvb, optoff, subopt_len + 2,
				    "Reserved: %s",
				    tvb_bytes_to_str(tvb, suboptoff, subopt_len));
		break;

	case 4: /
		proto_tree_add_text(v_tree, tvb, optoff, subopt_len + 2,
				    "DOCSIS Device Class: %s",
				    tvb_bytes_to_str(tvb, suboptoff, subopt_len));
		break;

	case 5: /
		proto_tree_add_text(v_tree, tvb, optoff, subopt_len + 2,
				    "Link selection: %s",
					ip_to_str(tvb_get_ptr(tvb, suboptoff, subopt_len)));
		break;

	case 6: /
		proto_tree_add_text(v_tree, tvb, optoff, subopt_len + 2,
					"Subscriber ID: %s",
					tvb_bytes_to_str(tvb, suboptoff, subopt_len));
		break;

	case 7: /
		proto_tree_add_text(v_tree, tvb, optoff, subopt_len + 2,
					"RADIUS Attributes: %s",
					tvb_bytes_to_str(tvb, suboptoff, subopt_len));
		break;

	case 8: /
		proto_tree_add_text(v_tree, tvb, optoff, subopt_len + 2,
					"Authentication: %s",
					tvb_bytes_to_str(tvb, suboptoff, subopt_len));
		break;

	case 9: /
		while (suboptoff < optend) {
			enterprise = tvb_get_ntohl(tvb, suboptoff);
			datalen = tvb_get_guint8(tvb, suboptoff+4);
			vti = proto_tree_add_text(v_tree, tvb, suboptoff, 4 + datalen + 1,
					    "Enterprise-number: %s (%u)",
					    val_to_str( enterprise, sminmpec_values, "Unknown"),
					    enterprise);
			suboptoff += 4;

			subtree = proto_item_add_subtree(vti, ett_bootp_option);
			proto_tree_add_text(subtree, tvb, suboptoff, 1,
					    "Data Length: %u", datalen);
			suboptoff++;

			switch (enterprise) {
			case 4491: /
				vs_opt = tvb_get_guint8(tvb, suboptoff);
				suboptoff++;
				vs_len = tvb_get_guint8(tvb, suboptoff);
				suboptoff++;

				switch (vs_opt) {

				case 1:
					if (vs_len == 4) {
						tag = tvb_get_guint8(tvb, suboptoff);
						tag_len = tvb_get_guint8(tvb, suboptoff+1);
						suboptoff+=2;
						if (tag == 1) {
							proto_tree_add_text(subtree, tvb, suboptoff, tag_len,
							    "DOCSIS Version Number %d.%d",
							    tvb_get_guint8(tvb, suboptoff),
							    tvb_get_guint8(tvb, suboptoff+1));
							suboptoff+=2;
						} else {
							proto_tree_add_text(subtree, tvb, suboptoff, tag_len,
							    "Unknown tag=%u %s (%d byte%s)", tag, 
							    tvb_bytes_to_str(tvb, suboptoff, tag_len),
							    tag_len, plurality(tag_len, "", "s"));
							suboptoff += tag_len;
						}
					} else {
						suboptoff += vs_len;
					}
					break;

				default:
					proto_tree_add_text(subtree, tvb, suboptoff, vs_len,
					    "Invalid suboption %d (%d byte%s)",
					    vs_opt, vs_len, plurality(vs_len, "", "s"));
					suboptoff += vs_len;
					break;
				}
				break;
			default:
		     		proto_tree_add_text(subtree, tvb, suboptoff, datalen,
				    "Suboption Data: %s", tvb_bytes_to_str(tvb, suboptoff, datalen));
		     		suboptoff += datalen;
				break;
			}
		}
		break;

	case 10: /
		proto_tree_add_text(v_tree, tvb, optoff, subopt_len + 2,
					"Flags: %s",
					tvb_bytes_to_str(tvb, suboptoff, subopt_len));
		break;

	case 11: /
		if (subopt_len == 4) {
			proto_tree_add_text(v_tree, tvb, optoff, subopt_len + 2,
				"Server ID Override: %s",
				ip_to_str(tvb_get_ptr(tvb, suboptoff, 4)));
		} else {
			proto_tree_add_text(v_tree, tvb, optoff, subopt_len + 2,
				"Server ID Override: Invalid length (%d instead of 4)",
				subopt_len);
		}
		break;

	default:
		proto_tree_add_text(v_tree, tvb, optoff, subopt_len + 2,
				    "Unknown agent suboption %d (%d bytes)",
				    subopt, subopt_len);
		break;
	}
	optoff += (subopt_len + 2);
	return optoff;
}
