static int
dissect_websocket(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, void *data _U_)
{
  proto_item *ti, *ti_len;
  guint8 fin, opcode, mask;
  int length, short_length, payload_length, recurse_length;
  int payload_offset, mask_offset, recurse_offset;
  proto_tree *ws_tree = NULL;
  const guint8 *masking_key = NULL;
  tvbuff_t *tvb_payload = NULL;

  length = tvb_length(tvb);
  if(length<2){
    pinfo->desegment_len = 2;
    return 0;
  }

  short_length = tvb_get_guint8(tvb, 1) & MASK_WS_PAYLOAD_LEN;
  if(short_length==126){
    if(length < 2+2){
      pinfo->desegment_len = 2+2;
      return 0;
    }
    payload_length = (guint64)tvb_get_ntohs(tvb, 2);
    mask_offset = 2+2;
  }
  else if(short_length==127){
    if(length < 2+8){
      pinfo->desegment_len = 2+8;
      return 0;
    }
    payload_length = (guint64)tvb_get_ntoh64(tvb, 2);
    mask_offset = 2+8;
  }
  else{
    payload_length = short_length;
    mask_offset = 2;
  }

  /
  mask = (tvb_get_guint8(tvb, 1) & MASK_WS_MASK) >> 4;
  payload_offset = mask_offset + (mask ? 4 : 0);

  if(length < payload_offset + payload_length){
    pinfo->desegment_len = payload_offset + payload_length - length;
    return 0;
  }

  /

  col_set_str(pinfo->cinfo, COL_PROTOCOL, "WebSocket");
  col_set_str(pinfo->cinfo, COL_INFO, "WebSocket");

  if(tree){
    ti = proto_tree_add_item(tree, proto_websocket, tvb, 0, payload_offset, ENC_NA);
    ws_tree = proto_item_add_subtree(ti, ett_ws);
  }

  /
  proto_tree_add_item(ws_tree, hf_ws_fin, tvb, 0, 1, ENC_NA);
  fin = (tvb_get_guint8(tvb, 0) & MASK_WS_FIN) >> 4;
  proto_tree_add_item(ws_tree, hf_ws_reserved, tvb, 0, 1, ENC_NA);

  /
  proto_tree_add_item(ws_tree, hf_ws_opcode, tvb, 0, 1, ENC_NA);
  opcode = tvb_get_guint8(tvb, 0) & MASK_WS_OPCODE;
  col_append_fstr(pinfo->cinfo, COL_INFO, " %s", val_to_str_const(opcode, ws_opcode_vals, "Unknown Opcode"));
  col_append_fstr(pinfo->cinfo, COL_INFO, " %s", fin ? "[FIN]" : "");

  /
  proto_tree_add_item(ws_tree, hf_ws_mask, tvb, 1, 1, ENC_NA);
  col_append_fstr(pinfo->cinfo, COL_INFO, " %s", mask ? "[MASKED]" : "");

  /
  ti_len = proto_tree_add_item(ws_tree, hf_ws_payload_length, tvb, 1, 1, ENC_NA);
  if(short_length==126){
    proto_item_append_text(ti_len, " Extended Payload Length (16 bits)");
    proto_tree_add_item(ws_tree, hf_ws_payload_length_ext_16, tvb, 2, 2, ENC_BIG_ENDIAN);
  }
  else if(short_length==127){
    proto_item_append_text(ti_len, " Extended Payload Length (64 bits)");
    proto_tree_add_item(ws_tree, hf_ws_payload_length_ext_64, tvb, 2, 8, ENC_BIG_ENDIAN);
  }

  /
  if(mask){
    proto_tree_add_item(ws_tree, hf_ws_masking_key, tvb, mask_offset, 4, ENC_NA);
    masking_key = tvb_get_ptr(tvb, mask_offset, 4);
  }

  tvb_payload = tvb_new_subset_remaining(tvb, payload_offset);
  dissect_websocket_payload(tvb_payload, pinfo, tree, ws_tree, opcode, payload_length, mask, masking_key);

  /

  recurse_offset = payload_offset + payload_length;
  if(length > recurse_offset){
    recurse_length = dissect_websocket(tvb_new_subset_remaining(tvb, payload_offset+payload_length), pinfo, tree, data);
    if(pinfo->desegment_len) pinfo->desegment_offset += recurse_offset;
    return recurse_offset + recurse_length;
  }
  return recurse_offset;
}
