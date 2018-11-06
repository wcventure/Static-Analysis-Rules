static int dissect_olsr(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, void *data _U_) {
  proto_item *ti;
  proto_tree *olsr_tree;

  int offset, message_len, message_end;
  guint message_type;
  double vTime;

  guint16 packet_len;

  /
  if (tvb_length(tvb) < 4) {
    col_add_fstr(pinfo->cinfo, COL_INFO, "OLSR Packet,  Length: %u Bytes (not enough data in packet)",
          tvb_length(tvb));
    return 0; /
  }
  packet_len = tvb_get_ntohs(tvb, 0);
  if (packet_len > tvb_length(tvb)) {
    col_add_fstr(pinfo->cinfo, COL_INFO, "OLSR Packet,  Length: %u Bytes (not enough data in packet)", packet_len);
    return 0;
  }
  /
  col_set_str(pinfo->cinfo, COL_PROTOCOL, "OLSR v1");
  col_clear(pinfo->cinfo, COL_INFO);

  if ((pinfo->src.type != AT_IPv4) && (pinfo->src.type != AT_IPv6)) {
    col_add_fstr(pinfo->cinfo, COL_INFO, "OLSR (unknown address type) Packet,  Length: %u Bytes", packet_len);
    return 0;
  }
  if (pinfo->src.type == AT_IPv4) {
    col_add_fstr(pinfo->cinfo, COL_INFO, "OLSR (IPv4) Packet,  Length: %u Bytes", packet_len);
  } else if (pinfo->src.type == AT_IPv6) {
    col_add_fstr(pinfo->cinfo, COL_INFO, "OLSR (IPv6) Packet,  Length: %u Bytes", packet_len);
  }

  /
    ti = proto_tree_add_item(tree, proto_olsr, tvb, 0, -1, ENC_NA);
    olsr_tree = proto_item_add_subtree(ti, ett_olsr);

    proto_tree_add_item(olsr_tree, hf_olsr_packet_len, tvb, 0, 2, ENC_BIG_ENDIAN);
    proto_tree_add_item(olsr_tree, hf_olsr_packet_seq_num, tvb, 2, 2, ENC_BIG_ENDIAN);

    offset = 4;

    while (offset < packet_len) {
      proto_item *message_item;
      proto_tree *message_tree;

      if (packet_len - offset < 4) {
        proto_tree_add_expert_format(olsr_tree, pinfo, &ei_olsr_not_enough_bytes, tvb, offset, packet_len - offset,
            "Message too short !");
        break;
      }

      message_type = tvb_get_guint8(tvb, offset);
      vTime = getOlsrTime(tvb_get_guint8(tvb, offset + 1));
      message_len = tvb_get_ntohs(tvb, offset + 2);

      message_item = proto_tree_add_bytes_format_value(olsr_tree, hf_olsr_message, tvb, offset, message_len,
          NULL, "%s (%d)", val_to_str_const(message_type, message_type_vals, "UNKNOWN"),
          message_type);
      message_tree = proto_item_add_subtree(message_item, ett_olsr_message[message_type]);

      proto_tree_add_uint(message_tree, hf_olsr_message_type, tvb, offset, 1, message_type);
      offset++;

      /
      proto_tree_add_double_format_value(message_tree, hf_olsr_vtime, tvb, offset, 1, vTime,
          "%.3f (in seconds)", vTime);
      offset++;

      /
      ti = proto_tree_add_item(message_tree, hf_olsr_message_size, tvb, offset, 2, ENC_BIG_ENDIAN);
      offset += 2;

      if (message_len < 8 + pinfo->src.len) {
        proto_item_append_text(ti, "(too short, must be >= %d)", 8 + pinfo->src.len);
        break;
      }

      message_end = offset + message_len - 4;
      if (message_end > packet_len) {
        proto_item_append_string(ti, "(not enough data for message)");
        break;
      }

      /
      if (pinfo->src.type == AT_IPv4) {
        proto_tree_add_item(message_tree, hf_olsr_origin_addr, tvb, offset, 4, ENC_BIG_ENDIAN);
        offset += 4;
      } else if (pinfo->src.type == AT_IPv6) {
        proto_tree_add_item(message_tree, hf_olsr_origin6_addr, tvb, offset, 16, ENC_NA);
        offset += 16;
      } else {
        break; /
      }

      proto_tree_add_item(message_tree, hf_olsr_ttl, tvb, offset, 1, ENC_BIG_ENDIAN);
      proto_tree_add_item(message_tree, hf_olsr_hop_count, tvb, offset + 1, 1, ENC_BIG_ENDIAN);
      proto_tree_add_item(message_tree, hf_olsr_message_seq_num, tvb, offset + 2, 2, ENC_BIG_ENDIAN);
      offset += 4;

      if (offset < message_end) {
        /
        if (message_type == TC) {
          dissect_olsr_tc(tvb, pinfo, message_tree, offset, message_end);
        }
        /
        else if (message_type == HELLO) {
          dissect_olsr_hello(tvb, pinfo, message_tree, offset, message_end, &handle_olsr_hello_rfc);
        }
        /
        else if (message_type == MID) {
          dissect_olsr_mid(tvb, pinfo, message_tree, offset, message_end);
        }
        /
        else if (message_type == HNA) {
          dissect_olsr_hna(tvb, pinfo, message_tree, offset, message_end);
        }
        /
        else if (global_olsr_olsrorg && message_type == OLSR_ORG_LQ_HELLO) {
          dissect_olsr_hello(tvb, pinfo, message_tree, offset, message_end, &handle_olsr_hello_olsrorg);
        }
        /
        else if (global_olsr_olsrorg && message_type == OLSR_ORG_LQ_TC) {
          dissect_olsrorg_lq_tc(tvb, pinfo, message_tree, offset, message_end);
        }

        /
        else if (global_olsr_olsrorg && message_type == OLSR_ORG_NAMESERVICE) {
          dissect_olsrorg_nameservice(tvb, pinfo, message_tree, offset, message_end);
        }
        /
        else if (global_olsr_nrlolsr && message_type == NRLOLSR_TC_EXTRA) {
          dissect_nrlolsr_tc(tvb, pinfo, message_tree, offset, message_end);
        }

        /
        else {
          ti = proto_tree_add_bytes_format(message_tree, hf_olsr_data, tvb, offset, message_len - 12,
              NULL, "Data (%u bytes)", message_len - 12);
          if ((message_len - 12) % 4) {
              expert_add_info(pinfo, ti, &ei_olsr_data_misaligned);
            break;
          }
          /
        } /
      }
      offset = message_end;
    } /

  return tvb_length(tvb);
} /
