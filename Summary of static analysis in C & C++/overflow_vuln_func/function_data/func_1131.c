static int
dissect_application_isup(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, void* data)
{
/
  proto_item *ti;
  proto_tree *isup_tree = NULL;
  tvbuff_t   *message_tvb;
  guint8      message_type;
  gchar      *content_type_parameter_str;
  guint8      itu_isup_variant = ISUP_ITU_STANDARD_VARIANT; /

  if (data) {
    content_type_parameter_str = ascii_strdown_inplace((gchar *)data);
    if (strstr(content_type_parameter_str, "ansi")) {
      isup_standard = ANSI_STANDARD;
      col_append_str(pinfo->cinfo, COL_PROTOCOL, "/ISUP(ANSI)");
      message_type = tvb_get_guint8(tvb, 0);
      /
      col_append_sep_fstr(pinfo->cinfo, COL_INFO, ", ",
                          "ISUP:%s",
                          val_to_str_ext_const(message_type, &ansi_isup_message_type_value_acro_ext, "reserved"));
      if (tree) {
        ti = proto_tree_add_item(tree, proto_isup, tvb, 0, -1, ENC_NA);
        isup_tree = proto_item_add_subtree(ti, ett_isup);
      }

      message_tvb = tvb_new_subset_remaining(tvb, 0);
      dissect_ansi_isup_message(message_tvb, pinfo, isup_tree, ISUP_ITU_STANDARD_VARIANT, 0);
      return tvb_reported_length(tvb);
    } else if (strstr(content_type_parameter_str, "spirou")) {
      isup_standard    = ITU_STANDARD;
      itu_isup_variant = ISUP_FRENCH_VARIANT;
    } else {
      isup_standard = ITU_STANDARD;
    }
  } else {
    /
    isup_standard = ITU_STANDARD;
  }


  /
  message_type = tvb_get_guint8(tvb, 0);

  switch (itu_isup_variant) {
    case ISUP_ITU_STANDARD_VARIANT:
      /
      col_append_str(pinfo->cinfo, COL_PROTOCOL, "/ISUP(ITU)");

      /
      col_append_sep_fstr(pinfo->cinfo, COL_INFO, ", ",
                          "ISUP:%s",
                          val_to_str_ext_const(message_type, &isup_message_type_value_acro_ext, "reserved"));
      break;
    case ISUP_FRENCH_VARIANT:
      /
      col_append_str(pinfo->cinfo, COL_PROTOCOL, "/ISUP(French)");

      /
      col_append_sep_fstr(pinfo->cinfo, COL_INFO, ", ",
                          "ISUP:%s",
                          val_to_str_ext_const(message_type, &french_isup_message_type_value_acro_ext, "reserved"));
      break;
#if 0
      /       * out of the content type
       */
    case ISUP_ISRAELI_VARIANT:
      /
      col_append_str(pinfo->cinfo, COL_PROTOCOL, "/ISUP(Israeli)");

      /
      col_append_sep_fstr(pinfo->cinfo, COL_INFO, ", ",
                          "ISUP:%s",
                          val_to_str_ext_const(message_type, &israeli_isup_message_type_value_acro_ext, "reserved"));
      break;
#endif
    default:
      col_append_sep_fstr(pinfo->cinfo, COL_INFO, ", ",
                          "ISUP: Unknown variant %d", itu_isup_variant);
      break;
  }

  if (tree) {
    ti = proto_tree_add_item(tree, proto_isup, tvb, 0, -1, ENC_NA);
    isup_tree = proto_item_add_subtree(ti, ett_isup);
  }

  message_tvb = tvb_new_subset_remaining(tvb, 0);
  dissect_isup_message(message_tvb, pinfo, isup_tree, itu_isup_variant, 0);
  return tvb_reported_length(tvb);
}
