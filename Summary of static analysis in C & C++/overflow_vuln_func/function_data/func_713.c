static char *
dissect_isup_digits_common(tvbuff_t *tvb, gint offset, packet_info *pinfo _U_, proto_tree *tree, proto_item *item,
                           const char *param_name, gint hf_number, gint hf_odd_digit, gint hf_even_digit,
                           gboolean even_indicator, e164_number_type_t number_type, guint nature_of_address)
{
  gint         i = 0;
  gint         length;
  proto_item  *digits_item;
  proto_tree  *digits_tree;
  guint8       digit_pair = 0;
  char         number[MAXDIGITS + 1] = "";
  e164_info_t  e164_info;
  gboolean     old_visible;
  gboolean     set_visibility = FALSE;

  length = tvb_reported_length_remaining(tvb, offset);
  if (length == 0) {
    expert_add_info(pinfo, item, &ei_isup_empty_number);
    proto_item_set_text(item, "%s: (empty)", param_name);
    return NULL;
  }

  /
  if(proto_field_is_referenced(tree, hf_number)) {
    old_visible = proto_tree_set_visible(tree, TRUE);
    set_visibility = TRUE;
  }
  digits_item = proto_tree_add_string(tree, hf_number, tvb, offset, -1, "");
  digits_tree = proto_item_add_subtree(digits_item, ett_isup_address_digits);
  if(set_visibility) {
    proto_tree_set_visible(tree, old_visible);
  }

  while (length > 0) {
    digit_pair = tvb_get_guint8(tvb, offset);
    proto_tree_add_uint(digits_tree, hf_odd_digit, tvb, offset, 1, digit_pair);
    number[i++] = number_to_char(digit_pair & ISUP_ODD_ADDRESS_SIGNAL_DIGIT_MASK);

    if (i > MAXDIGITS) {
      expert_add_info(pinfo, digits_item, &ei_isup_too_many_digits);
      break;
    }

    if ((length - 1) > 0) {
      proto_tree_add_uint(digits_tree, hf_even_digit, tvb, offset, 1, digit_pair);
      number[i++] = number_to_char((digit_pair & ISUP_EVEN_ADDRESS_SIGNAL_DIGIT_MASK) / 0x10);

      if (i > MAXDIGITS) {
        expert_add_info(pinfo, digits_item, &ei_isup_too_many_digits);
        break;
      }
    }

    offset += 1;
    length = tvb_reported_length_remaining(tvb, offset);
  }

  if  (even_indicator && (tvb_reported_length(tvb) > 0)) {
    /
    proto_tree_add_uint(digits_tree, hf_isup_calling_party_even_address_signal_digit,
                        tvb, offset - 1, 1, digit_pair);
    number[i++] = number_to_char((digit_pair & ISUP_EVEN_ADDRESS_SIGNAL_DIGIT_MASK) / 0x10);
  }

  number[i++] = '\0';

  /
  proto_item_append_string(digits_item, number);

  if (number_type != NONE) {
    e164_info.e164_number_type = number_type;
    e164_info.nature_of_address = nature_of_address;
    e164_info.E164_number_str = number;
    e164_info.E164_number_length = i - 1;
    dissect_e164_number(tvb, digits_tree, 2, (offset - 2), e164_info);
  }

  proto_item_set_text(item, "%s: %s", param_name, number);

  return wmem_strdup(wmem_packet_scope(), number);
}
