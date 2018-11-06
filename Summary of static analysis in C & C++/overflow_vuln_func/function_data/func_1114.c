static void
dissect_isakmp(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree)
{
  int			offset = 0;
  struct isakmp_hdr 	hdr;
  proto_item *		ti;
  proto_tree *		isakmp_tree = NULL;
  guint32		len;

  if (check_col(pinfo->cinfo, COL_PROTOCOL))
    col_set_str(pinfo->cinfo, COL_PROTOCOL, "ISAKMP");
  if (check_col(pinfo->cinfo, COL_INFO))
    col_clear(pinfo->cinfo, COL_INFO);

  if (tree) {
    ti = proto_tree_add_item(tree, proto_isakmp, tvb, offset, -1, FALSE);
    isakmp_tree = proto_item_add_subtree(ti, ett_isakmp);
  }

  hdr.length = tvb_get_ntohl(tvb, offset + sizeof(hdr) - sizeof(hdr.length));
  hdr.exch_type = tvb_get_guint8(tvb, sizeof(hdr.icookie) + sizeof(hdr.rcookie) + sizeof(hdr.next_payload) + sizeof(hdr.version));
  if (check_col(pinfo->cinfo, COL_INFO))
    col_add_str(pinfo->cinfo, COL_INFO, exchtype2str(hdr.exch_type));

  if (tree) {
    tvb_memcpy(tvb, (guint8 *)&hdr.icookie, offset, sizeof(hdr.icookie));
    proto_tree_add_text(isakmp_tree, tvb, offset, sizeof(hdr.icookie),
			"Initiator cookie: 0x%s", tvb_bytes_to_str(tvb, offset, sizeof(hdr.icookie)));
    offset += sizeof(hdr.icookie);

    tvb_memcpy(tvb, (guint8 *)&hdr.rcookie, offset, sizeof(hdr.rcookie));
    proto_tree_add_text(isakmp_tree, tvb, offset, sizeof(hdr.rcookie),
			"Responder cookie: 0x%s", tvb_bytes_to_str(tvb, offset, sizeof(hdr.rcookie)));
    offset += sizeof(hdr.rcookie);

    hdr.next_payload = tvb_get_guint8(tvb, offset);
    proto_tree_add_text(isakmp_tree, tvb, offset, sizeof(hdr.next_payload),
			"Next payload: %s (%u)",
			payloadtype2str(hdr.next_payload), hdr.next_payload);
    offset += sizeof(hdr.next_payload);

    hdr.version = tvb_get_guint8(tvb, offset);
    proto_tree_add_text(isakmp_tree, tvb, offset, sizeof(hdr.version),
			"Version: %u.%u",
			hi_nibble(hdr.version), lo_nibble(hdr.version));
    offset += sizeof(hdr.version);

    hdr.exch_type = tvb_get_guint8(tvb, offset);
    proto_tree_add_text(isakmp_tree, tvb, offset, sizeof(hdr.exch_type),
			"Exchange type: %s (%u)",
			exchtype2str(hdr.exch_type), hdr.exch_type);
    offset += sizeof(hdr.exch_type);

    {
      proto_item *	fti;
      proto_tree *	ftree;

      hdr.flags = tvb_get_guint8(tvb, offset);
      fti   = proto_tree_add_text(isakmp_tree, tvb, offset, sizeof(hdr.flags), "Flags");
      ftree = proto_item_add_subtree(fti, ett_isakmp_flags);

      proto_tree_add_text(ftree, tvb, offset, 1, "%s",
			  decode_boolean_bitfield(hdr.flags, E_FLAG, sizeof(hdr.flags)*8,
						  "Encryption", "No encryption"));
      proto_tree_add_text(ftree, tvb, offset, 1, "%s",
			  decode_boolean_bitfield(hdr.flags, C_FLAG, sizeof(hdr.flags)*8,
						  "Commit", "No commit"));
      proto_tree_add_text(ftree, tvb, offset, 1, "%s",
			  decode_boolean_bitfield(hdr.flags, A_FLAG, sizeof(hdr.flags)*8,
						  "Authentication", "No authentication"));
      offset += sizeof(hdr.flags);
    }

    proto_tree_add_text(isakmp_tree, tvb, offset, sizeof(hdr.message_id),
        "Message ID: 0x%s", tvb_bytes_to_str(tvb, offset, sizeof(hdr.message_id)));
    offset += sizeof(hdr.message_id);

    if (hdr.length < sizeof(hdr)) {
        proto_tree_add_text(isakmp_tree, tvb, offset, sizeof(hdr.length),
			    "Length: (bogus, length is %u, should be at least %lu)",
			    hdr.length, (unsigned long)sizeof(hdr));
        return;
    }

    proto_tree_add_text(isakmp_tree, tvb, offset, sizeof(hdr.length),
			"Length: %u", hdr.length);
    offset += sizeof(hdr.length);

    len = hdr.length - sizeof(hdr);

    if (hdr.flags & E_FLAG) {
      if (len && isakmp_tree) {
        proto_tree_add_text(isakmp_tree, tvb, offset, len,
			"Encrypted payload (%d byte%s)",
			len, plurality(len, "", "s"));
      }
    } else
      dissect_payloads(tvb, isakmp_tree, hdr.next_payload, offset, len, pinfo);
  }
}
