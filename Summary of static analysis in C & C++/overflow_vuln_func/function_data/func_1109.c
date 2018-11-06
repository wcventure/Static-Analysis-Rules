static void
dissect_isup_transit_network_selection_parameter(tvbuff_t *parameter_tvb, proto_tree *parameter_tree, proto_item *parameter_item)
{
  proto_item *address_digits_item;
  proto_tree *address_digits_tree;
  guint8 indicators;
  guint8 address_digit_pair=0;
  gint offset=0;
  gint i=0;
  gint length;
  char network_id[MAXLENGTH]="";

  indicators = tvb_get_guint8(parameter_tvb, 0);
  proto_tree_add_boolean(parameter_tree, hf_isup_odd_even_indicator, parameter_tvb, 0, 1, indicators);
  proto_tree_add_uint(parameter_tree, hf_isup_type_of_network_identification, parameter_tvb, 0, 1, indicators);
  proto_tree_add_uint(parameter_tree, hf_isup_network_identification_plan, parameter_tvb, 0, 1, indicators);
  offset = 1;

  address_digits_item = proto_tree_add_text(parameter_tree, parameter_tvb,
					    offset, -1,
					    "Network identification");
  address_digits_tree = proto_item_add_subtree(address_digits_item, ett_isup_address_digits);

  length = tvb_length_remaining(parameter_tvb, offset);
  while(length > 0){
    address_digit_pair = tvb_get_guint8(parameter_tvb, offset);
    proto_tree_add_uint(address_digits_tree, hf_isup_calling_party_odd_address_signal_digit, parameter_tvb, offset, 1, address_digit_pair);
    network_id[i++] = number_to_char(address_digit_pair & ISUP_ODD_ADDRESS_SIGNAL_DIGIT_MASK);
    if ((length - 1) > 0 ){
      proto_tree_add_uint(address_digits_tree, hf_isup_calling_party_even_address_signal_digit, parameter_tvb, offset, 1, address_digit_pair);
      network_id[i++] = number_to_char((address_digit_pair & ISUP_EVEN_ADDRESS_SIGNAL_DIGIT_MASK) / 0x10);
    }
    offset++;
    length = tvb_length_remaining(parameter_tvb, offset);
  }

  if  (((indicators & 0x80) == 0) && (tvb_length(parameter_tvb) > 0)){ /
      proto_tree_add_uint(address_digits_tree, hf_isup_calling_party_even_address_signal_digit, parameter_tvb, offset - 1, 1, address_digit_pair);
      network_id[i++] = number_to_char((address_digit_pair & ISUP_EVEN_ADDRESS_SIGNAL_DIGIT_MASK) / 0x10);
  }
  network_id[i++] = '\0';

  proto_item_set_text(address_digits_item, "Network identification: %s", network_id);
  proto_item_set_text(parameter_item, "Transit network selection: %s", network_id);

}
