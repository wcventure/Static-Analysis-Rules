static int
dissect_h225_DialedDigits(tvbuff_t *tvb, int offset, packet_info *pinfo _U_, proto_tree *tree, int hf_index) {
  tvbuff_t *value_tvb = NULL;
  guint len = 0;

  offset = dissect_per_restricted_character_string(tvb, offset, pinfo, tree, hf_index,
                                                      1, 128, "0123456789#*,", strlen("0123456789#*,"),
                                                      &value_tvb);

  if (h225_pi->is_destinationInfo == TRUE) {
    if (value_tvb) {
      len = tvb_length(value_tvb);
      tvb_memcpy(value_tvb, (guint8*)h225_pi->dialedDigits, 0, len);
    }
    h225_pi->dialedDigits[len] = '\0';
	h225_pi->is_destinationInfo = FALSE;
  }

  return offset;
}
