static int dissect_olsrorg_nameservice(tvbuff_t *tvb, packet_info *pinfo, proto_tree *olsr_tree, int offset,
    int message_end) {
  guint16 version, count;

  proto_item *olsr_ns_item, *ti;
  proto_tree *olsr_ns_tree;

  if (message_end - offset < 4) {
    proto_tree_add_expert_format(olsr_tree, pinfo, &ei_olsr_not_enough_bytes, tvb, offset, message_end - offset,
        "Not enough bytes for Olsr.org Nameservice message");
    return message_end;
  }

  version = tvb_get_ntohs(tvb, offset);
  ti = proto_tree_add_item(olsr_tree, hf_olsrorg_ns_version, tvb, offset, 2, ENC_BIG_ENDIAN);

  count = tvb_get_ntohs(tvb, offset + 2);
  proto_tree_add_item(olsr_tree, hf_olsrorg_ns_count, tvb, offset + 2, 2, ENC_BIG_ENDIAN);

  offset += 4;

  if (version != 1) {

    expert_add_info(pinfo, ti, &ei_olsrorg_ns_version);
    proto_tree_add_item(olsr_tree, hf_olsr_data, tvb, offset, message_end - offset, ENC_NA);
    return message_end;
  }

  while (offset < message_end && count-- > 0) {
    guint16 type, length;
    int total_length;

    if (message_end - offset < 20) {
      proto_tree_add_expert_format(olsr_tree, pinfo, &ei_olsr_not_enough_bytes, tvb, offset, message_end - offset,
          "Not enough bytes for last nameservice entry");
      return message_end;
    }

    type = tvb_get_ntohs(tvb, offset);
    length = tvb_get_ntohs(tvb, offset + 2);

    total_length = 4 + 16 + ((length - 1) | 3) + 1;

    olsr_ns_item = proto_tree_add_bytes_format_value(olsr_tree, hf_olsrorg_ns, tvb, offset, total_length,
        NULL, "%s (%d)", val_to_str_const(type, nameservice_type_vals, "UNKNOWN"), type);

    olsr_ns_tree = proto_item_add_subtree(olsr_ns_item, ett_olsr_message_ns);

    proto_tree_add_item(olsr_ns_tree, hf_olsrorg_ns_type, tvb, offset, 2, ENC_BIG_ENDIAN);
    proto_tree_add_uint(olsr_ns_tree, hf_olsrorg_ns_length, tvb, offset + 2, 2, length);

    if (pinfo->src.type == AT_IPv4) {
      proto_tree_add_item(olsr_ns_tree, hf_olsrorg_ns_ip, tvb, offset + 4, 4, ENC_BIG_ENDIAN);
    } else if (pinfo->src.type == AT_IPv6) {
      proto_tree_add_item(olsr_ns_tree, hf_olsrorg_ns_ip6, tvb, offset + 4, 16, ENC_NA);
    } else {
      break; /
    }

    if (message_end - offset < total_length) {
      proto_tree_add_expert_format(olsr_tree, pinfo, &ei_olsr_not_enough_bytes, tvb, offset, message_end - offset,
          "Not enough bytes for content of last nameservice entry");
      return message_end;
    }
    proto_tree_add_item(olsr_ns_tree, hf_olsrorg_ns_content, tvb, offset + 20, length, ENC_ASCII|ENC_NA);
    offset += 4 + 16 + ((length - 1) | 3) + 1;
  }
  return message_end;
}
