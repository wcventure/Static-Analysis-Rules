static void
dissect_a11_extensions( tvbuff_t *tvb, int offset, proto_tree *tree)
{
  proto_item   *ti;
  proto_tree   *exts_tree=NULL;
  proto_tree   *ext_tree;
  size_t        ext_len;
  guint8        ext_type;
  guint8        ext_subtype=0;
  size_t        hdrLen;

  gint16       apptype = -1;

  /
  if (!tree) return;

  /
  ti = proto_tree_add_text(tree, tvb, offset, -1, "Extensions");
  exts_tree = proto_item_add_subtree(ti, ett_a11_exts);

  /
  while (tvb_reported_length_remaining(tvb, offset) > 0) {

    /
    ext_type = tvb_get_guint8(tvb, offset);
    if (ext_type == GEN_AUTH_EXT) {
      /
      ext_subtype = tvb_get_guint8(tvb, offset + 1);
      ext_len = tvb_get_ntohs(tvb, offset + 2);
      hdrLen = 4;
    } else if (ext_type == CVSE_EXT || ext_type == OLD_CVSE_EXT) {
      ext_len = tvb_get_ntohs(tvb, offset + 2);
      ext_subtype = tvb_get_guint8(tvb, offset + 8);    
      hdrLen = 4;
    } else {
      ext_len = tvb_get_guint8(tvb, offset + 1);
      hdrLen = 2;
    }
    
    ti = proto_tree_add_text(exts_tree, tvb, offset, ext_len + hdrLen,
                 "Extension: %s",
                 val_to_str(ext_type, a11_ext_types,
                            "Unknown Extension %u"));
    ext_tree = proto_item_add_subtree(ti, ett_a11_ext);

    proto_tree_add_item(ext_tree, hf_a11_ext_type, tvb, offset, 1, ext_type);
    offset++;

    if (ext_type == SS_EXT) {
      proto_tree_add_uint(ext_tree, hf_a11_ext_len, tvb, offset, 1, ext_len);
      offset++;
    }
    else if(ext_type == CVSE_EXT || ext_type == OLD_CVSE_EXT) {
      offset++;
      proto_tree_add_uint(ext_tree, hf_a11_ext_len, tvb, offset, 2, ext_len);
      offset+=2;
    }
    else if (ext_type != GEN_AUTH_EXT) {
      /
      proto_tree_add_uint(ext_tree, hf_a11_ext_len, tvb, offset, 1, ext_len);
      offset++;
    }

    switch(ext_type) {
    case SS_EXT:
      decode_sse(ext_tree, tvb, offset, ext_len);
      offset += ext_len;
      ext_len = 0;
      break;

    case MH_AUTH_EXT:
    case MF_AUTH_EXT:
    case FH_AUTH_EXT:
    case RU_AUTH_EXT:
      /
      if (ext_len < 4)
        break;
      proto_tree_add_item(ext_tree, hf_a11_aext_spi, tvb, offset, 4, FALSE);
      offset += 4;
      ext_len -= 4;
      if (ext_len == 0)
        break;
      proto_tree_add_item(ext_tree, hf_a11_aext_auth, tvb, offset, ext_len,
                          FALSE);
      break;
    case MN_NAI_EXT:
      if (ext_len == 0)
        break;
      proto_tree_add_item(ext_tree, hf_a11_next_nai, tvb, offset, 
                          ext_len, FALSE);
      break;

    case GEN_AUTH_EXT:      /
      /
      proto_tree_add_uint(ext_tree, hf_a11_ext_stype, tvb, offset, 1, ext_subtype);
      offset++;
      proto_tree_add_uint(ext_tree, hf_a11_ext_len, tvb, offset, 2, ext_len);
      offset+=2;
      /
      if (ext_len < 4)
        break;
      proto_tree_add_item(ext_tree, hf_a11_aext_spi, tvb, offset, 4, FALSE);
      offset += 4;
      ext_len -= 4;
      /
      if (ext_len == 0)
        break;
      proto_tree_add_item(ext_tree, hf_a11_aext_auth, tvb, offset,
                          ext_len, FALSE);
      
      break;
    case OLD_CVSE_EXT:      /
    case CVSE_EXT:          /
      if (ext_len < 4)
        break;
      proto_tree_add_item(ext_tree, hf_a11_vse_vid, tvb, offset, 4, FALSE);
      offset += 4;
      ext_len -= 4;
      if (ext_len < 2)
        break;
      apptype = tvb_get_ntohs(tvb, offset);
      proto_tree_add_uint(ext_tree, hf_a11_vse_apptype, tvb, offset, 2, apptype);
      offset += 2;
      ext_len -= 2;
      if(apptype == 0x0101) {
        if (tvb_reported_length_remaining(tvb, offset) > 0) {
          dissect_a11_radius(tvb, offset, ext_tree, ext_len + 2);
        }
      }
      break;
    case OLD_NVSE_EXT:      /
    case NVSE_EXT:          /
      if (ext_len < 6)
        break;
      proto_tree_add_item(ext_tree, hf_a11_vse_vid, tvb, offset+2, 4, FALSE);
      offset += 6;
      ext_len -= 6;
      proto_tree_add_item(ext_tree, hf_a11_vse_apptype, tvb, offset, 2, FALSE);

      if (ext_len < 2)
        break;
      apptype = tvb_get_ntohs(tvb, offset);
      offset += 2;
      ext_len -= 2;
      switch(apptype) {
        case 0x0401:
          if (ext_len < 5)
            break;
          proto_tree_add_item(ext_tree, hf_a11_vse_panid, tvb, offset, 5, FALSE);
          offset += 5;
          ext_len -= 5;
          if (ext_len < 5)
            break;
          proto_tree_add_item(ext_tree, hf_a11_vse_canid, tvb, offset, 5, FALSE);
          break;
        case 0x0501:
          if (ext_len < 4)
            break;
          proto_tree_add_item(ext_tree, hf_a11_vse_ppaddr, tvb, offset, 4, FALSE);
          break;
        case 0x0601:
          if (ext_len < 2)
            break;
          proto_tree_add_item(ext_tree, hf_a11_vse_dormant, tvb, offset, 2, FALSE);
          break;
        case 0x0701:
          if (ext_len < 1)
            break;
          proto_tree_add_item(ext_tree, hf_a11_vse_code, tvb, offset, 1, FALSE);
          break;
        case 0x0801:
          if (ext_len < 1)
            break;
          proto_tree_add_item(ext_tree, hf_a11_vse_pdit, tvb, offset, 1, FALSE);
          break;
        case 0x0802:
          proto_tree_add_text(ext_tree, tvb, offset, -1, "Session Parameter - Always On");
          break;
        case 0x0803:
          proto_tree_add_item(ext_tree, hf_a11_vse_qosmode, tvb, offset, 1, FALSE);
          break;
        case 0x0901:
          if (ext_len < 2)
            break;
          proto_tree_add_item(ext_tree, hf_a11_vse_srvopt, tvb, offset, 2, FALSE);
          break;
        case 0x0C01:
          dissect_ase(tvb, offset, ext_len, ext_tree);
          break;
        case 0x0D01:
          dissect_fwd_qosinfo(tvb, offset, ext_tree);
          break;
        case 0x0D02:
          dissect_rev_qosinfo(tvb, offset, ext_tree);
          break;
        case 0x0D03:
          dissect_subscriber_qos_profile(tvb, offset, ext_len, ext_tree);
          break;
        case 0x0DFE:
          dissect_fwd_qosupdate_info(tvb, offset, ext_tree);
          break;
        case 0x0DFF:
          dissect_rev_qosupdate_info(tvb, offset, ext_tree);
          break;
      }

      break;
    case MF_CHALLENGE_EXT:  /
      /
    default:
      proto_tree_add_item(ext_tree, hf_a11_ext, tvb, offset, ext_len, FALSE);
      break;
    } /

    offset += ext_len;
  } /

} /
