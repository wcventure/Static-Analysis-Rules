static int
dissect_glbp(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree)
{
  proto_tree *glbp_tree = NULL;
  proto_tree *tlv_tree = NULL;
  proto_item *ti = NULL;
  guint32 type;
  int offset = 0;
  guint32 length;
  guint32 group;

  group = tvb_get_ntohs(tvb, 2);

  col_set_str(pinfo->cinfo, COL_PROTOCOL, "GLBP");
  col_add_fstr(pinfo->cinfo, COL_INFO, "G: %d", group);

  if (tree) {
    ti = proto_tree_add_item(tree, proto_glbp, tvb, 0, -1, FALSE);
    glbp_tree = proto_item_add_subtree(ti, ett_glbp);

    /
    proto_tree_add_item(glbp_tree, hf_glbp_version, tvb, offset, 1,  FALSE);
    offset++;
    proto_tree_add_item(glbp_tree, hf_glbp_unknown1, tvb, offset, 1,  FALSE);
    offset++;
    proto_tree_add_item(glbp_tree, hf_glbp_group, tvb, offset, 2,  FALSE);
    offset += 2;
    proto_tree_add_item(glbp_tree, hf_glbp_unknown2, tvb, offset, 2,  FALSE);
    offset += 2;
    proto_tree_add_item(glbp_tree, hf_glbp_ownerid, tvb, offset, 6, FALSE);
    offset += 6;
    while (tvb_length_remaining(tvb, offset) > 0) {

      type = tvb_get_guint8(tvb, offset);
      length = tvb_get_guint8(tvb, offset+1) - 2;

      ti = proto_tree_add_item(glbp_tree, hf_glbp_tlv, tvb, offset, length+2, FALSE);
      tlv_tree = proto_item_add_subtree(ti, ett_glbp_tlv);
      proto_item_append_text(ti, " l=%d, t=%s", length+2,
        val_to_str(type, glbp_type_vals, "%d"));

      proto_tree_add_item(tlv_tree, hf_glbp_type, tvb, offset, 1,  FALSE);
      offset++;
      proto_tree_add_item(tlv_tree, hf_glbp_length, tvb, offset, 1,  FALSE);
      offset++;
      col_append_fstr(pinfo->cinfo, COL_INFO, ", %s",
        val_to_str(type, glbp_type_vals, "%d"));
  
      switch(type) {
        case 1: /
	  dissect_glbp_hello(tvb, offset, length, pinfo, tlv_tree);
  	break;
        case 2: /
	  dissect_glbp_reqresp(tvb, offset, length, pinfo, tlv_tree);
  	break;
        case 3: /
	  dissect_glbp_auth(tvb, offset, length, pinfo, tlv_tree);
  	break;
        default:
	  dissect_glbp_unknown(tvb, offset, length, pinfo, tlv_tree);
  	break;
      }
      offset += length;
    }
  }
  return offset;
}
