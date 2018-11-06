void
proto_register_wlan (void)
{
  static const value_string frame_type[] = {
    {MGT_FRAME,     "Management frame"},
    {CONTROL_FRAME, "Control frame"},
    {DATA_FRAME,    "Data frame"},
    {0,             NULL}
  };

  static const value_string tofrom_ds[] = {
    {0,                       "Not leaving DS or network is operating in AD-HOC mode (To DS: 0  From DS: 0)"},
    {FLAG_TO_DS,              "Frame is entering DS (To DS: 1  From DS: 0)"},
    {FLAG_FROM_DS,            "Frame is exiting DS (To DS: 0  From DS: 1)"},
    {FLAG_TO_DS|FLAG_FROM_DS, "Frame part of WDS (To DS: 1  From DS: 1)"},
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

  static const true_false_string wep_flags = {
    "WEP is enabled",
    "WEP is disabled"
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


  static const true_false_string cf_ibss_flags = {
    "Transmitter belongs to an IBSS",
    "Transmitter belongs to a BSS"
  };

  static const value_string sta_cf_pollable[] = {
    {0x00, "Station is not CF-Pollable"},
    {0x02, "Station is CF-Pollable, "
     "not requesting to be placed on the  CF-polling list"},
    {0x01, "Station is CF-Pollable, "
     "requesting to be placed on the CF-polling list"},
    {0x03, "Station is CF-Pollable, requesting never to be polled"},
    {0, NULL}
  };

  static const value_string ap_cf_pollable[] = {
    {0x00, "No point coordinator at AP"},
    {0x02, "Point coordinator at AP for delivery only (no polling)"},
    {0x01, "Point coordinator at AP for delivery and polling"},
    {0x03, "Reserved"},
    {0, NULL}
  };


  static const value_string auth_alg[] = {
    {0x00, "Open System"},
    {0x01, "Shared key"},
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
    {0x00, NULL}
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

    {&hf_fc_wep,
     {"WEP flag", "wlan.fc.wep", FT_BOOLEAN, 8, TFS (&wep_flags), FLAG_WEP,
      "WEP flag", HFILL }},

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
       FT_NONE, BASE_NONE, NULL, 0x0,
       "Defragmentation error due to illegal fragments", HFILL }},

    {&hf_fragment,
      {"802.11 Fragment", "wlan.fragment", FT_NONE, BASE_NONE, NULL, 0x0,
       "802.11 Fragment", HFILL }},

    {&hf_fragments,
      {"802.11 Fragments", "wlan.fragments", FT_NONE, BASE_NONE, NULL, 0x0,
       "802.11 Fragments", HFILL }},

    {&hf_wep_iv,
     {"Initialization Vector", "wlan.wep.iv", FT_UINT24, BASE_HEX, NULL, 0,
      "Initialization Vector", HFILL }},

    {&hf_wep_key,
     {"Key", "wlan.wep.key", FT_UINT8, BASE_DEC, NULL, 0,
      "Key", HFILL }},

    {&hf_wep_icv,
     {"WEP ICV", "wlan.wep.icv", FT_UINT32, BASE_HEX, NULL, 0,
      "WEP ICV", HFILL }},
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

    {&ff_cf_sta_poll,
     {"CFP participation capabilities", "wlan_mgt.fixed.capabilities.cfpoll.sta",
      FT_UINT16, BASE_HEX, VALS (&sta_cf_pollable), 0,
      "CF-Poll capabilities for a STA", HFILL }},

    {&ff_cf_ap_poll,
     {"CFP participation capabilities", "wlan_mgt.fixed.capabilities.cfpoll.ap",
      FT_UINT16, BASE_HEX, VALS (&ap_cf_pollable), 0,
      "CF-Poll capabilities for an AP", HFILL }},

    {&ff_cf_ess,
     {"ESS capabilities", "wlan_mgt.fixed.capabilities.ess",
      FT_BOOLEAN, 8, TFS (&cf_ess_flags), 0x0001, "ESS capabilities", HFILL }},


    {&ff_cf_ibss,
     {"IBSS status", "wlan_mgt.fixed.capabilities.ibss",
      FT_BOOLEAN, 8, TFS (&cf_ibss_flags), 0x0002, "IBSS participation", HFILL }},
    {&ff_cf_privacy,
     {"Privacy", "wlan_mgt.fixed.capabilities.privacy",
      FT_BOOLEAN, 8, TFS (&cf_privacy_flags), 0x0010, "WEP support", HFILL }},

    {&ff_cf_preamble,
     {"Short Preamble", "wlan_mgt.fixed.capabilities.preamble",
      FT_BOOLEAN, 8, TFS (&cf_preamble_flags), 0x0020, "Short Preamble", HFILL }},

    {&ff_cf_pbcc,
     {"PBCC", "wlan_mgt.fixed.capabilities.pbcc",
      FT_BOOLEAN, 8, TFS (&cf_pbcc_flags), 0x0040, "PBCC Modulation", HFILL }},

    {&ff_cf_agility,
     {"Channel Agility", "wlan_mgt.fixed.capabilities.agility",
      FT_BOOLEAN, 8, TFS (&cf_agility_flags), 0x0080, "Channel Agility", HFILL }},

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

    {&tag_number,
     {"Tag", "wlan_mgt.tag.number",
      FT_UINT16, BASE_DEC, NULL, 0,
      "Element ID", HFILL }},

    {&tag_length,
     {"Tag length", "wlan_mgt.tag.length",
      FT_UINT16, BASE_DEC, NULL, 0, "Length of tag", HFILL }},

    {&tag_interpretation,
     {"Tag interpretation", "wlan_mgt.tag.interpretation",
      FT_STRING, BASE_NONE, NULL, 0, "Interpretation of tag", HFILL }}

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
    &ett_wep_parameters,
    &ett_cap_tree,
  };
  module_t *wlan_module;

  static const enum_val_t wep_keys_options[] = {
    {"0", 0},
    {"1", 1},
    {"2", 2},
    {"3", 3},
    {"4", 4},
    {NULL, -1},
  };


  proto_wlan = proto_register_protocol ("IEEE 802.11 wireless LAN",
					"IEEE 802.11", "wlan");
  proto_register_field_array (proto_wlan, hf, array_length (hf));
  proto_wlan_mgt = proto_register_protocol ("IEEE 802.11 wireless LAN management frame",
					"802.11 MGT", "wlan_mgt");
  proto_register_field_array (proto_wlan_mgt, ff, array_length (ff));
  proto_register_subtree_array (tree_array, array_length (tree_array));

  register_dissector("wlan", dissect_ieee80211, proto_wlan);
  register_dissector("wlan_fixed", dissect_ieee80211_fixed, proto_wlan);
  register_init_routine(wlan_defragment_init);

  /
  wlan_module = prefs_register_protocol(proto_wlan, NULL);
  prefs_register_bool_preference(wlan_module, "defragment",
	"Reassemble fragmented 802.11 datagrams",
	"Whether fragmented 802.11 datagrams should be reassembled",
	&wlan_defragment);

  prefs_register_bool_preference(wlan_module, "check_fcs",
				 "Assume packets have FCS",
				 "Some 802.11 cards include the FCS at the end of a packet, others do not.",
				 &wlan_check_fcs);

  prefs_register_bool_preference(wlan_module, "ignore_wep",
				 "Ignore the WEP bit",
				 "Some 802.11 cards leave the WEP bit set even though the packet is decrypted.",
				 &wlan_ignore_wep);

#ifndef USE_ENV
  prefs_register_enum_preference(wlan_module, "wep_keys",
				 "WEP key count",
				 "How many WEP keys do we have to choose from? (0 to disable, up to 4)",
				 &num_wepkeys, wep_keys_options, FALSE);

  prefs_register_string_preference(wlan_module, "wep_key1",
				   "WEP key #1",
				   "First WEP key (A:B:C:D:E:F)",
				   &wep_keystr[0]);
  prefs_register_string_preference(wlan_module, "wep_key2",
				   "WEP key #2",
				   "Second WEP key (A:B:C:D:E:F)",
				   &wep_keystr[1]);
  prefs_register_string_preference(wlan_module, "wep_key3",
				   "WEP key #3",
				   "Third WEP key (A:B:C:D:E:F)",
				   &wep_keystr[2]);
  prefs_register_string_preference(wlan_module, "wep_key4",
				   "WEP key #4",
				   "Fourth WEP key (A:B:C:D:E:F)",
				   &wep_keystr[3]);
#endif
}
