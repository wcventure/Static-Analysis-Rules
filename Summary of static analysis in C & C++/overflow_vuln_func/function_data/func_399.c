static int handle_olsr_hello_olsrorg(tvbuff_t *tvb, packet_info *pinfo, proto_tree *olsr_tree, int offset,
    int link_message_end) {
  /

  while (offset < link_message_end) {
    proto_item *address_group;
    proto_tree *address_tree;
    guint8 lq, nlq;

    if (link_message_end - offset < pinfo->src.len + 4) {
      proto_tree_add_expert_format(olsr_tree, pinfo, &ei_olsr_not_enough_bytes, tvb, offset, link_message_end - offset,
          "Not enough bytes for last Olsr.org LQ-Hello entry");
      return link_message_end;
    }

    if (pinfo->src.type == AT_IPv4) {
      lq = tvb_get_guint8(tvb, offset + 4);
      nlq = tvb_get_guint8(tvb, offset + 5);

      address_group = proto_tree_add_bytes_format_value(olsr_tree, hf_olsr_neighbor, tvb, offset, 8,
           NULL, "%s (%d/%d)", tvb_ip_to_str(tvb, offset), lq, nlq);

      address_tree = proto_item_add_subtree(address_group, ett_olsr_message_neigh);

      proto_tree_add_item(address_tree, hf_olsr_neighbor_addr, tvb, offset, 4, ENC_BIG_ENDIAN);
      offset += 4;
    } else if (pinfo->src.type == AT_IPv6) {
      lq = tvb_get_guint8(tvb, offset + 16);
      nlq = tvb_get_guint8(tvb, offset + 17);

      address_group = proto_tree_add_bytes_format_value(olsr_tree, hf_olsr_neighbor, tvb, offset, 20,
          NULL, "%s (%d/%d)", tvb_ip6_to_str(tvb, offset), lq, nlq);

      address_tree = proto_item_add_subtree(address_group, ett_olsr_message_neigh);

      proto_tree_add_item(address_tree, hf_olsr_neighbor6_addr, tvb, offset, 16, ENC_NA);
      offset += 16;
    } else {
      break; /
    }

    proto_tree_add_item(address_tree, hf_olsrorg_lq, tvb, offset++, 1, ENC_BIG_ENDIAN);
    proto_tree_add_item(address_tree, hf_olsrorg_nlq, tvb, offset++, 1, ENC_BIG_ENDIAN);
    offset += 2;
  } /
  return link_message_end;
}
