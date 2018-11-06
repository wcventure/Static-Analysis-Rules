void dissect_umts_cell_broadcast_message(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree)
{
   guint8 sms_encoding;
   guint32       offset = 0;
   guint32       len;
   proto_item    *cbs_item = NULL, *cbs_item2 = NULL;
   proto_tree    *cbs_tree = NULL, *cbs_subtree = NULL;
   guint16  msg_len;
   tvbuff_t * cbs_msg_tvb = NULL;

   len = tvb_length(tvb);

   col_append_str(pinfo->cinfo, COL_PROTOCOL, " Cell Broadcast");
   col_append_str(pinfo->cinfo, COL_INFO, " (CBS Message)");

   cbs_item = proto_tree_add_protocol_format(proto_tree_get_root(tree), proto_cell_broadcast, tvb, 0, len, "Cell Broadcast");
   cbs_tree = proto_item_add_subtree(cbs_item, ett_cbs_msg);

   sms_encoding = dissect_cbs_data_coding_scheme(tvb, pinfo, cbs_tree, 0);
   offset++;
   cbs_msg_tvb = dissect_cbs_data(sms_encoding, tvb, cbs_tree, pinfo, offset );

   msg_len = tvb_length(cbs_msg_tvb);
   cbs_item2 = proto_tree_add_text(cbs_tree, tvb, offset, -1, "Cell Broadcast Message Contents (length: %d)", msg_len);
   cbs_subtree = proto_item_add_subtree(cbs_item2, ett_cbs_msg);
   proto_tree_add_text(cbs_subtree, cbs_msg_tvb , 0, tvb_length(cbs_msg_tvb), "%s", tvb_get_string(wmem_packet_scope(), cbs_msg_tvb, 0, msg_len));
}
