static void
dissect_sccp_gt_address_information(tvbuff_t *tvb, proto_tree *tree,
				    guint length, gboolean even_length,
				    gboolean called)
{
  guint offset = 0;
  guint8 odd_signal, even_signal = 0x0f;
  char gt_digits[GT_MAX_SIGNALS] = { 0 };

  while(offset < length)
  {
    odd_signal = tvb_get_guint8(tvb, offset) & GT_ODD_SIGNAL_MASK;
    even_signal = tvb_get_guint8(tvb, offset) & GT_EVEN_SIGNAL_MASK;
    even_signal >>= GT_EVEN_SIGNAL_SHIFT;

    strcat(gt_digits, val_to_str(odd_signal, sccp_address_signal_values,
				 "Unknown"));

    /
    if (offset != (length - 1) || even_length == TRUE)
      strcat(gt_digits, val_to_str(even_signal, sccp_address_signal_values,
				   "Unknown"));

    offset += GT_SIGNAL_LENGTH;
  }

  proto_tree_add_string_format(tree, called ? hf_sccp_called_gt_digits
					    : hf_sccp_calling_gt_digits,
			     tvb, 0, length,
			     gt_digits,
			     "Address information (digits): %s", gt_digits);
  proto_tree_add_string_hidden(tree, called ? hf_sccp_gt_digits
					    : hf_sccp_gt_digits,
			     tvb, 0, length,
			     gt_digits);
}
