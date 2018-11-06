void
proto_register_ieee80211 (void)
{
  int i;
  GString *key_name, *key_title, *key_desc;

  static const value_string frame_type[] = {
    {MGT_FRAME,     "Management frame"},
    {CONTROL_FRAME, "Control frame"},
    {DATA_FRAME,    "Data frame"},
    {0,             NULL}
  };

  static const value_string tofrom_ds[] = {
    {0,                       "Not leaving DS or network is operating "
      "in AD-HOC mode (To DS: 0 From DS: 0)"},
    {FLAG_TO_DS,              "Frame from STA to DS via an AP (To DS: 1 "
      "From DS: 0)"},
    {FLAG_FROM_DS,            "Frame from DS to a STA via AP(To DS: 0 "
      "From DS: 1)"},
    {FLAG_TO_DS|FLAG_FROM_DS, "Frame part of WDS from one AP to another "
      "AP (To DS: 1 From DS: 1)"},
    {0, NULL}
  };

  static const true_false_string tods_flag = {
    "Frame is entering DS",
    "Frame is not entering DS"
  };

  static const true_false_string fromds_flag = {
    "Frame is exiting DS",
    "Frame is not exiting DS"
  };

  static const true_false_string more_frags = {
    "More fragments follow",
    "This is the last fragment"
  };

  static const true_false_string retry_flags = {
    "Frame is being retransmitted",
    "Frame is not being retransmitted"
  };

  static const true_false_string pm_flags = {
    "STA will go to sleep",
    "STA will stay up"
  };

  static const true_false_string md_flags = {
    "Data is buffered for STA at AP",
    "No data buffered"
  };

  static const true_false_string protected_flags = {
    "Data is protected",
    "Data is not protected"
  };

  static const true_false_string order_flags = {
    "Strictly ordered",
    "Not strictly ordered"
  };

  static const true_false_string cf_ess_flags = {
    "Transmitter is an AP",
    "Transmitter is a STA"
  };


  static const true_false_string cf_privacy_flags = {
    "AP/STA can support WEP",
    "AP/STA cannot support WEP"
  };

  static const true_false_string cf_preamble_flags = {
    "Short preamble allowed",
    "Short preamble not allowed"
  };

  static const true_false_string cf_pbcc_flags = {
    "PBCC modulation allowed",
    "PBCC modulation not allowed"
  };

  static const true_false_string cf_agility_flags = {
    "Channel agility in use",
    "Channel agility not in use"
  };

  static const true_false_string short_slot_time_flags = {
    "Short slot time in use",
    "Short slot time not in use"
  };

  static const true_false_string dsss_ofdm_flags = {
    "DSSS-OFDM modulation allowed",
    "DSSS-OFDM modulation not allowed"
  };

  static const true_false_string cf_spec_man_flags = {
    "dot11SpectrumManagementRequired TRUE",
    "dot11SpectrumManagementRequired FALSE",
  };

  static const true_false_string cf_apsd_flags = {
    "apsd implemented",
    "apsd not implemented",
  };

  static const true_false_string cf_del_blk_ack_flags = {
    "delayed block ack implemented",
    "delayed block ack not implented",
  };

  static const true_false_string cf_imm_blk_ack_flags = {
    "immediate block ack implemented",
    "immediate block ack not implented",
  };
  static const true_false_string cf_ibss_flags = {
    "Transmitter belongs to an IBSS",
    "Transmitter belongs to a BSS"
  };

  static const true_false_string eosp_flag = {
    "End of service period",
    "Service period"
  };

  static const value_string sta_cf_pollable[] = {
    {0x00, "Station is not CF-Pollable"},
    {0x02, "Station is CF-Pollable, "
     "not requesting to be placed on the  CF-polling list"},
    {0x01, "Station is CF-Pollable, "
     "requesting to be placed on the CF-polling list"},
    {0x03, "Station is CF-Pollable, requesting never to be polled"},
    {0x0200, "QSTA requesting association in QBSS"},
    {0, NULL}
  };

  static const value_string ap_cf_pollable[] = {
    {0x00, "No point coordinator at AP"},
    {0x02, "Point coordinator at AP for delivery only (no polling)"},
    {0x01, "Point coordinator at AP for delivery and polling"},
    {0x03, "Reserved"},
    {0x0200, "QAP (HC) does not use CFP for delivery of unicast data type frames"},
    {0x0202, "QAP (HC) uses CFP for delivery, but does not send CF-Polls to non-QoS STAs"},
    {0x0201, "QAP (HC) uses CFP for delivery, and sends CF-Polls to non-QoS STAs"},
    {0x0203, "Reserved"},
    {0, NULL}
  };


  static const value_string auth_alg[] = {
    {0x00, "Open System"},
    {0x01, "Shared key"},
    {0x80, "Network EAP"},	/
    {0, NULL}
  };

  static const value_string reason_codes[] = {
    {0x00, "Reserved"},
    {0x01, "Unspecified reason"},
    {0x02, "Previous authentication no longer valid"},
    {0x03, "Deauthenticated because sending STA is leaving (has left) "
     "IBSS or ESS"},
    {0x04, "Disassociated due to inactivity"},
    {0x05, "Disassociated because AP is unable to handle all currently "
     "associated stations"},
    {0x06, "Class 2 frame received from nonauthenticated station"},
    {0x07, "Class 3 frame received from nonassociated station"},
    {0x08, "Disassociated because sending STA is leaving (has left) BSS"},
    {0x09, "Station requesting (re)association is not authenticated with "
      "responding station"},
    {0x0A, "Disassociated because the information in the Power Capability "
      "element is unacceptable"},
    {0x0B, "Disassociated because the information in the Supported"
      "Channels element is unacceptable"},
    {0x0D, "Invalid Information Element"},
    {0x0E, "Michael MIC failure"},
    {0x0F, "4-Way Handshake timeout"},
    {0x10, "Group key update timeout"},
    {0x11, "Information element in 4-Way Handshake different from "
     "(Re)Association Request/Probe Response/Beacon"},
    {0x12, "Group Cipher is not valid"},
    {0x13, "Pairwise Cipher is not valid"},
    {0x14, "AKMP is not valid"},
    {0x15, "Unsupported RSN IE version"},
    {0x16, "Invalid RSN IE Capabilities"},
    {0x17, "IEEE 802.1X Authentication failed"},
    {0x18, "Cipher suite is rejected per security policy"},
    {0x20, "Disassociated for unspecified, QoS-related reason"},
    {0x21, "Disassociated because QAP lacks sufficient bandwidth for this QSTA"},
    {0x22, "Disassociated because of excessive number of frames that need to be "
      "acknowledged, but are not acknowledged for AP transmissions and/or poor "
	"channel conditions"},
    {0x23, "Disassociated because QSTA is transmitting outside the limits of its TXOPs"},
    {0x24, "Requested from peer QSTA as the QSTA is leaving the QBSS (or resetting)"},
    {0x25, "Requested from peer QSTA as it does not want to use the mechanism"},
    {0x26, "Requested from peer QSTA as the QSTA received frames using the mechanism "
      "for which a set up is required"},
    {0x27, "Requested from peer QSTA due to time out"},
    {0x2D, "Peer QSTA does not support the requested cipher suite"},
    {0x00, NULL}
  };


  static const value_string status_codes[] = {
    {0x00, "Successful"},
    {0x01, "Unspecified failure"},
    {0x0A, "Cannot support all requested capabilities in the "
     "Capability information field"},
    {0x0B, "Reassociation denied due to inability to confirm that "
     "association exists"},
    {0x0C, "Association denied due to reason outside the scope of this "
     "standard"},

    {0x0D, "Responding station does not support the specified authentication "
     "algorithm"},
    {0x0E, "Received an Authentication frame with authentication sequence "
     "transaction sequence number out of expected sequence"},
    {0x0F, "Authentication rejected because of challenge failure"},
    {0x10, "Authentication rejected due to timeout waiting for next "
     "frame in sequence"},
    {0x11, "Association denied because AP is unable to handle additional "
     "associated stations"},
    {0x12, "Association denied due to requesting station not supporting all "
     "of the datarates in the BSSBasicServiceSet Parameter"},
    {0x13, "Association denied due to requesting station not supporting "
     "short preamble operation"},
    {0x14, "Association denied due to requesting station not supporting "
     "PBCC encoding"},
    {0x15, "Association denied due to requesting station not supporting "
     "channel agility"},
    {0x16, "Association request rejected because Spectrum Management"
      "capability is required"},
    {0x17, "Association request rejected because the information in the"
      "Power Capability element is unacceptable"},
    {0x18, "Association request rejected because the information in the"
      "Supported Channels element is unacceptable"},
    {0x19, "Association denied due to requesting station not supporting "
     "short slot operation"},
    {0x1A, "Association denied due to requesting station not supporting "
     "DSSS-OFDM operation"},
    {0x20, "Unspecified, QoS-related failure"},
    {0x21, "Association denied due to QAP having insufficient bandwidth "
      "to handle another QSTA"},
    {0x22, "Association denied due to excessive frame loss rates and/or "
      "poor conditions on current operating channel"},
    {0x23, "Association (with QBSS) denied due to requesting station not "
      "supporting the QoS facility"},
    {0x24, "Association denied due to requesting station not supporting "
      "Block Ack"},
    {0x25, "The request has been declined."},
    {0x26, "The request has not been successful as one or more parameters "
      "have invalid values."},
    {0x27, "The TS has not been created because the request cannot be honored. "
      "However, a suggested TSPEC is provided so that the initiating QSTA may "
	"attempt to set another TS with the suggested changes to the TSPEC."},
    {0x28, "Invalid Information Element"},
    {0x29, "Group Cipher is not valid"},
    {0x2A, "Pairwise Cipher is not valid"},
    {0x2B, "AKMP is not valid"},
    {0x2C, "Unsupported RSN IE version"},
    {0x2D, "Invalid RSN IE Capabilities"},
    {0x2E, "Cipher suite is rejected per security policy"},
    {0x2F, "The TS has not been created. However, the HC may be capable of "
      "creating a TS, in response to a request, after the time indicated in the TS Delay element."},
    {0x30, "Direct Link is not allowed in the BSS by policy"},
    {0x31, "Destination STA is not present within this QBSS."},
    {0x32, "The Destination STA is not a QSTA."},
    {0x00, NULL}
  };

  static const value_string category_codes[] = {
    {CAT_SPECTRUM_MGMT, "Spectrum Management"},
    {CAT_QOS, "QoS"},
    {CAT_DLS, "DLS"},
    {CAT_BLOCK_ACK, "Block Ack"},
    {CAT_MGMT_NOTIFICATION, "Management notification frame"},
    {0, NULL}
  };

  static const value_string action_codes[] ={
    {SM_ACTION_MEASUREMENT_REQUEST, "Measurement Request"},
    {SM_ACTION_MEASUREMENT_REPORT, "Measurement Report"},
    {SM_ACTION_TPC_REQUEST, "TPC Request"},
    {SM_ACTION_TPC_REPORT, "TPC Report"},
    {SM_ACTION_CHAN_SWITCH_ANNC, "Channel Switch Announcement"},
    {0, NULL}
  };

  static const value_string wme_action_codes[] = {
    {0x00, "Setup request"},
    {0x01, "Setup response"},
    {0x02, "Teardown"},
    {0x00, NULL}
  };

  static const value_string wme_status_codes[] = {
    {0x00, "Admission accepted"},
    {0x01, "Invalid parameters"},
    {0x03, "Refused"},
    {0x00, NULL}
  };

  static const value_string ack_policy[] = {
    {0x00, "Normal Ack"},
    {0x01, "No Ack"},
    {0x02, "No explicit Ack"},
    {0x03, "Block Ack"},
    {0x00, NULL}
  };

  static const value_string qos_action_codes[] = {
    {SM_ACTION_ADDTS_REQUEST, "ADDTS Request"},
    {SM_ACTION_ADDTS_RESPONSE, "ADDTS Response"},
    {SM_ACTION_DELTS, "DELTS"},
    {SM_ACTION_QOS_SCHEDULE, "Schedule"},
    {0, NULL}
  };

  static const value_string dls_action_codes[] = {
    {SM_ACTION_DLS_REQUEST, "DLS Request"},
    {SM_ACTION_DLS_RESPONSE, "DLS Response"},
    {SM_ACTION_DLS_TEARDOWN, "DLS Teardown"},
    {0, NULL}
  };

  static const value_string tsinfo_type[] = {
    {0x0, "Aperiodic or unspecified Traffic"},
    {0x1, "Periodic Traffic"},
    {0, NULL}
  };

  static const value_string tsinfo_direction[] = {
    {0x00, "Uplink"},
    {0x01, "Downlink"},
    {0x02, "Direct link"},
    {0x03, "Bidirectional link"},
    {0, NULL}
  };

  static const value_string tsinfo_access[] = {
    {0x00, "Reserved"},
    {0x01, "EDCA"},
    {0x02, "HCCA"},
    {0x03, "HEMM"},
    {0, NULL}
  };

  static const value_string qos_up[] = {
    {0x00, "Best Effort"},
    {0x01, "Background"},
    {0x02, "Spare"},
    {0x03, "Excellent Effort"},
    {0x04, "Controlled Load"},
    {0x05, "Video"},
    {0x06, "Voice"},
    {0x07, "Network Control"},
    {0, NULL}
  };

  static const value_string classifier_type[] = {
    {0x00, "Ethernet parameters"},
    {0x01, "TCP/UDP IP parameters"},
    {0x02, "IEEE 802.1D/Q parameters"},
    {0, NULL}
  };

  static const value_string tclas_process[] = {
    {0x00, "Incoming MSDU's higher layer parameters have to match to the parameters in all associated TCLAS elements."},
    {0x01, "Incoming MSDU's higher layer parameters have to match to at least one of the associated TCLAS elements."},
    {0x02, "Incoming MSDU's that do not belong to any other TS are classified to the TS for which this TCLAS Processing element is used. In this case, there will not be any associated TCLAS elements."},
    {0, NULL}
  };

  static hf_register_info hf[] = {
    {&hf_data_rate,
     {"Data Rate", "wlan.data_rate", FT_UINT8, BASE_DEC, NULL, 0,
      "Data rate (.5 Mb/s units)", HFILL }},

    {&hf_channel,
     {"Channel", "wlan.channel", FT_UINT8, BASE_DEC, NULL, 0,
      "Radio channel", HFILL }},

    {&hf_signal_strength,
     {"Signal Strength", "wlan.signal_strength", FT_UINT8, BASE_DEC, NULL, 0,
      "Signal strength (percentage)", HFILL }},

    {&hf_fc_field,
     {"Frame Control Field", "wlan.fc", FT_UINT16, BASE_HEX, NULL, 0,
      "MAC Frame control", HFILL }},

    {&hf_fc_proto_version,
     {"Version", "wlan.fc.version", FT_UINT8, BASE_DEC, NULL, 0,
      "MAC Protocol version", HFILL }},	/

    {&hf_fc_frame_type,
     {"Type", "wlan.fc.type", FT_UINT8, BASE_DEC, VALS(frame_type), 0,
      "Frame type", HFILL }},

    {&hf_fc_frame_subtype,
     {"Subtype", "wlan.fc.subtype", FT_UINT8, BASE_DEC, NULL, 0,
      "Frame subtype", HFILL }},	/

    {&hf_fc_frame_type_subtype,
     {"Type/Subtype", "wlan.fc.type_subtype", FT_UINT16, BASE_DEC, VALS(frame_type_subtype_vals), 0,
      "Type and subtype combined", HFILL }},

    {&hf_fc_flags,
     {"Protocol Flags", "wlan.flags", FT_UINT8, BASE_HEX, NULL, 0,
      "Protocol flags", HFILL }},

    {&hf_fc_data_ds,
     {"DS status", "wlan.fc.ds", FT_UINT8, BASE_HEX, VALS (&tofrom_ds), 0,
      "Data-frame DS-traversal status", HFILL }},	/

    {&hf_fc_to_ds,
     {"To DS", "wlan.fc.tods", FT_BOOLEAN, 8, TFS (&tods_flag), FLAG_TO_DS,
      "To DS flag", HFILL }},		/

    {&hf_fc_from_ds,
     {"From DS", "wlan.fc.fromds", FT_BOOLEAN, 8, TFS (&fromds_flag), FLAG_FROM_DS,
      "From DS flag", HFILL }},		/

    {&hf_fc_more_frag,
     {"More Fragments", "wlan.fc.frag", FT_BOOLEAN, 8, TFS (&more_frags), FLAG_MORE_FRAGMENTS,
      "More Fragments flag", HFILL }},	/

    {&hf_fc_retry,
     {"Retry", "wlan.fc.retry", FT_BOOLEAN, 8, TFS (&retry_flags), FLAG_RETRY,
      "Retransmission flag", HFILL }},

    {&hf_fc_pwr_mgt,
     {"PWR MGT", "wlan.fc.pwrmgt", FT_BOOLEAN, 8, TFS (&pm_flags), FLAG_POWER_MGT,
      "Power management status", HFILL }},

    {&hf_fc_more_data,
     {"More Data", "wlan.fc.moredata", FT_BOOLEAN, 8, TFS (&md_flags), FLAG_MORE_DATA,
      "More data flag", HFILL }},

    {&hf_fc_protected,
     {"Protected flag", "wlan.fc.protected", FT_BOOLEAN, 8, TFS (&protected_flags), FLAG_PROTECTED,
      "Protected flag", HFILL }},

    {&hf_fc_order,
     {"Order flag", "wlan.fc.order", FT_BOOLEAN, 8, TFS (&order_flags), FLAG_ORDER,
      "Strictly ordered flag", HFILL }},

    {&hf_assoc_id,
     {"Association ID","wlan.aid",FT_UINT16, BASE_DEC,NULL,0,
      "Association-ID field", HFILL }},

    {&hf_did_duration,
     {"Duration", "wlan.duration", FT_UINT16, BASE_DEC, NULL, 0,
      "Duration field", HFILL }},

    {&hf_addr_da,
     {"Destination address", "wlan.da", FT_ETHER, BASE_NONE, NULL, 0,
      "Destination Hardware Address", HFILL }},

    {&hf_addr_sa,
     {"Source address", "wlan.sa", FT_ETHER, BASE_NONE, NULL, 0,
      "Source Hardware Address", HFILL }},

    { &hf_addr,
      {"Source or Destination address", "wlan.addr", FT_ETHER, BASE_NONE, NULL, 0,
       "Source or Destination Hardware Address", HFILL }},

    {&hf_addr_ra,
     {"Receiver address", "wlan.ra", FT_ETHER, BASE_NONE, NULL, 0,
      "Receiving Station Hardware Address", HFILL }},

    {&hf_addr_ta,
     {"Transmitter address", "wlan.ta", FT_ETHER, BASE_NONE, NULL, 0,
      "Transmitting Station Hardware Address", HFILL }},

    {&hf_addr_bssid,
     {"BSS Id", "wlan.bssid", FT_ETHER, BASE_NONE, NULL, 0,
      "Basic Service Set ID", HFILL }},

    {&hf_frag_number,
     {"Fragment number", "wlan.frag", FT_UINT16, BASE_DEC, NULL, 0,
      "Fragment number", HFILL }},

    {&hf_seq_number,
     {"Sequence number", "wlan.seq", FT_UINT16, BASE_DEC, NULL, 0,
      "Sequence number", HFILL }},

    {&hf_qos_priority,
     {"Priority", "wlan.qos.priority", FT_UINT16, BASE_DEC, NULL, 0,
      "802.1D Tag", HFILL }},

    {&hf_qos_eosp,
     {"EOSP", "wlan.qos.eosp", FT_BOOLEAN, 8, TFS (&eosp_flag), QOS_FLAG_EOSP,
      "EOSP Field", HFILL }},

    {&hf_qos_ack_policy,
     {"Ack Policy", "wlan.qos.ack", FT_UINT16, BASE_HEX,  VALS (&ack_policy), 0,
      "Ack Policy", HFILL }},

    {&hf_qos_field_content,
     {"Content", "wlan.qos.fc_content", FT_UINT16, BASE_DEC, NULL, 0,
      "Content1", HFILL }},

/

    {&hf_fcs,
     {"Frame check sequence", "wlan.fcs", FT_UINT32, BASE_HEX,
      NULL, 0, "FCS", HFILL }},

    {&hf_fragment_overlap,
      {"Fragment overlap", "wlan.fragment.overlap", FT_BOOLEAN, BASE_NONE,
       NULL, 0x0, "Fragment overlaps with other fragments", HFILL }},

    {&hf_fragment_overlap_conflict,
      {"Conflicting data in fragment overlap", "wlan.fragment.overlap.conflict",
       FT_BOOLEAN, BASE_NONE, NULL, 0x0,
       "Overlapping fragments contained conflicting data", HFILL }},

    {&hf_fragment_multiple_tails,
      {"Multiple tail fragments found", "wlan.fragment.multipletails",
       FT_BOOLEAN, BASE_NONE, NULL, 0x0,
       "Several tails were found when defragmenting the packet", HFILL }},

    {&hf_fragment_too_long_fragment,
      {"Fragment too long", "wlan.fragment.toolongfragment",
       FT_BOOLEAN, BASE_NONE, NULL, 0x0,
       "Fragment contained data past end of packet", HFILL }},

    {&hf_fragment_error,
      {"Defragmentation error", "wlan.fragment.error",
       FT_FRAMENUM, BASE_NONE, NULL, 0x0,
       "Defragmentation error due to illegal fragments", HFILL }},

    {&hf_fragment,
      {"802.11 Fragment", "wlan.fragment", FT_FRAMENUM, BASE_NONE, NULL, 0x0,
       "802.11 Fragment", HFILL }},

    {&hf_fragments,
      {"802.11 Fragments", "wlan.fragments", FT_NONE, BASE_NONE, NULL, 0x0,
       "802.11 Fragments", HFILL }},

    {&hf_reassembled_in,
      {"Reassembled 802.11 in frame", "wlan.reassembled_in", FT_FRAMENUM, BASE_NONE, NULL, 0x0,
       "This 802.11 packet is reassembled in this frame", HFILL }},

    {&hf_wep_iv,
     {"Initialization Vector", "wlan.wep.iv", FT_UINT24, BASE_HEX, NULL, 0,
      "Initialization Vector", HFILL }},

    {&hf_wep_iv_weak,
     {"Weak IV", "wlan.wep.weakiv", FT_BOOLEAN,BASE_NONE, NULL,0x0,
       "Weak IV",HFILL}},

    {&hf_tkip_extiv,
     {"TKIP Ext. Initialization Vector", "wlan.tkip.extiv", FT_STRING,
      BASE_HEX, NULL, 0, "TKIP Extended Initialization Vector", HFILL }},

    {&hf_ccmp_extiv,
     {"CCMP Ext. Initialization Vector", "wlan.ccmp.extiv", FT_STRING,
      BASE_HEX, NULL, 0, "CCMP Extended Initialization Vector", HFILL }},

    {&hf_wep_key,
     {"Key Index", "wlan.wep.key", FT_UINT8, BASE_DEC, NULL, 0,
      "Key Index", HFILL }},

    {&hf_wep_icv,
     {"WEP ICV", "wlan.wep.icv", FT_UINT32, BASE_HEX, NULL, 0,
      "WEP ICV", HFILL }},
  };

  static const true_false_string rsn_preauth_flags = {
    "Transmitter supports pre-authentication",
    "Transmitter does not support pre-authentication"
  };

  static const true_false_string rsn_no_pairwise_flags = {
    "Transmitter cannot support WEP default key 0 simultaneously with "
    "Pairwise key",
    "Transmitter can support WEP default key 0 simultaneously with "
    "Pairwise key"
  };

  static const value_string rsn_cap_replay_counter[] = {
    {0x00, "1 replay counter per PTKSA/GTKSA/STAKeySA"},
    {0x01, "2 replay counters per PTKSA/GTKSA/STAKeySA"},
    {0x02, "4 replay counters per PTKSA/GTKSA/STAKeySA"},
    {0x03, "16 replay counters per PTKSA/GTKSA/STAKeySA"},
    {0, NULL}
  };

  static hf_register_info ff[] = {
    {&ff_timestamp,
     {"Timestamp", "wlan_mgt.fixed.timestamp", FT_STRING, BASE_NONE,
      NULL, 0, "", HFILL }},

    {&ff_auth_alg,
     {"Authentication Algorithm", "wlan_mgt.fixed.auth.alg",
      FT_UINT16, BASE_DEC, VALS (&auth_alg), 0, "", HFILL }},

    {&ff_beacon_interval,
     {"Beacon Interval", "wlan_mgt.fixed.beacon", FT_DOUBLE, BASE_DEC, NULL, 0,
      "", HFILL }},

    {&hf_fixed_parameters,
     {"Fixed parameters", "wlan_mgt.fixed.all", FT_UINT16, BASE_DEC, NULL, 0,
      "", HFILL }},

    {&hf_tagged_parameters,
     {"Tagged parameters", "wlan_mgt.tagged.all", FT_UINT16, BASE_DEC, NULL, 0,
      "", HFILL }},

    {&ff_capture,
     {"Capabilities", "wlan_mgt.fixed.capabilities", FT_UINT16, BASE_HEX, NULL, 0,
      "Capability information", HFILL }},

    {&ff_cf_ess,
     {"ESS capabilities", "wlan_mgt.fixed.capabilities.ess",
      FT_BOOLEAN, 16, TFS (&cf_ess_flags), 0x0001, "ESS capabilities", HFILL }},

    {&ff_cf_ibss,
     {"IBSS status", "wlan_mgt.fixed.capabilities.ibss",
      FT_BOOLEAN, 16, TFS (&cf_ibss_flags), 0x0002, "IBSS participation", HFILL }},

    {&ff_cf_sta_poll,
     {"CFP participation capabilities", "wlan_mgt.fixed.capabilities.cfpoll.sta",
      FT_UINT16, BASE_HEX, VALS (&sta_cf_pollable), 0x020C,
      "CF-Poll capabilities for a STA", HFILL }},

    {&ff_cf_ap_poll,
     {"CFP participation capabilities", "wlan_mgt.fixed.capabilities.cfpoll.ap",
      FT_UINT16, BASE_HEX, VALS (&ap_cf_pollable), 0x020C,
      "CF-Poll capabilities for an AP", HFILL }},

    {&ff_cf_privacy,
     {"Privacy", "wlan_mgt.fixed.capabilities.privacy",
      FT_BOOLEAN, 16, TFS (&cf_privacy_flags), 0x0010, "WEP support", HFILL }},

    {&ff_cf_preamble,
     {"Short Preamble", "wlan_mgt.fixed.capabilities.preamble",
      FT_BOOLEAN, 16, TFS (&cf_preamble_flags), 0x0020, "Short Preamble", HFILL }},

    {&ff_cf_pbcc,
     {"PBCC", "wlan_mgt.fixed.capabilities.pbcc",
      FT_BOOLEAN, 16, TFS (&cf_pbcc_flags), 0x0040, "PBCC Modulation", HFILL }},

    {&ff_cf_agility,
     {"Channel Agility", "wlan_mgt.fixed.capabilities.agility",
      FT_BOOLEAN, 16, TFS (&cf_agility_flags), 0x0080, "Channel Agility", HFILL }},

    {&ff_cf_spec_man,
     {"Spectrum Management", "wlan_mgt.fixed.capabilities.spec_man",
      FT_BOOLEAN, 16, TFS (&cf_spec_man_flags), 0x0100, "Spectrum Management", HFILL }},

    {&ff_short_slot_time,
     {"Short Slot Time", "wlan_mgt.fixed.capabilities.short_slot_time",
      FT_BOOLEAN, 16, TFS (&short_slot_time_flags), 0x0400, "Short Slot Time",
      HFILL }},

    {&ff_cf_apsd,
     {"Automatic Power Save Delivery", "wlan_mgt.fixed.capabilities.apsd",
      FT_BOOLEAN, 16, TFS (&cf_apsd_flags), 0x0800, "Automatic Power Save "
	"Delivery", HFILL }},

    {&ff_dsss_ofdm,
     {"DSSS-OFDM", "wlan_mgt.fixed.capabilities.dsss_ofdm",
      FT_BOOLEAN, 16, TFS (&dsss_ofdm_flags), 0x2000, "DSSS-OFDM Modulation",
      HFILL }},

    {&ff_cf_del_blk_ack,
     {"Delayed Block Ack", "wlan_mgt.fixed.capabilities.del_blk_ack",
      FT_BOOLEAN, 16, TFS (&cf_del_blk_ack_flags), 0x4000, "Delayed Block "
	"Ack", HFILL }},

    {&ff_cf_imm_blk_ack,
     {"Immediate Block Ack", "wlan_mgt.fixed.capabilities.imm_blk_ack",
      FT_BOOLEAN, 16, TFS (&cf_imm_blk_ack_flags), 0x8000, "Immediate Block "
	"Ack", HFILL }},

    {&ff_auth_seq,
     {"Authentication SEQ", "wlan_mgt.fixed.auth_seq",
      FT_UINT16, BASE_HEX, NULL, 0, "Authentication sequence number", HFILL }},

    {&ff_assoc_id,
     {"Association ID", "wlan_mgt.fixed.aid",
      FT_UINT16, BASE_HEX, NULL, 0, "Association ID", HFILL }},

    {&ff_listen_ival,
     {"Listen Interval", "wlan_mgt.fixed.listen_ival",
      FT_UINT16, BASE_HEX, NULL, 0, "Listen Interval", HFILL }},

    {&ff_current_ap,
     {"Current AP", "wlan_mgt.fixed.current_ap",
      FT_ETHER, BASE_NONE, NULL, 0, "MAC address of current AP", HFILL }},

    {&ff_reason,
     {"Reason code", "wlan_mgt.fixed.reason_code",
      FT_UINT16, BASE_HEX, VALS (&reason_codes), 0,
      "Reason for unsolicited notification", HFILL }},

    {&ff_status_code,
     {"Status code", "wlan_mgt.fixed.status_code",
      FT_UINT16, BASE_HEX, VALS (&status_codes), 0,
      "Status of requested event", HFILL }},

    {&ff_category_code,
     {"Category code", "wlan_mgt.fixed.category_code",
      FT_UINT16, BASE_DEC, VALS (&category_codes), 0,
      "Management action category", HFILL }},

    {&ff_action_code,
     {"Action code", "wlan_mgt.fixed.action_code",
      FT_UINT16, BASE_DEC, VALS (&action_codes), 0,
      "Management action code", HFILL }},

    {&ff_dialog_token,
     {"Dialog token", "wlan_mgt.fixed.dialog_token",
      FT_UINT16, BASE_HEX, NULL, 0, "Management action dialog token", HFILL }},

    {&ff_wme_action_code,
     {"Action code", "wlan_mgt.fixed.action_code",
      FT_UINT16, BASE_HEX, VALS (&wme_action_codes), 0,
      "Management notification action code", HFILL }},

    {&ff_wme_status_code,
     {"Status code", "wlan_mgt.fixed.status_code",
      FT_UINT16, BASE_HEX, VALS (&wme_status_codes), 0,
      "Management notification setup response status code", HFILL }},

    {&ff_qos_action_code,
     {"Action code", "wlan_mgt.fixed.action_code",
      FT_UINT16, BASE_HEX, VALS (&qos_action_codes), 0,
      "QoS management action code", HFILL }},

    {&ff_dls_action_code,
     {"Action code", "wlan_mgt.fixed.action_code",
      FT_UINT16, BASE_HEX, VALS (&dls_action_codes), 0,
      "DLS management action code", HFILL }},

    {&ff_dst_mac_addr,
     {"Destination address", "wlan_mgt.fixed.dst_mac_addr",
      FT_ETHER, BASE_NONE, NULL, 0, "Destination MAC address", HFILL }},

    {&ff_src_mac_addr,
     {"Source address", "wlan_mgt.fixed.src_mac_addr",
      FT_ETHER, BASE_NONE, NULL, 0, "Source MAC address", HFILL }},

    {&ff_dls_timeout,
     {"DLS timeout", "wlan_mgt.fixed.dls_timeout",
      FT_UINT16, BASE_HEX, NULL, 0, "DLS timeout value", HFILL }},

    {&tag_number,
     {"Tag", "wlan_mgt.tag.number",
      FT_UINT8, BASE_DEC, VALS(tag_num_vals), 0,
      "Element ID", HFILL }},

    {&tag_length,
     {"Tag length", "wlan_mgt.tag.length",
      FT_UINT8, BASE_DEC, NULL, 0, "Length of tag", HFILL }},

    {&tag_interpretation,
     {"Tag interpretation", "wlan_mgt.tag.interpretation",
      FT_STRING, BASE_NONE, NULL, 0, "Interpretation of tag", HFILL }},

    {&tag_oui,
     {"OUI", "wlan_mgt.tag.oui",
      FT_BYTES, BASE_NONE, NULL, 0, "OUI of vendor specific IE", HFILL }},

    {&tim_length,
     {"TIM length", "wlan_mgt.tim.length",
      FT_UINT8, BASE_DEC, NULL, 0,
      "Traffic Indication Map length", HFILL }},

    {&tim_dtim_count,
     {"DTIM count", "wlan_mgt.tim.dtim_count",
      FT_UINT8, BASE_DEC, NULL, 0,
      "DTIM count", HFILL }},

    {&tim_dtim_period,
     {"DTIM period", "wlan_mgt.tim.dtim_period",
      FT_UINT8, BASE_DEC, NULL, 0,
      "DTIM period", HFILL }},

    {&tim_bmapctl,
     {"Bitmap control", "wlan_mgt.tim.bmapctl",
      FT_UINT8, BASE_HEX, NULL, 0,
      "Bitmap control", HFILL }},

    {&rsn_cap,
     {"RSN Capabilities", "wlan_mgt.rsn.capabilities", FT_UINT16, BASE_HEX,
      NULL, 0, "RSN Capability information", HFILL }},

    {&rsn_cap_preauth,
     {"RSN Pre-Auth capabilities", "wlan_mgt.rsn.capabilities.preauth",
      FT_BOOLEAN, 16, TFS (&rsn_preauth_flags), 0x0001,
      "RSN Pre-Auth capabilities", HFILL }},

    {&rsn_cap_no_pairwise,
     {"RSN No Pairwise capabilities", "wlan_mgt.rsn.capabilities.no_pairwise",
      FT_BOOLEAN, 16, TFS (&rsn_no_pairwise_flags), 0x0002,
      "RSN No Pairwise capabilities", HFILL }},

    {&rsn_cap_ptksa_replay_counter,
     {"RSN PTKSA Replay Counter capabilities",
      "wlan_mgt.rsn.capabilities.ptksa_replay_counter",
      FT_UINT16, BASE_HEX, VALS (&rsn_cap_replay_counter), 0x000C,
      "RSN PTKSA Replay Counter capabilities", HFILL }},

    {&rsn_cap_gtksa_replay_counter,
     {"RSN GTKSA Replay Counter capabilities",
      "wlan_mgt.rsn.capabilities.gtksa_replay_counter",
      FT_UINT16, BASE_HEX, VALS (&rsn_cap_replay_counter), 0x0030,
      "RSN GTKSA Replay Counter capabilities", HFILL }},

    {&hf_aironet_ie_type,
     {"Aironet IE type", "wlan_mgt.aironet.type",
      FT_UINT8, BASE_DEC, VALS(aironet_ie_type_vals), 0, "", HFILL }},

    {&hf_aironet_ie_version,
     {"Aironet IE CCX version?", "wlan_mgt.aironet.version",
      FT_UINT8, BASE_DEC, NULL, 0, "", HFILL }},

    { &hf_aironet_ie_data,
      { "Aironet IE data", "wlan_mgmt.aironet.data",
        FT_BYTES, BASE_NONE, NULL, 0x0, "", HFILL }},

    {&hf_qbss_version,
     {"QBSS Version", "wlan_mgt.qbss.version",
      FT_UINT8, BASE_DEC, NULL, 0, "", HFILL }},

    {&hf_qbss_scount,
     {"Station Count", "wlan_mgt.qbss.scount",
      FT_UINT16, BASE_DEC, NULL, 0, "", HFILL }},

    {&hf_qbss_cu,
     {"Channel Utilization", "wlan_mgt.qbss.cu",
       FT_UINT8, BASE_DEC, NULL, 0, "", HFILL }},

    {&hf_qbss_adc,
     {"Available Admission Capabilities", "wlan_mgt.qbss.adc",
     FT_UINT8, BASE_DEC, NULL, 0, "", HFILL }},

    {&hf_qbss2_cu,
     {"Channel Utilization", "wlan_mgt.qbss2.cu",
       FT_UINT8, BASE_DEC, NULL, 0, "", HFILL }},

    {&hf_qbss2_gl,
     {"G.711 CU Quantum", "wlan_mgt.qbss2.glimit",
      FT_UINT8, BASE_DEC, NULL, 0, "", HFILL }},

    {&hf_qbss2_cal,
     {"Call Admission Limit", "wlan_mgt.qbss2.cal",
      FT_UINT8, BASE_DEC, NULL, 0, "", HFILL }},

    {&hf_qbss2_scount,
     {"Station Count", "wlan_mgt.qbss2.scount",
      FT_UINT16, BASE_DEC, NULL, 0, "", HFILL }},

    {&hf_aironet_ie_qos_unk1,
     {"Aironet IE QoS unknown1", "wlan_mgt.aironet.qos.unk1",
      FT_UINT8, BASE_HEX, NULL, 0, "", HFILL }},

    {&hf_aironet_ie_qos_paramset,
     {"Aironet IE QoS paramset", "wlan_mgt.aironet.qos.paramset",
      FT_UINT8, BASE_DEC, NULL, 0, "", HFILL }},

    {&hf_aironet_ie_qos_val,
     {"Aironet IE QoS valueset", "wlan_mgt.aironet.qos.val",
      FT_BYTES, BASE_NONE, NULL, 0, "", HFILL }},

    {&hf_ts_info,
     {"TS Info", "wlan_mgt.ts_info",
      FT_UINT24, BASE_HEX, NULL, 0, "TS Info field", HFILL }},

    {&hf_tsinfo_type,
     {"Traffic Type", "wlan_mgt.ts_info.type", FT_UINT8, BASE_DEC,
      VALS (&tsinfo_type), 0, "TS Info Traffic Type", HFILL }},

    {&hf_tsinfo_tsid,
     {"TSID", "wlan_mgt.ts_info.tsid",
      FT_UINT8, BASE_DEC, NULL, 0, "TS Info TSID", HFILL }},

    {&hf_tsinfo_dir,
     {"Direction", "wlan_mgt.ts_info.dir", FT_UINT8, BASE_DEC,
      VALS (&tsinfo_direction), 0, "TS Info Direction", HFILL }},

    {&hf_tsinfo_access,
     {"Access Policy", "wlan_mgt.ts_info.dir", FT_UINT8, BASE_DEC,
      VALS (&tsinfo_access), 0, "TS Info Access Policy", HFILL }},

    {&hf_tsinfo_agg,
     {"Aggregation", "wlan_mgt.ts_info.agg", FT_UINT8, BASE_DEC,
      NULL, 0, "TS Info Access Policy", HFILL }},

    {&hf_tsinfo_apsd,
     {"APSD", "wlan_mgt.ts_info.apsd", FT_UINT8, BASE_DEC,
      NULL, 0, "TS Info APSD", HFILL }},

    {&hf_tsinfo_up,
     {"UP", "wlan_mgt.ts_info.up", FT_UINT8, BASE_DEC,
      VALS (&qos_up), 0, "TS Info User Priority", HFILL }},

    {&hf_tsinfo_ack,
     {"Ack Policy", "wlan_mgt.ts_info.ack", FT_UINT8, BASE_DEC,
      VALS (&ack_policy), 0, "TS Info Ack Policy", HFILL }},

    {&hf_tsinfo_sched,
     {"Schedule", "wlan_mgt.ts_info.sched", FT_UINT8, BASE_DEC,
      NULL, 0, "TS Info Schedule", HFILL }},

    {&tspec_nor_msdu,
     {"Normal MSDU Size", "wlan_mgt.tspec.nor_msdu",
      FT_UINT16, BASE_DEC, NULL, 0, "Normal MSDU Size", HFILL }},

    {&tspec_max_msdu,
     {"Maximum MSDU Size", "wlan_mgt.tspec.max_msdu",
      FT_UINT16, BASE_DEC, NULL, 0, "Maximum MSDU Size", HFILL }},

    {&tspec_min_srv,
     {"Minimum Service Interval", "wlan_mgt.tspec.min_srv",
      FT_UINT32, BASE_DEC, NULL, 0, "Minimum Service Interval", HFILL }},

    {&tspec_max_srv,
     {"Maximum Service Interval", "wlan_mgt.tspec.max_srv",
      FT_UINT32, BASE_DEC, NULL, 0, "Maximum Service Interval", HFILL }},

    {&tspec_inact_int,
     {"Inactivity Interval", "wlan_mgt.tspec.inact_int",
      FT_UINT32, BASE_DEC, NULL, 0, "Inactivity Interval", HFILL }},

    {&tspec_susp_int,
     {"Suspension Interval", "wlan_mgt.tspec.susp_int",
      FT_UINT32, BASE_DEC, NULL, 0, "Suspension Interval", HFILL }},

    {&tspec_srv_start,
     {"Service Start Time", "wlan_mgt.tspec.srv_start",
      FT_UINT32, BASE_DEC, NULL, 0, "Service Start Time", HFILL }},

    {&tspec_min_data,
     {"Minimum Data Rate", "wlan_mgt.tspec.min_data",
      FT_UINT32, BASE_DEC, NULL, 0, "Minimum Data Rate", HFILL }},

    {&tspec_mean_data,
     {"Mean Data Rate", "wlan_mgt.tspec.mean_data",
      FT_UINT32, BASE_DEC, NULL, 0, "Mean Data Rate", HFILL }},

    {&tspec_peak_data,
     {"Peak Data Rate", "wlan_mgt.tspec.peak_data",
      FT_UINT32, BASE_DEC, NULL, 0, "Peak Data Rate", HFILL }},

    {&tspec_burst_size,
     {"Burst Size", "wlan_mgt.tspec.burst_size",
      FT_UINT32, BASE_DEC, NULL, 0, "Burst Size", HFILL }},

    {&tspec_delay_bound,
     {"Delay Bound", "wlan_mgt.tspec.delay_bound",
      FT_UINT32, BASE_DEC, NULL, 0, "Delay Bound", HFILL }},

    {&tspec_min_phy,
     {"Minimum PHY Rate", "wlan_mgt.tspec.min_phy",
      FT_UINT32, BASE_DEC, NULL, 0, "Minimum PHY Rate", HFILL }},

    {&tspec_surplus,
     {"Surplus Bandwidth Allowance", "wlan_mgt.tspec.surplus",
      FT_UINT16, BASE_DEC, NULL, 0, "Surplus Bandwidth Allowance", HFILL }},

    {&tspec_medium,
     {"Medium Time", "wlan_mgt.tspec.medium",
      FT_UINT16, BASE_DEC, NULL, 0, "Medium Time", HFILL }},

    {&ts_delay,
     {"TS Delay", "wlan_mgt.ts_delay",
      FT_UINT32, BASE_DEC, NULL, 0, "TS Delay", HFILL }},

    {&hf_class_type,
     {"Classifier Type", "wlan_mgt.tclas.class_type", FT_UINT8, BASE_DEC,
      VALS (classifier_type), 0, "Classifier Type", HFILL }},

    {&hf_class_mask,
     {"Classifier Mask", "wlan_mgt.tclas.class_mask", FT_UINT8, BASE_HEX,
      NULL, 0, "Classifier Mask", HFILL }},

    {&hf_ether_type,
     {"Type", "wlan_mgt.tclas.params.type", FT_UINT8, BASE_DEC,
      NULL, 0, "Classifier Parameters Ethernet Type", HFILL }},

    {&hf_tclas_process,
     {"Processing", "wlan_mgt.tclas_proc.processing", FT_UINT8, BASE_DEC,
      VALS (tclas_process), 0, "TCLAS Porcessing", HFILL }},

    {&hf_sched_info,
     {"Schedule Info", "wlan_mgt.sched.sched_info",
      FT_UINT16, BASE_HEX, NULL, 0, "Schedule Info field", HFILL }},

    {&hf_sched_srv_start,
     {"Service Start Time", "wlan_mgt.sched.srv_start",
      FT_UINT32, BASE_HEX, NULL, 0, "Service Start Time", HFILL }},

    {&hf_sched_srv_int,
     {"Service Interval", "wlan_mgt.sched.srv_int",
      FT_UINT32, BASE_HEX, NULL, 0, "Service Interval", HFILL }},

    {&hf_sched_spec_int,
     {"Specification Interval", "wlan_mgt.sched.spec_int",
      FT_UINT16, BASE_HEX, NULL, 0, "Specification Interval", HFILL }},

    {&cf_version,
     {"IP Version", "wlan_mgt.tclas.params.version",
      FT_UINT8, BASE_DEC, NULL, 0, "IP Version", HFILL }},

    {&cf_ipv4_src,
     {"IPv4 Src Addr", "wlan_mgt.tclas.params.ipv4_src",
      FT_IPv4, BASE_NONE, NULL, 0, "IPv4 Src Addr", HFILL }},

    {&cf_ipv4_dst,
     {"IPv4 Dst Addr", "wlan_mgt.tclas.params.ipv4_dst",
      FT_IPv4, BASE_NONE, NULL, 0, "IPv4 Dst Addr", HFILL }},

    {&cf_src_port,
     {"Source Port", "wlan_mgt.tclas.params.src_port",
      FT_UINT16, BASE_DEC, NULL, 0, "Source Port", HFILL }},

    {&cf_dst_port,
     {"Destination Port", "wlan_mgt.tclas.params.dst_port",
      FT_UINT16, BASE_DEC, NULL, 0, "Destination Port", HFILL }},

    {&cf_dscp,
     {"DSCP", "wlan_mgt.tclas.params.dscp",
      FT_UINT8, BASE_HEX, NULL, 0, "IPv4 DSCP Field", HFILL }},

    {&cf_protocol,
     {"Protocol", "wlan_mgt.tclas.params.protocol",
      FT_UINT8, BASE_HEX, NULL, 0, "IPv4 Protocol", HFILL }},

    {&cf_ipv6_src,
     {"IPv6 Src Addr", "wlan_mgt.tclas.params.ipv6_src",
      FT_IPv6, BASE_NONE, NULL, 0, "IPv6 Src Addr", HFILL }},

    {&cf_ipv6_dst,
     {"IPv6 Dst Addr", "wlan_mgt.tclas.params.ipv6_dst",
      FT_IPv6, BASE_NONE, NULL, 0, "IPv6 Dst Addr", HFILL }},

    {&cf_flow,
     {"Flow Label", "wlan_mgt.tclas.params.flow",
      FT_UINT24, BASE_HEX, NULL, 0, "IPv6 Flow Label", HFILL }},

    {&cf_tag_type,
     {"802.1Q Tag Type", "wlan_mgt.tclas.params.tag_type",
      FT_UINT16, BASE_HEX, NULL, 0, "802.1Q Tag Type", HFILL }},

  };

  static gint *tree_array[] = {
    &ett_80211,
    &ett_fc_tree,
    &ett_proto_flags,
    &ett_fragments,
    &ett_fragment,
    &ett_80211_mgt,
    &ett_fixed_parameters,
    &ett_tagged_parameters,
    &ett_qos_parameters,
    &ett_qos_ps_buf_state,
    &ett_wep_parameters,
    &ett_cap_tree,
    &ett_rsn_cap_tree,
    &ett_80211_mgt_ie,
    &ett_tsinfo_tree,
    &ett_sched_tree,
  };
  module_t *wlan_module;


  proto_wlan = proto_register_protocol ("IEEE 802.11 wireless LAN",
					"IEEE 802.11", "wlan");
  proto_register_field_array (proto_wlan, hf, array_length (hf));
  proto_wlan_mgt = proto_register_protocol ("IEEE 802.11 wireless LAN management frame",
					"802.11 MGT", "wlan_mgt");
  proto_register_field_array (proto_wlan_mgt, ff, array_length (ff));
  proto_register_subtree_array (tree_array, array_length (tree_array));

  register_dissector("wlan", dissect_ieee80211, proto_wlan);
  register_dissector("wlan_fixed", dissect_ieee80211_fixed, proto_wlan);
  register_dissector("wlan_bsfc", dissect_ieee80211_bsfc, proto_wlan);
  register_dissector("wlan_datapad", dissect_ieee80211_datapad, proto_wlan);
  register_dissector("wlan_radio", dissect_ieee80211_radio, proto_wlan);
  register_init_routine(wlan_defragment_init);

  wlan_tap = register_tap("wlan");

  /
  wlan_module = prefs_register_protocol(proto_wlan, init_wepkeys);
  prefs_register_bool_preference(wlan_module, "defragment",
	"Reassemble fragmented 802.11 datagrams",
	"Whether fragmented 802.11 datagrams should be reassembled",
	&wlan_defragment);

  prefs_register_bool_preference(wlan_module, "check_fcs",
				 "Assume packets have FCS",
				 "Some 802.11 cards include the FCS at the end of a packet, others do not.",
				 &wlan_check_fcs);

	/
	/
  prefs_register_bool_preference(wlan_module, "ignore_wep",
		"Ignore the Protection bit",
		"Some 802.11 cards leave the Protection bit set even though the packet is decrypted.",
				 &wlan_ignore_wep);

#ifndef USE_ENV

  prefs_register_obsolete_preference(wlan_module, "wep_keys");

#ifdef HAVE_AIRPDCAP
  /
  prefs_register_bool_preference(wlan_module, "enable_decryption",
	"Enable decryption", "Enable WEP and WPA/WPA2 decryption",
	&enable_decryption);
#else
  prefs_register_bool_preference(wlan_module, "enable_decryption",
	"Enable decryption", "Enable WEP decryption",
	&enable_decryption);
#endif

#ifdef HAVE_AIRPDCAP
  prefs_register_static_text_preference(wlan_module, "info_decryption_key",
	  "Key examples: 01:02:03:04:05 (40/64-bit WEP),\n"
	  "010203040506070809101111213 (104/128-bit WEP),\n"
	  "wpa-pwd:MyPassword[:MyAP]> (WPA + plaintext password [+ SSID]),\n"
	  "wpa-psk:0102030405...6061626364 (WPA + 256-bit key).  "
	  "Invalid keys will be ignored.",
	  "This is just a static text");
#else
  prefs_register_static_text_preference(wlan_module, "info_decryption_key",
	  "Key examples: 01:02:03:04:05 (40/64-bit WEP),\n"
	  "010203040506070809101111213 (104/128-bit WEP)",
	  "This is just a static text");
#endif

  for (i = 0; i < MAX_ENCRYPTION_KEYS; i++) {
    key_name = g_string_new("");
    key_title = g_string_new("");
    key_desc = g_string_new("");
    wep_keystr[i] = NULL;
    /
#ifdef HAVE_AIRPDCAP
  g_string_sprintf(key_name, "wep_key%d", i + 1);
  g_string_sprintf(key_title, "Key #%d", i + 1);
  /
  g_string_sprintf(key_desc,
	"Key #%d string can be:"
	"   <wep hexadecimal key>;"
	"   WEP:<wep hexadecimal key>;"
	"   WPA-PWD:<passphrase>[:<ssid>];"
	"   WPA-PSK:<wpa hexadecimal key>", i + 1);
#else
    g_string_sprintf(key_name, "wep_key%d", i + 1);
    g_string_sprintf(key_title, "WEP key #%d", i + 1);
    g_string_sprintf(key_desc, "WEP key #%d bytes in hexadecimal (A:B:C:D:E) "
	    "[40bit], (A:B:C:D:E:F:G:H:I:J:K:L:M) [104bit], or whatever key "
	    "length you're using", i + 1);
#endif

    prefs_register_string_preference(wlan_module, key_name->str,
	    key_title->str, key_desc->str, &wep_keystr[i]);

    g_string_free(key_name, FALSE);
    g_string_free(key_title, FALSE);
    g_string_free(key_desc, FALSE);
  }
#endif
}
