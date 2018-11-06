static void
dissect_japan_isup_carrier_information(tvbuff_t *parameter_tvb, packet_info *pinfo, proto_tree *parameter_tree, proto_item *parameter_item)
{
  proto_item *catagory_of_carrier;
  proto_tree *carrier_info_tree;

  proto_item *digits_item;
  proto_tree *digits_tree;

  proto_item *type_of_carrier;
  proto_tree *type_of_carrier_tree;

  guint8 octet;
  guint8 odd_even;
  guint8 type_of_carrier_info;
  guint8 carrier_info_length;
  guint8 carrierX_end_index;

  gint offset = 0;
  gint length = 0;

  gint num_octets_with_digits = 0;

  gint digit_index = 0;
  char cid_number[MAXDIGITS + 1] = "";
  char ca_number[MAXDIGITS + 1]  = "";

  /
  octet = tvb_get_guint8(parameter_tvb, 0);
  proto_tree_add_uint(parameter_tree, hf_isup_carrier_info_iec, parameter_tvb, 0, 1, octet);


  length = tvb_reported_length_remaining(parameter_tvb, 2);
  if (length == 0) {
    expert_add_info(pinfo, parameter_item, &ei_isup_empty_number);
    proto_item_set_text(parameter_item, "Carrier Information (empty)");
    return;
  }

  offset = 1;

  /

  while (length > 0) {

    carrier_info_tree = proto_tree_add_subtree(parameter_tree, parameter_tvb, offset, -1,
                        ett_isup_carrier_info, &catagory_of_carrier, "Category of Carrier:");

    /
    octet = tvb_get_guint8(parameter_tvb, offset);
    /
    proto_item_set_text(catagory_of_carrier, "%s (%u)", val_to_str_ext_const(octet, &isup_carrier_info_category_vals_ext, "spare"), octet );


    /
    offset += 1;
    carrierX_end_index = tvb_get_guint8(parameter_tvb, offset)+offset;

    while (offset < carrierX_end_index) {

      type_of_carrier_tree = proto_tree_add_subtree(carrier_info_tree, parameter_tvb, offset, -1,
                                    ett_isup_carrier_info, &type_of_carrier, "Type of Carrier:");

      /
      offset += 1;
      type_of_carrier_info = tvb_get_guint8(parameter_tvb, offset);
      /
      proto_item_set_text(type_of_carrier, "%s (%u)", val_to_str_ext_const(type_of_carrier_info, &isup_carrier_info_type_of_carrier_vals_ext, "spare"), type_of_carrier_info );


      /
      offset += 1;
      carrier_info_length = tvb_get_guint8(parameter_tvb, offset);

      /

      if (type_of_carrier_info == CARRIER_INFO_TYPE_OF_CARRIER_POIHIE) {
        /
        offset += 1;
        octet = tvb_get_guint8(parameter_tvb, offset);
        proto_tree_add_uint(type_of_carrier_tree, hf_isup_carrier_info_poi_entry_HEI, parameter_tvb, 0, 1, octet);
        proto_tree_add_uint(type_of_carrier_tree, hf_isup_carrier_info_poi_exit_HEI, parameter_tvb, 0, 1, octet);
      }

      /
      if (type_of_carrier_info == CARRIER_INFO_TYPE_OF_CARRIER_POICA) {

        digits_tree = proto_tree_add_subtree(type_of_carrier_tree, parameter_tvb, offset, -1,
                                        ett_isup_address_digits, &digits_item, "Charge Area");

        /
        offset += 1;
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

      }

      /

      if (type_of_carrier_info == CARRIER_INFO_TYPE_OF_CARRIER_CARID) {
        digits_tree = proto_tree_add_subtree(type_of_carrier_tree, parameter_tvb, offset, -1,
                                        ett_isup_address_digits, &digits_item, "Carrier ID Code");

        offset += 1;
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
          proto_tree_add_uint(digits_tree, hf_isup_carrier_info_odd_no_digits, parameter_tvb, 0, 1, octet);
          cid_number[digit_index++] = number_to_char(octet & ISUP_ODD_ADDRESS_SIGNAL_DIGIT_MASK);
          if (num_octets_with_digits == 1) {
            if (odd_even == 0) {
              proto_tree_add_uint(digits_tree, hf_isup_carrier_info_even_no_digits, parameter_tvb, 0, 1, octet);
              cid_number[digit_index++] = number_to_char((octet & ISUP_EVEN_ADDRESS_SIGNAL_DIGIT_MASK) / 0x10);
            }
          }
          else {
            proto_tree_add_uint(digits_tree, hf_isup_carrier_info_even_no_digits, parameter_tvb, 0, 1, octet);
            cid_number[digit_index++] = number_to_char((octet & ISUP_EVEN_ADDRESS_SIGNAL_DIGIT_MASK) / 0x10);
          }
          num_octets_with_digits--;
        }
        cid_number[digit_index++] = '\0';
        proto_item_append_text(digits_item, ": %s", cid_number);
      }
    }

    offset += 1;
    length = tvb_reported_length_remaining(parameter_tvb, offset);
  }

  /
  proto_item_set_text(parameter_item, "Carrier Information");

}
