static void dissect_r3_upstreamcommand_queryconfig (tvbuff_t *tvb, guint32 start_offset _U_, guint32 length _U_, packet_info *pinfo _U_, proto_tree *tree)
{
  guint32 offset = 0;

  while (offset < tvb_reported_length (tvb))
  {
    proto_item *upstreamfield_item = NULL;
    proto_tree *upstreamfield_tree = NULL;
    const gchar *ci = NULL;

    ci = val_to_str_ext_const (tvb_get_guint8 (tvb, offset + 1), &r3_configitemnames_ext, "[Unknown Configuration Item]");

    upstreamfield_item = proto_tree_add_text (tree, tvb, offset + 0, tvb_get_guint8 (tvb, offset + 0), "Config Field: %s (%u)", ci, tvb_get_guint8 (tvb, offset + 1));
    upstreamfield_tree = proto_item_add_subtree (upstreamfield_item, ett_r3upstreamfield);

    proto_tree_add_item (upstreamfield_tree, hf_r3_configitemlength, tvb, offset + 0, 1, ENC_LITTLE_ENDIAN);
    proto_tree_add_item (upstreamfield_tree, hf_r3_configitem, tvb, offset + 1, 1, ENC_LITTLE_ENDIAN);
    proto_tree_add_item (upstreamfield_tree, hf_r3_configitemtype, tvb, offset + 2, 1, ENC_LITTLE_ENDIAN);

    switch (tvb_get_guint8 (tvb, offset + 2))
    {
      case CONFIGTYPE_NONE :
        proto_tree_add_item (upstreamfield_tree, hf_r3_configitemdata, tvb, offset + 3, tvb_get_guint8 (tvb, offset + 0) - 3, ENC_NA);
        break;

      case CONFIGTYPE_BOOL :
        proto_tree_add_item (upstreamfield_tree, hf_r3_configitemdata_bool, tvb, offset + 3, 1, ENC_LITTLE_ENDIAN);
        break;

      case CONFIGTYPE_8 :
        proto_tree_add_item (upstreamfield_tree, hf_r3_configitemdata_8, tvb, offset + 3, 1, ENC_LITTLE_ENDIAN);
        break;

      case CONFIGTYPE_16 :
        proto_tree_add_item (upstreamfield_tree, hf_r3_configitemdata_16, tvb, offset + 3, 2, ENC_LITTLE_ENDIAN);
        break;

      case CONFIGTYPE_32 :
        proto_tree_add_item (upstreamfield_tree, hf_r3_configitemdata_32, tvb, offset + 3, 4, ENC_LITTLE_ENDIAN);
        break;

      case CONFIGTYPE_STRING :
        proto_tree_add_item (upstreamfield_tree, hf_r3_configitemdata_string, tvb, offset + 3, tvb_get_guint8 (tvb, offset + 0) - 3, ENC_ASCII|ENC_NA);
        break;

      default :
        proto_tree_add_none_format (upstreamfield_tree, hf_r3_upstreamfielderror, tvb, offset + 3, tvb_get_guint8 (tvb, offset + 0) - 3, "Unknown Field Type");
        break;
    }

    offset += tvb_get_guint8 (tvb, offset + 0);
  }
}
