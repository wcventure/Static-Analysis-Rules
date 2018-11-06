static void
dissect_japan_isup_charge_area_info(tvbuff_t *parameter_tvb, proto_tree *parameter_tree, proto_item *parameter_item)
{
  proto_item *digits_item;
  proto_tree *digits_tree;

  guint8 octet;

  gint nat_of_info_indic;
  gint length;
  gint offset;
  gint odd_even;
  gint digit_index = 0;

  char ca_number[MAXDIGITS + 1] = "";

  /
  octet = tvb_get_guint8(parameter_tvb, 0);
  nat_of_info_indic = octet & 0x7F;
  odd_even = octet & 0x80;
  proto_tree_add_boolean(parameter_tree, hf_isup_odd_even_indicator, parameter_tvb, 0, 1, octet);
  proto_tree_add_uint(parameter_tree, hf_japan_isup_charge_area_nat_of_info_value, parameter_tvb, 0, 1, octet);

  offset = 1;
  length = tvb_reported_length_remaining(parameter_tvb, offset);

  /
  if (nat_of_info_indic == CHARGE_AREA_NAT_INFO_CA) {
    digits_tree = proto_tree_add_subtree(parameter_tree, parameter_tvb, offset, -1,
                                ett_isup_address_digits, &digits_item, "Charge Area");

    while (length > 0) {
      octet = tvb_get_guint8(parameter_tvb, offset);
      proto_tree_add_uint(digits_tree, hf_isup_carrier_info_ca_odd_no_digits, parameter_tvb, 0, 1, octet);
      ca_number[digit_index++] = number_to_char(octet & ISUP_ODD_ADDRESS_SIGNAL_DIGIT_MASK);
      if (length == 1) {
        if (odd_even == 0) {
          proto_tree_add_uint(digits_tree, hf_isup_carrier_info_ca_even_no_digits, parameter_tvb, 0, 1, octet);
          ca_number[digit_index++] = number_to_char((octet & ISUP_EVEN_ADDRESS_SIGNAL_DIGIT_MASK) / 0x10);
        }
      }
      else {
        proto_tree_add_uint(digits_tree, hf_isup_carrier_info_ca_even_no_digits, parameter_tvb, 0, 1, octet);
        ca_number[digit_index++] = number_to_char((octet & ISUP_EVEN_ADDRESS_SIGNAL_DIGIT_MASK) / 0x10);
      }
      offset += 1;
      length -= 1;
    }
    ca_number[digit_index++] = '\0';
    proto_item_append_text(digits_item, ": %s", ca_number);
  }
  /
  if (nat_of_info_indic == CHARGE_AREA_NAT_INFO_MA) {
    digits_tree = proto_tree_add_subtree(parameter_tree, parameter_tvb, offset, -1,
                                ett_isup_address_digits, &digits_item, "Message Area:");

    /
    /
    octet = tvb_get_guint8(parameter_tvb, offset);
    proto_tree_add_uint(digits_tree, hf_japan_isup_charging_info_nc_odd_digits, parameter_tvb, 0, 1, octet);
    proto_tree_add_uint(digits_tree, hf_japan_isup_charging_info_nc_even_digits, parameter_tvb, 0, 1, octet);
    octet++;
    octet = tvb_get_guint8(parameter_tvb, offset);
    proto_tree_add_uint(digits_tree, hf_japan_isup_charging_info_nc_odd_digits, parameter_tvb, 0, 1, octet);
    proto_tree_add_uint(digits_tree, hf_japan_isup_charging_info_nc_even_digits, parameter_tvb, 0, 1, octet);
    octet++;

    /
    length = tvb_reported_length_remaining(parameter_tvb, offset);

    while (length > 0) {
      octet = tvb_get_guint8(parameter_tvb, offset);
      proto_tree_add_uint(digits_tree, hf_isup_charging_info_maca_odd_digits, parameter_tvb, 0, 1, octet);
      if (length == 1) {
        if (odd_even == 0) {
          proto_tree_add_uint(digits_tree, hf_isup_charging_info_maca_even_digits, parameter_tvb, 0, 1, octet);
        }
      }
      else {
        proto_tree_add_uint(digits_tree, hf_isup_charging_info_maca_even_digits, parameter_tvb, 0, 1, octet);
      }
      offset += 1;
      length -= 1;
    }

  }

  /
  proto_item_set_text(parameter_item, "Charge Area Information");
}
