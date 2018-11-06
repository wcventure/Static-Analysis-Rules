static void dissect_cops_object_data(tvbuff_t *tvb, packet_info *pinfo, guint32 offset, proto_tree *tree,
                                     guint8 op_code, guint16 client_type, guint8 c_num, guint8 c_type, guint16 len)
{
  proto_item *ti;
  proto_tree *r_type_tree, *itf_tree, *reason_tree, *dec_tree, *error_tree, *clientsi_tree, *pdp_tree;
  guint16 r_type, m_type, reason, reason_sub, cmd_code, cmd_flags, error, error_sub, tcp_port;
  guint32 ipv4addr, ifindex;
  struct e_in6_addr ipv6addr;

  switch (c_num) {
  case COPS_OBJ_CONTEXT:
    r_type = tvb_get_ntohs(tvb, offset);
    m_type = tvb_get_ntohs(tvb, offset + 2);
    ti = proto_tree_add_text(tree, tvb, offset, 4, "Contents: R-Type: %s, M-Type: %u",
                             val_to_str(r_type, cops_r_type_vals, "Unknown"),
                             m_type);

    r_type_tree = proto_item_add_subtree(ti, ett_cops_r_type_flags);
    proto_tree_add_uint(r_type_tree, hf_cops_r_type_flags, tvb, offset, 2, r_type);
    offset += 2;
    proto_tree_add_uint(r_type_tree, hf_cops_m_type_flags, tvb, offset, 2, m_type);

    break;
  case COPS_OBJ_IN_INT:
  case COPS_OBJ_OUT_INT:
    if (c_type == 1) {          /
      tvb_memcpy(tvb, (guint8 *)&ipv4addr, offset, 4);
      ifindex = tvb_get_ntohl(tvb, offset + 4);
      ti = proto_tree_add_text(tree, tvb, offset, 8, "Contents: IPv4 address %s, ifIndex: %u",
                               ip_to_str((guint8 *)&ipv4addr), ifindex);
      itf_tree = proto_item_add_subtree(ti, ett_cops_itf);
      proto_tree_add_ipv4(itf_tree,
                          (c_num == COPS_OBJ_IN_INT) ? hf_cops_in_int_ipv4 : hf_cops_out_int_ipv4,
                          tvb, offset, 4, ipv4addr);
      offset += 4;
    } else if (c_type == 2) {   /
      tvb_memcpy(tvb, (guint8 *)&ipv6addr, offset, sizeof ipv6addr);
      ifindex = tvb_get_ntohl(tvb, offset + sizeof ipv6addr);
      ti = proto_tree_add_text(tree, tvb, offset, 20, "Contents: IPv6 address %s, ifIndex: %u",
                               ip6_to_str(&ipv6addr), ifindex);
      itf_tree = proto_item_add_subtree(ti, ett_cops_itf);
      proto_tree_add_ipv6(itf_tree,
                          (c_num == COPS_OBJ_IN_INT) ? hf_cops_in_int_ipv6 : hf_cops_out_int_ipv6,
                          tvb, offset, 16, (guint8 *)&ipv6addr);
      offset += 16;
    } else {
      break;
    }
    proto_tree_add_uint(itf_tree, hf_cops_int_ifindex, tvb, offset, 4, ifindex);

    break;
  case COPS_OBJ_REASON:
    reason = tvb_get_ntohs(tvb, offset);
    reason_sub = tvb_get_ntohs(tvb, offset + 2);
    ti = proto_tree_add_text(tree, tvb, offset, 4, "Contents: Reason-Code: %s, Reason Sub-code: 0x%04x",
                             val_to_str(reason, cops_reason_vals, "<Unknown value>"), reason_sub);
    reason_tree = proto_item_add_subtree(ti, ett_cops_reason);
    proto_tree_add_uint(reason_tree, hf_cops_reason, tvb, offset, 2, reason);
    offset += 2;
    if (reason == 13) {
      proto_tree_add_text(reason_tree, tvb, offset, 2, "Reason Sub-code: "
                          "Unknown object's C-Num %u, C-Type %u",
                          tvb_get_guint8(tvb, offset), tvb_get_guint8(tvb, offset + 1));
    } else
      proto_tree_add_uint(reason_tree, hf_cops_reason_sub, tvb, offset, 2, reason_sub);

    break;
  case COPS_OBJ_DECISION:
  case COPS_OBJ_LPDPDECISION:
    if (c_type == 1) {
      cmd_code = tvb_get_ntohs(tvb, offset);
      cmd_flags = tvb_get_ntohs(tvb, offset + 2);
      ti = proto_tree_add_text(tree, tvb, offset, 4, "Contents: Command-Code: %s, Flags: %s",
                               val_to_str(cmd_code, cops_dec_cmd_code_vals, "<Unknown value>"),
                               val_to_str(cmd_flags, cops_dec_cmd_flag_vals, "<Unknown flag>"));
      dec_tree = proto_item_add_subtree(ti, ett_cops_decision);
      proto_tree_add_uint(dec_tree, hf_cops_dec_cmd_code, tvb, offset, 2, cmd_code);
      offset += 2;
      proto_tree_add_uint(dec_tree, hf_cops_dec_flags, tvb, offset, 2, cmd_flags);
    } else if (c_type == 5) { /
      ti = proto_tree_add_text(tree, tvb, offset, 4, "Contents: %u bytes", len);
      dec_tree = proto_item_add_subtree(ti, ett_cops_decision);
      dissect_cops_pr_objects(tvb, offset, dec_tree, len);
    }

    /
    if (client_type == COPS_CLIENT_PC_DQOS && c_type == 4) {
	cops_analyze_packetcable_dqos_obj(tvb, pinfo, tree, op_code, offset);
    } else if (client_type == COPS_CLIENT_PC_MM && c_type == 4) {
	cops_analyze_packetcable_mm_obj(tvb, pinfo, tree, op_code, offset);
    }

    break;
  case COPS_OBJ_ERROR:
    if (c_type != 1)
      break;

    error = tvb_get_ntohs(tvb, offset);
    error_sub = tvb_get_ntohs(tvb, offset + 2);
    ti = proto_tree_add_text(tree, tvb, offset, 4, "Contents: Error-Code: %s, Error Sub-code: 0x%04x",
                             val_to_str(error, cops_error_vals, "<Unknown value>"), error_sub);
    error_tree = proto_item_add_subtree(ti, ett_cops_error);
    proto_tree_add_uint(error_tree, hf_cops_error, tvb, offset, 2, error);
    offset += 2;
    if (error == 13) {
      proto_tree_add_text(error_tree, tvb, offset, 2, "Error Sub-code: "
                          "Unknown object's C-Num %u, C-Type %u",
                          tvb_get_guint8(tvb, offset), tvb_get_guint8(tvb, offset + 1));
    } else
      proto_tree_add_uint(error_tree, hf_cops_error_sub, tvb, offset, 2, error_sub);

    break;
  case COPS_OBJ_CLIENTSI:

    /
    if (client_type == COPS_CLIENT_PC_DQOS && c_type == 1) {
       cops_analyze_packetcable_dqos_obj(tvb, pinfo, tree, op_code, offset);
       break;
    } else if (client_type == COPS_CLIENT_PC_MM && c_type == 1) {
       cops_analyze_packetcable_mm_obj(tvb, pinfo, tree, op_code, offset);
       break;
    }

    if (c_type != 2) /
      break;

    ti = proto_tree_add_text(tree, tvb, offset, 4, "Contents: %u bytes", len);
    clientsi_tree = proto_item_add_subtree(ti, ett_cops_clientsi);

    dissect_cops_pr_objects(tvb, offset, clientsi_tree, len);

    break;
  case COPS_OBJ_KATIMER:
    if (c_type != 1)
      break;

    proto_tree_add_item(tree, hf_cops_katimer, tvb, offset + 2, 2, FALSE);
    if (tvb_get_ntohs(tvb, offset + 2) == 0)
      proto_tree_add_text(tree, tvb, offset, 0, "Value of zero implies infinity.");

    break;
  case COPS_OBJ_PEPID:
    if (c_type != 1)
      break;

    if (tvb_strnlen(tvb, offset, len) == -1)
      proto_tree_add_text(tree, tvb, offset, len, "<PEP Id is not a NUL terminated ASCII string>");
    else
      proto_tree_add_item(tree, hf_cops_pepid, tvb, offset,
                          tvb_strnlen(tvb, offset, len) + 1, FALSE);

    break;
  case COPS_OBJ_REPORT_TYPE:
    if (c_type != 1)
      break;

    proto_tree_add_item(tree, hf_cops_report_type, tvb, offset, 2, FALSE);

    break;
  case COPS_OBJ_PDPREDIRADDR:
  case COPS_OBJ_LASTPDPADDR:
    if (c_type == 1) {          /
      tvb_memcpy(tvb, (guint8 *)&ipv4addr, offset, 4);
      tcp_port = tvb_get_ntohs(tvb, offset + 4 + 2);
      ti = proto_tree_add_text(tree, tvb, offset, 8, "Contents: IPv4 address %s, TCP Port Number: %u",
                               ip_to_str((guint8 *)&ipv4addr), tcp_port);
      pdp_tree = proto_item_add_subtree(ti, ett_cops_pdp);
      proto_tree_add_ipv4(pdp_tree,
                          (c_num == COPS_OBJ_PDPREDIRADDR) ? hf_cops_pdprediraddr_ipv4 : hf_cops_lastpdpaddr_ipv4,
                          tvb, offset, 4, ipv4addr);
      offset += 4;
    } else if (c_type == 2) {   /
      tvb_memcpy(tvb, (guint8 *)&ipv6addr, offset, sizeof ipv6addr);
      tcp_port = tvb_get_ntohs(tvb, offset + sizeof ipv6addr + 2);
      ti = proto_tree_add_text(tree, tvb, offset, 20, "Contents: IPv6 address %s, TCP Port Number: %u",
                               ip6_to_str(&ipv6addr), tcp_port);
      pdp_tree = proto_item_add_subtree(ti, ett_cops_pdp);
      proto_tree_add_ipv6(pdp_tree,
                          (c_num == COPS_OBJ_PDPREDIRADDR) ? hf_cops_pdprediraddr_ipv6 : hf_cops_lastpdpaddr_ipv6,
                          tvb, offset, 16, (guint8 *)&ipv6addr);
      offset += 16;
    } else {
      break;
    }
    offset += 2;
    proto_tree_add_uint(pdp_tree, hf_cops_pdp_tcp_port, tvb, offset, 2, tcp_port);

    break;
  case COPS_OBJ_ACCTTIMER:
    if (c_type != 1)
      break;

    proto_tree_add_item(tree, hf_cops_accttimer, tvb, offset + 2, 2, FALSE);
    if (tvb_get_ntohs(tvb, offset + 2) == 0)
      proto_tree_add_text(tree, tvb, offset, 0, "Value of zero means "
                          "there SHOULD be no unsolicited accounting updates.");

    break;
  case COPS_OBJ_INTEGRITY:
    if (c_type != 1)
      break;      /

    proto_tree_add_item(tree, hf_cops_key_id, tvb, offset, 4, FALSE);
    proto_tree_add_item(tree, hf_cops_seq_num, tvb, offset + 4, 4, FALSE);
    proto_tree_add_text(tree, tvb, offset + 8 , len - 8, "Contents: Keyed Message Digest");

    break;
  default:
    proto_tree_add_text(tree, tvb, offset, len, "Contents: %u bytes", len);

    break;
  }
}
