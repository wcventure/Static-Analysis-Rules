static void
dissect_eap_aka(proto_tree *eap_tree, tvbuff_t *tvb, int offset, gint size)
{
  gint left = size;
  proto_tree_add_item(eap_tree, hf_eap_aka_subtype, tvb, offset, 1, ENC_BIG_ENDIAN);

  offset += 1;
  left   -= 1;

  if (left < 2)
    return;
  proto_tree_add_item(eap_tree, hf_eap_aka_reserved, tvb, offset, 2, ENC_BIG_ENDIAN);
  offset += 2;
  left   -= 2;

  /
  while (left >= 2) {
    guint8       type, length;
    proto_item  *pi;
    proto_tree  *attr_tree;
    int          aoffset;
    gint         aleft;

    aoffset = offset;
    type    = tvb_get_guint8(tvb, aoffset);
    length  = tvb_get_guint8(tvb, aoffset + 1);
    aleft   = 4 *  length;

    pi = proto_tree_add_none_format(eap_tree, hf_eap_aka_subtype_attribute, tvb,
                                    aoffset, aleft, "EAP-AKA Attribute: %s (%d)",
                                    val_to_str_ext_const(type,
                                                         &eap_sim_aka_attribute_vals_ext,
                                                         "Unknown"),
                                    type);
    attr_tree = proto_item_add_subtree(pi, ett_eap_aka_attr);
    proto_tree_add_uint(attr_tree, hf_eap_aka_subtype_type, tvb, aoffset, 1, type);
    aoffset += 1;
    aleft   -= 1;

    if (aleft <= 0)
      break;
    proto_tree_add_item(attr_tree, hf_eap_aka_subtype_length, tvb, aoffset, 1, ENC_BIG_ENDIAN);
    aoffset += 1;
    aleft   -= 1;

    if (type == AT_IDENTITY) {
      guint8 eap_identity_prefix;

      proto_tree_add_item(attr_tree, hf_eap_identity_actual_len, tvb, aoffset, 2, ENC_BIG_ENDIAN);

      eap_identity_prefix = tvb_get_guint8(tvb, aoffset + 2) - '0';
      proto_tree_add_string_format(attr_tree, hf_eap_identity_prefix, tvb, aoffset + 2, 1,
            &eap_identity_prefix, "Identity Prefix: %s (%u)",
            val_to_str(eap_identity_prefix, eap_identity_prefix_vals, "Unknown"),
            eap_identity_prefix);
      proto_tree_add_item(attr_tree, hf_eap_identity, tvb, aoffset + 2, aleft - 2, ENC_ASCII|ENC_NA);
    }
    else
      proto_tree_add_item(attr_tree, hf_eap_aka_subtype_value, tvb, aoffset, aleft, ENC_NA);

    offset += 4 * length;
    left   -= 4 * length;
  }
}
