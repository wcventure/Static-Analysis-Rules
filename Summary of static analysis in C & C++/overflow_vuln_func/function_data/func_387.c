static int
dissect_eap(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, void *data _U_)
{
  guint8          eap_code;
  guint16         eap_len;
  guint8          eap_type;
  guint8          eap_identity_prefix;
  gint            len;
  conversation_t *conversation       = NULL;
  conv_state_t   *conversation_state;
  frame_state_t  *packet_state;
  int             leap_state;
  proto_tree     *ti;
  proto_tree     *eap_tree;
  proto_tree     *eap_identity_tree;
  proto_tree     *eap_tls_flags_tree;
  proto_item     *eap_type_item;
  proto_item     *eap_identity_item;

  col_set_str(pinfo->cinfo, COL_PROTOCOL, "EAP");
  col_clear(pinfo->cinfo, COL_INFO);

  eap_code = tvb_get_guint8(tvb, 0);

  col_add_str(pinfo->cinfo, COL_INFO,
                val_to_str(eap_code, eap_code_vals, "Unknown code (0x%02X)"));

  /
  if (!eap_maybe_from_server(pinfo, eap_code, FALSE)) {
    conversation = find_conversation(pinfo->num, &pinfo->dst, &pinfo->src,
                                     pinfo->ptype, pinfo->destport,
                                     0, NO_PORT_B);
  }
  if (conversation == NULL && eap_maybe_from_server(pinfo, eap_code, TRUE)) {
    conversation = find_conversation(pinfo->num, &pinfo->src, &pinfo->dst,
                                     pinfo->ptype, pinfo->srcport,
                                     0, NO_PORT_B);
  }
  if (conversation == NULL) {
    if (!eap_maybe_from_server(pinfo, eap_code, FALSE)) {
      conversation = conversation_new(pinfo->num, &pinfo->dst, &pinfo->src,
                                      pinfo->ptype, pinfo->destport,
                                      0, NO_PORT2);
    } else {
      conversation = conversation_new(pinfo->num, &pinfo->src, &pinfo->dst,
                                      pinfo->ptype, pinfo->srcport,
                                      0, NO_PORT2);
    }
  }

  /
  conversation_state = (conv_state_t *)conversation_get_proto_data(conversation, proto_eap);
  if (conversation_state == NULL) {
    /
    conversation_state = wmem_new(wmem_file_scope(), conv_state_t);
    conversation_state->eap_tls_seq      = -1;
    conversation_state->eap_reass_cookie =  0;
    conversation_state->leap_state       = -1;
    conversation_add_proto_data(conversation, proto_eap, conversation_state);
  }

  /
  if (eap_code == EAP_FAILURE)
    conversation_state->leap_state = -1;

  eap_len = tvb_get_ntohs(tvb, 2);
  len     = eap_len;

  ti = proto_tree_add_item(tree, proto_eap, tvb, 0, len, ENC_NA);
  eap_tree = proto_item_add_subtree(ti, ett_eap);

  proto_tree_add_item(eap_tree, hf_eap_code,       tvb, 0, 1, ENC_BIG_ENDIAN);
  proto_tree_add_item(eap_tree, hf_eap_identifier, tvb, 1, 1, ENC_BIG_ENDIAN);
  proto_tree_add_item(eap_tree, hf_eap_len,        tvb, 2, 2, ENC_BIG_ENDIAN);

  switch (eap_code) {

  case EAP_SUCCESS:
  case EAP_FAILURE:
    break;

  case EAP_REQUEST:
  case EAP_RESPONSE:
    eap_type = tvb_get_guint8(tvb, 4);

    col_append_fstr(pinfo->cinfo, COL_INFO, ", %s",
                      val_to_str_ext(eap_type, &eap_type_vals_ext,
                                     "Unknown type (0x%02x)"));
    eap_type_item = proto_tree_add_item(eap_tree, hf_eap_type, tvb, 4, 1, ENC_BIG_ENDIAN);

    if ((len > 5) || ((len == 5) && (eap_type == EAP_TYPE_ID))) {
      int     offset = 5;
      gint    size   = len - offset;

      switch (eap_type) {
        /
      case EAP_TYPE_ID:
        if (size > 0) {
          eap_identity_item = proto_tree_add_item(eap_tree, hf_eap_identity, tvb, offset, size, ENC_ASCII|ENC_NA);
          eap_identity_tree = proto_item_add_subtree(eap_identity_item, ett_identity);
          eap_identity_prefix = tvb_get_guint8(tvb, offset) - '0';
          proto_tree_add_string_format(eap_identity_tree, hf_eap_identity_prefix, tvb, offset, 1,
            &eap_identity_prefix, "Identity Prefix: %s (%u)",
            val_to_str(eap_identity_prefix, eap_identity_prefix_vals, "Unknown"),
            eap_identity_prefix);
        }
        if(!pinfo->fd->flags.visited) {
          conversation_state->leap_state  =  0;
          conversation_state->eap_tls_seq = -1;
        }
        break;

        /
      case EAP_TYPE_NOTIFY:
        proto_tree_add_item(eap_tree, hf_eap_notification, tvb,
            offset, size, ENC_ASCII|ENC_NA);
        break;

        /
      case EAP_TYPE_NAK:
        proto_tree_add_item(eap_tree, hf_eap_type_nak, tvb,
            offset, 1, ENC_BIG_ENDIAN);
        break;
        /
      case EAP_TYPE_MD5:
      {
        guint8      value_size = tvb_get_guint8(tvb, offset);
        gint        extra_len  = size - 1 - value_size;
        proto_item *item;

        /
        expert_add_info(pinfo, eap_type_item, &ei_eap_mitm_attacks);

        item = proto_tree_add_item(eap_tree, hf_eap_md5_value_size, tvb, offset, 1, ENC_BIG_ENDIAN);
        if (value_size > (size - 1))
        {
          expert_add_info(pinfo, item, &ei_eap_md5_value_size_overflow);
          value_size = size - 1;
        }

        offset += 1;
        proto_tree_add_item(eap_tree, hf_eap_md5_value, tvb, offset, value_size, ENC_NA);
        offset += value_size;
        if (extra_len > 0) {
          proto_tree_add_item(eap_tree, hf_eap_md5_extra_data, tvb, offset, extra_len, ENC_NA);
        }
      }
      break;

      /
      case EAP_TYPE_FAST:
      case EAP_TYPE_PEAP:
      case EAP_TYPE_TTLS:
      case EAP_TYPE_TLS:
      {
        guint8   flags            = tvb_get_guint8(tvb, offset);
        gboolean more_fragments;
        gboolean has_length;
        gboolean is_start;
        int      eap_tls_seq      = -1;
        guint32  eap_reass_cookie =  0;
        gboolean needs_reassembly =  FALSE;

        more_fragments = test_flag(flags,EAP_TLS_FLAG_M);
        has_length     = test_flag(flags,EAP_TLS_FLAG_L);
        is_start       = test_flag(flags,EAP_TLS_FLAG_S);

        if (is_start)
          conversation_state->eap_tls_seq = -1;

        /
        ti = proto_tree_add_item(eap_tree, hf_eap_tls_flags, tvb, offset, 1, ENC_BIG_ENDIAN);
        eap_tls_flags_tree = proto_item_add_subtree(ti, ett_eap_tls_flags);
        proto_tree_add_item(eap_tls_flags_tree, hf_eap_tls_flag_l, tvb, offset, 1, ENC_BIG_ENDIAN);
        proto_tree_add_item(eap_tls_flags_tree, hf_eap_tls_flag_m, tvb, offset, 1, ENC_BIG_ENDIAN);
        proto_tree_add_item(eap_tls_flags_tree, hf_eap_tls_flag_s, tvb, offset, 1, ENC_BIG_ENDIAN);

        if ((eap_type == EAP_TYPE_PEAP) || (eap_type == EAP_TYPE_TTLS) ||
            (eap_type == EAP_TYPE_FAST)) {
          proto_tree_add_item(eap_tls_flags_tree, hf_eap_tls_flags_version, tvb, offset, 1, ENC_BIG_ENDIAN);
        }
        size   -= 1;
        offset += 1;

        /
        if (has_length) {
          proto_tree_add_item(eap_tree, hf_eap_tls_len, tvb, offset, 4, ENC_BIG_ENDIAN);
          size   -= 4;
          offset += 4;
        }

        if (size > 0) {

          tvbuff_t *next_tvb;
          gint      tvb_len;
          gboolean  save_fragmented;

          tvb_len = tvb_captured_length_remaining(tvb, offset);
          if (size < tvb_len)
            tvb_len = size;

          /
          /
          packet_state = (frame_state_t *)p_get_proto_data(wmem_file_scope(), pinfo, proto_eap, 0);
          if (packet_state == NULL) {
            /
            if (!pinfo->fd->flags.visited) {
              /
              if (conversation_state->eap_tls_seq != -1) {
                /
                needs_reassembly = TRUE;
                conversation_state->eap_tls_seq++;

                eap_reass_cookie = conversation_state->eap_reass_cookie;
                eap_tls_seq = conversation_state->eap_tls_seq;
              } else if (more_fragments && has_length) {
                /
                needs_reassembly = TRUE;
                conversation_state->eap_reass_cookie = pinfo->num;

                /
                conversation_state->eap_tls_seq = 0;

                eap_tls_seq = conversation_state->eap_tls_seq;
                eap_reass_cookie = conversation_state->eap_reass_cookie;
              }

              if (needs_reassembly) {
                /
                packet_state = wmem_new(wmem_file_scope(), frame_state_t);
                packet_state->info = eap_reass_cookie;
                p_add_proto_data(wmem_file_scope(), pinfo, proto_eap, 0, packet_state);
              }
            }
          } else {
            /
            needs_reassembly = TRUE;
            eap_reass_cookie = packet_state->info;
            eap_tls_seq = 0;
          }

          /
          if (needs_reassembly) {
            fragment_head   *fd_head;

            /
            save_fragmented   = pinfo->fragmented;
            pinfo->fragmented = TRUE;
            fd_head = fragment_add_seq(&eap_tls_reassembly_table,
                                       tvb, offset,
                                       pinfo, eap_reass_cookie, NULL,
                                       eap_tls_seq,
                                       size,
                                       more_fragments, 0);

            if (fd_head != NULL)            /
            {
              proto_item *frag_tree_item;

              next_tvb = tvb_new_chain(tvb, fd_head->tvb_data);
              add_new_data_source(pinfo, next_tvb, "Reassembled EAP-TLS");

              show_fragment_seq_tree(fd_head, &eap_tls_frag_items,
                                     eap_tree, pinfo, next_tvb, &frag_tree_item);

              call_dissector(ssl_handle, next_tvb, pinfo, eap_tree);

              /
              if (!pinfo->fd->flags.visited)
                conversation_state->eap_tls_seq = -1;
            }

            pinfo->fragmented = save_fragmented;

          } else { /
            next_tvb = tvb_new_subset(tvb, offset, tvb_len, size);
            call_dissector(ssl_handle, next_tvb, pinfo, eap_tree);
          }
        }
      }
      break; /

      /
      case EAP_TYPE_LEAP:
      {
        guint8 count, namesize;

        /
        expert_add_info(pinfo, eap_type_item, &ei_eap_dictionary_attacks);

        /
        proto_tree_add_item(eap_tree, hf_eap_leap_version, tvb, offset, 1, ENC_BIG_ENDIAN);
        offset += 1;

        /
        proto_tree_add_item(eap_tree, hf_eap_leap_reserved, tvb, offset, 1, ENC_BIG_ENDIAN);
        offset += 1;

        /
        count = tvb_get_guint8(tvb, offset);
        proto_tree_add_item(eap_tree, hf_eap_leap_count, tvb, offset, 1, ENC_BIG_ENDIAN);
        offset += 1;

        /
        /

        /
        packet_state = (frame_state_t *)p_get_proto_data(wmem_file_scope(), pinfo, proto_eap, 0);
        if (packet_state == NULL) {
          /
          leap_state = conversation_state->leap_state;

          /
          if (leap_state==0) leap_state =  1; else
            if (leap_state==1) leap_state =  2; else
              if (leap_state==2) leap_state =  3; else
                if (leap_state==3) leap_state =  4; else
                  if (leap_state==4) leap_state = -1;

          /
          packet_state = wmem_new(wmem_file_scope(), frame_state_t);
          packet_state->info = leap_state;
          p_add_proto_data(wmem_file_scope(), pinfo, proto_eap, 0, packet_state);

          /
          conversation_state->leap_state = leap_state;
        }

        /
        leap_state = packet_state->info;

        switch (leap_state) {
          case 1:
            proto_tree_add_item(eap_tree, hf_eap_leap_peer_challenge, tvb, offset, count, ENC_NA);
            break;

          case 2:
            proto_tree_add_item(eap_tree, hf_eap_leap_peer_response, tvb, offset, count, ENC_NA);
            break;

          case 3:
            proto_tree_add_item(eap_tree, hf_eap_leap_ap_challenge, tvb, offset, count, ENC_NA);
            break;

          case 4:
            proto_tree_add_item(eap_tree, hf_eap_leap_ap_response, tvb, offset, count, ENC_NA);
            break;

          default:
            proto_tree_add_item(eap_tree, hf_eap_leap_data, tvb, offset, count, ENC_NA);
            break;
        }

        offset += count;

        /
        namesize = eap_len - (8+count);
        proto_tree_add_item(eap_tree, hf_eap_leap_name, tvb, offset, namesize, ENC_ASCII|ENC_NA);
      }

      break; /

      /
      case EAP_TYPE_MSCHAPV2:
        dissect_eap_mschapv2(eap_tree, tvb, pinfo, offset, size);
        break; /

        /
      case EAP_TYPE_SIM:
        dissect_eap_sim(eap_tree, tvb, offset, size);
        break; /

        /
      case EAP_TYPE_AKA:
      case EAP_TYPE_AKA_PRIME:
        dissect_eap_aka(eap_tree, tvb, offset, size);
        break; /

        /
      case EAP_TYPE_EXT:
      {
        proto_tree *exptree;

        exptree   = proto_tree_add_subtree(eap_tree, tvb, offset, size, ett_eap_exp_attr, NULL, "Expanded Type");
        dissect_exteap(exptree, tvb, offset, size, pinfo);
      }
      break;

      /
      default:
        proto_tree_add_item(eap_tree, hf_eap_data, tvb, offset, size, ENC_NA);
        break;
        /
      } /

    }

  } /

  return tvb_captured_length(tvb);
}
