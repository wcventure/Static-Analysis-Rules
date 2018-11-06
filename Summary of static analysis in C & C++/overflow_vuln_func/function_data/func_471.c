static void dissect_rev_qosinfo(tvbuff_t* tvb, int offset, proto_tree* ext_tree)
{
   int clen = 0; /
   guint8 srid = 0;
   guint8 flow_count = 0;
   guint8 flow_index = 0;

   /
   srid = tvb_get_guint8(tvb, offset+clen); 
   proto_tree_add_item(ext_tree, hf_a11_rqi_srid, tvb, offset+clen, 1, FALSE);
   clen++;

   /
   flow_count = tvb_get_guint8(tvb, offset+clen);
   flow_count &= 0x1F;
   proto_tree_add_item(ext_tree, hf_a11_rqi_flowcount, tvb, offset+clen, 1, FALSE);
   clen++;

   for(flow_index=0; flow_index<flow_count; flow_index++)
   {
      guint8 requested_qos_len = 0;
      guint8 granted_qos_len = 0;

      guint8 entry_len = tvb_get_guint8(tvb, offset+clen);
      guint8 flow_id = tvb_get_guint8(tvb, offset+clen+1);

      proto_item* ti = proto_tree_add_text
          (ext_tree, tvb, offset+clen, entry_len+1, "Reverse Flow Entry (Flow Id: %d)", flow_id);

      proto_tree* exts_tree = proto_item_add_subtree(ti, ett_a11_rqi_flowentry);
   
      /
      proto_tree_add_item(exts_tree, hf_a11_rqi_entrylen, tvb, offset+clen, 1, FALSE);
      clen++;

      /
      proto_tree_add_item(exts_tree, hf_a11_rqi_flowid, tvb, offset+clen, 1, FALSE);
      clen++;

      /
      dissect_rqi_entry_flags(tvb, offset+clen, exts_tree);
      clen++;

      /
      requested_qos_len = tvb_get_guint8(tvb, offset+clen);
      proto_tree_add_item(exts_tree, hf_a11_rqi_requested_qoslen, tvb, offset+clen, 1, FALSE);
      clen++;

      /
      if(requested_qos_len)
      {
        proto_tree_add_item
          (exts_tree, hf_a11_rqi_requested_qos, tvb, offset+clen, requested_qos_len, FALSE);
        clen += requested_qos_len;
      }

      /
      granted_qos_len = tvb_get_guint8(tvb, offset+clen);
      proto_tree_add_item(exts_tree, hf_a11_rqi_granted_qoslen, tvb, offset+clen, 1, FALSE);
      clen++;

      /
      if(granted_qos_len)
      {
        proto_tree_add_item
          (exts_tree, hf_a11_rqi_granted_qos, tvb, offset+clen, granted_qos_len, FALSE);
        clen += granted_qos_len;
      }
   }
}
