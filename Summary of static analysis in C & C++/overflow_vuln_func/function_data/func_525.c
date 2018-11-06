static void
dissect_GAP(tvbuff_t *tvb, gint offset, guint8 flags,
            gboolean little_endian, int next_submsg_offset,
            proto_tree *rtps_submessage_tree)
{
  char                    buff[40];
  SequenceNumber          sequenceNumber;      /

  proto_tree_add_uint(rtps_submessage_tree, hf_rtps_submessage_flags,
                      tvb, offset, 1, flags);
  offset +=1;

  if (next_submsg_offset < 28)
  {
    proto_tree_add_uint_format(rtps_submessage_tree, hf_rtps_octets_to_next_header,
                               tvb, offset, 2, next_submsg_offset,
                               "Octets to next header: %u (bogus, must be >= 28)",
                               next_submsg_offset);
    return;
  }
  proto_tree_add_uint(rtps_submessage_tree, hf_rtps_octets_to_next_header,
                      tvb, offset, 2, next_submsg_offset);
  offset +=2;
  next_submsg_offset += offset;

  /
  proto_tree_add_text(rtps_submessage_tree, tvb, offset, 4,
                      "Reader Object ID:          %s ",
                      object_id_to_string(offset, tvb, buff));
  offset +=4;

  /
  proto_tree_add_text(rtps_submessage_tree, tvb, offset, 4,
                      "Writer Object ID:          %s ",
                      object_id_to_string(offset, tvb, buff));
  offset +=4;

  /
  seq_nr_to_string(offset, little_endian, tvb, &sequenceNumber);
  proto_tree_add_text(rtps_submessage_tree, tvb, offset, 8,
                      "firstSeqNumber:   0x%X%X",
                      sequenceNumber.high, sequenceNumber.low);
  offset +=8;

  get_bitmap(tvb,&offset,little_endian,next_submsg_offset,rtps_submessage_tree);

}
