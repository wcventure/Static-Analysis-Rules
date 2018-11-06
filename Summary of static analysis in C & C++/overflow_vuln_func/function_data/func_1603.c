static void dissect_r3_cmd_setconfig (tvbuff_t *tvb, guint32 start_offset, guint32 length _U_, packet_info *pinfo _U_, proto_tree *tree)
{
  guint8 cmdLen = tvb_get_guint8 (tvb, start_offset + 0);
  tvbuff_t *payload_tvb = tvb_new_subset (tvb, start_offset + 2, cmdLen - 2, cmdLen - 2);
  guint32 offset = 0;

  proto_tree_add_item (tree, hf_r3_commandlength, tvb, start_offset + 0, 1, ENC_LITTLE_ENDIAN);
  proto_tree_add_item (tree, hf_r3_command, tvb, start_offset + 1, 1, ENC_LITTLE_ENDIAN);

  while (offset < tvb_reported_length (payload_tvb))
  {
    proto_item *sc_item = NULL;
    proto_tree *sc_tree = NULL;
    const gchar *ci = NULL;

    ci = val_to_str_ext_const (tvb_get_guint8 (payload_tvb, offset + 1), &r3_configitemnames_ext,  "[Unknown Configuration Item]");

    sc_item = proto_tree_add_text (tree, payload_tvb, offset + 0, tvb_get_guint8 (payload_tvb, offset + 0), "Config Field: %s (%u)", ci, tvb_get_guint8 (payload_tvb, offset + 1));
    sc_tree = proto_item_add_subtree (sc_item, ett_r3upstreamfield);

    proto_tree_add_item (sc_tree, hf_r3_configitemlength, payload_tvb, offset + 0, 1, ENC_LITTLE_ENDIAN);
    proto_tree_add_item (sc_tree, hf_r3_configitem, payload_tvb, offset + 1, 1, ENC_LITTLE_ENDIAN);

    if (tvb_get_guint8 (payload_tvb, offset + 1) < array_length (configMap))
    {
      switch (configMap [tvb_get_guint8 (payload_tvb, offset + 1)])
      {
        case CONFIGTYPE_NONE :
          proto_tree_add_item (sc_tree, hf_r3_configitemdata, payload_tvb, offset + 2, tvb_get_guint8 (payload_tvb, offset + 0) - 3, ENC_NA);
          break;

        case CONFIGTYPE_BOOL :
          proto_tree_add_item (sc_tree, hf_r3_configitemdata_bool, payload_tvb, offset + 2, 1, ENC_LITTLE_ENDIAN);
          break;

        case CONFIGTYPE_8 :
          proto_tree_add_item (sc_tree, hf_r3_configitemdata_8, payload_tvb, offset + 2, 1, ENC_LITTLE_ENDIAN);
          break;

        case CONFIGTYPE_16 :
          proto_tree_add_item (sc_tree, hf_r3_configitemdata_16, payload_tvb, offset + 2, 2, ENC_LITTLE_ENDIAN);
          break;

        case CONFIGTYPE_32 :
          proto_tree_add_item (sc_tree, hf_r3_configitemdata_32, payload_tvb, offset + 2, 4, ENC_LITTLE_ENDIAN);
          break;

        case CONFIGTYPE_STRING :
          proto_tree_add_item (sc_tree, hf_r3_configitemdata_string, payload_tvb, offset + 2, tvb_get_guint8 (payload_tvb, offset + 0) - 2, ENC_ASCII|ENC_NA);
          break;

        default :
          proto_tree_add_none_format (sc_tree, hf_r3_upstreamfielderror, payload_tvb, offset + 2, tvb_get_guint8 (payload_tvb, offset + 0) - 2, "Unknown Field Type");
          break;
      }
    }
    else
      proto_tree_add_text (sc_tree, payload_tvb, offset + 2, tvb_get_guint8 (payload_tvb, offset + 0) - 2, "[Unknown Field Type]");

    offset += tvb_get_guint8 (payload_tvb, offset + 0);
  }
}
