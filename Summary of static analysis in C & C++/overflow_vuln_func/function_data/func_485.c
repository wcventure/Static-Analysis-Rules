static void
dissect_chap( tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree ) {
  proto_item *ti = NULL;
  proto_tree *fh_tree = NULL;
  proto_item *tf;
  proto_tree *field_tree;

  guint8 code, value_size;
  guint32 length;
  int offset;

  code = tvb_get_guint8(tvb, 0);
  if(check_col(pinfo->cinfo, COL_PROTOCOL))
    col_set_str(pinfo->cinfo, COL_PROTOCOL, "PPP CHAP");

  if(check_col(pinfo->cinfo, COL_INFO))
    col_add_str(pinfo->cinfo, COL_INFO,
                val_to_str(code, chap_vals, "Unknown"));

  if(tree) {
    /
    ti = proto_tree_add_item(tree, proto_chap, tvb, 0, -1, FALSE);
    fh_tree = proto_item_add_subtree(ti, ett_chap);

    /
    proto_tree_add_item(fh_tree, hf_chap_code, tvb, 0, 1, FALSE);

    /
    proto_tree_add_item(fh_tree, hf_chap_identifier, tvb, 1, 1, FALSE);
  }
    
  /
  length = tvb_get_ntohs(tvb, 2);
  if(length < 4) {
    if(tree) {
      proto_tree_add_uint_format(fh_tree, hf_chap_length, tvb, 2, 2, length,
                                 "Length: %u (invalid, must be >= 4)",
                                 length);
    }
    return;
  }
  proto_item_set_len(ti, length);
  if(tree) {
    proto_tree_add_item(fh_tree, hf_chap_length, tvb, 2, 2, FALSE);    
  }

  /
  offset = 4;
  / 
  length -= 4;

  switch (code) {
    /
    case CHAP_CHAL:
    case CHAP_RESP:
      if(tree) {
        if (length > 0) {
          guint value_offset=0;
          guint name_offset=0, name_size = 0;

          /
          tf = proto_tree_add_text(fh_tree, tvb, offset, length,
                                   "Data (%d byte%s)", length,
                                   plurality(length, "", "s"));
          field_tree = proto_item_add_subtree(tf, ett_chap_data);
          length--;

          /
          value_size = tvb_get_guint8(tvb, offset);
          if (value_size > length) {
            proto_tree_add_text(field_tree, tvb, offset, 1,
                                "Value Size: %d byte%s (invalid, must be <= %u)",
                                value_size, plurality(value_size, "", "s"),
                                length);
            return;
          }
          proto_tree_add_item(field_tree, hf_chap_value_size, tvb, offset, 1, FALSE);
          offset++;

          /
          if (length > 0) {
            value_offset = offset;
            proto_tree_add_item(field_tree, hf_chap_value, tvb, offset, value_size, FALSE);

            /
            offset+=value_size;
            length-=value_size;

            /
            if (length > 0) {
              tvb_ensure_bytes_exist(tvb, offset, length);
              proto_tree_add_item(field_tree, hf_chap_name, tvb, offset, length, FALSE);
              name_offset = offset;
              name_size = length;
            }

            /
            if(check_col(pinfo->cinfo, COL_INFO)){
              col_append_fstr(pinfo->cinfo, COL_INFO, " (NAME='%s%s', VALUE=0x%s)",
                              tvb_format_text(tvb, name_offset,
                                              (name_size > 20) ? 20 : name_size),
                              (name_size > 20) ? "..." : "",
                              tvb_bytes_to_str(tvb, value_offset, value_size));
            }
          }
        }
      }
      break;

    /
    case CHAP_SUCC:
    case CHAP_FAIL:
      if(tree) {
        if (length > 0) {
          proto_tree_add_item(fh_tree, hf_chap_message, tvb, offset, length, FALSE);
        }
      }
      
      /
      if(check_col(pinfo->cinfo, COL_INFO)){
        col_append_fstr(pinfo->cinfo, COL_INFO, " (MESSAGE='%s')",
                        tvb_format_text(tvb, offset, length));
      }
      break;

    /
    default:
      if (length > 0)
        proto_tree_add_text(fh_tree, tvb, offset, length, "Stuff (%u byte%s)",
                            length, plurality(length, "", "s"));
      break;
  }
}
