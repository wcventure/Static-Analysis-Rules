static void
dissect_x11_replies(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree)
{
/
      volatile int offset, plen;
      tvbuff_t * volatile next_tvb;
      conversation_t *conversation;
      x11_conv_data_t *volatile state;
      volatile gboolean little_endian;
      int length_remaining;
      const char *volatile sep = NULL;


      /
      conversation = find_or_create_conversation(pinfo);

      /
      if ((state = conversation_get_proto_data(conversation, proto_x11))
          == NULL) {
            /
            state = x11_stateinit(conversation);
      }

      /
      little_endian = guess_byte_ordering(tvb, pinfo, state);

      offset = 0;
      while (tvb_reported_length_remaining(tvb, offset) != 0) {
            /
            length_remaining = tvb_ensure_length_remaining(tvb, offset);

            /
            if (x11_desegment && pinfo->can_desegment) {
                  /
                  if (length_remaining < 8) {
                        /
                        pinfo->desegment_offset = offset;
                        pinfo->desegment_len = DESEGMENT_ONE_MORE_SEGMENT;
                        return;
                  }
            }

            /
            if (g_hash_table_lookup(state->seqtable,
                                    GINT_TO_POINTER(state->sequencenumber)) == (int *)INITIAL_CONN
                || (state->iconn_reply == pinfo->fd->num)) {
                  /
                  plen = 8 + VALUE16(tvb, offset + 6) * 4;

                  HANDLE_REPLY(plen, length_remaining,
                               "Initial connection reply",
                               dissect_x11_initial_reply);
            } else {
                  /
                  switch (tvb_get_guint8(tvb, offset)) {

                        case 0:
                              plen = 32;
                              HANDLE_REPLY(plen, length_remaining,
                                           "Error", dissect_x11_error);
                              break;

                        case 1:
                        {
                              /
                              int tmp_plen;

                              /
                              tmp_plen = plen = 32 + VALUE32(tvb, offset + 4) * 4;
                              DISSECTOR_ASSERT(tmp_plen >= 32);
                              HANDLE_REPLY(plen, length_remaining,
                                           "Reply", dissect_x11_reply);
                              break;
                        }

                        default:
                              /
                              plen = 32;
                              HANDLE_REPLY(plen, length_remaining,
                                           "Event", dissect_x11_event);
                              break;
                  }
            }

            offset += plen;
      }

      return;
}
