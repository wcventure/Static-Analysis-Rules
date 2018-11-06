static void
display_req_body(tvbuff_t *tvb, packet_info *pinfo,
                 proto_tree *ajp13_tree)
{
  /

  if (ajp13_tree) {

    guint8 body_bytes[128*1024]; /
    int pos = 0;
    guint16 body_len;

    /
    proto_tree_add_item(ajp13_tree, hf_ajp13_magic, tvb, pos, 2, 0);
    pos+=2;

    /
    proto_tree_add_item(ajp13_tree, hf_ajp13_len, tvb, pos, 2, 0);
    pos+=2;
    
    /
    body_len = get_nstring(tvb, pos, body_bytes);
    proto_tree_add_item(ajp13_tree, hf_ajp13_data, tvb, pos+2, body_len-1, 0);
  }
}
