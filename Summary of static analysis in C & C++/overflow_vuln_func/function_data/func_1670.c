static void
dissect_isup_connection_request_parameter(tvbuff_t *parameter_tvb, proto_tree *parameter_tree, proto_item *parameter_item)
{
  guint32 local_ref;
  guint16 spc;
  guint8 protocol_class, credit, offset=0;

  local_ref = tvb_get_ntoh24(parameter_tvb, 0);
  proto_tree_add_text(parameter_tree, parameter_tvb, offset, LOCAL_REF_LENGTH, "Local Reference: %u", local_ref);
  offset = LOCAL_REF_LENGTH;
  spc = tvb_get_letohs(parameter_tvb,offset) & 0x3FFF; /
  proto_tree_add_text(parameter_tree, parameter_tvb, offset, SPC_LENGTH, "Signalling Point Code: %u", spc);
  offset += SPC_LENGTH;
  protocol_class = tvb_get_guint8(parameter_tvb, offset);
  proto_tree_add_text(parameter_tree, parameter_tvb, offset, PROTOCOL_CLASS_LENGTH, "Protocol Class: %u", protocol_class);
  offset += PROTOCOL_CLASS_LENGTH;
  credit = tvb_get_guint8(parameter_tvb, offset);
  proto_tree_add_text(parameter_tree, parameter_tvb, offset, CREDIT_LENGTH, "Credit: %u", credit);
  /

  proto_item_set_text(parameter_item, "Connection request: Local Reference = %u, SPC = %u, Protocol Class = %u, Credit = %u", local_ref, spc, protocol_class, credit);
}
