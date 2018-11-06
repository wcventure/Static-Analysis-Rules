guint16 dissect_cbs_serial_number(tvbuff_t *tvb, proto_tree *tree, guint16 offset)
{
   guint16 serial_number = tvb_get_ntohs(tvb, offset) ;
   proto_item *item;
   proto_tree *subtree;

   item = proto_tree_add_item(tree, hf_gsm_cbs_serial_number, tvb, offset, 2, ENC_BIG_ENDIAN);
   proto_item_append_text(item, ", Message Code: %d, Update Number: %d", (serial_number & 0x3FF) >> 4, serial_number & 0x0F);
   subtree = proto_item_add_subtree(item, ett_cbs_serial_no);
   proto_tree_add_item(subtree, hf_gsm_cbs_geographic_scope, tvb, offset, 2, ENC_BIG_ENDIAN);
   proto_tree_add_item(subtree, hf_gsm_cbs_message_code, tvb, offset, 2, ENC_BIG_ENDIAN);
   proto_tree_add_item(subtree, hf_gsm_cbs_update_number, tvb, offset, 2, ENC_BIG_ENDIAN);
   offset +=2;
   return offset;
}
