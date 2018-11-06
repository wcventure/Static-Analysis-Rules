static void
get_parameter_sequence(tvbuff_t *tvb, gint *p_offset, gboolean little_endian,
                       gint next_submsg_offset, proto_tree *tree)
{
  proto_item             *ti;
  proto_tree             *rtps_parameter_sequence_tree;
  proto_tree             *rtps_parameter_tree;
  gint                    offset = *p_offset;
  guint16                 parameter, param_length;
  gint                    str_length;
  SequenceNumber          seqNumber;
  char                    buff_tmp[MAX_PATHNAME];
  int                     i;
  char                    sep;

  ti = proto_tree_add_text(tree, tvb, offset, (next_submsg_offset - offset),
                      "Parameters:");
  rtps_parameter_sequence_tree = proto_item_add_subtree(ti,
                                                        ett_rtps_parameter_sequence);
  for (;;)
  {
    if (next_submsg_offset-offset < 2)
    {
      proto_tree_add_text(rtps_parameter_sequence_tree, tvb, offset,
                          next_submsg_offset-offset,
                          "Parameter: < 2 bytes remain in message");
      offset = next_submsg_offset;
      break;
    }
    parameter    = get_guint16(tvb, offset, little_endian);
    ti = proto_tree_add_text(rtps_parameter_sequence_tree, tvb, offset, 2,
                             "%s",
                             val_to_str(parameter, parameter_id_vals,
                                        "Unknown parameter (0x%04X)"));
    rtps_parameter_tree = proto_item_add_subtree(ti, ett_rtps_parameter);
    proto_tree_add_uint(rtps_parameter_tree, hf_rtps_parameter_id,
                        tvb, offset, 2, parameter);
    offset +=2;
    if (next_submsg_offset-offset < 2)
    {
      proto_tree_add_text(rtps_parameter_tree, tvb, offset,
                          next_submsg_offset-offset,
                          "Parameter length: < 2 bytes remain in message");
      offset = next_submsg_offset;
      proto_item_set_end(ti, tvb, offset);
      break;
    }
    param_length = get_guint16(tvb, offset, little_endian);
    proto_tree_add_uint(rtps_parameter_tree, hf_rtps_parameter_length,
                        tvb, offset, 2, param_length);
    offset +=2;

    if (parameter == PID_SENTINEL) {
      proto_item_set_end(ti, tvb, offset);
      break;
    }

    if (next_submsg_offset-offset < param_length)
    {
      proto_tree_add_text(rtps_parameter_tree, tvb, offset,
                          next_submsg_offset-offset,
                          "Parameter value: < %u bytes remain in message",
                          param_length);
      offset = next_submsg_offset;
      proto_item_set_end(ti, tvb, offset);
      break;
    }
    proto_item_set_end(ti, tvb, offset + param_length);

    switch (parameter)
    {
      case PID_PAD:
        proto_item_append_text(ti, ": -");
        proto_tree_add_text(rtps_parameter_tree, tvb, offset, param_length,
                            "Padding");
        break;

      case PID_EXPIRATION_TIME:
      	if (param_length < 8)
      	{
          proto_tree_add_text(rtps_parameter_tree, tvb, offset, param_length,
                              "Bad parameter: length < 8");
        }
        else
        {
          char *ntp_time_str;

          ntp_time_str = get_NtpTime(offset, tvb, little_endian,buff_tmp);
          proto_item_append_text(ti, ": %s", ntp_time_str);
          proto_tree_add_text(rtps_parameter_tree, tvb, offset, param_length,
                              "Expiration time: %s", ntp_time_str);
        }
        break;

      case PID_PERSISTENCE:
      	if (param_length < 8)
      	{
          proto_tree_add_text(rtps_parameter_tree, tvb, offset, param_length,
                              "Bad parameter: length < 8");
        }
        else
        {
          char *ntp_time_str;

          ntp_time_str = get_NtpTime(offset, tvb, little_endian,buff_tmp);
          proto_item_append_text(ti, ": %s", ntp_time_str);
          proto_tree_add_text(rtps_parameter_tree, tvb, offset, param_length,
                              "Persistence: %s", ntp_time_str);
        }
        break;

      case PID_MINIMUM_SEPARATION:
      	if (param_length < 8)
      	{
          proto_tree_add_text(rtps_parameter_tree, tvb, offset, param_length,
                              "Bad parameter: length < 8");
        }
        else
        {
          char *ntp_time_str;

          ntp_time_str = get_NtpTime(offset, tvb, little_endian,buff_tmp);
          proto_item_append_text(ti, ": %s", ntp_time_str);
          proto_tree_add_text(rtps_parameter_tree, tvb, offset, param_length,
                              "Minimum separation: %s", ntp_time_str);
        }
        break;

      case PID_TOPIC: /
        str_length = tvb_strnlen(tvb, offset, param_length);
        if (str_length == -1)
        {
          proto_tree_add_text(rtps_parameter_tree, tvb, offset, param_length,
                              "Bad parameter: Terminating zero missing");
        }
        else
        {
          char *str;

          str = tvb_format_text(tvb, offset, str_length);
          proto_item_append_text(ti, ": %s", str);
          proto_tree_add_text(rtps_parameter_tree, tvb, offset, param_length,
                              "Topic: %s", str);
        }
        break;   

      case PID_STRENGTH:
      	if (param_length < 4)
      	{
          proto_tree_add_text(rtps_parameter_tree, tvb, offset, param_length,
                              "Bad parameter: length < 4");
        }
        else
        {
          guint32 strength;

          strength = get_guint32(tvb, offset, little_endian);
          proto_item_append_text(ti, ": 0x%X", strength);
          proto_tree_add_text(rtps_parameter_tree, tvb, offset, param_length,
                              "Strength: 0x%X", strength);
        }
        break;

      case PID_TYPE_NAME: /
        str_length = tvb_strnlen(tvb, offset, param_length);
        if (str_length == -1)
        {
          proto_tree_add_text(rtps_parameter_tree, tvb, offset, param_length,
                              "Bad parameter: Terminating zero missing");
        }
        else
        {
          char *str;

          str = tvb_format_text(tvb, offset, str_length);
          proto_item_append_text(ti, ": %s", str);
          proto_tree_add_text(rtps_parameter_tree, tvb, offset, param_length,
                              "Type name: %s", str);
        }
        break;   

      case PID_TYPE_CHECKSUM:
        /
      	if (param_length < 4)
      	{
          proto_tree_add_text(rtps_parameter_tree, tvb, offset, param_length,
                              "Bad parameter: length < 4");
        }
        else
        {
          guint32 checksum;

          checksum = get_guint32(tvb, offset, little_endian);
          proto_item_append_text(ti, ": 0x%X", checksum);
          proto_tree_add_text(rtps_parameter_tree, tvb, offset, param_length,
                              "Checksum: 0x%X", checksum);
        }
        break;

      case RTPS_PID_TYPE2_NAME:
        proto_tree_add_text(rtps_parameter_tree, tvb, offset, param_length,
                            "Parameter data");
        break;
                          
      case RTPS_PID_TYPE2_CHECKSUM:
        proto_tree_add_text(rtps_parameter_tree, tvb, offset, param_length,
                            "Parameter data");
        break;

      case PID_METATRAFFIC_MULTICAST_IPADDRESS:
        i = 0;
        sep = ':';
        while (param_length >= 4)
      	{
      	  char *ip_string;

          ip_string = IP_to_string(offset, tvb, little_endian,buff_tmp);
          proto_item_append_text(ti, "%c %s", sep, ip_string);
          proto_tree_add_text(rtps_parameter_tree, tvb, offset, param_length,
                              "Address[%d]: %s", i, ip_string);
          ++i;
          offset +=4;
          sep = ',';
          param_length -= 4; /       
        }
        offset += param_length;
        break;

      case PID_APP_IPADDRESS:
        i = 0;
        sep = ':';
        while (param_length >= 4)
      	{
      	  char *ip_string;

          ip_string = IP_to_string(offset, tvb, little_endian,buff_tmp);
          proto_item_append_text(ti, "%c %s", sep, ip_string);
          proto_tree_add_text(rtps_parameter_tree, tvb, offset, param_length,
                              "Address[%d]: %s", i, ip_string);
          ++i;
          offset +=4;
          sep = ',';
          param_length -= 4; /       
        }
        offset += param_length;
        break;

      case PID_METATRAFFIC_UNICAST_PORT:
      	if (param_length < 4)
      	{
          proto_tree_add_text(rtps_parameter_tree, tvb, offset, param_length,
                              "Bad parameter: length < 4");
        }
        else
        {
          char *port_str;

          port_str = port_to_string(offset, tvb, little_endian,buff_tmp);
          proto_item_append_text(ti, ": %s", port_str);
          proto_tree_add_text(rtps_parameter_tree, tvb, offset, param_length,
                              "Port: %s", port_str);
        }
        break;

      case PID_USERDATA_UNICAST_PORT:
      	if (param_length < 4)
      	{
          proto_tree_add_text(rtps_parameter_tree, tvb, offset, param_length,
                              "Bad parameter: length < 4");
        }
        else
        {
          char *port_str;

          port_str = port_to_string(offset, tvb, little_endian,buff_tmp);
          proto_item_append_text(ti, ": %s", port_str);
          proto_tree_add_text(rtps_parameter_tree, tvb, offset, param_length,
                              "Port: %s", port_str);
        }
        break;

      case PID_EXPECTS_ACK:
      	if (param_length < 1)
      	{
          proto_tree_add_text(rtps_parameter_tree, tvb, offset, param_length,
                              "Bad parameter: length < 1");
        }
        else
        {
          if (tvb_get_guint8(tvb, offset) == 0)
          {
            proto_item_append_text(ti, ": No");
            proto_tree_add_text(rtps_parameter_tree, tvb, offset, param_length,
                                "ACK expected: No");
          }
          else
          {
            proto_item_append_text(ti, ": Yes");
            proto_tree_add_text(rtps_parameter_tree, tvb, offset, param_length,
                                "ACK expected: Yes");
          }
        }
        break;

      case PID_USERDATA_MULTICAST_IPADDRESS:
        i = 0;
        sep = ':';
        while (param_length >= 4)
      	{
      	  char *ip_string;

          ip_string = IP_to_string(offset, tvb, little_endian,buff_tmp);
          proto_item_append_text(ti, "%c %s", sep, ip_string);
          proto_tree_add_text(rtps_parameter_tree, tvb, offset, param_length,
                              "Address[%d]: %s", i, ip_string);
          ++i;
          offset +=4;
          param_length -= 4; /       
        }
        offset += param_length;
        break;

      case PID_MANAGER_KEY:
        i = 0;
        sep = ':';
        while (param_length >= 4)
      	{
      	  guint32 manager_key;

          manager_key = get_guint32(tvb, offset, little_endian);
          proto_item_append_text(ti, "%c 0x%X", sep, manager_key);
          proto_tree_add_text(rtps_parameter_tree, tvb, offset, param_length,
                              "Key[%d]: 0x%X", i, manager_key);
          ++i;
          offset +=4;
          sep = ',';
          param_length -= 4; /       
        }
        offset += param_length;
        break;

      case PID_SEND_QUEUE_SIZE:
      	if (param_length < 4)
      	{
          proto_tree_add_text(rtps_parameter_tree, tvb, offset, param_length,
                              "Bad parameter: length < 4");
        }
        else
        {
          guint32 send_queue_size;

          send_queue_size = get_guint32(tvb, offset, little_endian);
          proto_item_append_text(ti, ": %u", send_queue_size);
          proto_tree_add_text(rtps_parameter_tree, tvb, offset, param_length,
                              "Send queue size: %u", send_queue_size);
        }
        break;

      case PID_PROTOCOL_VERSION:
      	if (param_length < 2)
      	{
          proto_tree_add_text(rtps_parameter_tree, tvb, offset, param_length,
                              "Bad parameter: length < 2");
        }
        else
        {
          char *protocol_version_str;

          protocol_version_str = protocol_version_to_string(offset, tvb, buff_tmp);
          proto_item_append_text(ti, ": %s", protocol_version_str);
          proto_tree_add_text(rtps_parameter_tree, tvb, offset, param_length,
                              "Protocol version: %s", protocol_version_str);
        }
        break;

      case PID_VENDOR_ID:
      	if (param_length < 2)
      	{
          proto_tree_add_text(rtps_parameter_tree, tvb, offset, param_length,
                              "Bad parameter: length < 2");
        }
        else
        {
          char *vendor_id_str;

          vendor_id_str = vendor_id_to_string(offset, tvb, buff_tmp);
          proto_item_append_text(ti, ": %s", vendor_id_str);
          proto_tree_add_text(rtps_parameter_tree, tvb, offset, param_length,
                              "Vendor ID: %s", vendor_id_str);
        }
        break;

      case PID_VARGAPPS_SEQUENCE_NUMBER_LAST:
      	if (param_length < 8)
      	{
          proto_tree_add_text(rtps_parameter_tree, tvb, offset, param_length,
                              "Bad parameter: length < 8");
        }
        else
        {
          seq_nr_to_string(offset, little_endian, tvb, &seqNumber);
          proto_item_append_text(ti, ": 0x%X%X",
                                 seqNumber.high, seqNumber.low);
          proto_tree_add_text(rtps_parameter_tree, tvb, offset, param_length,
                              "Sequence number: 0x%X%X",
                              seqNumber.high, seqNumber.low);
        }
        break;

      case PID_RECV_QUEUE_SIZE:
      	if (param_length < 4)
      	{
          proto_tree_add_text(rtps_parameter_tree, tvb, offset, param_length,
                              "Bad parameter: length < 4");
        }
        else
        {
          guint32 recv_queue_size;

          recv_queue_size = get_guint32(tvb, offset, little_endian);
          proto_item_append_text(ti, ": %u", recv_queue_size);
          proto_tree_add_text(rtps_parameter_tree, tvb, offset, param_length,
                              "Receive queue size: %u", recv_queue_size);
        }
        break;

      case PID_RELIABILITY_OFFERED:
        if (param_length < 4)
        {
          proto_tree_add_text(rtps_parameter_tree, tvb, offset, param_length,
                              "Bad parameter: length < 4");
        }
        else
        {
          guint32 reliability_offered;

          reliability_offered = get_guint32(tvb, offset, little_endian);
          proto_item_append_text(ti, ": 0x%X", reliability_offered);
          proto_tree_add_text(rtps_parameter_tree, tvb, offset, param_length,
                              "Reliability offered: 0x%X", reliability_offered);
        }
        break;

      case PID_RELIABILITY_REQUESTED:
        if (param_length < 4)
        {
          proto_tree_add_text(rtps_parameter_tree, tvb, offset, param_length,
                              "Bad parameter: length < 4");
        }
        else
        {
          guint32 reliability_requested;

          reliability_requested = get_guint32(tvb, offset, little_endian);
          proto_item_append_text(ti, ": 0x%X", reliability_requested);
          proto_tree_add_text(rtps_parameter_tree, tvb, offset, param_length,
                              "Reliability requested: 0x%X", reliability_requested);
        }
        break;

      default:
        proto_tree_add_text(rtps_parameter_tree, tvb, offset, param_length,
                            "Unknown parameter value");
        break;
    }   /

    offset += param_length;
  }

  *p_offset = offset;
}
