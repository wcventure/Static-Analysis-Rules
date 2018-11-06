static void
dissect_INFO_TS(tvbuff_t *tvb, gint offset, guint8 flags,
                gboolean little_endian, int next_submsg_offset,
                proto_tree *rtps_submessage_tree)
{
  char                     buff[10];

  proto_tree_add_uint(rtps_submessage_tree, hf_rtps_submessage_flags,
                      tvb, offset, 1, flags);
  offset +=1;

  /
  if ((flags & FLAG_I) != 0)
  {
    if (next_submsg_offset < 8)
    {
      proto_tree_add_uint_format(rtps_submessage_tree, hf_rtps_octets_to_next_header,
                                 tvb, offset, 2, next_submsg_offset,
                                 "Octets to next header: %u (bogus, must be >= 8)",
                                 next_submsg_offset);
      return;
    }
  }
  proto_tree_add_uint(rtps_submessage_tree, hf_rtps_octets_to_next_header,
                      tvb, offset, 2, next_submsg_offset);
  offset +=2;
  next_submsg_offset += offset;

  /
  if ((flags & FLAG_I) != 0)
  {
    proto_tree_add_text(rtps_submessage_tree, tvb, offset, 8,
                        "ntpTimestamp: %s (sec)",
                        get_NtpTime(offset, tvb, little_endian,buff));
    offset +=8;
  }

}
