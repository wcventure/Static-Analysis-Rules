static void dissect_fwd_qosupdate_info(tvbuff_t* tvb, int offset, proto_tree* ext_tree)
{
   int clen = 0; /
   guint8 flow_count = 0;
   guint8 flow_index = 0;

   /
   flow_count = tvb_get_guint8(tvb, offset+clen);
   proto_tree_add_item(ext_tree, hf_a11_fqui_flowcount, tvb, offset+clen, 1, FALSE);
   clen++;

   for(flow_index=0; flow_index<flow_count; flow_index++)
   {
      proto_tree* exts_tree = 0;
      guint8 granted_qos_len = 0;

      guint8 flow_id = tvb_get_guint8(tvb, offset+clen);

      proto_item* ti = proto_tree_add_text
          (ext_tree, tvb, offset+clen, 1, "Forward Flow Entry (Flow Id: %d)", flow_id);

      clen++;
      exts_tree = proto_item_add_subtree(ti, ett_a11_fqui_flowentry);
   
      /
      granted_qos_len = tvb_get_guint8(tvb, offset+clen);
      proto_tree_add_item
        (exts_tree, hf_a11_fqui_updated_qoslen, tvb, offset+clen, 1, FALSE);
      clen++;

      /
      if(granted_qos_len)
      {
        proto_tree_add_item
          (exts_tree, hf_a11_fqui_updated_qos, tvb, offset+clen, 
              granted_qos_len, FALSE);
        clen += granted_qos_len;
      }
   }
}
