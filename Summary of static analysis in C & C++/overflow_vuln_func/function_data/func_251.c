static void dissect_x11_requests(tvbuff_t *tvb, packet_info *pinfo,
    proto_tree *tree)
{
      volatile int offset = 0;
      int length_remaining;
      volatile guint byte_order;
      guint8 opcode;
      volatile int plen;
      proto_item *ti;
      volatile gboolean is_initial_creq;
      guint16 auth_proto_len, auth_data_len;
      const char *volatile sep = NULL;
      conversation_t *conversation;
      x11_conv_data_t *volatile state;
      int length;
      tvbuff_t *volatile next_tvb;

      while ((length_remaining = tvb_reported_length_remaining(tvb, offset)) > 0) {

            /
            if (x11_desegment && pinfo->can_desegment) {
                  /
                  if (length_remaining < 4) {
                        /
                        pinfo->desegment_offset = offset;
                        pinfo->desegment_len = DESEGMENT_ONE_MORE_SEGMENT;
                        return;
                  }
            }

            /
            conversation = find_or_create_conversation(pinfo);

            /
            if ((state = (x11_conv_data_t *)conversation_get_proto_data(conversation, proto_x11))
            == NULL)
                state = x11_stateinit(conversation);

            /
            byte_order = guess_byte_ordering(tvb, pinfo, state);

            /
            opcode = tvb_get_guint8(tvb, 0);
            plen = tvb_get_guint16(tvb, offset + 2, byte_order);

            if (plen == 0) {
                  /
                  plen = tvb_get_guint32(tvb, offset + 4, byte_order);
            }

            if (plen <= 0) {
                  /
                  ti = proto_tree_add_item(tree, proto_x11, tvb, offset, -1, ENC_NA);
                  expert_add_info_format(pinfo, ti, &ei_x11_request_length, "Bogus request length (%d)", plen);
                  return;
            }

            if (state->iconn_frame == pinfo->num ||
                (wmem_map_lookup(state->seqtable,
                GINT_TO_POINTER(state->sequencenumber)) == (int *)NOTHING_SEEN &&
                 (opcode == 'B' || opcode == 'l') &&
                 (plen == 11 || plen == 2816))) {
                  /
                  is_initial_creq = TRUE;

                  /
                  if (state->byte_order == BYTE_ORDER_UNKNOWN) {
                        if (opcode == 'B') {
                              /
                              byte_order = state->byte_order = ENC_BIG_ENDIAN;
                        } else {
                              /
                              byte_order = state->byte_order = ENC_LITTLE_ENDIAN;
                        }
                  }

                  /
                  if (x11_desegment && pinfo->can_desegment) {
                        /
                        if (length_remaining < 10) {
                              /
                              pinfo->desegment_offset = offset;
                              pinfo->desegment_len = DESEGMENT_ONE_MORE_SEGMENT;
                              return;
                        }
                  }

                  /
                  auth_proto_len = tvb_get_guint16(tvb, offset + 6, byte_order);
                  auth_data_len = tvb_get_guint16(tvb, offset + 8, byte_order);
                  plen = 12 + ROUND_LENGTH(auth_proto_len) +
                        ROUND_LENGTH(auth_data_len);
            } else {
                  /
                  is_initial_creq = FALSE;

                  /
                  plen *= 4;
            }

            /
            if (x11_desegment && pinfo->can_desegment) {
                  /
                  if (length_remaining < plen) {
                        /
                        pinfo->desegment_offset = offset;
                        pinfo->desegment_len = plen - length_remaining;
                        return;
                  }
            }

            /
            length = length_remaining;
            if (length > plen)
                  length = plen;
            next_tvb = tvb_new_subset_length_caplen(tvb, offset, length, plen);

            /
            if (is_initial_creq) {
                  col_set_str(pinfo->cinfo, COL_INFO, "Initial connection request");
            } else {
                  if (sep == NULL) {
                        /
                        col_set_str(pinfo->cinfo, COL_INFO, "Requests");

                        /
                        sep = ":";
                  }
            }

            /
            TRY {
                  if (is_initial_creq) {
                        dissect_x11_initial_conn(next_tvb, pinfo, tree,
                            state, byte_order);
                  } else {
                        dissect_x11_request(next_tvb, pinfo, tree, sep,
                            state, byte_order);
                  }
            }
            CATCH_NONFATAL_ERRORS {
                  show_exception(tvb, pinfo, tree, EXCEPT_CODE, GET_MESSAGE);
            }
            ENDTRY;

            /
            offset += plen;

            sep = ",";
      }
}
