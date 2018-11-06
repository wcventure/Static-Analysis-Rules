static void
dissect_japan_isup_network_poi_cad(tvbuff_t *parameter_tvb, proto_tree *parameter_tree, proto_item *parameter_item)
{
  proto_item *digits_item;
  proto_tree *digits_tree;
  int         offset = 0;
  guint8      octet;
  guint8      odd_even;
  guint8      carrier_info_length;
  gint        num_octets_with_digits = 0;
  gint        digit_index = 0;
  char ca_number[MAXDIGITS + 1] = "";

  /

  /
  proto_tree_add_item(parameter_tree, hf_isup_carrier_info_poi_entry_HEI, parameter_tvb, offset, 1, ENC_BIG_ENDIAN);
  proto_tree_add_item(parameter_tree, hf_isup_carrier_info_poi_exit_HEI, parameter_tvb, offset, 1, ENC_BIG_ENDIAN);
  offset += 1;

  /
  carrier_info_length = tvb_get_guint8(parameter_tvb, offset);
  proto_tree_add_item(parameter_tree, hf_japan_isup_carrier_info_length, parameter_tvb, offset, 1, ENC_BIG_ENDIAN);
  offset += 1;

  /

  digits_tree = proto_tree_add_subtree(parameter_tree, parameter_tvb, offset, -1,
                                ett_isup_address_digits, &digits_item, "Charge Area Number");

  /
  odd_even = tvb_get_guint8(parameter_tvb, offset);
  proto_tree_add_boolean(digits_tree, hf_isup_odd_even_indicator, parameter_tvb, 0, 1, odd_even);

  /
  num_octets_with_digits = carrier_info_length - 1;

  /
  /
  /
  digit_index = 0;
  while (num_octets_with_digits > 0) {
    offset += 1;
    octet = tvb_get_guint8(parameter_tvb, offset);
    proto_tree_add_uint(digits_tree, hf_isup_carrier_info_ca_odd_no_digits, parameter_tvb, 0, 1, octet);
    ca_number[digit_index++] = number_to_char(octet & ISUP_ODD_ADDRESS_SIGNAL_DIGIT_MASK);
    if (num_octets_with_digits == 1) {
      if (odd_even == 0) {
        proto_tree_add_uint(digits_tree, hf_isup_carrier_info_ca_even_no_digits, parameter_tvb, 0, 1, octet);
        ca_number[digit_index++] = number_to_char((octet & ISUP_EVEN_ADDRESS_SIGNAL_DIGIT_MASK) / 0x10);
      }
    }
    else {
      proto_tree_add_uint(digits_tree, hf_isup_carrier_info_ca_even_no_digits, parameter_tvb, 0, 1, octet);
      ca_number[digit_index++] = number_to_char((octet & ISUP_EVEN_ADDRESS_SIGNAL_DIGIT_MASK) / 0x10);
    }

    num_octets_with_digits--;
  }
  ca_number[digit_index++] = '\0';
  proto_item_append_text(digits_item, ": %s", ca_number);

  proto_item_set_text(parameter_item, "Network POI-CA");

}
