static int
decode_gtp_tft(tvbuff_t *tvb, int offset, packet_info *pinfo _U_, proto_tree *tree) {

	guint16		length, port1, port2, tos;
	guint8		tft_flags, tft_code, no_packet_filters, i, pf_id, pf_eval, pf_len, pf_content_id, pf_offset, proto, spare;
	guint32		mask_ipv4, addr_ipv4, ipsec_id, label;
	struct	e_in6_addr addr_ipv6, mask_ipv6;
	proto_tree	*ext_tree_tft, *ext_tree_tft_pf, *ext_tree_tft_flags;
	proto_item	*te, *tee, *tef;

	length = tvb_get_ntohs(tvb, offset+1);

	te = proto_tree_add_text(tree, tvb, offset, 3+length, "Traffic flow template");
	ext_tree_tft = proto_item_add_subtree(te, ett_gtp_tft);

	tft_flags = tvb_get_guint8(tvb, offset+3);
	tft_code = (tft_flags >> 5) & 0x07;
	spare = (tft_flags >> 4) & 0x01;
	no_packet_filters = tft_flags & 0x0F;

	proto_tree_add_text(ext_tree_tft, tvb, offset+1, 2, "TFT length: %u", length);

	tef = proto_tree_add_text (ext_tree_tft, tvb, offset + 3, 1, "TFT flags");
	ext_tree_tft_flags = proto_item_add_subtree (tef, ett_gtp_tft_flags);
	proto_tree_add_uint (ext_tree_tft_flags, hf_gtpv1_tft_code, tvb, offset + 3, 1, tft_flags);
	proto_tree_add_uint (ext_tree_tft_flags, hf_gtpv1_tft_spare, tvb, offset + 3, 1, tft_flags);
	proto_tree_add_uint (ext_tree_tft_flags, hf_gtpv1_tft_number, tvb, offset + 3, 1, tft_flags);

	offset = offset + 4;

	for (i=0;i<no_packet_filters;i++) {

		pf_id = tvb_get_guint8(tvb, offset);

		tee = proto_tree_add_text (ext_tree_tft, tvb, offset, 1, "Packet filter id: %u", pf_id);
		ext_tree_tft_pf = proto_item_add_subtree (tee, ett_gtp_tft_pf);
		offset++;

		if (tft_code != 2) {

			pf_eval = tvb_get_guint8(tvb, offset);
			pf_len = tvb_get_guint8(tvb, offset + 1);

			proto_tree_add_uint (ext_tree_tft_pf, hf_gtpv1_tft_eval, tvb, offset, 1, pf_eval);
			proto_tree_add_text (ext_tree_tft_pf, tvb, offset+1, 1, "Content length: %u", pf_len);

			offset = offset + 2;
			pf_offset = 0;

			while (pf_offset < pf_len) {

				pf_content_id = tvb_get_guint8 (tvb, offset + pf_offset);

				switch (pf_content_id) {
					/
					case 0x10:
						tvb_memcpy (tvb, (guint8 *)&addr_ipv4, offset + pf_offset + 1, sizeof addr_ipv4);
						tvb_memcpy (tvb, (guint8 *)&mask_ipv4, offset + pf_offset + 5, sizeof mask_ipv4);
						proto_tree_add_text (ext_tree_tft_pf, tvb, offset + pf_offset, 9, "ID 0x10: IPv4/mask: %s/%s", ip_to_str ((guint8 *)&addr_ipv4), ip_to_str ((guint8 *)&mask_ipv4));
						pf_offset = pf_offset + 9;
						break;
					/
					case 0x20:
						tvb_memcpy (tvb, (guint8 *)&addr_ipv6, offset+pf_offset+1, sizeof addr_ipv6);
						tvb_memcpy (tvb, (guint8 *)&mask_ipv6, offset+pf_offset+17, sizeof mask_ipv6);
						proto_tree_add_text (ext_tree_tft_pf, tvb, offset+pf_offset, 33, "ID 0x20: IPv6/mask: %s/%s", ip6_to_str ((struct e_in6_addr*)&addr_ipv6), ip6_to_str ((struct e_in6_addr*)&mask_ipv6));
						pf_offset = pf_offset + 33;
						break;
					/
					case 0x30:
						proto = tvb_get_guint8 (tvb, offset + pf_offset + 1);
						proto_tree_add_text (ext_tree_tft_pf, tvb, offset + pf_offset, 2, "ID 0x30: IPv4 protocol identifier/IPv6 next header: %u (%x)", proto, proto);
						pf_offset = pf_offset + 2;
						break;
					/
					case 0x40:
						port1 = tvb_get_ntohs (tvb, offset + pf_offset + 1);
						proto_tree_add_text (ext_tree_tft_pf, tvb, offset + pf_offset, 3, "ID 0x40: destination port: %u", port1);
						pf_offset = pf_offset + 3;
						break;
					/
					case 0x41:
						port1 = tvb_get_ntohs (tvb, offset + pf_offset + 1);
						port2 = tvb_get_ntohs (tvb, offset + pf_offset + 3);
						proto_tree_add_text (ext_tree_tft_pf, tvb, offset + pf_offset, 5, "ID 0x41: destination port range: %u - %u", port1, port2);
						pf_offset = pf_offset + 5;
						break;
					/
					case 0x50:
						port1 = tvb_get_ntohs (tvb, offset + pf_offset + 1);
						proto_tree_add_text (ext_tree_tft_pf, tvb, offset + pf_offset, 3, "ID 0x50: source port: %u", port1);
						pf_offset = pf_offset + 3;
						break;
					/
					case 0x51:
						port1 = tvb_get_ntohs (tvb, offset + pf_offset + 1);
						port2 = tvb_get_ntohs (tvb, offset + pf_offset + 3);
						proto_tree_add_text (ext_tree_tft_pf, tvb, offset + pf_offset, 5, "ID 0x51: source port range: %u - %u", port1, port2);
						pf_offset = pf_offset + 5;
						break;
					/
					case 0x60:
						ipsec_id = tvb_get_ntohl (tvb, offset + pf_offset + 1);
						proto_tree_add_text (ext_tree_tft_pf, tvb, offset + pf_offset, 5, "ID 0x60: security parameter index: %x", ipsec_id);
						pf_offset = pf_offset + 5;
						break;
					/
					case 0x70:
						tos = tvb_get_ntohs (tvb, offset + pf_offset + 1);
						proto_tree_add_text (ext_tree_tft_pf, tvb, offset + pf_offset, 2, "ID 0x70: Type of Service/Traffic Class: %u (%x)", tos, tos);
						pf_offset = pf_offset + 3;
						break;
					/
					case 0x80:
						label = tvb_get_ntoh24(tvb, offset + pf_offset + 1) & 0x0FFFFF;;
						proto_tree_add_text (ext_tree_tft_pf, tvb, offset + pf_offset, 4, "ID 0x80: Flow Label: %u (%x)", label, label);
						pf_offset = pf_offset + 4;
						break;

					default:
						proto_tree_add_text (ext_tree_tft_pf, tvb, offset + pf_offset, 1, "Unknown value: %x ", pf_content_id);
						pf_offset++; /
						break;
				}
			}

			offset = offset + pf_offset;
		}
	}

	return 3 + length;
}
