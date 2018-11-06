static int
dissect_skinny_pdu(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, void* data _U_)
{
  int      offset   = 0;
  gboolean is_video = FALSE;    /

  /
  guint32  hdr_data_length;
  guint32  hdr_version;
  guint32  data_messageid;
  /

  guint    i = 0;
  guint    t = 0;
  int      j = 0;
  guint    count;
  int      val;
  guint32  ipversion;

  guint32 capCount;
  guint32 softKeyCount;
  guint32 softKeySetCount;
  guint16 validKeyMask;

  /
  proto_item *ti;
  proto_tree *skinny_tree = NULL;
  proto_tree *skinny_sub_tree;
  proto_tree *skinny_sub_tree_sav;
  proto_tree *skinny_sub_tree_sav_sav;

  proto_item *skm = NULL;
  proto_item *skm_tree = NULL;

  hdr_data_length = tvb_get_letohl(tvb, offset);
  hdr_version     = tvb_get_letohl(tvb, offset+4);
  data_messageid  = tvb_get_letohl(tvb, offset+8);

  /
  pi_current++;
  if (pi_current == MAX_SKINNY_MESSAGES_IN_PACKET)
  {
    /
    pi_current = 0;
  }
  si = &pi_arr[pi_current];
  si->messId = data_messageid;
  si->messageName = val_to_str_ext(data_messageid, &message_id_ext, "0x%08X (Unknown)");
  si->callId = 0;
  si->lineId = 0;
  si->passThruId = 0;
  si->callState = 0;
  g_free(si->callingParty);
  si->callingParty = NULL;
  g_free(si->calledParty);
  si->calledParty = NULL;

  /
  if (tree) {
    ti = proto_tree_add_item(tree, proto_skinny, tvb, offset, hdr_data_length+8, ENC_NA);
    skinny_tree = proto_item_add_subtree(ti, ett_skinny);
    proto_tree_add_uint(skinny_tree, hf_skinny_data_length, tvb, offset, 4, hdr_data_length);
    proto_tree_add_uint(skinny_tree, hf_skinny_hdr_version, tvb, offset+4, 4, hdr_version);
  }

  col_add_fstr(pinfo->cinfo, COL_INFO,"%s ", si->messageName);
  col_set_fence(pinfo->cinfo, COL_INFO);

  proto_tree_add_uint(skinny_tree, hf_skinny_messageid, tvb,offset+8, 4, data_messageid );

  {
    switch(data_messageid) {

    case 0x0000: /
      break;

    case 0x0001: /
      proto_tree_add_item(skinny_tree, hf_skinny_deviceName, tvb, offset+12, StationMaxDeviceNameSize, ENC_ASCII|ENC_NA);
      proto_tree_add_item(skinny_tree, hf_skinny_stationUserId, tvb, offset+28, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_stationInstance, tvb, offset+32, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_ipAddress, tvb, offset+36, 4, ENC_BIG_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_deviceType, tvb, offset+40, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_maxStreams, tvb, offset+44, 4, ENC_LITTLE_ENDIAN);
      if (hdr_data_length > 52) {
        proto_tree_add_item(skinny_tree, hf_skinny_activeStreams, tvb, offset+48, 4, ENC_LITTLE_ENDIAN);
        proto_tree_add_item(skinny_tree, hf_skinny_phoneFeatures, tvb, offset+52, 4, ENC_LITTLE_ENDIAN);
        proto_tree_add_item(skinny_tree, hf_skinny_IPVersion, tvb, offset+56, 4, ENC_LITTLE_ENDIAN);
      }
      if (hdr_data_length > 100) {
        proto_tree_add_item(skinny_tree, hf_skinny_unknown, tvb, offset+60, 4, ENC_LITTLE_ENDIAN);
        proto_tree_add_item(skinny_tree, hf_skinny_macAddress, tvb, offset+64, 12, ENC_ASCII|ENC_NA);
        proto_tree_add_item(skinny_tree, hf_skinny_unknown, tvb, offset+76, 4, ENC_LITTLE_ENDIAN);
        proto_tree_add_item(skinny_tree, hf_skinny_maxButtons, tvb, offset+80, 4, ENC_LITTLE_ENDIAN);
        proto_tree_add_item(skinny_tree, hf_skinny_ipV6Address, tvb, offset+84, 16, ENC_NA);
        proto_tree_add_item(skinny_tree, hf_skinny_unknown, tvb, offset+100, 4, ENC_LITTLE_ENDIAN);
        proto_tree_add_item(skinny_tree, hf_skinny_loadInfo, tvb, offset+104, hdr_data_length-96, ENC_ASCII|ENC_NA);
      }
      break;

    case 0x0002: /
      proto_tree_add_item(skinny_tree, hf_skinny_stationIpPort, tvb, offset+12, 2, ENC_BIG_ENDIAN);
      break;

    case 0x0003: /
      proto_tree_add_item(skinny_tree, hf_skinny_stationKeypadButton, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      if (hdr_data_length > 8) {
          proto_tree_add_item(skinny_tree, hf_skinny_lineInstance, tvb, offset+16, 4, ENC_LITTLE_ENDIAN);
          proto_tree_add_item(skinny_tree, hf_skinny_callIdentifier, tvb, offset+20, 4, ENC_LITTLE_ENDIAN);
          si->lineId = tvb_get_letohl(tvb, offset+16);
          si->callId = tvb_get_letohl(tvb, offset+20);
      }
      break;

    case 0x0004: /
      proto_tree_add_item(skinny_tree, hf_skinny_calledPartyNumber, tvb, offset+12, StationMaxDirnumSize, ENC_ASCII|ENC_NA);
      break;

    case 0x0005: /
      proto_tree_add_item(skinny_tree, hf_skinny_stimulus, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_stimulusInstance, tvb, offset+16, 4, ENC_LITTLE_ENDIAN);
      if (hdr_data_length > 12) {
        proto_tree_add_item(skinny_tree, hf_skinny_callIdentifier, tvb, offset+20, 4, ENC_LITTLE_ENDIAN);
        si->callId = tvb_get_letohl(tvb, offset+20);
      }
      break;

    case 0x0006: /
      if (hdr_data_length > 4) {
        proto_tree_add_item(skinny_tree, hf_skinny_lineInstance, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
        proto_tree_add_item(skinny_tree, hf_skinny_callIdentifier, tvb, offset+16, 4, ENC_LITTLE_ENDIAN);
        si->lineId = tvb_get_letohl(tvb, offset+12);
        si->callId = tvb_get_letohl(tvb, offset+16);
      }
      break;

    case 0x0007: /
      if (hdr_data_length > 4) {
        proto_tree_add_item(skinny_tree, hf_skinny_lineInstance, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
        proto_tree_add_item(skinny_tree, hf_skinny_callIdentifier, tvb, offset+16, 4, ENC_LITTLE_ENDIAN);
        si->lineId = tvb_get_letohl(tvb, offset+12);
        si->callId = tvb_get_letohl(tvb, offset+16);
      }
      break;

    case 0x0008: /
      break;

    case 0x0009: /
      proto_tree_add_item(skinny_tree, hf_skinny_lineNumber, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      break;

    case 0x000a: /
      proto_tree_add_item(skinny_tree, hf_skinny_speedDialNumber, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      break;

    case 0x000b: /
      proto_tree_add_item(skinny_tree, hf_skinny_lineNumber, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      break;

    case 0x000c: /
      break;

    case 0x000d: /
      break;

    case 0x000e: /
      break;

    case 0x000f: /
      break;

    case 0x0010: /
      capCount = tvb_get_letohl(tvb, offset+12);
      proto_tree_add_uint(skinny_tree, hf_skinny_capCount, tvb, offset+12, 4, capCount);
      for (i = 0; i < capCount; i++) {
        proto_tree_add_item(skinny_tree, hf_skinny_payloadCapability, tvb, offset+(i*16)+16, 4, ENC_LITTLE_ENDIAN);
        proto_tree_add_item(skinny_tree, hf_skinny_maxFramesPerPacket, tvb, offset+(i*16)+20, 2, ENC_LITTLE_ENDIAN);

        /
        /
        proto_tree_add_item(skinny_tree, hf_skinny_g723BitRate, tvb, offset+(i*16)+24, 2, ENC_LITTLE_ENDIAN);
        proto_tree_add_item(skinny_tree, hf_skinny_unknown, tvb, offset+(i*16)+26, 2, ENC_LITTLE_ENDIAN);
      }
      break;

    case 0x0011: /
      break;

    case 0x0012: /
      break;

    case 0x0020: /
      proto_tree_add_item(skinny_tree, hf_skinny_alarmSeverity, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_displayMessage, tvb, offset+16, StationMaxAlarmMessageSize, ENC_ASCII|ENC_NA);
      proto_tree_add_item(skinny_tree, hf_skinny_alarmParam1, tvb, offset+96, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_alarmParam2, tvb, offset+100, 4, ENC_BIG_ENDIAN);
      break;

    case 0x0021: /
      proto_tree_add_item(skinny_tree, hf_skinny_receptionStatus, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_passThruPartyID, tvb, offset+16, 4, ENC_LITTLE_ENDIAN);
      si->passThruId = tvb_get_letohl(tvb, offset+16);
      break;

    case 0x0022: /
      if (hdr_version == BASIC_MSG_TYPE) {
        address src_addr;
        guint32 ipv4_address;
        proto_tree_add_item(skinny_tree, hf_skinny_ORCStatus, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
        proto_tree_add_item(skinny_tree, hf_skinny_ipAddress, tvb, offset+16, 4, ENC_BIG_ENDIAN);
        proto_tree_add_item(skinny_tree, hf_skinny_portNumber, tvb, offset+20, 4, ENC_LITTLE_ENDIAN);
        proto_tree_add_item(skinny_tree, hf_skinny_passThruPartyID, tvb, offset+24, 4, ENC_LITTLE_ENDIAN);

        src_addr.type = AT_IPv4;
        src_addr.len = 4;
        src_addr.data = (guint8 *)&ipv4_address;
        ipv4_address = tvb_get_ipv4(tvb, offset+16);
        rtp_add_address(pinfo, &src_addr, tvb_get_letohl(tvb, offset+20), 0, "Skinny", pinfo->fd->num, is_video, NULL);

        si->passThruId = tvb_get_letohl(tvb, offset+24);
      } else if (hdr_version == CM7_MSG_TYPE_A || hdr_version == CM7_MSG_TYPE_B || hdr_version == CM7_MSG_TYPE_C || hdr_version == CM7_MSG_TYPE_D) {
        address src_addr;
        guint32 ipv4_address;

        proto_tree_add_item(skinny_tree, hf_skinny_ORCStatus, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
        /
        ipversion = tvb_get_ntohl(tvb, offset+16);
        proto_tree_add_item(skinny_tree, hf_skinny_IPVersion, tvb, offset+16, 4, ENC_LITTLE_ENDIAN);
        /
        if (ipversion == 0) {
          proto_tree_add_item(skinny_tree, hf_skinny_ipAddress, tvb, offset+20, 4, ENC_BIG_ENDIAN);
        } else {
          proto_tree_add_item(skinny_tree, hf_skinny_ipV6Address, tvb, offset+20, 16, ENC_NA);
        }
        proto_tree_add_item(skinny_tree, hf_skinny_portNumber, tvb, offset+36, 4, ENC_LITTLE_ENDIAN);
        proto_tree_add_item(skinny_tree, hf_skinny_passThruPartyID, tvb, offset+40, 4, ENC_LITTLE_ENDIAN);

        src_addr.type = AT_IPv4;
        src_addr.len = 4;
        src_addr.data = (guint8 *)&ipv4_address;
        ipv4_address = tvb_get_ipv4(tvb, offset+20);
        rtp_add_address(pinfo, &src_addr, tvb_get_letohl(tvb, offset+36), 0, "Skinny", pinfo->fd->num, is_video, NULL);

        si->passThruId = tvb_get_letohl(tvb, offset+40);
      }
      break;

    case 0x0023: /
      proto_tree_add_item(skinny_tree, hf_skinny_directoryNumber, tvb, offset+12, StationMaxDirnumSize, ENC_ASCII|ENC_NA);
      proto_tree_add_item(skinny_tree, hf_skinny_callIdentifier, tvb, offset+36, 4, ENC_LITTLE_ENDIAN);
      if (hdr_version == BASIC_MSG_TYPE || hdr_version == CM7_MSG_TYPE_A || hdr_version == CM7_MSG_TYPE_B) {
        proto_tree_add_item(skinny_tree, hf_skinny_statsProcessingType, tvb, offset+40, 4, ENC_LITTLE_ENDIAN);
        proto_tree_add_item(skinny_tree, hf_skinny_packetsSent, tvb, offset+44, 4, ENC_LITTLE_ENDIAN);
        proto_tree_add_item(skinny_tree, hf_skinny_octetsSent, tvb, offset+48, 4, ENC_LITTLE_ENDIAN);
        proto_tree_add_item(skinny_tree, hf_skinny_packetsRecv, tvb, offset+52, 4, ENC_LITTLE_ENDIAN);
        proto_tree_add_item(skinny_tree, hf_skinny_octetsRecv, tvb, offset+56, 4, ENC_LITTLE_ENDIAN);
        proto_tree_add_item(skinny_tree, hf_skinny_packetsLost, tvb, offset+60, 4, ENC_LITTLE_ENDIAN);
        proto_tree_add_item(skinny_tree, hf_skinny_jitter, tvb, offset+64, 4, ENC_LITTLE_ENDIAN);
        proto_tree_add_item(skinny_tree, hf_skinny_latency, tvb, offset+68, 4, ENC_LITTLE_ENDIAN);
        proto_tree_add_item(skinny_tree, hf_skinny_unknown, tvb, offset+72, 4, ENC_LITTLE_ENDIAN);
        proto_tree_add_item(skinny_tree, hf_skinny_unknown, tvb, offset+76, 4, ENC_LITTLE_ENDIAN);
        proto_tree_add_item(skinny_tree, hf_skinny_unknown, tvb, offset+80, 4, ENC_LITTLE_ENDIAN);
        proto_tree_add_item(skinny_tree, hf_skinny_unknown, tvb, offset+84, 4, ENC_LITTLE_ENDIAN);
        proto_tree_add_item(skinny_tree, hf_skinny_qualityStats, tvb, offset+88, hdr_data_length-80, ENC_ASCII|ENC_NA);
      } else if (hdr_version == CM7_MSG_TYPE_C || hdr_version == CM7_MSG_TYPE_D) {
        proto_tree_add_item(skinny_tree, hf_skinny_packetsSent, tvb, offset+40, 4, ENC_LITTLE_ENDIAN);
        proto_tree_add_item(skinny_tree, hf_skinny_octetsSent, tvb, offset+44, 4, ENC_LITTLE_ENDIAN);
        proto_tree_add_item(skinny_tree, hf_skinny_packetsRecv, tvb, offset+48, 4, ENC_LITTLE_ENDIAN);
        proto_tree_add_item(skinny_tree, hf_skinny_octetsRecv, tvb, offset+52, 4, ENC_LITTLE_ENDIAN);
        proto_tree_add_item(skinny_tree, hf_skinny_packetsLost, tvb, offset+56, 4, ENC_LITTLE_ENDIAN);
        proto_tree_add_item(skinny_tree, hf_skinny_jitter, tvb, offset+60, 4, ENC_LITTLE_ENDIAN);
        proto_tree_add_item(skinny_tree, hf_skinny_latency, tvb, offset+64, 4, ENC_LITTLE_ENDIAN);
        proto_tree_add_item(skinny_tree, hf_skinny_unknown, tvb, offset+68, 4, ENC_LITTLE_ENDIAN);
        proto_tree_add_item(skinny_tree, hf_skinny_unknown, tvb, offset+72, 4, ENC_LITTLE_ENDIAN);
        proto_tree_add_item(skinny_tree, hf_skinny_unknown, tvb, offset+76, 1, ENC_LITTLE_ENDIAN);
        proto_tree_add_item(skinny_tree, hf_skinny_qualityStats, tvb, offset+77, hdr_data_length-69, ENC_ASCII|ENC_NA);
      }
      si->callId = tvb_get_letohl(tvb, offset+36);
      break;

    case 0x0024: /
      proto_tree_add_item(skinny_tree, hf_skinny_calledPartyNumber, tvb, offset+12,StationMaxDirnumSize, ENC_ASCII|ENC_NA);
      break;

    case 0x0025: /
      break;

    case 0x0026: /
      proto_tree_add_item(skinny_tree, hf_skinny_softKeyEvent, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_lineInstance, tvb, offset+16, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_callIdentifier, tvb, offset+20, 4, ENC_LITTLE_ENDIAN);
      si->lineId = tvb_get_letohl(tvb, offset+16);
      si->callId = tvb_get_letohl(tvb, offset+20);
      break;

    case 0x0027: /
      break;

    case 0x0028: /
      break;

    case 0x0029: /
      proto_tree_add_item(skinny_tree, hf_skinny_deviceName, tvb, offset+12, StationMaxDeviceNameSize, ENC_ASCII|ENC_NA);
      i = offset+12+StationMaxDeviceNameSize;
      proto_tree_add_item(skinny_tree, hf_skinny_stationUserId, tvb, i, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_stationInstance, tvb, i+4, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_ipAddress, tvb, i+8, 4, ENC_BIG_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_deviceType, tvb, i+12, 4, ENC_LITTLE_ENDIAN);
      break;

    case 0x002A: /
      proto_tree_add_item(skinny_tree, hf_skinny_conferenceID, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_passThruPartyID, tvb, offset+16, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_ipAddress, tvb, offset+20, 4, ENC_BIG_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_portNumber, tvb, offset+24, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_callIdentifier, tvb, offset+28, 4, ENC_LITTLE_ENDIAN);
      si->passThruId = tvb_get_letohl(tvb, offset+16);
      si->callId = tvb_get_letohl(tvb, offset+28);
      break;

    case 0x002B: /
      proto_tree_add_item(skinny_tree, hf_skinny_headsetMode, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      break;

    case 0x002C: /
      proto_tree_add_item(skinny_tree, hf_skinny_deviceType, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_numberOfInServiceStreams, tvb, offset+16, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_maxStreamsPerConf, tvb, offset+20, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_numberOfOutOfServiceStreams, tvb, offset+24, 4, ENC_LITTLE_ENDIAN);
      break;

    case 0x002D: /
      proto_tree_add_item(skinny_tree, hf_skinny_numberLines, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      break;

    case 0x002E: /
      proto_tree_add_item(skinny_tree, hf_skinny_applicationID, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_lineInstance, tvb, offset+16, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_callIdentifier, tvb, offset+20, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_transactionID, tvb, offset+24, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_data_length, tvb, offset+28, 4, ENC_LITTLE_ENDIAN);
      count = tvb_get_letohl( tvb, offset+28);
      dissect_skinny_xml(skinny_tree, tvb, pinfo, offset+30, count);
      si->lineId = tvb_get_letohl(tvb, offset+16);
      si->callId = tvb_get_letohl(tvb, offset+20);
      break;

    case 0x002F: /
      proto_tree_add_item(skinny_tree, hf_skinny_applicationID, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_lineInstance, tvb, offset+16, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_callIdentifier, tvb, offset+20, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_transactionID, tvb, offset+24, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_data_length, tvb, offset+28, 4, ENC_LITTLE_ENDIAN);
      count = tvb_get_letohl( tvb, offset+28);
      dissect_skinny_xml(skinny_tree, tvb, pinfo, offset+30, count);
      si->lineId = tvb_get_letohl(tvb, offset+12);
      si->callId = tvb_get_letohl(tvb, offset+16);
      break;

    case 0x0030: /
      /
      /
      proto_tree_add_item(skinny_tree, hf_skinny_audioCapCount, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_videoCapCount, tvb, offset+16, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_dataCapCount, tvb, offset+20, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_RTPPayloadFormat, tvb, offset+24, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_customPictureFormatCount, tvb, offset+28, 4, ENC_LITTLE_ENDIAN);
      count = offset+32;
      capCount=tvb_get_letohl(tvb, offset+28);
      for ( i = 0; i < MAX_CUSTOM_PICTURES; i++ ) {
        if (i < capCount) {
          skinny_sub_tree = proto_tree_add_subtree_format(skinny_tree, tvb, offset, 20, ett_skinny_tree, NULL,
                                    "customPictureFormat[%d]", i);
          proto_tree_add_item(skinny_sub_tree, hf_skinny_pictureWidth, tvb, count, 4, ENC_LITTLE_ENDIAN);
          count+= 4;
          proto_tree_add_item(skinny_sub_tree, hf_skinny_pictureHeight, tvb, count, 4, ENC_LITTLE_ENDIAN);
          count+= 4;
          proto_tree_add_item(skinny_sub_tree, hf_skinny_pixelAspectRatio, tvb, count, 4, ENC_LITTLE_ENDIAN);
          count+= 4;
          proto_tree_add_item(skinny_sub_tree, hf_skinny_clockConversionCode, tvb, count, 4, ENC_LITTLE_ENDIAN);
          count+= 4;
          proto_tree_add_item(skinny_sub_tree, hf_skinny_clockDivisor, tvb, count, 4, ENC_LITTLE_ENDIAN);
          count+= 4;
        } else {
          count+=20;
        }
      }
      skinny_sub_tree = proto_tree_add_subtree(skinny_tree, tvb, offset, 8, ett_skinny_tree, NULL, "confResources");
      proto_tree_add_item(skinny_sub_tree, hf_skinny_activeStreamsOnRegistration, tvb, count, 4, ENC_LITTLE_ENDIAN);
      count+= 4;
      proto_tree_add_item(skinny_sub_tree, hf_skinny_maxBW, tvb, count, 4, ENC_LITTLE_ENDIAN);
      count+= 4;
      proto_tree_add_item(skinny_sub_tree, hf_skinny_serviceResourceCount, tvb, count, 4, ENC_LITTLE_ENDIAN);
      count+= 4;
      skinny_sub_tree_sav = skinny_sub_tree;
      for ( i = 0; i < MAX_SERVICE_TYPE; i++ ) {
        skinny_sub_tree = proto_tree_add_subtree_format(skinny_sub_tree_sav, tvb, offset, 20, ett_skinny_tree, NULL,
                                                        "serviceResource[%d]", i);
        proto_tree_add_item(skinny_sub_tree, hf_skinny_layoutCount, tvb, count, 4, ENC_LITTLE_ENDIAN);
        count+= 4;
        skinny_sub_tree_sav_sav = skinny_sub_tree_sav;
        for ( t = 0; t < MAX_LAYOUT_WITH_SAME_SERVICE; t++ ) {
          skinny_sub_tree = proto_tree_add_subtree_format(skinny_sub_tree_sav, tvb, offset, 20, ett_skinny_tree, NULL, "layouts[%d]", t);
          proto_tree_add_item(skinny_sub_tree, hf_skinny_layout, tvb, count, 4, ENC_LITTLE_ENDIAN);
          count+= 4;
        }
        skinny_sub_tree = skinny_sub_tree_sav_sav;
        proto_tree_add_item(skinny_sub_tree, hf_skinny_serviceNum, tvb, count, 4, ENC_LITTLE_ENDIAN);
        count+= 4;
        proto_tree_add_item(skinny_sub_tree, hf_skinny_maxStreams, tvb, count, 4, ENC_LITTLE_ENDIAN);
        count+= 4;
        proto_tree_add_item(skinny_sub_tree, hf_skinny_maxConferences, tvb, count, 4, ENC_LITTLE_ENDIAN);
        count+= 4;
        proto_tree_add_item(skinny_sub_tree, hf_skinny_activeConferenceOnRegistration, tvb, count, 4, ENC_LITTLE_ENDIAN);
        count+= 4;
      }
      capCount=tvb_get_letohl(tvb, offset+12);
      for ( i = 0; i < StationMaxCapabilities; i++ ) {
        if (i < capCount) {
          skinny_sub_tree = proto_tree_add_subtree_format(skinny_tree, tvb, offset, 20, ett_skinny_tree, NULL, "audiocaps[%d]", i);
          proto_tree_add_item(skinny_sub_tree, hf_skinny_payloadCapability, tvb, count, 4, ENC_LITTLE_ENDIAN);
          count+= 4;
          proto_tree_add_item(skinny_sub_tree, hf_skinny_maxFramesPerPacket, tvb, count, 2, ENC_LITTLE_ENDIAN);
          count+= 2;
          proto_tree_add_item(skinny_sub_tree, hf_skinny_g723BitRate, tvb, count, 2, ENC_LITTLE_ENDIAN);
          count+= 2;
          proto_tree_add_item(skinny_sub_tree, hf_skinny_unknown, tvb, count, 2, ENC_LITTLE_ENDIAN);
          count+= 8;
        } else {
          count+= 16;
        }
      }
      capCount=tvb_get_letohl(tvb, offset+16);
      for ( i = 0; i < StationMaxVideoCapabilities; i++ ) {
        if (i < capCount) {
          skinny_sub_tree = proto_tree_add_subtree_format(skinny_tree, tvb, offset, 20, ett_skinny_tree, NULL, "vidCaps[%d]", i);
          proto_tree_add_item(skinny_sub_tree, hf_skinny_payloadCapability, tvb, count, 4, ENC_LITTLE_ENDIAN);
          count+= 4;
          proto_tree_add_item(skinny_sub_tree, hf_skinny_transmitOrReceive, tvb, count, 4, ENC_LITTLE_ENDIAN);
          count+= 4;
          proto_tree_add_item(skinny_sub_tree, hf_skinny_levelPreferenceCount, tvb, count, 4, ENC_LITTLE_ENDIAN);
          count+= 4;
          skinny_sub_tree_sav = skinny_sub_tree;
          for ( t = 0; t < MAX_LEVEL_PREFERENCE; t++ ) {
            skinny_sub_tree = proto_tree_add_subtree_format(skinny_sub_tree_sav, tvb, offset, 20, ett_skinny_tree, NULL, "levelPreference[%d]", t);
            proto_tree_add_item(skinny_sub_tree, hf_skinny_transmitPreference, tvb, count, 4, ENC_LITTLE_ENDIAN);
            count+= 4;
            proto_tree_add_item(skinny_sub_tree, hf_skinny_format, tvb, count, 4, ENC_LITTLE_ENDIAN);
            count+= 4;
            proto_tree_add_item(skinny_sub_tree, hf_skinny_maxBitRate, tvb, count, 4, ENC_LITTLE_ENDIAN);
            count+= 4;
            proto_tree_add_item(skinny_sub_tree, hf_skinny_minBitRate, tvb, count, 4, ENC_LITTLE_ENDIAN);
            count+= 4;
            proto_tree_add_item(skinny_sub_tree, hf_skinny_MPI, tvb, count, 4, ENC_LITTLE_ENDIAN);
            count+= 4;
            proto_tree_add_item(skinny_sub_tree, hf_skinny_serviceNumber, tvb, count, 4, ENC_LITTLE_ENDIAN);
            count+= 4;
          }
          val = count;

          /
          skinny_sub_tree = proto_tree_add_subtree(skinny_sub_tree_sav, tvb, offset, 8, ett_skinny_tree, NULL, "h261VideoCapability");
          proto_tree_add_item(skinny_sub_tree, hf_skinny_temporalSpatialTradeOffCapability, tvb, count, 4, ENC_LITTLE_ENDIAN);
          count+= 4;
          proto_tree_add_item(skinny_sub_tree, hf_skinny_stillImageTransmission, tvb, count, 4, ENC_LITTLE_ENDIAN);
          /

          /
          count = val;
          skinny_sub_tree = proto_tree_add_subtree(skinny_sub_tree_sav, tvb, offset, 8, ett_skinny_tree, NULL, "h263VideoCapability");
          proto_tree_add_item(skinny_sub_tree, hf_skinny_h263_capability_bitfield, tvb, count, 4, ENC_LITTLE_ENDIAN);
          count+= 4;
          proto_tree_add_item(skinny_sub_tree, hf_skinny_annexNandWFutureUse, tvb, count, 4, ENC_LITTLE_ENDIAN);
          /

          /
          count = val;
          skinny_sub_tree = proto_tree_add_subtree(skinny_sub_tree_sav, tvb, offset, 8, ett_skinny_tree, NULL, "vieoVideoCapability");
          proto_tree_add_item(skinny_sub_tree, hf_skinny_modelNumber, tvb, count, 4, ENC_LITTLE_ENDIAN);
          count+= 4;
          proto_tree_add_item(skinny_sub_tree, hf_skinny_bandwidth, tvb, count, 4, ENC_LITTLE_ENDIAN);
          count+= 4;
        } else {
          count+=28 + (MAX_LEVEL_PREFERENCE-1)*4;
        }
      }
      capCount=tvb_get_letohl(tvb, offset+20);
      for ( i = 0; i < StationMaxDataCapabilities; i++ ) {
        if ( i < capCount) {
          skinny_sub_tree = proto_tree_add_subtree_format(skinny_tree, tvb, offset, 20, ett_skinny_tree, NULL, "dataCaps[%d]", i);
          proto_tree_add_item(skinny_sub_tree, hf_skinny_payloadCapability, tvb, count, 4, ENC_LITTLE_ENDIAN);
          count+= 4;
          proto_tree_add_item(skinny_sub_tree, hf_skinny_transmitOrReceive, tvb, count, 4, ENC_LITTLE_ENDIAN);
          count+= 4;
          proto_tree_add_item(skinny_sub_tree, hf_skinny_protocolDependentData, tvb, count, 4, ENC_LITTLE_ENDIAN);
          count+= 4;
          proto_tree_add_item(skinny_sub_tree, hf_skinny_maxBitRate, tvb, count, 4, ENC_LITTLE_ENDIAN);
          count+= 4;
        } else {
          count+= 16;
        }
      }
      break;

    case 0x0031: /
      proto_tree_add_item(skinny_tree, hf_skinny_ORCStatus, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_ipAddress, tvb, offset+16, 4, ENC_BIG_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_portNumber, tvb, offset+20, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_passThruPartyID, tvb, offset+24, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_callIdentifier, tvb, offset+28, 4, ENC_LITTLE_ENDIAN);
      si->passThruId = tvb_get_letohl(tvb, offset+24);
      si->callId = tvb_get_letohl(tvb, offset+28);
      break;

    case 0x0032: /
      proto_tree_add_item(skinny_tree, hf_skinny_conferenceID, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_serviceNum, tvb, offset+16, 4, ENC_LITTLE_ENDIAN);
      break;

    case 0x0033: /
      proto_tree_add_item(skinny_tree, hf_skinny_serviceURLIndex, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      break;

    case 0x0034: /
      proto_tree_add_item(skinny_tree, hf_skinny_featureIndex, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      break;

    case 0x0035: /
      proto_tree_add_item(skinny_tree, hf_skinny_conferenceID, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_createConfResults, tvb, offset+16, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_data_length, tvb, offset+20, 4, ENC_LITTLE_ENDIAN);
      count = tvb_get_letohl( tvb, offset+20);
      proto_tree_add_uint(skinny_tree, hf_skinny_passThruData, tvb, offset+24, 1, count);
      break;

    case 0x0036: /
      proto_tree_add_item(skinny_tree, hf_skinny_conferenceID, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_deleteConfResults, tvb, offset+16, 4, ENC_LITTLE_ENDIAN);
      break;

    case 0x0037: /
      proto_tree_add_item(skinny_tree, hf_skinny_conferenceID, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_modifyConfResults, tvb, offset+16, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_data_length, tvb, offset+20, 4, ENC_LITTLE_ENDIAN);
      count = tvb_get_letohl( tvb, offset+20);
      proto_tree_add_uint(skinny_tree, hf_skinny_passThruData, tvb, offset+24, 1, count);
      break;

    case 0x0038: /
      proto_tree_add_item(skinny_tree, hf_skinny_conferenceID, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_callIdentifier, tvb, offset+16, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_addParticipantResults, tvb, offset+20, 4, ENC_LITTLE_ENDIAN);
      si->callId = tvb_get_letohl(tvb, offset+16);
      break;

    case 0x0039: /
      proto_tree_add_item(skinny_tree, hf_skinny_last, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_numberOfEntries, tvb, offset+16, 4, ENC_LITTLE_ENDIAN);
      count = offset+20;
      for ( i = 0; i < StationMaxConference; i++ ) {
        proto_tree_add_item(skinny_tree, hf_skinny_conferenceID, tvb, count, 4, ENC_LITTLE_ENDIAN);
        count += 4;
        proto_tree_add_item(skinny_tree, hf_skinny_resourceTypes, tvb, count, 4, ENC_LITTLE_ENDIAN);
        count += 4;
        proto_tree_add_item(skinny_tree, hf_skinny_numberOfReservedParticipants, tvb, count, 4, ENC_LITTLE_ENDIAN);
        count += 4;
        proto_tree_add_item(skinny_tree, hf_skinny_numberOfActiveParticipants, tvb, count, 4, ENC_LITTLE_ENDIAN);
        count += 4;
        proto_tree_add_item(skinny_tree, hf_skinny_appID, tvb, count, 4, ENC_LITTLE_ENDIAN);
        count += 4;
        proto_tree_add_uint(skinny_tree, hf_skinny_appConfID, tvb, count, 1, AppConferenceIDSize);
        count += AppConferenceIDSize;
        proto_tree_add_uint(skinny_tree, hf_skinny_appData, tvb, count, 1, AppDataSize);
        count += AppDataSize;
      }
      break;

    case 0x0040: /
      proto_tree_add_item(skinny_tree, hf_skinny_auditParticipantResults, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_last, tvb, offset+16, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_conferenceID, tvb, offset+20, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_numberOfEntries, tvb, offset+24, 4, ENC_LITTLE_ENDIAN);
      count = tvb_get_letohl( tvb, offset+24);
      for ( i = 0; i < count; i++ ) {
        proto_tree_add_item(skinny_tree, hf_skinny_participantEntry, tvb, offset+28+(i*4), 4, ENC_LITTLE_ENDIAN);
      }
      break;

    case 0x0041: /
      proto_tree_add_item(skinny_tree, hf_skinny_applicationID, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_lineInstance, tvb, offset+16, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_callIdentifier, tvb, offset+20, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_transactionID, tvb, offset+24, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_data_length, tvb, offset+28, 4, ENC_LITTLE_ENDIAN);
      count = tvb_get_letohl( tvb, offset+28);
      proto_tree_add_item(skinny_tree, hf_skinny_sequenceFlag, tvb, offset+30, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_displayPriority, tvb, offset+34, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_conferenceID, tvb, offset+38, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_appInstanceID, tvb, offset+42, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_routingID, tvb, offset+46, 4, ENC_LITTLE_ENDIAN);
      dissect_skinny_xml(skinny_tree, tvb, pinfo, offset+50, count);
      si->lineId = tvb_get_letohl(tvb, offset+16);
      si->callId = tvb_get_letohl(tvb, offset+20);
      break;

    case 0x0042: /
      proto_tree_add_item(skinny_tree, hf_skinny_applicationID, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_lineInstance, tvb, offset+16, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_callIdentifier, tvb, offset+20, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_transactionID, tvb, offset+24, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_data_length, tvb, offset+28, 4, ENC_LITTLE_ENDIAN);
      count = tvb_get_letohl( tvb, offset+28);
      proto_tree_add_item(skinny_tree, hf_skinny_sequenceFlag, tvb, offset+30, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_displayPriority, tvb, offset+34, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_conferenceID, tvb, offset+38, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_appInstanceID, tvb, offset+42, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_routingID, tvb, offset+46, 4, ENC_LITTLE_ENDIAN);
      dissect_skinny_xml(skinny_tree, tvb, pinfo, offset+50, count);
      si->lineId = tvb_get_letohl(tvb, offset+16);
      si->callId = tvb_get_letohl(tvb, offset+20);
      break;

    case 0x0044: /
      proto_tree_add_item(skinny_tree, hf_skinny_audioCapCount, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_videoCapCount, tvb, offset+16, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_dataCapCount, tvb, offset+20, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_RTPPayloadFormat, tvb, offset+24, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_customPictureFormatCount, tvb, offset+28, 4, ENC_LITTLE_ENDIAN);
      count = offset+32;
      capCount=tvb_get_letohl(tvb, offset+28);
      for ( i = 0; i < MAX_CUSTOM_PICTURES; i++ ) {
        if (i < capCount) {
          skinny_sub_tree = proto_tree_add_subtree_format(skinny_tree, tvb, offset, 20, ett_skinny_tree, NULL, "customPictureFormat[%d]", i);
          proto_tree_add_item(skinny_sub_tree, hf_skinny_pictureWidth, tvb, count, 4, ENC_LITTLE_ENDIAN);
          count+= 4;
          proto_tree_add_item(skinny_sub_tree, hf_skinny_pictureHeight, tvb, count, 4, ENC_LITTLE_ENDIAN);
          count+= 4;
          proto_tree_add_item(skinny_sub_tree, hf_skinny_pixelAspectRatio, tvb, count, 4, ENC_LITTLE_ENDIAN);
          count+= 4;
          proto_tree_add_item(skinny_sub_tree, hf_skinny_clockConversionCode, tvb, count, 4, ENC_LITTLE_ENDIAN);
          count+= 4;
          proto_tree_add_item(skinny_sub_tree, hf_skinny_clockDivisor, tvb, count, 4, ENC_LITTLE_ENDIAN);
          count+= 4;
        } else {
          count+=20;
        }
      }
      skinny_sub_tree = proto_tree_add_subtree(skinny_tree, tvb, offset, 8, ett_skinny_tree, NULL, "confResources");
      proto_tree_add_item(skinny_sub_tree, hf_skinny_activeStreamsOnRegistration, tvb, count, 4, ENC_LITTLE_ENDIAN);
      count+= 4;
      proto_tree_add_item(skinny_sub_tree, hf_skinny_maxBW, tvb, count, 4, ENC_LITTLE_ENDIAN);
      count+= 4;
      proto_tree_add_item(skinny_sub_tree, hf_skinny_serviceResourceCount, tvb, count, 4, ENC_LITTLE_ENDIAN);
      count+= 4;
      skinny_sub_tree_sav = skinny_sub_tree;
      for ( i = 0; i < MAX_SERVICE_TYPE; i++ ) {
        skinny_sub_tree = proto_tree_add_subtree_format(skinny_sub_tree_sav, tvb, offset, 20, ett_skinny_tree, NULL, "serviceResource[%d]", i);
        proto_tree_add_item(skinny_sub_tree, hf_skinny_layoutCount, tvb, count, 4, ENC_LITTLE_ENDIAN);
        count+= 4;
        skinny_sub_tree_sav_sav = skinny_sub_tree_sav;
        for ( t = 0; t < MAX_LAYOUT_WITH_SAME_SERVICE; t++ ) {
          skinny_sub_tree = proto_tree_add_subtree_format(skinny_sub_tree_sav, tvb, offset, 20, ett_skinny_tree, NULL, "layouts[%d]", t);
          proto_tree_add_item(skinny_sub_tree, hf_skinny_layout, tvb, count, 4, ENC_LITTLE_ENDIAN);
          count+= 4;
        }
        skinny_sub_tree = skinny_sub_tree_sav_sav;
        proto_tree_add_item(skinny_sub_tree, hf_skinny_serviceNum, tvb, count, 4, ENC_LITTLE_ENDIAN);
        count+= 4;
        proto_tree_add_item(skinny_sub_tree, hf_skinny_maxStreams, tvb, count, 4, ENC_LITTLE_ENDIAN);
        count+= 4;
        proto_tree_add_item(skinny_sub_tree, hf_skinny_maxConferences, tvb, count, 4, ENC_LITTLE_ENDIAN);
        count+= 4;
        proto_tree_add_item(skinny_sub_tree, hf_skinny_activeConferenceOnRegistration, tvb, count, 4, ENC_LITTLE_ENDIAN);
        count+= 4;
      }
      capCount=tvb_get_letohl(tvb, offset+12);
      for ( i = 0; i < StationMaxCapabilities; i++ ) {
        if (i < capCount) {
          skinny_sub_tree = proto_tree_add_subtree_format(skinny_tree, tvb, offset, 20, ett_skinny_tree, NULL, "audiocaps[%d]", i);
          proto_tree_add_item(skinny_sub_tree, hf_skinny_payloadCapability, tvb, count, 4, ENC_LITTLE_ENDIAN);
          count+= 4;
          proto_tree_add_item(skinny_sub_tree, hf_skinny_maxFramesPerPacket, tvb, count, 2, ENC_LITTLE_ENDIAN);
          count+= 2;
          proto_tree_add_item(skinny_sub_tree, hf_skinny_g723BitRate, tvb, count, 2, ENC_LITTLE_ENDIAN);
          count+= 2;
          proto_tree_add_item(skinny_sub_tree, hf_skinny_unknown, tvb, count, 2, ENC_LITTLE_ENDIAN);
          count+= 8;
        } else {
          count+= 16;
        }
      }
      capCount=tvb_get_letohl(tvb, offset+16);
      for ( i = 0; i < StationMaxVideoCapabilities; i++ ) {
        if (i < capCount) {
          skinny_sub_tree = proto_tree_add_subtree_format(skinny_tree, tvb, offset, 20, ett_skinny_tree, NULL, "vidCaps[%d]", i);
          proto_tree_add_item(skinny_sub_tree, hf_skinny_payloadCapability, tvb, count, 4, ENC_LITTLE_ENDIAN);
          count+= 4;
          proto_tree_add_item(skinny_sub_tree, hf_skinny_transmitOrReceive, tvb, count, 4, ENC_LITTLE_ENDIAN);
          count+= 4;
          proto_tree_add_item(skinny_sub_tree, hf_skinny_levelPreferenceCount, tvb, count, 4, ENC_LITTLE_ENDIAN);
          count+= 4;
          skinny_sub_tree_sav = skinny_sub_tree;
          for ( t = 0; t < MAX_LEVEL_PREFERENCE; t++ ) {
            skinny_sub_tree = proto_tree_add_subtree_format(skinny_sub_tree_sav, tvb, offset, 20, ett_skinny_tree, NULL, "levelPreference[%d]", t);
            proto_tree_add_item(skinny_sub_tree, hf_skinny_transmitPreference, tvb, count, 4, ENC_LITTLE_ENDIAN);
            count+= 4;
            proto_tree_add_item(skinny_sub_tree, hf_skinny_format, tvb, count, 4, ENC_LITTLE_ENDIAN);
            count+= 4;
            proto_tree_add_item(skinny_sub_tree, hf_skinny_maxBitRate, tvb, count, 4, ENC_LITTLE_ENDIAN);
            count+= 4;
            proto_tree_add_item(skinny_sub_tree, hf_skinny_minBitRate, tvb, count, 4, ENC_LITTLE_ENDIAN);
            count+= 4;
            proto_tree_add_item(skinny_sub_tree, hf_skinny_MPI, tvb, count, 4, ENC_LITTLE_ENDIAN);
            count+= 4;
            proto_tree_add_item(skinny_sub_tree, hf_skinny_serviceNumber, tvb, count, 4, ENC_LITTLE_ENDIAN);
            count+= 4;
          }
          val = count;

          /
          skinny_sub_tree = proto_tree_add_subtree(skinny_sub_tree_sav, tvb, offset, 8, ett_skinny_tree, NULL, "h261VideoCapability");
          proto_tree_add_item(skinny_sub_tree, hf_skinny_temporalSpatialTradeOffCapability, tvb, count, 4, ENC_LITTLE_ENDIAN);
          count+= 4;
          proto_tree_add_item(skinny_sub_tree, hf_skinny_stillImageTransmission, tvb, count, 4, ENC_LITTLE_ENDIAN);
          /

          /
          count = val;
          skinny_sub_tree = proto_tree_add_subtree(skinny_sub_tree_sav, tvb, offset, 8, ett_skinny_tree, NULL, "h263VideoCapability");
          proto_tree_add_item(skinny_sub_tree, hf_skinny_h263_capability_bitfield, tvb, count, 4, ENC_LITTLE_ENDIAN);
          count+= 4;
          proto_tree_add_item(skinny_sub_tree, hf_skinny_annexNandWFutureUse, tvb, count, 4, ENC_LITTLE_ENDIAN);
          /

          /
          count = val;
          skinny_sub_tree = proto_tree_add_subtree(skinny_sub_tree_sav, tvb, offset, 8, ett_skinny_tree, NULL, "vieoVideoCapability");
          proto_tree_add_item(skinny_sub_tree, hf_skinny_modelNumber, tvb, count, 4, ENC_LITTLE_ENDIAN);
          count+= 4;
          proto_tree_add_item(skinny_sub_tree, hf_skinny_bandwidth, tvb, count, 4, ENC_LITTLE_ENDIAN);
          count+= 4;
        } else {
          count+=28 + (MAX_LEVEL_PREFERENCE-1)*4;
        }
      }
      capCount=tvb_get_letohl(tvb, offset+20);
      for ( i = 0; i < StationMaxDataCapabilities; i++ ) {
        if ( i < capCount) {
          skinny_sub_tree = proto_tree_add_subtree_format(skinny_tree, tvb, offset, 20, ett_skinny_tree, NULL, "dataCaps[%d]", i);
          proto_tree_add_item(skinny_sub_tree, hf_skinny_payloadCapability, tvb, count, 4, ENC_LITTLE_ENDIAN);
          count+= 4;
          proto_tree_add_item(skinny_sub_tree, hf_skinny_transmitOrReceive, tvb, count, 4, ENC_LITTLE_ENDIAN);
          count+= 4;
          proto_tree_add_item(skinny_sub_tree, hf_skinny_protocolDependentData, tvb, count, 4, ENC_LITTLE_ENDIAN);
          count+= 4;
          proto_tree_add_item(skinny_sub_tree, hf_skinny_maxBitRate, tvb, count, 4, ENC_LITTLE_ENDIAN);
          count+= 4;
        } else {
          count+= 16;
        }
      }
      break;

    case 0x0048: /
      proto_tree_add_item(skinny_tree, hf_skinny_directoryIndex, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_lineInstance, tvb, offset+16, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_unknown, tvb, offset+20, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_directoryPhoneNumber, tvb, offset+24, hdr_data_length-16, ENC_ASCII|ENC_NA);
      break;

    case 0x0049: /
      proto_tree_add_item(skinny_tree, hf_skinny_accessory, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_accessoryState, tvb, offset+16, 4, ENC_LITTLE_ENDIAN);
      if (hdr_data_length > 16) {
        proto_tree_add_item(skinny_tree, hf_skinny_unknown, tvb, offset+20, 4, ENC_LITTLE_ENDIAN);
      }
      break;

    case 0x004A: /
      proto_tree_add_item(skinny_tree, hf_skinny_unknown, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_unknown, tvb, offset+16, 4, ENC_LITTLE_ENDIAN);
      if (hdr_data_length > 16) {
        proto_tree_add_item(skinny_tree, hf_skinny_unknown, tvb, offset+20, 4, ENC_LITTLE_ENDIAN);
      }
      break;

    case 0x004C: /
      proto_tree_add_item(skinny_tree, hf_skinny_unknown, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      break;

      /
    case 0x0081: /
      proto_tree_add_item(skinny_tree, hf_skinny_keepAliveInterval, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_dateTemplate, tvb, offset+16, StationDateTemplateSize, ENC_ASCII|ENC_NA);
      proto_tree_add_item(skinny_tree, hf_skinny_secondaryKeepAliveInterval, tvb, offset+24, 4, ENC_LITTLE_ENDIAN);
      break;

    case 0x0082: /
      proto_tree_add_item(skinny_tree, hf_skinny_deviceTone, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      /
      if (hdr_data_length > 12) {
        proto_tree_add_item(skinny_tree, hf_skinny_lineInstance, tvb, offset+20, 4, ENC_LITTLE_ENDIAN);
        proto_tree_add_item(skinny_tree, hf_skinny_callIdentifier, tvb, offset+24, 4, ENC_LITTLE_ENDIAN);
        si->lineId = tvb_get_letohl(tvb, offset+20);
        si->callId = tvb_get_letohl(tvb, offset+24);
      }
      break;

    case 0x0083: /
      if (hdr_data_length > 4) {
        proto_tree_add_item(skinny_tree, hf_skinny_lineInstance, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
        proto_tree_add_item(skinny_tree, hf_skinny_callIdentifier, tvb, offset+16, 4, ENC_LITTLE_ENDIAN);
        si->lineId = tvb_get_letohl(tvb, offset+12);
        si->callId = tvb_get_letohl(tvb, offset+16);
      }
      break;

    case 0x0085: /
      proto_tree_add_item(skinny_tree, hf_skinny_ringType, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_ringMode, tvb, offset+16, 4, ENC_LITTLE_ENDIAN);
      if (hdr_data_length > 12) {
        proto_tree_add_item(skinny_tree, hf_skinny_lineInstance, tvb, offset+20, 4, ENC_LITTLE_ENDIAN);
        proto_tree_add_item(skinny_tree, hf_skinny_callIdentifier, tvb, offset+24, 4, ENC_LITTLE_ENDIAN);
        si->lineId = tvb_get_letohl(tvb, offset+20);
        si->callId = tvb_get_letohl(tvb, offset+24);
      }
      break;

    case 0x0086: /
      proto_tree_add_item(skinny_tree, hf_skinny_stimulus, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_stimulusInstance, tvb, offset+16, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_lampMode, tvb, offset+20, 4, ENC_LITTLE_ENDIAN);
      break;

    case 0x0087: /
      proto_tree_add_item(skinny_tree, hf_skinny_hookFlashDetectMode, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_detectInterval, tvb, offset+16, 4, ENC_LITTLE_ENDIAN);
      break;

    case 0x0088: /

      proto_tree_add_item(skinny_tree, hf_skinny_speakerMode, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      break;

    case 0x0089: /
      proto_tree_add_item(skinny_tree, hf_skinny_microphoneMode, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      break;

    case 0x008a: /
      if (hdr_version == BASIC_MSG_TYPE) {
        address src_addr;
        guint32 ipv4_address;

        proto_tree_add_item(skinny_tree, hf_skinny_conferenceID,          tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
        proto_tree_add_item(skinny_tree, hf_skinny_passThruPartyID,       tvb, offset+16, 4, ENC_LITTLE_ENDIAN);
        proto_tree_add_item(skinny_tree, hf_skinny_remoteIpAddr,          tvb, offset+20, 4, ENC_BIG_ENDIAN);
        proto_tree_add_item(skinny_tree, hf_skinny_remotePortNumber,      tvb, offset+24, 4, ENC_LITTLE_ENDIAN);
        proto_tree_add_item(skinny_tree, hf_skinny_millisecondPacketSize, tvb, offset+28, 4, ENC_LITTLE_ENDIAN);
        proto_tree_add_item(skinny_tree, hf_skinny_payloadCapability,     tvb, offset+32, 4, ENC_LITTLE_ENDIAN);
        proto_tree_add_item(skinny_tree, hf_skinny_precedenceValue,       tvb, offset+36, 4, ENC_LITTLE_ENDIAN);
        proto_tree_add_item(skinny_tree, hf_skinny_silenceSuppression,    tvb, offset+40, 4, ENC_LITTLE_ENDIAN);
        proto_tree_add_item(skinny_tree, hf_skinny_maxFramesPerPacket,    tvb, offset+44, 2, ENC_LITTLE_ENDIAN);
        proto_tree_add_item(skinny_tree, hf_skinny_g723BitRate,           tvb, offset+48, 4, ENC_LITTLE_ENDIAN);

        src_addr.type = AT_IPv4;
        src_addr.len = 4;
        src_addr.data = (char *)&ipv4_address;
        ipv4_address = tvb_get_ipv4(tvb, offset+20);
        rtp_add_address(pinfo, &src_addr, tvb_get_letohl(tvb, offset+24), 0, "Skinny", pinfo->fd->num, is_video, NULL);

        si->passThruId = tvb_get_letohl(tvb, offset+16);
      }
      else if (hdr_version == CM7_MSG_TYPE_A || hdr_version == CM7_MSG_TYPE_B || hdr_version == CM7_MSG_TYPE_C || hdr_version == CM7_MSG_TYPE_D)
      {
        address src_addr;
        guint32 ipv4_address;

        proto_tree_add_item(skinny_tree, hf_skinny_conferenceID,          tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
        proto_tree_add_item(skinny_tree, hf_skinny_passThruPartyID,       tvb, offset+16, 4, ENC_LITTLE_ENDIAN);
        /
        ipversion = tvb_get_ntohl(tvb, offset+20);
        proto_tree_add_item(skinny_tree, hf_skinny_IPVersion, tvb, offset+20, 4, ENC_LITTLE_ENDIAN);
        /
        if (ipversion ==0) {
          proto_tree_add_item(skinny_tree, hf_skinny_remoteIpAddr,          tvb, offset+24, 4, ENC_BIG_ENDIAN);
        } else {
          proto_tree_add_item(skinny_tree, hf_skinny_ipV6Address, tvb, offset+24, 16, ENC_NA);
        }
        proto_tree_add_item(skinny_tree, hf_skinny_remotePortNumber,      tvb, offset+40, 4, ENC_LITTLE_ENDIAN);
        proto_tree_add_item(skinny_tree, hf_skinny_millisecondPacketSize, tvb, offset+44, 4, ENC_LITTLE_ENDIAN);
        proto_tree_add_item(skinny_tree, hf_skinny_payloadCapability,     tvb, offset+48, 4, ENC_LITTLE_ENDIAN);
        /
        proto_tree_add_item(skinny_tree, hf_skinny_precedenceValue,       tvb, offset+52, 4, ENC_LITTLE_ENDIAN);
        proto_tree_add_item(skinny_tree, hf_skinny_silenceSuppression,    tvb, offset+56, 4, ENC_LITTLE_ENDIAN);
        proto_tree_add_item(skinny_tree, hf_skinny_maxFramesPerPacket,    tvb, offset+60, 2, ENC_LITTLE_ENDIAN);
        proto_tree_add_item(skinny_tree, hf_skinny_g723BitRate,           tvb, offset+62, 4, ENC_LITTLE_ENDIAN);
        /
        /
        /

        src_addr.type = AT_IPv4;
        src_addr.len = 4;
        src_addr.data = (char *)&ipv4_address;
        ipv4_address = tvb_get_ipv4(tvb, offset+24);
        rtp_add_address(pinfo, &src_addr, tvb_get_letohl(tvb, offset+40), 0, "Skinny", pinfo->fd->num, is_video, NULL);

        si->passThruId = tvb_get_letohl(tvb, offset+16);
      }
      break;

    case 0x008b: /
      proto_tree_add_item(skinny_tree, hf_skinny_conferenceID, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_passThruPartyID, tvb, offset+16, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_conferenceID, tvb, offset+20, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_unknown, tvb, offset+24, 4, ENC_LITTLE_ENDIAN);
      si->passThruId = tvb_get_letohl(tvb, offset+16);
      break;

    case 0x008c: /
      break;

    case 0x008d: /
      break;

    case 0x008f: /
      i = offset+12;
      proto_tree_add_item(skinny_tree, hf_skinny_callingPartyName, tvb, i, StationMaxNameSize, ENC_ASCII|ENC_NA);
      proto_tree_add_item(skinny_tree, hf_skinny_callingPartyNumber, tvb, i, StationMaxDirnumSize, ENC_ASCII|ENC_NA);
      i += StationMaxNameSize;
      si->callingParty = g_strdup(tvb_format_stringzpad(tvb, i, StationMaxDirnumSize));
      i += StationMaxDirnumSize;
      proto_tree_add_item(skinny_tree, hf_skinny_calledPartyName, tvb, i, StationMaxNameSize, ENC_ASCII|ENC_NA);
      i += StationMaxNameSize;
      proto_tree_add_item(skinny_tree, hf_skinny_calledPartyNumber, tvb, i, StationMaxDirnumSize, ENC_ASCII|ENC_NA);
      si->calledParty = g_strdup(tvb_format_stringzpad(tvb, i, StationMaxDirnumSize));
      i += StationMaxDirnumSize;
      proto_tree_add_item(skinny_tree, hf_skinny_lineInstance, tvb, i, 4, ENC_LITTLE_ENDIAN);
      si->lineId = tvb_get_letohl(tvb, i);
      i += 4;
      proto_tree_add_item(skinny_tree, hf_skinny_callIdentifier, tvb, i, 4, ENC_LITTLE_ENDIAN);
      si->callId = tvb_get_letohl(tvb, i);
      i += 4;
      proto_tree_add_item(skinny_tree, hf_skinny_callType, tvb, i, 4, ENC_LITTLE_ENDIAN);
      i += 4;
      proto_tree_add_item(skinny_tree, hf_skinny_originalCalledPartyName, tvb, i, StationMaxNameSize, ENC_ASCII|ENC_NA);
      i += StationMaxNameSize;
      proto_tree_add_item(skinny_tree, hf_skinny_originalCalledParty, tvb, i, StationMaxDirnumSize, ENC_ASCII|ENC_NA);
      i += StationMaxDirnumSize;
      proto_tree_add_item(skinny_tree, hf_skinny_lastRedirectingPartyName, tvb, i, StationMaxNameSize, ENC_ASCII|ENC_NA);
      i += StationMaxNameSize;
      proto_tree_add_item(skinny_tree, hf_skinny_lastRedirectingParty, tvb, i, StationMaxDirnumSize, ENC_ASCII|ENC_NA);
      i += StationMaxDirnumSize;
      proto_tree_add_item(skinny_tree, hf_skinny_originalCdpnRedirectReason, tvb, i, 4, ENC_LITTLE_ENDIAN);
      i += 4;
      proto_tree_add_item(skinny_tree, hf_skinny_lastRedirectingReason, tvb, i, 4, ENC_LITTLE_ENDIAN);
      i += 4;
      proto_tree_add_item(skinny_tree, hf_skinny_cgpnVoiceMailbox, tvb, i, StationMaxDirnumSize, ENC_ASCII|ENC_NA);
      i += StationMaxDirnumSize;
      proto_tree_add_item(skinny_tree, hf_skinny_cdpnVoiceMailbox, tvb, i, StationMaxDirnumSize, ENC_ASCII|ENC_NA);
      i += StationMaxDirnumSize;
      proto_tree_add_item(skinny_tree, hf_skinny_originalCdpnVoiceMailbox, tvb, i, StationMaxDirnumSize, ENC_ASCII|ENC_NA);
      i += StationMaxDirnumSize;
      proto_tree_add_item(skinny_tree, hf_skinny_lastRedirectingVoiceMailbox, tvb, i, StationMaxDirnumSize, ENC_ASCII|ENC_NA);
      i += StationMaxDirnumSize;
      proto_tree_add_item(skinny_tree, hf_skinny_callInstance, tvb, i, 4, ENC_LITTLE_ENDIAN);
      i += 4;
      proto_tree_add_item(skinny_tree, hf_skinny_callSecurityStatus, tvb, i, 4, ENC_LITTLE_ENDIAN);
      i += 4;
      skinny_sub_tree = proto_tree_add_subtree(skinny_tree, tvb, i, 8, ett_skinny_tree, NULL, "partyPIRestrictionBits");
      proto_tree_add_item(skinny_sub_tree, hf_skinny_partyPIRestrictionBits_CallingPartyName, tvb, i, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_sub_tree, hf_skinny_partyPIRestrictionBits_CallingPartyNumber, tvb, i, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_sub_tree, hf_skinny_partyPIRestrictionBits_CalledPartyName, tvb, i, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_sub_tree, hf_skinny_partyPIRestrictionBits_CalledPartyNumber, tvb, i, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_sub_tree, hf_skinny_partyPIRestrictionBits_OriginalCalledPartyName, tvb, i, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_sub_tree, hf_skinny_partyPIRestrictionBits_OriginalCalledPartyNumber, tvb, i, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_sub_tree, hf_skinny_partyPIRestrictionBits_LastRedirectPartyName, tvb, i, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_sub_tree, hf_skinny_partyPIRestrictionBits_LastRedirectPartyNumber, tvb, i, 4, ENC_LITTLE_ENDIAN);
      break;

    case 0x0090: /
      proto_tree_add_item(skinny_tree, hf_skinny_activeForward, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_lineNumber, tvb, offset+16, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_forwardAllActive, tvb, offset+20, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_forwardNumber, tvb, offset+24, StationMaxDirnumSize, ENC_ASCII|ENC_NA);
      i = offset+24+StationMaxDirnumSize;
      proto_tree_add_item(skinny_tree, hf_skinny_forwardBusyActive, tvb, i, 4, ENC_LITTLE_ENDIAN);
      i += 4;
      proto_tree_add_item(skinny_tree, hf_skinny_forwardNumber, tvb, i, StationMaxDirnumSize, ENC_ASCII|ENC_NA);
      i += StationMaxDirnumSize;
      proto_tree_add_item(skinny_tree, hf_skinny_forwardNoAnswerActive, tvb, i, 4, ENC_LITTLE_ENDIAN);
      i += 4;
      proto_tree_add_item(skinny_tree, hf_skinny_forwardNumber, tvb, i, StationMaxDirnumSize, ENC_ASCII|ENC_NA);
      break;

    case 0x0091: /
      proto_tree_add_item(skinny_tree, hf_skinny_speedDialNumber, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_speedDialDirNumber, tvb, offset+16, StationMaxDirnumSize, ENC_ASCII|ENC_NA);
      proto_tree_add_item(skinny_tree, hf_skinny_speedDialDisplayName, tvb, offset+40, StationMaxNameSize, ENC_ASCII|ENC_NA);
      break;

    case 0x0092: /
      proto_tree_add_item(skinny_tree, hf_skinny_lineNumber, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_lineDirNumber, tvb, offset+16, StationMaxDirnumSize, ENC_ASCII|ENC_NA);
      proto_tree_add_item(skinny_tree, hf_skinny_lineFullyQualifiedDisplayName, tvb, offset+16+StationMaxDirnumSize, StationMaxNameSize, ENC_ASCII|ENC_NA);
      proto_tree_add_item(skinny_tree, hf_skinny_lineDisplayName, tvb, offset+16+StationMaxDirnumSize+StationMaxNameSize, StationMaxDisplayNameSize, ENC_ASCII|ENC_NA);
      break;

    case 0x0093: /
      proto_tree_add_item(skinny_tree, hf_skinny_deviceName, tvb, offset+12, StationMaxDeviceNameSize, ENC_ASCII|ENC_NA);
      i = offset+12+StationMaxDeviceNameSize;
      proto_tree_add_item(skinny_tree, hf_skinny_stationUserId, tvb, i, 4, ENC_LITTLE_ENDIAN);
      i += 4;
      proto_tree_add_item(skinny_tree, hf_skinny_stationInstance, tvb, i, 4, ENC_LITTLE_ENDIAN);
      i += 4;
      proto_tree_add_item(skinny_tree, hf_skinny_userName, tvb, i, StationMaxNameSize, ENC_ASCII|ENC_NA);
      i += StationMaxNameSize;
      proto_tree_add_item(skinny_tree, hf_skinny_serverName, tvb, i, StationMaxNameSize, ENC_ASCII|ENC_NA);
      i += StationMaxNameSize;
      proto_tree_add_item(skinny_tree, hf_skinny_numberLines, tvb, i, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_numberSpeedDials, tvb, i+4, 4, ENC_LITTLE_ENDIAN);
      break;

    case 0x0094: /
      proto_tree_add_item(skinny_tree, hf_skinny_dateYear,   tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_dateMonth,  tvb, offset+16, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_dayOfWeek,  tvb, offset+20, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_dateDay,    tvb, offset+24, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_dateHour,   tvb, offset+28, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_dateMinute, tvb, offset+32, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_dateSeconds,tvb, offset+36, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_dateMilliseconds,tvb, offset+40, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_timeStamp, tvb, offset+44, 4, ENC_LITTLE_ENDIAN);
      break;

    case 0x0095: /
      proto_tree_add_item(skinny_tree, hf_skinny_remoteIpAddr,  tvb, offset+12, 4, ENC_BIG_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_sessionType, tvb, offset+16, 4, ENC_LITTLE_ENDIAN);
      break;

    case 0x0096: /
      proto_tree_add_item(skinny_tree, hf_skinny_remoteIpAddr,  tvb, offset+12, 4, ENC_BIG_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_sessionType, tvb, offset+16, 4, ENC_LITTLE_ENDIAN);
      break;

    case 0x0097: /
      /
      proto_tree_add_item(skinny_tree, hf_skinny_buttonOffset, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_buttonCount,  tvb, offset+16, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_totalButtonCount, tvb, offset+20, 4, ENC_LITTLE_ENDIAN);
      for (i = 0; i < StationMaxButtonTemplateSize; i++) {
        proto_tree_add_item(skinny_tree, hf_skinny_buttonInstanceNumber, tvb, offset+(i*2)+24, 1, ENC_LITTLE_ENDIAN);
        proto_tree_add_item(skinny_tree, hf_skinny_buttonDefinition, tvb, offset+(i*2)+25, 1, ENC_LITTLE_ENDIAN);
      }
      break;

    case 0x0098: /
      proto_tree_add_item(skinny_tree, hf_skinny_version, tvb, offset+12, StationMaxVersionSize, ENC_ASCII|ENC_NA);
      break;

    case 0x0099: /
      proto_tree_add_item(skinny_tree, hf_skinny_displayMessage, tvb, offset+12, StationMaxDisplayTextSize, ENC_ASCII|ENC_NA);
      break;

    case 0x009a: /
      break;

    case 0x009b: /
      break;

    case 0x009c: /
      proto_tree_add_item(skinny_tree, hf_skinny_mediaEnunciationType, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      for (i = 0; i < StationMaxDirnumSize; i++) {
        proto_tree_add_item(skinny_tree, hf_skinny_unknown, tvb, offset+16+(i*4), 4, ENC_LITTLE_ENDIAN);
      }
      i = offset+16+StationMaxDirnumSize;
      proto_tree_add_item(skinny_tree, hf_skinny_mediaEnunciationType, tvb, i, 4, ENC_LITTLE_ENDIAN);
      break;

    case 0x009d: /
      proto_tree_add_item(skinny_tree, hf_skinny_displayMessage, tvb, offset+12, StationMaxDisplayTextSize, ENC_ASCII|ENC_NA);
      break;

    case 0x009e: /
      for (i = 0; i < StationMaxServers; i++) {
        proto_tree_add_item(skinny_tree, hf_skinny_serverIdentifier,
                            tvb, offset+12+(i*StationMaxServers), StationMaxServerNameSize, ENC_ASCII|ENC_NA);
      }
      j = offset+12+(i*StationMaxServers);
      for (i = 0; i < StationMaxServers; i++) {
        proto_tree_add_item(skinny_tree, hf_skinny_serverListenPort, tvb, j+(i*4), 4,  ENC_LITTLE_ENDIAN);
      }
      j = j+(i*4);
      for (i = 0; i < StationMaxServers; i++) {
        proto_tree_add_item(skinny_tree, hf_skinny_serverIpAddress, tvb, j+(i*4), 4, ENC_BIG_ENDIAN);
      }
      break;

    case 0x009f: /
      proto_tree_add_item(skinny_tree, hf_skinny_deviceResetType, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      break;

    case 0x0100: /
      break;

    case 0x0101: /
      proto_tree_add_item(skinny_tree, hf_skinny_conferenceID, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_passThruPartyID, tvb, offset+16, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_multicastIpAddress, tvb, offset+20, 4, ENC_BIG_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_multicastPort, tvb, offset+24, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_millisecondPacketSize, tvb, offset+28, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_payloadCapability, tvb, offset+32, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_echoCancelType, tvb, offset+36, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_g723BitRate, tvb, offset+40, 4, ENC_LITTLE_ENDIAN);
      si->passThruId = tvb_get_letohl(tvb, offset+16);
      break;

    case 0x0102: /
      proto_tree_add_item(skinny_tree, hf_skinny_conferenceID, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_passThruPartyID, tvb, offset+16, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_multicastIpAddress, tvb, offset+20, 4, ENC_BIG_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_multicastPort, tvb, offset+24, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_millisecondPacketSize, tvb, offset+28, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_payloadCapability, tvb, offset+32, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_precedenceValue, tvb, offset+36, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_silenceSuppression, tvb, offset+40, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_maxFramesPerPacket, tvb, offset+44, 2, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_g723BitRate, tvb, offset+48, 4, ENC_LITTLE_ENDIAN);
      si->passThruId = tvb_get_letohl(tvb, offset+16);
      break;

    case 0x0103: /
      proto_tree_add_item(skinny_tree, hf_skinny_conferenceID, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_passThruPartyID, tvb, offset+16, 4, ENC_LITTLE_ENDIAN);
      si->passThruId = tvb_get_letohl(tvb, offset+16);
      break;

    case 0x0104: /
      proto_tree_add_item(skinny_tree, hf_skinny_conferenceID, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_passThruPartyID, tvb, offset+16, 4, ENC_LITTLE_ENDIAN);
      si->passThruId = tvb_get_letohl(tvb, offset+16);
      break;

    case 0x105: /
      proto_tree_add_item(skinny_tree, hf_skinny_conferenceID,            tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_passThruPartyID,         tvb, offset+16, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_millisecondPacketSize,   tvb, offset+20, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_payloadCapability,       tvb, offset+24, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_echoCancelType,          tvb, offset+28, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_g723BitRate,             tvb, offset+32, 4, ENC_LITTLE_ENDIAN);
      si->passThruId = tvb_get_letohl(tvb, offset+16);
      break;

    case 0x0106: /
      proto_tree_add_item(skinny_tree, hf_skinny_conferenceID, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_passThruPartyID, tvb, offset+16, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_conferenceID, tvb, offset+20, 4, ENC_LITTLE_ENDIAN);
      si->passThruId = tvb_get_letohl(tvb, offset+16);
      break;

    case 0x0107: /
      i = 12;
      proto_tree_add_item(skinny_tree, hf_skinny_directoryNumber, tvb, i, StationMaxDirnumSize, ENC_ASCII|ENC_NA);
      i = 12 + StationMaxDirnumSize;
      proto_tree_add_item(skinny_tree, hf_skinny_callIdentifier, tvb, i, 4, ENC_LITTLE_ENDIAN);
      si->callId = tvb_get_letohl(tvb, i);
      i = i+4;
      proto_tree_add_item(skinny_tree, hf_skinny_statsProcessingType, tvb, i, 4, ENC_LITTLE_ENDIAN);
      break;

    case 0x0108: /
      proto_tree_add_item(skinny_tree, hf_skinny_softKeyOffset, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      softKeyCount = tvb_get_letohl(tvb, offset+16);
      proto_tree_add_uint(skinny_tree, hf_skinny_softKeyCount, tvb, offset+16, 4, softKeyCount);
      proto_tree_add_item(skinny_tree, hf_skinny_totalSoftKeyCount, tvb, offset+20, 4, ENC_LITTLE_ENDIAN);
      for (i = 0; ((i < StationMaxSoftKeyDefinition) && (i < softKeyCount)); i++){
        proto_tree_add_item(skinny_tree, hf_skinny_softKeyLabel,
                            tvb, offset+(i*20)+24, StationMaxSoftKeyLabelSize, ENC_ASCII|ENC_NA);
        proto_tree_add_item(skinny_tree, hf_skinny_softKeyEvent, tvb, offset+(i*20)+40, 4, ENC_LITTLE_ENDIAN);
      }
      /
      break;

    case 0x0109: /
      proto_tree_add_item(skinny_tree, hf_skinny_softKeySetOffset, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      softKeySetCount = tvb_get_letohl(tvb, offset+16);
      proto_tree_add_uint(skinny_tree, hf_skinny_softKeySetCount, tvb, offset+16, 4, softKeySetCount);
      proto_tree_add_item(skinny_tree, hf_skinny_totalSoftKeySetCount, tvb, offset+20, 4, ENC_LITTLE_ENDIAN);
      for (i = 0; ((i < StationMaxSoftKeySetDefinition) && (i < softKeySetCount)); i++) {
        proto_tree_add_uint(skinny_tree, hf_skinny_softKeySetDescription, tvb, offset+24+(i*48) , 1, i);
        for (j = 0; j < StationMaxSoftKeyIndex; j++) {
          proto_tree_add_item(skinny_tree, hf_skinny_softKeyTemplateIndex, tvb, offset+24+(i*48)+j, 1, ENC_LITTLE_ENDIAN);
        }
        for (j = 0; j < StationMaxSoftKeyIndex; j++) {
          proto_tree_add_item(skinny_tree, hf_skinny_softKeyInfoIndex,
                              tvb, offset+24+(i*48)+StationMaxSoftKeyIndex+(j*2), 2, ENC_LITTLE_ENDIAN);
        }
      }
      break;

    case 0x0110: /
      proto_tree_add_item(skinny_tree, hf_skinny_lineInstance, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_callIdentifier, tvb, offset+16, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_softKeySetDescription, tvb, offset+20, 4, ENC_LITTLE_ENDIAN);
      validKeyMask = tvb_get_letohs(tvb, offset + 24);
      skm = proto_tree_add_uint(skinny_tree, hf_skinny_softKeyMap, tvb, offset + 24, 4, validKeyMask);
      skm_tree = proto_item_add_subtree(skm, ett_skinny_softKeyMap);
      proto_tree_add_boolean(skm_tree, hf_skinny_softKey0,  tvb, offset + 24, 4, validKeyMask);
      proto_tree_add_boolean(skm_tree, hf_skinny_softKey1,  tvb, offset + 24, 4, validKeyMask);
      proto_tree_add_boolean(skm_tree, hf_skinny_softKey2,  tvb, offset + 24, 4, validKeyMask);
      proto_tree_add_boolean(skm_tree, hf_skinny_softKey3,  tvb, offset + 24, 4, validKeyMask);
      proto_tree_add_boolean(skm_tree, hf_skinny_softKey4,  tvb, offset + 24, 4, validKeyMask);
      proto_tree_add_boolean(skm_tree, hf_skinny_softKey5,  tvb, offset + 24, 4, validKeyMask);
      proto_tree_add_boolean(skm_tree, hf_skinny_softKey6,  tvb, offset + 24, 4, validKeyMask);
      proto_tree_add_boolean(skm_tree, hf_skinny_softKey7,  tvb, offset + 24, 4, validKeyMask);
      proto_tree_add_boolean(skm_tree, hf_skinny_softKey8,  tvb, offset + 24, 4, validKeyMask);
      proto_tree_add_boolean(skm_tree, hf_skinny_softKey9,  tvb, offset + 24, 4, validKeyMask);
      proto_tree_add_boolean(skm_tree, hf_skinny_softKey10, tvb, offset + 24, 4, validKeyMask);
      proto_tree_add_boolean(skm_tree, hf_skinny_softKey11, tvb, offset + 24, 4, validKeyMask);
      proto_tree_add_boolean(skm_tree, hf_skinny_softKey12, tvb, offset + 24, 4, validKeyMask);
      proto_tree_add_boolean(skm_tree, hf_skinny_softKey13, tvb, offset + 24, 4, validKeyMask);
      proto_tree_add_boolean(skm_tree, hf_skinny_softKey14, tvb, offset + 24, 4, validKeyMask);
      proto_tree_add_boolean(skm_tree, hf_skinny_softKey15, tvb, offset + 24, 4, validKeyMask);
      si->lineId = tvb_get_letohl(tvb, offset+12);
      si->callId = tvb_get_letohl(tvb, offset+16);
      break;

    case 0x0111: /
      proto_tree_add_item(skinny_tree, hf_skinny_callState, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_lineInstance, tvb, offset+16, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_callIdentifier, tvb, offset+20, 4, ENC_LITTLE_ENDIAN);
      si->callState = tvb_get_letohl(tvb, offset+12);
      si->lineId = tvb_get_letohl(tvb, offset+16);
      si->callId = tvb_get_letohl(tvb, offset+20);
      proto_tree_add_item(skinny_tree, hf_skinny_visibility, tvb, offset+24, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_priority, tvb, offset+28, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_unknown, tvb, offset+32, 4, ENC_LITTLE_ENDIAN);
      break;

    case 0x0112: /
      proto_tree_add_item(skinny_tree, hf_skinny_messageTimeOutValue, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_displayMessage, tvb, offset+16, StationMaxDisplayPromptStatusSize, ENC_ASCII|ENC_NA);
      proto_tree_add_item(skinny_tree, hf_skinny_lineInstance, tvb, offset+48, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_callIdentifier, tvb, offset+52, 4, ENC_LITTLE_ENDIAN);
      si->lineId = tvb_get_letohl(tvb, offset+48);
      si->callId = tvb_get_letohl(tvb, offset+52);
      break;

    case 0x0113: /
      proto_tree_add_item(skinny_tree, hf_skinny_lineInstance  , tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_callIdentifier, tvb, offset+16, 4, ENC_LITTLE_ENDIAN);
      si->lineId = tvb_get_letohl(tvb, offset+12);
      si->callId = tvb_get_letohl(tvb, offset+16);
      break;

    case 0x0114: /
      proto_tree_add_item(skinny_tree, hf_skinny_messageTimeOutValue, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_displayMessage, tvb, offset+16, StationMaxDisplayNotifySize , ENC_ASCII|ENC_NA);
      break;

    case 0x0115: /
      break;

    case 0x0116: /
      proto_tree_add_item(skinny_tree, hf_skinny_lineInstance, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      si->lineId = tvb_get_letohl(tvb, offset+12);
      break;

    case 0x0117: /
      break;

    case 0x0118: /
      proto_tree_add_item(skinny_tree, hf_skinny_deviceUnregisterStatus, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      break;

    case 0x0119: /
      proto_tree_add_item(skinny_tree, hf_skinny_lineInstance, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_callIdentifier, tvb, offset+16, 4, ENC_LITTLE_ENDIAN);
      si->lineId = tvb_get_letohl(tvb, offset+12);
      si->callId = tvb_get_letohl(tvb, offset+16);
      break;

    case 0x011a: /
      break;

    case 0x011B: /
      proto_tree_add_item(skinny_tree, hf_skinny_tokenRejWaitTime, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      break;

    case 0x011C: /
      proto_tree_add_item(skinny_tree, hf_skinny_conferenceID, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_passThruPartyID, tvb, offset+16, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_millisecondPacketSize, tvb, offset+20, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_payloadCapability, tvb, offset+24, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_echoCancelType, tvb, offset+28, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_g723BitRate, tvb, offset+32, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_callIdentifier, tvb, offset+34, 4, ENC_LITTLE_ENDIAN);
      si->passThruId = tvb_get_letohl(tvb, offset+16);
      si->callId = tvb_get_letohl(tvb, offset+34);
      break;

    case 0x011D: /
      proto_tree_add_item(skinny_tree, hf_skinny_calledPartyNumber, tvb, offset+12, StationMaxDirnumSize, ENC_ASCII|ENC_NA);
      proto_tree_add_item(skinny_tree, hf_skinny_lineInstance, tvb, offset+12+StationMaxDirnumSize, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_callIdentifier, tvb, offset+12+StationMaxDirnumSize+4, 4, ENC_LITTLE_ENDIAN);
      si->lineId = tvb_get_letohl(tvb, offset+12+StationMaxDirnumSize);
      si->callId = tvb_get_letohl(tvb, offset+16+StationMaxDirnumSize);
      break;

    case 0x011E: /
      proto_tree_add_item(skinny_tree, hf_skinny_applicationID, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_lineInstance, tvb, offset+16, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_callIdentifier, tvb, offset+20, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_transactionID, tvb, offset+24, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_data_length, tvb, offset+28, 4, ENC_LITTLE_ENDIAN);
      count = tvb_get_letohl( tvb, offset+28);
      dissect_skinny_xml(skinny_tree, tvb, pinfo, offset+30, count);
      si->lineId = tvb_get_letohl(tvb, offset+16);
      si->callId = tvb_get_letohl(tvb, offset+20);
      break;

    case 0x011F: /
      proto_tree_add_item(skinny_tree, hf_skinny_featureIndex, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_featureID, tvb, offset+16, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_featureTextLabel, tvb, offset+20, StationMaxNameSize, ENC_ASCII|ENC_NA);
      proto_tree_add_item(skinny_tree, hf_skinny_featureStatus, tvb, offset+20+StationMaxNameSize, 4, ENC_LITTLE_ENDIAN);
      break;

    case 0x0120: /
      proto_tree_add_item(skinny_tree, hf_skinny_messageTimeOutValue, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_priority, tvb, offset+16, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_notify, tvb, offset+16, StationMaxDisplayNotifySize, ENC_ASCII|ENC_NA);
      break;

    case 0x0121: /
      proto_tree_add_item(skinny_tree, hf_skinny_priority, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      break;

    case 0x0122: /
      count = offset+12;
      for ( i = 0; i < MaxAnnouncementList; i++ ) {
        proto_tree_add_item(skinny_tree, hf_skinny_locale, tvb, count, 4, ENC_LITTLE_ENDIAN);
        count += 4;
        proto_tree_add_item(skinny_tree, hf_skinny_country, tvb, count, 4, ENC_LITTLE_ENDIAN);
        count += 4;
        proto_tree_add_item(skinny_tree, hf_skinny_deviceTone, tvb, count, 4, ENC_LITTLE_ENDIAN);
        count += 4;
      }
      proto_tree_add_item(skinny_tree, hf_skinny_endOfAnnAck, tvb, count, 4, ENC_LITTLE_ENDIAN);
      count += 4;
      proto_tree_add_item(skinny_tree, hf_skinny_conferenceID, tvb, count, 4, ENC_LITTLE_ENDIAN);
      count += 4;

      for ( i = 0; i < StationMaxMonitorParties; i++ ) {
        proto_tree_add_item(skinny_tree, hf_skinny_matrixConfPartyID, tvb, count, 4, ENC_LITTLE_ENDIAN);
        count += 4;
      }
      proto_tree_add_item(skinny_tree, hf_skinny_hearingConfPartyMask, tvb, count, 4, ENC_LITTLE_ENDIAN);
      count += 4;
      proto_tree_add_item(skinny_tree, hf_skinny_annPlayMode, tvb, count, 4, ENC_LITTLE_ENDIAN);
      break;

    case 0x0123: /
      proto_tree_add_item(skinny_tree, hf_skinny_conferenceID, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      break;

    case 0x0124: /
      proto_tree_add_item(skinny_tree, hf_skinny_conferenceID, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_annPlayStatus, tvb, offset+16, 4, ENC_LITTLE_ENDIAN);
      break;

    case 0x0127: /
      proto_tree_add_item(skinny_tree, hf_skinny_deviceTone, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_conferenceID, tvb, offset+16, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_passThruPartyID, tvb, offset+20, 4, ENC_LITTLE_ENDIAN);
      si->passThruId = tvb_get_letohl(tvb, offset+20);
      break;

    case 0x0128: /
      proto_tree_add_item(skinny_tree, hf_skinny_deviceTone, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_conferenceID, tvb, offset+16, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_passThruPartyID, tvb, offset+20, 4, ENC_LITTLE_ENDIAN);
      si->passThruId = tvb_get_letohl(tvb, offset+20);
      break;

    case 0x0129: /
      proto_tree_add_item(skinny_tree, hf_skinny_payloadDtmf, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_conferenceID, tvb, offset+16, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_passThruPartyID, tvb, offset+20, 4, ENC_LITTLE_ENDIAN);
      si->passThruId = tvb_get_letohl(tvb, offset+20);
      break;

    case 0x012A: /
      proto_tree_add_item(skinny_tree, hf_skinny_payloadDtmf, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_conferenceID, tvb, offset+16, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_passThruPartyID, tvb, offset+20, 4, ENC_LITTLE_ENDIAN);
      si->passThruId = tvb_get_letohl(tvb, offset+20);
      break;

    case 0x012B: /
      proto_tree_add_item(skinny_tree, hf_skinny_payloadDtmf, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_conferenceID, tvb, offset+16, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_passThruPartyID, tvb, offset+20, 4, ENC_LITTLE_ENDIAN);
      si->passThruId = tvb_get_letohl(tvb, offset+20);
      break;

    case 0x012C: /
      proto_tree_add_item(skinny_tree, hf_skinny_payloadDtmf, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_conferenceID, tvb, offset+16, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_passThruPartyID, tvb, offset+20, 4, ENC_LITTLE_ENDIAN);
      si->passThruId = tvb_get_letohl(tvb, offset+20);
      break;

    case 0x012D: /
      proto_tree_add_item(skinny_tree, hf_skinny_payloadDtmf, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_conferenceID, tvb, offset+16, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_passThruPartyID, tvb, offset+20, 4, ENC_LITTLE_ENDIAN);
      si->passThruId = tvb_get_letohl(tvb, offset+20);
      break;

    case 0x012E: /
      proto_tree_add_item(skinny_tree, hf_skinny_payloadDtmf, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_conferenceID, tvb, offset+16, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_passThruPartyID, tvb, offset+20, 4, ENC_LITTLE_ENDIAN);
      si->passThruId = tvb_get_letohl(tvb, offset+20);
      break;

    case 0x012F: /
      proto_tree_add_item(skinny_tree, hf_skinny_serviceURLIndex, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_serviceURL, tvb, offset+16, StationMaxServiceURLSize, ENC_ASCII|ENC_NA);
      proto_tree_add_item(skinny_tree, hf_skinny_serviceURLDisplayName, tvb, offset+16+StationMaxServiceURLSize, StationMaxNameSize, ENC_ASCII|ENC_NA);
      break;

    case 0x0130: /
      proto_tree_add_item(skinny_tree, hf_skinny_callSelectStat, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_callIdentifier, tvb, offset+16, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_lineInstance, tvb, offset+20, 4, ENC_LITTLE_ENDIAN);
      si->lineId = tvb_get_letohl(tvb, offset+16);
      si->callId = tvb_get_letohl(tvb, offset+20);
      break;

    case 0x0131: /
      proto_tree_add_item(skinny_tree, hf_skinny_conferenceID, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_passThruPartyID, tvb, offset+16, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_payloadCapability, tvb, offset+20, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_lineInstance, tvb, offset+24, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_callIdentifier, tvb, offset+28, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_payload_rfc_number, tvb, offset+32, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_payloadType, tvb, offset+36, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_isConferenceCreator, tvb, offset+40, 4, ENC_LITTLE_ENDIAN);
      si->passThruId = tvb_get_letohl(tvb, offset+16);
      si->lineId = tvb_get_letohl(tvb, offset+24);
      si->callId = tvb_get_letohl(tvb, offset+28);

      /
      skinny_sub_tree = proto_tree_add_subtree(skinny_tree, tvb, offset, 12, ett_skinny_tree, NULL, "audioParameters");
      proto_tree_add_item(skinny_sub_tree, hf_skinny_millisecondPacketSize, tvb, offset+44, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_sub_tree, hf_skinny_echoCancelType, tvb, offset+48, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_sub_tree, hf_skinny_g723BitRate, tvb, offset+52, 4, ENC_LITTLE_ENDIAN);

      /
      skinny_sub_tree = proto_tree_add_subtree(skinny_tree, tvb, offset, 30, ett_skinny_tree, NULL, "vidParameters");
      proto_tree_add_item(skinny_sub_tree, hf_skinny_bitRate, tvb, offset+44, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_sub_tree, hf_skinny_pictureFormatCount, tvb, offset+48, 4, ENC_LITTLE_ENDIAN);
      skinny_sub_tree_sav = skinny_sub_tree;
      count = offset+52;
      for ( i = 0; i < MAX_PICTURE_FORMAT; i++ ) {
        skinny_sub_tree = proto_tree_add_subtree_format(skinny_sub_tree_sav, tvb, offset, 8 * MAX_PICTURE_FORMAT,
                                            ett_skinny_tree, NULL, "pictureFormat[%d]", i);
        proto_tree_add_item(skinny_sub_tree, hf_skinny_format, tvb, count, 4, ENC_LITTLE_ENDIAN);
        count += 4;
        proto_tree_add_item(skinny_sub_tree, hf_skinny_MPI, tvb, count, 4, ENC_LITTLE_ENDIAN);
        count += 4;
      }
      skinny_sub_tree = skinny_sub_tree_sav;
      proto_tree_add_item(skinny_sub_tree, hf_skinny_confServiceNum, tvb, count, 4, ENC_LITTLE_ENDIAN);
      count += 4;

      val = count;
      /
      skinny_sub_tree = proto_tree_add_subtree(skinny_sub_tree_sav, tvb, offset, 8, ett_skinny_tree, NULL, "h261VideoCapability");
      proto_tree_add_item(skinny_sub_tree, hf_skinny_temporalSpatialTradeOffCapability, tvb, count, 4, ENC_LITTLE_ENDIAN);
      count += 4;
      proto_tree_add_item(skinny_sub_tree, hf_skinny_stillImageTransmission, tvb, count, 4, ENC_LITTLE_ENDIAN);

      /
      count = val;
      skinny_sub_tree = proto_tree_add_subtree(skinny_sub_tree_sav, tvb, offset, 8, ett_skinny_tree, NULL, "h263VideoCapability");
      proto_tree_add_item(skinny_sub_tree, hf_skinny_h263_capability_bitfield, tvb, count, 4, ENC_LITTLE_ENDIAN);
      count += 4;
      proto_tree_add_item(skinny_sub_tree, hf_skinny_annexNandWFutureUse, tvb, count, 4, ENC_LITTLE_ENDIAN);

      /
      count = val;
      skinny_sub_tree = proto_tree_add_subtree(skinny_sub_tree_sav, tvb, offset, 8, ett_skinny_tree, NULL, "vieoVideoCapability");
      proto_tree_add_item(skinny_sub_tree, hf_skinny_modelNumber, tvb, count, 4, ENC_LITTLE_ENDIAN);
      count += 4;
      proto_tree_add_item(skinny_sub_tree, hf_skinny_bandwidth, tvb, count, 4, ENC_LITTLE_ENDIAN);

      /
      skinny_sub_tree = proto_tree_add_subtree(skinny_tree, tvb, offset, 8, ett_skinny_tree, NULL, "dataParameters");
      proto_tree_add_item(skinny_sub_tree, hf_skinny_protocolDependentData, tvb, offset+44, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_sub_tree, hf_skinny_maxBitRate, tvb, offset+48, 4, ENC_LITTLE_ENDIAN);
      break;

    case 0x0132: /
      proto_tree_add_item(skinny_tree, hf_skinny_conferenceID, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_passThruPartyID, tvb, offset+16, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_payloadCapability, tvb, offset+20, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_ipAddress, tvb, offset+24, 4, ENC_BIG_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_portNumber, tvb, offset+28, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_callIdentifier, tvb, offset+32, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_payload_rfc_number, tvb, offset+36, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_payloadType, tvb, offset+40, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_DSCPValue, tvb, offset+44, 4, ENC_LITTLE_ENDIAN);
      si->passThruId = tvb_get_letohl(tvb, offset+16);
      si->callId = tvb_get_letohl(tvb, offset+32);

      /
      skinny_sub_tree = proto_tree_add_subtree(skinny_tree, tvb, offset, 12, ett_skinny_tree, NULL, "audioParameters");
      proto_tree_add_item(skinny_sub_tree, hf_skinny_millisecondPacketSize, tvb, offset+48, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_sub_tree, hf_skinny_echoCancelType, tvb, offset+52, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_sub_tree, hf_skinny_g723BitRate, tvb, offset+56, 4, ENC_LITTLE_ENDIAN);

      /
      skinny_sub_tree = proto_tree_add_subtree(skinny_tree, tvb, offset, 30, ett_skinny_tree, NULL, "vidParameters");
      proto_tree_add_item(skinny_sub_tree, hf_skinny_bitRate, tvb, offset+48, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_sub_tree, hf_skinny_pictureFormatCount, tvb, offset+52, 4, ENC_LITTLE_ENDIAN);
      skinny_sub_tree_sav = skinny_sub_tree;
      count = offset+56;
      for ( i = 0; i < MAX_PICTURE_FORMAT; i++ ) {
        skinny_sub_tree = proto_tree_add_subtree_format(skinny_sub_tree_sav, tvb, offset, 8 * MAX_PICTURE_FORMAT,
                                            ett_skinny_tree, NULL, "pictureFormat[%d]", i);
        proto_tree_add_item(skinny_sub_tree, hf_skinny_format, tvb, count, 4, ENC_LITTLE_ENDIAN);
        count += 4;
        proto_tree_add_item(skinny_sub_tree, hf_skinny_MPI, tvb, count, 4, ENC_LITTLE_ENDIAN);
        count += 4;
      }
      skinny_sub_tree = skinny_sub_tree_sav;
      proto_tree_add_item(skinny_sub_tree, hf_skinny_confServiceNum, tvb, count, 4, ENC_LITTLE_ENDIAN);
      count += 4;

      val = count;
      /
      skinny_sub_tree = proto_tree_add_subtree(skinny_sub_tree_sav, tvb, offset, 8, ett_skinny_tree, NULL, "h261VideoCapability");
      proto_tree_add_item(skinny_sub_tree, hf_skinny_temporalSpatialTradeOffCapability, tvb, count, 4, ENC_LITTLE_ENDIAN);
      count += 4;
      proto_tree_add_item(skinny_sub_tree, hf_skinny_stillImageTransmission, tvb, count, 4, ENC_LITTLE_ENDIAN);

      /
      count = val;
      skinny_sub_tree = proto_tree_add_subtree(skinny_sub_tree_sav, tvb, offset, 8, ett_skinny_tree, NULL, "h263VideoCapability");
      proto_tree_add_item(skinny_sub_tree, hf_skinny_h263_capability_bitfield, tvb, count, 4, ENC_LITTLE_ENDIAN);
      count += 4;
      proto_tree_add_item(skinny_sub_tree, hf_skinny_annexNandWFutureUse, tvb, count, 4, ENC_LITTLE_ENDIAN);

      /
      count = val;
      skinny_sub_tree = proto_tree_add_subtree(skinny_sub_tree_sav, tvb, offset, 8, ett_skinny_tree, NULL, "vieoVideoCapability");
      proto_tree_add_item(skinny_sub_tree, hf_skinny_modelNumber, tvb, count, 4, ENC_LITTLE_ENDIAN);
      count += 4;
      proto_tree_add_item(skinny_sub_tree, hf_skinny_bandwidth, tvb, count, 4, ENC_LITTLE_ENDIAN);

      /
      skinny_sub_tree = proto_tree_add_subtree(skinny_tree, tvb, offset, 8, ett_skinny_tree, NULL, "dataParameters");
      proto_tree_add_item(skinny_sub_tree, hf_skinny_protocolDependentData, tvb, offset+48, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_sub_tree, hf_skinny_maxBitRate, tvb, offset+52, 4, ENC_LITTLE_ENDIAN);
      break;

    case 0x0133: /
      proto_tree_add_item(skinny_tree, hf_skinny_conferenceID, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_passThruPartyID, tvb, offset+16, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_callIdentifier, tvb, offset+20, 4, ENC_LITTLE_ENDIAN);
      si->passThruId = tvb_get_letohl(tvb, offset+16);
      si->callId = tvb_get_letohl(tvb, offset+20);
      break;

    case 0x0134: /
      proto_tree_add_item(skinny_tree, hf_skinny_conferenceID, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_passThruPartyID, tvb, offset+16, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_callIdentifier, tvb, offset+20, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_miscCommandType, tvb, offset+24, 4, ENC_LITTLE_ENDIAN);
      si->passThruId = tvb_get_letohl(tvb, offset+16);
      si->callId = tvb_get_letohl(tvb, offset+20);

      /
      /

      /
      /

      /
      skinny_sub_tree = proto_tree_add_subtree(skinny_tree, tvb, offset, 8, ett_skinny_tree, NULL, "videoFastUpdateGOB");
      proto_tree_add_item(skinny_sub_tree, hf_skinny_firstGOB, tvb, offset+28, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_sub_tree, hf_skinny_numberOfGOBs, tvb, offset+32, 4, ENC_LITTLE_ENDIAN);

      /
      skinny_sub_tree = proto_tree_add_subtree(skinny_tree, tvb, offset, 8, ett_skinny_tree, NULL, "videoFastUpdateGOB");
      proto_tree_add_item(skinny_sub_tree, hf_skinny_firstGOB, tvb, offset+28, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_sub_tree, hf_skinny_firstMB, tvb, offset+32, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_sub_tree, hf_skinny_numberOfMBs, tvb, offset+36, 4, ENC_LITTLE_ENDIAN);

      /
      skinny_sub_tree = proto_tree_add_subtree(skinny_tree, tvb, offset, 8, ett_skinny_tree, NULL, "lostPicture");
      proto_tree_add_item(skinny_sub_tree, hf_skinny_pictureNumber, tvb, offset+28, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_sub_tree, hf_skinny_longTermPictureIndex, tvb, offset+32, 4, ENC_LITTLE_ENDIAN);

      /
      skinny_sub_tree = proto_tree_add_subtree(skinny_tree, tvb, offset, 8, ett_skinny_tree, NULL, "lostPartialPicture");
      proto_tree_add_item(skinny_sub_tree, hf_skinny_pictureNumber, tvb, offset+28, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_sub_tree, hf_skinny_longTermPictureIndex, tvb, offset+32, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_sub_tree, hf_skinny_firstMB, tvb, offset+36, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_sub_tree, hf_skinny_numberOfMBs, tvb, offset+40, 4, ENC_LITTLE_ENDIAN);

      /
      skinny_sub_tree = proto_tree_add_subtree(skinny_tree, tvb, offset, 8, ett_skinny_tree, NULL, "recoveryReferencePicture");
      proto_tree_add_item(skinny_sub_tree, hf_skinny_recoveryReferencePictureCount, tvb, offset+28, 4, ENC_LITTLE_ENDIAN);
      skinny_sub_tree_sav = skinny_sub_tree;
      for ( i = 0; i < MAX_REFERENCE_PICTURE; i++ ) {
        skinny_sub_tree = proto_tree_add_subtree_format(skinny_sub_tree_sav, tvb, offset, 8, ett_skinny_tree, NULL, "recoveryReferencePicture[%d]", i);
        proto_tree_add_item(skinny_sub_tree, hf_skinny_pictureNumber, tvb, offset+32+(i*8), 4, ENC_LITTLE_ENDIAN);
        proto_tree_add_item(skinny_sub_tree, hf_skinny_longTermPictureIndex, tvb, offset+36+(i*8), 4, ENC_LITTLE_ENDIAN);
      }

      /
      skinny_sub_tree = proto_tree_add_subtree(skinny_tree, tvb, offset, 4, ett_skinny_tree, NULL, "temporalSpatialTradeOff");
      proto_tree_add_item(skinny_sub_tree, hf_skinny_temporalSpatialTradeOff, tvb, offset+28, 4, ENC_LITTLE_ENDIAN);
      break;

    case 0x0135: /
      proto_tree_add_item(skinny_tree, hf_skinny_conferenceID, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_passThruPartyID, tvb, offset+16, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_callIdentifier, tvb, offset+20, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_maxBitRate, tvb, offset+24, 4, ENC_LITTLE_ENDIAN);
      si->passThruId = tvb_get_letohl(tvb, offset+16);
      si->callId = tvb_get_letohl(tvb, offset+20);
      break;

    case 0x0136: /
      proto_tree_add_item(skinny_tree, hf_skinny_conferenceID, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_passThruPartyID, tvb, offset+16, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_callIdentifier, tvb, offset+20, 4, ENC_LITTLE_ENDIAN);
      si->passThruId = tvb_get_letohl(tvb, offset+16);
      si->callId = tvb_get_letohl(tvb, offset+20);
      break;

    case 0x0137: /
      proto_tree_add_item(skinny_tree, hf_skinny_conferenceID, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_numberOfReservedParticipants, tvb, offset+16, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_resourceTypes, tvb, offset+16, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_appID, tvb, offset+20, 4, ENC_LITTLE_ENDIAN);
      count = offset+24;
      proto_tree_add_uint(skinny_tree, hf_skinny_appConfID, tvb, count, 1, AppConferenceIDSize);
      count += AppConferenceIDSize;
      proto_tree_add_uint(skinny_tree, hf_skinny_appData, tvb, count, 1, AppDataSize);
      count += AppDataSize;
      proto_tree_add_item(skinny_tree, hf_skinny_data_length, tvb, count, 4, ENC_LITTLE_ENDIAN);
      val = tvb_get_letohl( tvb, count);
      count += 4;
      proto_tree_add_uint(skinny_tree, hf_skinny_passThruData, tvb, count, 1, val);
      break;

    case 0x0138: /
      proto_tree_add_item(skinny_tree, hf_skinny_conferenceID, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      break;

    case 0x0139: /
      proto_tree_add_item(skinny_tree, hf_skinny_conferenceID, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_numberOfReservedParticipants, tvb, offset+16, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_appID, tvb, offset+20, 4, ENC_LITTLE_ENDIAN);
      count = offset+24;
      proto_tree_add_uint(skinny_tree, hf_skinny_appConfID, tvb, count, 1, AppConferenceIDSize);
      count += AppConferenceIDSize;
      proto_tree_add_uint(skinny_tree, hf_skinny_appData, tvb, count, 1, AppDataSize);
      count += AppDataSize;
      proto_tree_add_item(skinny_tree, hf_skinny_data_length, tvb, count, 4, ENC_LITTLE_ENDIAN);
      val = tvb_get_letohl( tvb, count);
      count += 4;
      proto_tree_add_uint(skinny_tree, hf_skinny_passThruData, tvb, count, 1, val);
      break;

    case 0x013A: /
      proto_tree_add_item(skinny_tree, hf_skinny_conferenceID, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_callIdentifier, tvb, offset+16, 4, ENC_LITTLE_ENDIAN);
      si->callId = tvb_get_letohl(tvb, offset+16);
      break;

    case 0x013B: /
      proto_tree_add_item(skinny_tree, hf_skinny_conferenceID, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_callIdentifier, tvb, offset+16, 4, ENC_LITTLE_ENDIAN);
      si->callId = tvb_get_letohl(tvb, offset+16);
      break;

    case 0x013C: /
      break;

    case 0x013D: /
      proto_tree_add_item(skinny_tree, hf_skinny_conferenceID, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      break;

    case 0x013F: /
      proto_tree_add_item(skinny_tree, hf_skinny_applicationID, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_lineInstance, tvb, offset+16, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_callIdentifier, tvb, offset+20, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_transactionID, tvb, offset+24, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_data_length, tvb, offset+28, 4, ENC_LITTLE_ENDIAN);
      count = tvb_get_letohl( tvb, offset+28);
      proto_tree_add_item(skinny_tree, hf_skinny_sequenceFlag, tvb, offset+30, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_displayPriority, tvb, offset+34, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_conferenceID, tvb, offset+38, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_appInstanceID, tvb, offset+42, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_routingID, tvb, offset+46, 4, ENC_LITTLE_ENDIAN);
      dissect_skinny_xml(skinny_tree, tvb, pinfo, offset+50, count);
      si->lineId = tvb_get_letohl(tvb, offset+16);
      si->callId = tvb_get_letohl(tvb, offset+20);
      break;

    case 0x0140: /
      proto_tree_add_item(skinny_tree, hf_skinny_unknown, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      break;

    case 0x0141: /
      proto_tree_add_item(skinny_tree, hf_skinny_conferenceID, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_passThruPartyID, tvb, offset+16, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_callIdentifier, tvb, offset+20, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_maxBitRate, tvb, offset+24, 4, ENC_LITTLE_ENDIAN);
      break;

    case 0x0143: /
      proto_tree_add_item(skinny_tree, hf_skinny_messageTimeOutValue, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_displayMessage, tvb, offset+16, hdr_data_length-8, ENC_ASCII|ENC_NA);
      break;

    case 0x0144: /
      proto_tree_add_item(skinny_tree, hf_skinny_messageTimeOutValue, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_priority, tvb, offset+16, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_displayMessage, tvb, offset+20, hdr_data_length-12, ENC_ASCII|ENC_NA);
      break;

    case 0x0145: /
      proto_tree_add_item(skinny_tree, hf_skinny_messageTimeOutValue, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_lineInstance, tvb, offset+16, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_callIdentifier, tvb, offset+20, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_displayMessage, tvb, offset+24, hdr_data_length-16, ENC_ASCII|ENC_NA);
      si->lineId = tvb_get_letohl(tvb, offset+16);
      si->callId = tvb_get_letohl(tvb, offset+20);
      break;

    case 0x0146: /
      proto_tree_add_item(skinny_tree, hf_skinny_featureIndex, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_featureID, tvb, offset+16, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_featureStatus, tvb, offset+20, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_featureTextLabel, tvb, offset+24, StationMaxNameSize, ENC_ASCII|ENC_NA);
      break;

    case 0x0147: /
      proto_tree_add_item(skinny_tree, hf_skinny_lineNumber, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_lineDirNumber, tvb, offset+16, 4, ENC_ASCII|ENC_NA);
      proto_tree_add_item(skinny_tree, hf_skinny_lineFullyQualifiedDisplayName, tvb, offset+16+StationMaxDirnumSize, StationMaxNameSize, ENC_ASCII|ENC_NA);
      proto_tree_add_item(skinny_tree, hf_skinny_lineDisplayName, tvb, offset+16+StationMaxDirnumSize+StationMaxNameSize, StationMaxDisplayNameSize, ENC_ASCII|ENC_NA);
      break;

    case 0x0148: /
      proto_tree_add_item(skinny_tree, hf_skinny_serviceURLIndex, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_serviceURL, tvb, offset+16, StationMaxServiceURLSize, ENC_ASCII|ENC_NA);
      proto_tree_add_item(skinny_tree, hf_skinny_serviceURLDisplayName, tvb, offset+16+StationMaxServiceURLSize, StationMaxNameSize, ENC_ASCII|ENC_NA);
      break;

    case 0x0149: /
      proto_tree_add_item(skinny_tree, hf_skinny_speedDialNumber, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_speedDialType, tvb, offset+16, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_speedDialStatus, tvb, offset+20, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_speedDialDisplayName, tvb, offset+24, StationMaxNameSize, ENC_ASCII|ENC_NA);
      break;

    case 0x014A: /
      proto_tree_add_item(skinny_tree, hf_skinny_lineInstance, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_callIdentifier, tvb, offset+16, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_callType, tvb, offset+20, 4, ENC_LITTLE_ENDIAN);
      si->lineId = tvb_get_letohl(tvb, offset+12);
      /
        /
        {
        i = offset+44;
        if(hdr_version == BASIC_MSG_TYPE)
        {
          /
          count = tvb_strnlen(tvb, i, -1)+1;
          proto_tree_add_item(skinny_tree, hf_skinny_callingPartyNumber, tvb, i, count, ENC_ASCII|ENC_NA);
          i += count;
          count = tvb_strnlen(tvb, i, -1)+1;
          proto_tree_add_item(skinny_tree, hf_skinny_calledPartyNumber, tvb, i, count, ENC_ASCII|ENC_NA);
          i += count;
          count = tvb_strnlen(tvb, i, -1)+1;
          proto_tree_add_item(skinny_tree, hf_skinny_originalCalledParty, tvb, i, count, ENC_ASCII|ENC_NA);
          i += count;
          count = tvb_strnlen(tvb, i, -1)+1;
          proto_tree_add_item(skinny_tree, hf_skinny_lastRedirectingParty, tvb, i, count, ENC_ASCII|ENC_NA);
          i += count;
          count = tvb_strnlen(tvb, i, -1)+1;
          proto_tree_add_item(skinny_tree, hf_skinny_cgpnVoiceMailbox, tvb, i, count, ENC_ASCII|ENC_NA);
          i += count;
          count = tvb_strnlen(tvb, i, -1)+1;
          proto_tree_add_item(skinny_tree, hf_skinny_cdpnVoiceMailbox, tvb, i, count, ENC_ASCII|ENC_NA);
          i += count;
          count = tvb_strnlen(tvb, i, -1)+1;
          proto_tree_add_item(skinny_tree, hf_skinny_originalCdpnVoiceMailbox, tvb, i, count, ENC_ASCII|ENC_NA);
          i += count;
          count = tvb_strnlen(tvb, i, -1)+1;
          proto_tree_add_item(skinny_tree, hf_skinny_lastRedirectingVoiceMailbox, tvb, i, count, ENC_ASCII|ENC_NA);
          i += count;
          /
          count = tvb_strnlen(tvb, i, -1)+1;
          proto_tree_add_item(skinny_tree, hf_skinny_callingPartyName, tvb, i, count, ENC_ASCII|ENC_NA);
          i += count;
          count = tvb_strnlen(tvb, i, -1)+1;
          proto_tree_add_item(skinny_tree, hf_skinny_calledPartyName, tvb, i, count, ENC_ASCII|ENC_NA);
          i += count;
          count = tvb_strnlen(tvb, i, -1)+1;
          proto_tree_add_item(skinny_tree, hf_skinny_originalCalledPartyName, tvb, i, count, ENC_ASCII|ENC_NA);
          i += count;
          count = tvb_strnlen(tvb, i, -1)+1;
          proto_tree_add_item(skinny_tree, hf_skinny_lastRedirectingPartyName, tvb, i, count, ENC_ASCII|ENC_NA);
        }
        else if(hdr_version == CM7_MSG_TYPE_B || hdr_version == CM7_MSG_TYPE_A)
        {/
          /
          count = tvb_strnlen(tvb, i, -1)+1;
          proto_tree_add_item(skinny_tree, hf_skinny_callingPartyNumber, tvb, i, count, ENC_ASCII|ENC_NA);
          i += count;
          count = tvb_strnlen(tvb, i, -1)+1;
          proto_tree_add_item(skinny_tree, hf_skinny_cgpnVoiceMailbox, tvb, i, count, ENC_ASCII|ENC_NA);
          i += count;
          count = tvb_strnlen(tvb, i, -1)+1;
          proto_tree_add_item(skinny_tree, hf_skinny_calledPartyNumber, tvb, i, count, ENC_ASCII|ENC_NA);
          i += count;
          count = tvb_strnlen(tvb, i, -1)+1;
          proto_tree_add_item(skinny_tree, hf_skinny_originalCalledParty, tvb, i, count, ENC_ASCII|ENC_NA);
          i += count;
          count = tvb_strnlen(tvb, i, -1)+1;
          proto_tree_add_item(skinny_tree, hf_skinny_lastRedirectingParty, tvb, i, count, ENC_ASCII|ENC_NA);
          i += count;
          count = tvb_strnlen(tvb, i, -1)+1;
          proto_tree_add_item(skinny_tree, hf_skinny_cdpnVoiceMailbox, tvb, i, count, ENC_ASCII|ENC_NA);
          i += count;
          count = tvb_strnlen(tvb, i, -1)+1;
          proto_tree_add_item(skinny_tree, hf_skinny_originalCdpnVoiceMailbox, tvb, i, count, ENC_ASCII|ENC_NA);
          i += count;
          count = tvb_strnlen(tvb, i, -1)+1;
          proto_tree_add_item(skinny_tree, hf_skinny_lastRedirectingVoiceMailbox, tvb, i, count, ENC_ASCII|ENC_NA);
          i += count;
          /
          count = tvb_strnlen(tvb, i, -1)+1;
          proto_tree_add_item(skinny_tree, hf_skinny_callingPartyName, tvb, i, count, ENC_ASCII|ENC_NA);
          i += count;
          count = tvb_strnlen(tvb, i, -1)+1;
          proto_tree_add_item(skinny_tree, hf_skinny_calledPartyName, tvb, i, count, ENC_ASCII|ENC_NA);
          i += count;
          count = tvb_strnlen(tvb, i, -1)+1;
          proto_tree_add_item(skinny_tree, hf_skinny_originalCalledPartyName, tvb, i, count, ENC_ASCII|ENC_NA);
          i += count;
          count = tvb_strnlen(tvb, i, -1)+1;
          proto_tree_add_item(skinny_tree, hf_skinny_lastRedirectingPartyName, tvb, i, count, ENC_ASCII|ENC_NA);
        }
      }
      break;

    case 0x0152: /
      proto_tree_add_item(skinny_tree, hf_skinny_directoryIndex, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_lineInstance, tvb, offset+16, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_unknown, tvb, offset+20, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_unknown, tvb, offset+24, 4, ENC_LITTLE_ENDIAN);
      break;

    case 0x0153: /
      proto_tree_add_item(skinny_tree, hf_skinny_directoryIndex, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_lineInstance, tvb, offset+16, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_unknown, tvb, offset+20, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_unknown, tvb, offset+24, 4, ENC_LITTLE_ENDIAN);
      break;

    case 0x0154: /
      proto_tree_add_item(skinny_tree, hf_skinny_callIdentifier, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_passThruPartyID, tvb, offset+16, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_callIdentifier, tvb, offset+20, 4, ENC_LITTLE_ENDIAN);
      si->lineId = tvb_get_letohl(tvb, offset+12);
      si->passThruId = tvb_get_letohl(tvb, offset+16);
      if(hdr_version == BASIC_MSG_TYPE)
      {
        proto_tree_add_item(skinny_tree, hf_skinny_ipAddress, tvb, offset+24, 4, ENC_BIG_ENDIAN);
        proto_tree_add_item(skinny_tree, hf_skinny_portNumber, tvb, offset+28, 4, ENC_LITTLE_ENDIAN);
        proto_tree_add_item(skinny_tree, hf_skinny_startMediaTransmissionStatus, tvb, offset+32, 4, ENC_LITTLE_ENDIAN);
        proto_tree_add_item(skinny_tree, hf_skinny_unknown, tvb, offset+36, 4, ENC_LITTLE_ENDIAN);
      } else if (hdr_version == CM7_MSG_TYPE_A || hdr_version == CM7_MSG_TYPE_B || hdr_version == CM7_MSG_TYPE_C || hdr_version == CM7_MSG_TYPE_D) {
        proto_tree_add_item(skinny_tree, hf_skinny_IPVersion, tvb, offset+24, 4, ENC_LITTLE_ENDIAN);
        ipversion = tvb_get_ntohl(tvb, offset+24);
        if (ipversion == 0) {
          proto_tree_add_item(skinny_tree, hf_skinny_ipAddress, tvb, offset+28, 4, ENC_BIG_ENDIAN);
        } else {
          proto_tree_add_item(skinny_tree, hf_skinny_ipV6Address, tvb, offset+28, 16, ENC_NA);
        }
        proto_tree_add_item(skinny_tree, hf_skinny_portNumber, tvb, offset+44, 4, ENC_LITTLE_ENDIAN);
        proto_tree_add_item(skinny_tree, hf_skinny_startMediaTransmissionStatus, tvb, offset+48, 4, ENC_LITTLE_ENDIAN);
      }
      break;

    case 0x0155: /
      proto_tree_add_item(skinny_tree, hf_skinny_callIdentifier, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_passThruPartyID, tvb, offset+16, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_callIdentifier, tvb, offset+20, 4, ENC_LITTLE_ENDIAN);
      si->lineId = tvb_get_letohl(tvb, offset+12);
      si->passThruId = tvb_get_letohl(tvb, offset+16);
      if(hdr_version == BASIC_MSG_TYPE)
      {
        proto_tree_add_item(skinny_tree, hf_skinny_ipAddress, tvb, offset+24, 4, ENC_BIG_ENDIAN);
        proto_tree_add_item(skinny_tree, hf_skinny_portNumber, tvb, offset+28, 4, ENC_LITTLE_ENDIAN);
        proto_tree_add_item(skinny_tree, hf_skinny_startMultiMediaTransmissionStatus, tvb, offset+32, 4, ENC_LITTLE_ENDIAN);
        proto_tree_add_item(skinny_tree, hf_skinny_unknown, tvb, offset+36, 4, ENC_LITTLE_ENDIAN);
      } else if (hdr_version == CM7_MSG_TYPE_A || hdr_version == CM7_MSG_TYPE_B || hdr_version == CM7_MSG_TYPE_C || hdr_version == CM7_MSG_TYPE_D) {
        ipversion = tvb_get_ntohl(tvb, offset+24);
        proto_tree_add_item(skinny_tree, hf_skinny_IPVersion, tvb, offset+24, 4, ENC_LITTLE_ENDIAN);
        if (ipversion == 0) {
          proto_tree_add_item(skinny_tree, hf_skinny_ipAddress, tvb, offset+28, 4, ENC_BIG_ENDIAN);
        } else {
          proto_tree_add_item(skinny_tree, hf_skinny_ipV6Address, tvb, offset+28, 16, ENC_NA);
        }
        proto_tree_add_item(skinny_tree, hf_skinny_portNumber, tvb, offset+44, 4, ENC_LITTLE_ENDIAN);
        proto_tree_add_item(skinny_tree, hf_skinny_startMultiMediaTransmissionStatus, tvb, offset+48, 4, ENC_LITTLE_ENDIAN);
      }
      break;

    case 0x0156: /
      proto_tree_add_item(skinny_tree, hf_skinny_unknown, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      break;

    case 0x0159: /
      proto_tree_add_item(skinny_tree, hf_skinny_unknown, tvb, offset+12, 672, ENC_ASCII|ENC_NA);
      break;


    case 0x015A: /
      dissect_skinny_xml(skinny_tree, tvb, pinfo, offset+12, hdr_data_length-4);
      break;

    case 0x015E: /
      if (hdr_data_length > 8) {
        proto_tree_add_item(skinny_tree, hf_skinny_unknown, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      }
      break;

      /
    case 0x8000: /
      proto_tree_add_item(skinny_tree, hf_skinny_deviceName, tvb, offset+12, StationMaxDeviceNameSize, ENC_ASCII|ENC_NA);
      i = offset+12+StationMaxDeviceNameSize;
      proto_tree_add_item(skinny_tree, hf_skinny_stationUserId, tvb, i, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_stationInstance, tvb, i+4, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_ipAddress, tvb, i+8, 4, ENC_BIG_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_deviceType, tvb, i+12, 4, ENC_LITTLE_ENDIAN);
      proto_tree_add_item(skinny_tree, hf_skinny_maxStreams, tvb, i+16, 4, ENC_LITTLE_ENDIAN);
      break;

      /
    case 0x8100: /
      proto_tree_add_item(skinny_tree, hf_skinny_featureID, tvb, offset+12, 4, ENC_ASCII|ENC_NA);
      break;

    case 0x8101: /
      proto_tree_add_item(skinny_tree, hf_skinny_tokenRejWaitTime, tvb, offset+12, 4, ENC_LITTLE_ENDIAN);
      break;

    default:
      proto_tree_add_item(skinny_tree, hf_skinny_rawData, tvb, offset+12, hdr_data_length-4, ENC_NA);
      break;
    }
  }
  tap_queue_packet(skinny_tap, pinfo, si);
  return tvb_captured_length(tvb);
}
