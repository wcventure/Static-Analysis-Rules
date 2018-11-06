static void
dissect_gsm_cell_broadcast(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree)
{
   guint8 sms_encoding, total_pages, current_page;
   guint32       offset = 0;
   guint         len, text_len;
   guint32       msg_key;
   proto_item    *cbs_page_item = NULL;
   proto_tree    *cbs_page_tree = NULL;
   guint16       serial_number, message_id;
   tvbuff_t      *cbs_page_tvb = NULL;
   tvbuff_t      *cbs_msg_tvb = NULL;
   fragment_head * frag_data = NULL;

   len = tvb_length(tvb);

   col_append_str(pinfo->cinfo, COL_PROTOCOL, " Cell Broadcast");
   col_append_str(pinfo->cinfo, COL_INFO, " (CBS Page)");

   cbs_page_item = proto_tree_add_protocol_format(proto_tree_get_root(tree), proto_cell_broadcast, tvb, 0, len, "GSM Cell Broadcast");
   cbs_page_tree = proto_item_add_subtree(cbs_page_item, ett_gsm_cbs_page);

   serial_number = tvb_get_ntohs(tvb, offset);
   offset += dissect_cbs_serial_number(tvb, cbs_page_tree, offset);
   message_id = tvb_get_ntohs(tvb, offset);
   offset += dissect_cbs_message_identifier(tvb, cbs_page_tree, offset);
   sms_encoding = dissect_cbs_data_coding_scheme(tvb, pinfo, cbs_page_tree, offset++);
   total_pages = tvb_get_guint8(tvb, offset);
   current_page = (total_pages & 0xF0) >> 4;
   total_pages &= 0x0F;
   proto_tree_add_item(cbs_page_tree, hf_gsm_cbs_current_page, tvb, offset, 1, ENC_BIG_ENDIAN);
   proto_tree_add_item(cbs_page_tree, hf_gsm_cbs_total_pages, tvb, offset++, 1, ENC_BIG_ENDIAN);
   cbs_page_tvb = dissect_cbs_data(sms_encoding, tvb, cbs_page_tree, pinfo, offset );

   if (cbs_page_tvb != NULL)
   {
      text_len = tvb_length(cbs_page_tvb);
      while (text_len && (tvb_get_guint8(cbs_page_tvb, text_len-1) == '\r')) {
         text_len--;
      }
      if (tree != NULL)
      {
         proto_item *item = proto_tree_add_text(cbs_page_tree, tvb, offset, -1, "Cell Broadcast Page Contents");
         proto_tree *cbs_page_subtree = proto_item_add_subtree(item, ett_gsm_cbs_page_content);
         len = tvb_length(cbs_page_tvb);
         proto_tree_add_string(cbs_page_subtree, hf_gsm_cbs_page_content, cbs_page_tvb, 0,
                               text_len, tvb_get_string(wmem_packet_scope(), cbs_page_tvb, 0, text_len));
         len -= text_len;
         if (len)
         {
            proto_tree_add_string(cbs_page_subtree, hf_gsm_cbs_page_content_padding, cbs_page_tvb, text_len, len,
                                  tvb_get_string(wmem_packet_scope(), cbs_page_tvb, text_len, len));
         }
      }
      if (text_len)
      {
         cbs_page_tvb = tvb_new_subset(cbs_page_tvb, 0, text_len, text_len);
         if (total_pages == 1)
         {
            /
            cbs_msg_tvb = cbs_page_tvb;
         }
         else
         {
             /
            /
            msg_key = (serial_number << 16) + message_id;
            frag_data = fragment_add_seq_check(&gsm_cbs_reassembly_table,
                                               cbs_page_tvb, 0, pinfo, msg_key, NULL,
                                               (current_page -1), text_len,
                                               (current_page!=total_pages));
            cbs_msg_tvb = process_reassembled_data(cbs_page_tvb, 0, pinfo, "Reassembled Cell Broadcast message",
                                                frag_data, &gsm_page_items, NULL, cbs_page_tree);
         }
      }
   }
   if (cbs_msg_tvb != NULL)
   {
      proto_item     *cbs_msg_item = NULL;
      proto_tree    *cbs_msg_tree = NULL;

      len = tvb_length(cbs_msg_tvb);
      col_append_str(pinfo->cinfo, COL_INFO, " (CBS Message)");

      cbs_msg_item = proto_tree_add_protocol_format(proto_tree_get_root(tree), proto_cell_broadcast, cbs_msg_tvb, 0, len, "GSM Cell Broadcast Message");
      cbs_msg_tree = proto_item_add_subtree(cbs_msg_item, ett_cbs_msg);

      proto_tree_add_string(cbs_msg_tree, hf_gsm_cbs_message_content, cbs_msg_tvb, 0, len, tvb_get_string(wmem_packet_scope(), cbs_msg_tvb, 0, len));
   }
}
