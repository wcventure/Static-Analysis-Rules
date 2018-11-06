void
tcp_dissect_pdus(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree,
		 gboolean proto_desegment, guint fixed_len,
		 guint (*get_pdu_len)(tvbuff_t *, int),
		 void (*dissect_pdu)(tvbuff_t *, packet_info *, proto_tree *))
{
  volatile int offset = 0;
  guint length_remaining;
  guint plen;
  guint length;
  tvbuff_t *next_tvb;

  while (tvb_reported_length_remaining(tvb, offset) != 0) {
    /
    length_remaining = tvb_ensure_length_remaining(tvb, offset);

    /
    if (proto_desegment && pinfo->can_desegment) {
      /
      if (length_remaining < fixed_len) {
	/
	pinfo->desegment_offset = offset;
	pinfo->desegment_len = fixed_len - length_remaining;
	return;
      }
    }

    /
    plen = (*get_pdu_len)(tvb, offset);

    /
    if (proto_desegment && pinfo->can_desegment) {
      /
      if (length_remaining < plen) {
	/
	pinfo->desegment_offset = offset;
	pinfo->desegment_len = plen - length_remaining;
	return;
      }
    }

    /
    if (plen < fixed_len) {
      /
      show_reported_bounds_error(tvb, pinfo, tree);
      return;
    }
    length = length_remaining;
    if (length > plen)
	length = plen;
    next_tvb = tvb_new_subset(tvb, offset, length, plen);

    /
    TRY {
      (*dissect_pdu)(next_tvb, pinfo, tree);
    }
    CATCH(BoundsError) {
      RETHROW;
    }
    CATCH(ReportedBoundsError) {
      show_reported_bounds_error(tvb, pinfo, tree);
    }
    ENDTRY;

    /
    offset += plen;
  }
}
