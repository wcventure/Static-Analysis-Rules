static int dissect_olsr_hello(tvbuff_t *tvb, packet_info *pinfo, proto_tree *olsr_tree, int offset, int message_end,
    int(*handleNeighbors)(tvbuff_t *, packet_info *, proto_tree *, int, int)) {
  double hTime;
  proto_item *ti;
  proto_tree *link_type_tree;

  guint16 message_size = 0;

  if (message_end - offset < 4) {
    proto_tree_add_expert_format(olsr_tree, pinfo, &ei_olsr_not_enough_bytes, tvb, offset, message_end - offset,
        "Not enough bytes for Hello");
    return message_end;
  }

  offset += 2;

  /
  hTime = getOlsrTime(tvb_get_guint8(tvb, offset));
  proto_tree_add_double_format_value(olsr_tree, hf_olsr_htime, tvb, offset, 1, hTime,
      "%.3f (in seconds)", hTime);
  offset++;

  /
  proto_tree_add_item(olsr_tree, hf_olsr_willingness, tvb, offset++, 1, ENC_BIG_ENDIAN);

  while (offset < message_end) {
    if (message_end - offset < 4) {
      proto_tree_add_expert_format(olsr_tree, pinfo, &ei_olsr_not_enough_bytes, tvb, offset, message_end - offset,
          "Not enough bytes for last Hello entry");
      return message_end;
    }

    /
    ti = proto_tree_add_item(olsr_tree, hf_olsr_link_type, tvb, offset++, 1, ENC_BIG_ENDIAN);
    link_type_tree = proto_item_add_subtree(ti, ett_olsr_message_linktype);

    /
    offset++;

    /
    message_size = tvb_get_ntohs(tvb, offset);
    ti = proto_tree_add_item(link_type_tree, hf_olsr_link_message_size, tvb, offset, 2, ENC_BIG_ENDIAN);
    offset += 2;

    if (message_size < 4) {
      proto_item_append_string(ti, "(too short, must be >= 4)");
      return message_end;
    }
    offset = handleNeighbors(tvb, pinfo, link_type_tree, offset, offset + message_size - 4);
  }
  return message_end;
}
