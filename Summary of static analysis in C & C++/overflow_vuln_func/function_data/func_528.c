static void
dissect_INFO_REPLY(tvbuff_t *tvb, gint offset, guint8 flags,
                   gboolean little_endian, int next_submsg_offset,
                   proto_tree *rtps_submessage_tree)
{
  int                     min_len;
  char                    buff_ip[10], buff_port[10];

  proto_tree_add_uint(rtps_submessage_tree, hf_rtps_submessage_flags,
                      tvb, offset, 1, flags);
  offset +=1;

 /

  if ((flags & FLAG_M) != 0)
    min_len = 16;
  else
    min_len = 8;
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
                      "Unicast Reply IP Adress: %s",
                      IP_to_string(offset, tvb, little_endian,buff_ip));
  offset +=4;


  /
  proto_tree_add_text(rtps_submessage_tree, tvb, offset, 4,
                      "Unicast Reply IP Port: %s",
                      port_to_string(offset, tvb, little_endian,buff_port));
  offset +=4;


 /

  if ((flags & FLAG_M) != 0)
  {
    /
    proto_tree_add_text(rtps_submessage_tree, tvb, offset, 4,
                        "Multicast Reply IP Adress: %s",
                        IP_to_string(offset, tvb, little_endian,buff_ip));
    offset +=4;

    /
    proto_tree_add_text(rtps_submessage_tree, tvb, offset, 4,
                        "Multicast Reply IP Port: %s",
                        port_to_string(offset, tvb, little_endian,buff_port));
    offset +=4;

  }
}
