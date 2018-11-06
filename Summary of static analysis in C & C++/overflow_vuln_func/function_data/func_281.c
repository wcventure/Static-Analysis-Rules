static int dissect_cops_pr_object_data(tvbuff_t *tvb, guint32 offset, proto_tree *tree,
                                       guint8 s_num, guint8 s_type, guint16 len)
{
  proto_item *ti;
  proto_tree *asn1_object_tree, *gperror_tree, *cperror_tree;
  guint16 gperror=0, gperror_sub=0, cperror=0, cperror_sub=0;

  switch (s_num){
  case COPS_OBJ_PRID:
   if (s_type != 1) /
      break;

    ti=proto_tree_add_text(tree, tvb, offset, len, "Contents:");
    asn1_object_tree = proto_item_add_subtree(ti, ett_cops_asn1);

    decode_cops_pr_asn1_data(tvb, offset, asn1_object_tree, len, COPS_OBJ_PRID);

    break;
  case COPS_OBJ_PPRID:
    if (s_type != 1) /
      break;

    ti = proto_tree_add_text(tree, tvb, offset, len, "Contents:");
    asn1_object_tree = proto_item_add_subtree(ti, ett_cops_asn1);

    decode_cops_pr_asn1_data(tvb, offset, asn1_object_tree, len, COPS_OBJ_PPRID);

    break;
  case COPS_OBJ_EPD:
    if (s_type != 1) /
      break;

    ti = proto_tree_add_text(tree, tvb, offset, len, "Contents:");
    asn1_object_tree = proto_item_add_subtree(ti, ett_cops_asn1);

    decode_cops_pr_asn1_data(tvb, offset, asn1_object_tree, len, COPS_OBJ_EPD);

    break;
  case COPS_OBJ_GPERR:
    if (s_type != 1) /
      break;

    gperror = tvb_get_ntohs(tvb, offset);
    gperror_sub = tvb_get_ntohs(tvb, offset + 2);
    ti = proto_tree_add_text(tree, tvb, offset, 4, "Contents: Error-Code: %s, Error Sub-code: 0x%04x",
                       val_to_str(gperror, cops_gperror_vals, "<Unknown value>"), gperror_sub);
    gperror_tree = proto_item_add_subtree(ti, ett_cops_gperror);
    proto_tree_add_uint(gperror_tree, hf_cops_gperror, tvb, offset, 2, gperror);
    offset += 2;
    if (cperror == 13) {
      proto_tree_add_text(gperror_tree, tvb, offset, 2, "Error Sub-code: "
                          "Unknown object's C-Num %u, C-Type %u",
                          tvb_get_guint8(tvb, offset), tvb_get_guint8(tvb, offset + 1));
    } else
      proto_tree_add_uint(gperror_tree, hf_cops_gperror_sub, tvb, offset, 2, gperror_sub);

    break;
  case COPS_OBJ_CPERR:
    if (s_type != 1) /
      break;

    break;

    cperror = tvb_get_ntohs(tvb, offset);
    cperror_sub = tvb_get_ntohs(tvb, offset + 2);
    ti = proto_tree_add_text(tree, tvb, offset, 4, "Contents: Error-Code: %s, Error Sub-code: 0x%04x",
                       val_to_str(cperror, cops_cperror_vals, "<Unknown value>"), cperror_sub);
    cperror_tree = proto_item_add_subtree(ti, ett_cops_cperror);
    proto_tree_add_uint(cperror_tree, hf_cops_cperror, tvb, offset, 2, cperror);
    offset += 2;
    if (cperror == 13) {
      proto_tree_add_text(cperror_tree, tvb, offset, 2, "Error Sub-code: "
                          "Unknown object's S-Num %u, C-Type %u",
                          tvb_get_guint8(tvb, offset), tvb_get_guint8(tvb, offset + 1));
    } else
      proto_tree_add_uint(cperror_tree, hf_cops_cperror_sub, tvb, offset, 2, cperror_sub);

    break;
  case COPS_OBJ_ERRPRID:
    if (s_type != 1) /
      break;

    ti = proto_tree_add_text(tree, tvb, offset, len, "Contents:");
    asn1_object_tree = proto_item_add_subtree(ti, ett_cops_asn1);

    decode_cops_pr_asn1_data(tvb, offset, asn1_object_tree, len, COPS_OBJ_ERRPRID);

    break;
  default:
    proto_tree_add_text(tree, tvb, offset, len, "Contents: %u bytes", len);
    break;
  }

  return 0;
}
