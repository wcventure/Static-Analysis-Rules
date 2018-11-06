void
dissect_isup_generic_number_parameter(tvbuff_t *parameter_tvb, proto_tree *parameter_tree, proto_item *parameter_item)
{
  proto_item *address_digits_item;
  proto_tree *address_digits_tree;
  guint8 indicators1, indicators2, nr_qualifier_ind;
  guint8 address_digit_pair=0;
  gint offset=0;
  gint i=0;
  gint length;
  char calling_number[MAXDIGITS + 1]="";

  nr_qualifier_ind = tvb_get_guint8(parameter_tvb, 0);
  proto_tree_add_text(parameter_tree, parameter_tvb, 0, 1, "Number qualifier indicator: 0x%x (refer to 3.26/Q.763 for detailed decoding)", nr_qualifier_ind);
  indicators1 = tvb_get_guint8(parameter_tvb, 1);
  proto_tree_add_boolean(parameter_tree, hf_isup_odd_even_indicator, parameter_tvb, 1, 1, indicators1);
  proto_tree_add_uint(parameter_tree, hf_isup_calling_party_nature_of_address_indicator, parameter_tvb, 0, 1, indicators1);
  indicators2 = tvb_get_guint8(parameter_tvb, 2);
  proto_tree_add_boolean(parameter_tree, hf_isup_ni_indicator, parameter_tvb, 2, 1, indicators2);
  proto_tree_add_uint(parameter_tree, hf_isup_numbering_plan_indicator, parameter_tvb, 2, 1, indicators2);
  if ((indicators2 & GFE_8BIT_MASK) == 0x50)
    proto_tree_add_text(parameter_tree, parameter_tvb, 2, 1, "Different meaning for Generic Number: Numbering plan indicator = private numbering plan");
  proto_tree_add_uint(parameter_tree, hf_isup_address_presentation_restricted_indicator, parameter_tvb, 2, 1, indicators2);
  proto_tree_add_uint(parameter_tree, hf_isup_screening_indicator_enhanced, parameter_tvb, 2, 1, indicators2);
  offset = 3;

  address_digits_item = proto_tree_add_text(parameter_tree, parameter_tvb,
                                            offset, -1,
                                            "Generic number");
  address_digits_tree = proto_item_add_subtree(address_digits_item, ett_isup_address_digits);

  length = tvb_length_remaining(parameter_tvb, offset);
  while(length > 0){
    address_digit_pair = tvb_get_guint8(parameter_tvb, offset);
    proto_tree_add_uint(address_digits_tree, hf_isup_calling_party_odd_address_signal_digit, parameter_tvb, offset, 1, address_digit_pair);
    calling_number[i++] = number_to_char(address_digit_pair & ISUP_ODD_ADDRESS_SIGNAL_DIGIT_MASK);
    if (i > MAXDIGITS)
      THROW(ReportedBoundsError);
    if ((length - 1) > 0 ){
      proto_tree_add_uint(address_digits_tree, hf_isup_calling_party_even_address_signal_digit, parameter_tvb, offset, 1, address_digit_pair);
      calling_number[i++] = number_to_char((address_digit_pair & ISUP_EVEN_ADDRESS_SIGNAL_DIGIT_MASK) / 0x10);
      if (i > MAXDIGITS)
        THROW(ReportedBoundsError);
    }
    offset++;
    length = tvb_length_remaining(parameter_tvb, offset);
  }

  if  (((indicators1 & 0x80) == 0) && (tvb_length(parameter_tvb) > 0)){ /
      proto_tree_add_uint(address_digits_tree, hf_isup_calling_party_even_address_signal_digit, parameter_tvb, offset - 1, 1, address_digit_pair);
      calling_number[i++] = number_to_char((address_digit_pair & ISUP_EVEN_ADDRESS_SIGNAL_DIGIT_MASK) / 0x10);
      if (i > MAXDIGITS)
        THROW(ReportedBoundsError);
  }
  calling_number[i+1] = '\0';

  /
  indicators1 = indicators1 & 0x7f;
  indicators2 = (indicators2 & 0x70)>>4;
  if ((indicators1 == ISUP_CALLED_PARTY_NATURE_INTERNATNL_NR)&&(indicators2==ISDN_NUMBERING_PLAN))
    dissect_e164_cc(parameter_tvb, address_digits_tree, 3, TRUE);

  proto_item_set_text(address_digits_item, "Generic number: %s", calling_number);
  proto_item_set_text(parameter_item, "Generic number: %s", calling_number);

}
