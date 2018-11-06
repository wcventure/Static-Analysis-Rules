static void
dissect_glbp_auth(tvbuff_t *tvb, int offset, guint32 length,
	packet_info *pinfo _U_, proto_tree *tlv_tree)
{
  guint32 authtype;
  guint32 authlength;

  proto_tree_add_item(tlv_tree, hf_glbp_auth_authtype, tvb, offset, 1,  FALSE);
  authtype = tvb_get_guint8(tvb, offset);
  offset++;
  proto_tree_add_item(tlv_tree, hf_glbp_auth_authlength, tvb, offset, 1,  FALSE);
  authlength = tvb_get_guint8(tvb, offset);
  offset++;
  switch(authtype) {
  case 1:
    proto_tree_add_item(tlv_tree, hf_glbp_auth_plainpass, tvb, offset, authlength, FALSE);
    break;
  case 2:
    proto_tree_add_item(tlv_tree, hf_glbp_auth_md5hash, tvb, offset, authlength, FALSE);
    break;
  case 3:
    proto_tree_add_item(tlv_tree, hf_glbp_auth_md5chainindex, tvb, offset, 4, FALSE);
    proto_tree_add_item(tlv_tree, hf_glbp_auth_md5chainhash, tvb, offset+4, authlength-4, FALSE);
    break;
  default:
    proto_tree_add_item(tlv_tree, hf_glbp_auth_authunknown, tvb, offset, authlength, FALSE);
    break;
  }
  offset += length;
}
