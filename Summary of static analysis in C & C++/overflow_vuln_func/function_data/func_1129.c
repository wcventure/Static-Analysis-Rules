static int
dissect_imf(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, void* data _U_)
{
  proto_item  *item;
  proto_tree  *unknown_tree, *text_tree;
  const guint8 *content_type_str = NULL;
  char  *content_encoding_str = NULL;
  const guint8 *parameters = NULL;
  int   hf_id;
  gint  start_offset = 0;
  gint  value_offset = 0;
  gint  unknown_offset = 0;
  gint  end_offset = 0;
  gint   max_length;
  guint8 *key;
  gboolean last_field = FALSE;
  tvbuff_t *next_tvb;
  struct imf_field *f_info;

  col_set_str(pinfo->cinfo, COL_PROTOCOL, PSNAME);
  col_clear(pinfo->cinfo, COL_INFO);

  item = proto_tree_add_item(tree, proto_imf, tvb, 0, -1, ENC_NA);
  tree = proto_item_add_subtree(item, ett_imf);

  max_length = tvb_captured_length(tvb);
  /

  /
  /

  while(!last_field) {

    /
    end_offset = tvb_find_guint8(tvb, start_offset, max_length - start_offset, ':');

    if(end_offset == -1) {
      /
      /
      break;
    } else {
      key = tvb_get_string_enc(wmem_packet_scope(), tvb, start_offset, end_offset - start_offset, ENC_ASCII);

      /
      ascii_strdown_inplace (key);

      /
      f_info = (struct imf_field *)g_hash_table_lookup(imf_field_table, key);

      if(f_info == NULL && custom_field_table) {
        /
        f_info = (struct imf_field *)g_hash_table_lookup(custom_field_table, key);
      }

      if(f_info == NULL) {
        /
        f_info = imf_fields;
        unknown_offset = start_offset;
      }

      hf_id = *(f_info->hf_id);

      /
      start_offset = end_offset+1;

      end_offset = imf_find_field_end(tvb, start_offset, max_length, &last_field);
      if(end_offset == -1) {
        break;   /
      }

      /

      for(value_offset = start_offset; value_offset < end_offset; value_offset++)
        if(!g_ascii_isspace(tvb_get_guint8(tvb, value_offset))) {
          break;
        }

      if(value_offset == end_offset) {
        /
        value_offset = start_offset;
      }

      if(hf_id == hf_imf_extension_type) {

        /
        item = proto_tree_add_item(tree, hf_imf_extension, tvb, unknown_offset, end_offset - unknown_offset - 2, ENC_ASCII|ENC_NA);

        proto_item_append_text(item, " (Contact Wireshark developers if you want this supported.)");

        unknown_tree = proto_item_add_subtree(item, ett_imf_extension);

        proto_tree_add_item(unknown_tree, hf_imf_extension_type, tvb, unknown_offset, start_offset - 1 - unknown_offset, ENC_ASCII|ENC_NA);

        /
        item = proto_tree_add_item(unknown_tree, hf_imf_extension_value, tvb, value_offset, end_offset - value_offset - 2, ENC_ASCII|ENC_NA);

      } else {
        /
        item = proto_tree_add_item(tree, hf_id, tvb, value_offset, end_offset - value_offset - 2, ENC_ASCII|ENC_NA);
      }
      if(f_info->add_to_col_info) {

        col_append_fstr(pinfo->cinfo, COL_INFO, "%s: %s, ", f_info->name,
                        tvb_format_text(tvb, value_offset, end_offset - value_offset - 2));
      }

      if(hf_id == hf_imf_content_type) {
        /

        dissect_imf_content_type(tvb, start_offset, end_offset - start_offset, item,
                                 &content_type_str, &parameters);

      } else if (hf_id == hf_imf_content_transfer_encoding) {
        content_encoding_str = tvb_get_string_enc (wmem_packet_scope(), tvb, value_offset, end_offset - value_offset - 2, ENC_ASCII);
      } else if(f_info->subdissector) {

        /
        f_info->subdissector(tvb, value_offset, end_offset - value_offset, item, pinfo);

      }
    }
    start_offset = end_offset;
  }

  if (last_field) {
    /
    end_offset += 2;
  }

  if (end_offset == -1) {
    end_offset = 0;
  }

  /
  /

  /

  if(content_type_str && media_type_dissector_table) {
    col_set_fence(pinfo->cinfo, COL_INFO);

    if(content_encoding_str && !g_ascii_strncasecmp(content_encoding_str, "base64", 6)) {
      char *string_data = tvb_get_string_enc(wmem_packet_scope(), tvb, end_offset, tvb_reported_length(tvb) - end_offset, ENC_ASCII);
      next_tvb = base64_to_tvb(tvb, string_data);
      add_new_data_source(pinfo, next_tvb, content_encoding_str);
    } else {
      next_tvb = tvb_new_subset_remaining(tvb, end_offset);
    }

    dissector_try_string(media_type_dissector_table, content_type_str, next_tvb, pinfo, tree, (void*)parameters);
  } else {

    /

    item = proto_tree_add_item(tree, hf_imf_message_text, tvb, end_offset, -1 , ENC_NA);
    text_tree = proto_item_add_subtree(item, ett_imf_message_text);

    start_offset = end_offset;
    while (tvb_offset_exists(tvb, start_offset)) {

      /
      tvb_find_line_end(tvb, start_offset, -1, &end_offset, FALSE);

      /
      proto_tree_add_format_wsp_text(text_tree, tvb, start_offset, end_offset - start_offset);
      col_append_sep_str(pinfo->cinfo, COL_INFO, ", ",
                         tvb_format_text_wsp(tvb, start_offset, end_offset - start_offset));

      /
      start_offset = end_offset;
    }
  }
  return tvb_captured_length(tvb);
}
