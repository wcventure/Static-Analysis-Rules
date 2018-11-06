static void
dissect_VAR(tvbuff_t *tvb, gint offset, guint8 flags, gboolean little_endian,
            int next_submsg_offset, proto_tree *rtps_submessage_tree)
{
  int                min_len;
  char               buff[200];
  SequenceNumber     writerSeqNumber;

  proto_tree_add_uint(rtps_submessage_tree, hf_rtps_submessage_flags,
                      tvb, offset, 1, flags);
  offset +=1;

  min_len = 20;
  if ((flags & FLAG_H) != 0)
    min_len += 8;
  if ((flags & FLAG_P) != 0)
    min_len += 4;
  if (next_submsg_offset < min_len)
  {
    proto_tree_add_uint_format(rtps_submessage_tree, hf_rtps_octets_to_next_header,
                               tvb, offset, 2, next_submsg_offset,
                               "Octets to next header: %u (bogus, must be >= %u)",
                               next_submsg_offset, min_len);
    return;
  }
  proto_tree_add_uint(rtps_submessage_tree, hf_rtps_octets_to_next_header,
                      tvb, offset, 2, next_submsg_offset);
  offset +=2;
  next_submsg_offset += offset;

  /
  proto_tree_add_text(rtps_submessage_tree, tvb, offset, 4,
                      "Reader Object ID:   %s ",
                       object_id_to_string(offset, tvb, buff));
  offset +=4;

  /
  proto_tree_add_text(rtps_submessage_tree, tvb, offset, 4,
                      "Writer Object ID:   %s ",
                      object_id_to_string(offset, tvb, buff));
  offset+=4;

  /
  if ((flags & FLAG_H) != 0)
  {
    /
    proto_tree_add_text(rtps_submessage_tree, tvb, offset, 4,
                        "Host ID:            %s",
                        host_id_to_string(offset,tvb,buff));
    offset+=4;

    /
    proto_tree_add_text(rtps_submessage_tree, tvb, offset, 4,
                        "App ID:             %s",
                        app_id_to_string(offset, tvb, buff));
    offset +=4;
  }

  /
  proto_tree_add_text(rtps_submessage_tree, tvb, offset, 4,
                      "Object ID:          %s ",
                      object_id_to_string(offset, tvb, buff));
  offset +=4;

  /
  seq_nr_to_string(offset, little_endian, tvb, &writerSeqNumber);
  proto_tree_add_text(rtps_submessage_tree, tvb, offset, 8,
                      "WriterSeqNumber:    0x%X%X",
                      writerSeqNumber.high, writerSeqNumber.low);
  offset +=8;

  /
  if ((flags & FLAG_P) != 0)
  {
    get_parameter_sequence(tvb, &offset, little_endian, next_submsg_offset,
                           rtps_submessage_tree);
  }
}
