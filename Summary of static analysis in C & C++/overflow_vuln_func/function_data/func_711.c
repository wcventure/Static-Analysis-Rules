void proto_register_ieee802154(void)
{
    /
    static hf_register_info hf_phy[] = {
        /

        { &hf_ieee802154_nonask_phy_preamble,
        { "Preamble",                       "wpan-nonask-phy.preamble", FT_UINT32, BASE_HEX, NULL, 0x0,
            NULL, HFILL }},

        { &hf_ieee802154_nonask_phy_sfd,
        { "Start of Frame Delimiter",       "wpan-nonask-phy.sfd", FT_UINT8, BASE_HEX, NULL, 0x0,
            NULL, HFILL }},

        { &hf_ieee802154_nonask_phy_length,
        { "Frame Length",                   "wpan-nonask-phy.frame_length", FT_UINT8, BASE_HEX, NULL,
            IEEE802154_PHY_LENGTH_MASK, NULL, HFILL }},

        { &hf_ieee802154_nonask_phr,
        { "PHR",                   "wpan-nonask-phy.phr", FT_UINT8, BASE_HEX, NULL,
            0x0, NULL, HFILL }},
    };

    static hf_register_info hf[] = {

        { &hf_ieee802154_frame_length,
        { "Frame Length",                   "wpan.frame_length", FT_UINT8, BASE_DEC, NULL, 0x0,
            "Frame Length as reported from lower layer", HFILL }},

        { &hf_ieee802154_fcf,
        { "Frame Control Field",            "wpan.fcf", FT_UINT16, BASE_HEX, NULL,
            0x0, NULL, HFILL }},

        { &hf_ieee802154_frame_type,
        { "Frame Type",                     "wpan.frame_type", FT_UINT16, BASE_HEX, VALS(ieee802154_frame_types),
            IEEE802154_FCF_TYPE_MASK, NULL, HFILL }},

        { &hf_ieee802154_security,
        { "Security Enabled",               "wpan.security", FT_BOOLEAN, 16, NULL, IEEE802154_FCF_SEC_EN,
            "Whether security operations are performed at the MAC layer or not.", HFILL }},

        { &hf_ieee802154_pending,
        { "Frame Pending",                  "wpan.pending", FT_BOOLEAN, 16, NULL, IEEE802154_FCF_FRAME_PND,
            "Indication of additional packets waiting to be transferred from the source device.", HFILL }},

        { &hf_ieee802154_ack_request,
        { "Acknowledge Request",            "wpan.ack_request", FT_BOOLEAN, 16, NULL, IEEE802154_FCF_ACK_REQ,
            "Whether the sender of this packet requests acknowledgment or not.", HFILL }},

        { &hf_ieee802154_pan_id_compression,
        { "PAN ID Compression",             "wpan.pan_id_compression", FT_BOOLEAN, 16, NULL, IEEE802154_FCF_PAN_ID_COMPRESSION,
            "Whether this packet contains the PAN ID or not.", HFILL }},

        { &hf_ieee802154_seqno_suppression,
        { "Sequence Number Suppression",    "wpan.seqno_suppression", FT_BOOLEAN, 16, NULL, IEEE802154_FCF_SEQNO_SUPPRESSION,
            "Whether this packet contains the Sequence Number or not.", HFILL }},

        { &hf_ieee802154_ie_present,
        { "Information Elements Present",   "wpan.ie_present", FT_BOOLEAN, 16, NULL, IEEE802154_FCF_IE_PRESENT,
            "Whether this packet contains the Information Elements or not.", HFILL }},

        { &hf_ieee802154_seqno,
        { "Sequence Number",                "wpan.seq_no", FT_UINT8, BASE_DEC, NULL, 0x0,
            NULL, HFILL }},

        { &hf_ieee802154_dst_addr_mode,
        { "Destination Addressing Mode",    "wpan.dst_addr_mode", FT_UINT16, BASE_HEX, VALS(ieee802154_addr_modes),
            IEEE802154_FCF_DADDR_MASK, NULL, HFILL }},

        { &hf_ieee802154_src_addr_mode,
        { "Source Addressing Mode",         "wpan.src_addr_mode", FT_UINT16, BASE_HEX, VALS(ieee802154_addr_modes),
            IEEE802154_FCF_SADDR_MASK, NULL, HFILL }},

        { &hf_ieee802154_version,
        { "Frame Version",                  "wpan.version", FT_UINT16, BASE_DEC, VALS(ieee802154_frame_versions),
            IEEE802154_FCF_VERSION, NULL, HFILL }},

        { &hf_ieee802154_dst_panID,
        { "Destination PAN",                "wpan.dst_pan", FT_UINT16, BASE_HEX, NULL, 0x0,
            NULL, HFILL }},

        { &hf_ieee802154_dst16,
        { "Destination",                    "wpan.dst16", FT_UINT16, BASE_HEX, NULL, 0x0,
            NULL, HFILL }},

        { &hf_ieee802154_dst64,
        { "Destination",                    "wpan.dst64", FT_EUI64, BASE_NONE, NULL, 0x0,
            NULL, HFILL }},

        { &hf_ieee802154_src_panID,
        { "Source PAN",                     "wpan.src_pan", FT_UINT16, BASE_HEX, NULL, 0x0,
            NULL, HFILL }},

        { &hf_ieee802154_src16,
        { "Source",                         "wpan.src16", FT_UINT16, BASE_HEX, NULL, 0x0,
            NULL, HFILL }},

        { &hf_ieee802154_src64,
        { "Extended Source",                "wpan.src64", FT_EUI64, BASE_NONE, NULL, 0x0,
            NULL, HFILL }},

        { &hf_ieee802154_src64_origin,
        { "Origin",                           "wpan.src64.origin", FT_FRAMENUM, BASE_NONE, NULL, 0x0,
            NULL, HFILL }},

        { &hf_ieee802154_fcs,
        { "FCS",                            "wpan.fcs", FT_UINT16, BASE_HEX, NULL, 0x0,
            NULL, HFILL }},

        { &hf_ieee802154_rssi,
        { "RSSI",                           "wpan.rssi", FT_INT8, BASE_DEC|BASE_UNIT_STRING, &units_decibels, 0x0,
            "Received Signal Strength", HFILL }},

        { &hf_ieee802154_fcs_ok,
        { "FCS Valid",                      "wpan.fcs_ok", FT_BOOLEAN, BASE_NONE, NULL, 0x0,
            NULL, HFILL }},

        { &hf_ieee802154_correlation,
        { "LQI Correlation Value",          "wpan.correlation", FT_UINT8, BASE_DEC, NULL, 0x0,
            NULL, HFILL }},

        /

        { &hf_ieee802154_header_ie,
        { "Header IE",                      "wpan.header_ie", FT_NONE, BASE_NONE, NULL,
            0x0, NULL, HFILL }},

        { &hf_ieee802154_header_ie_type,
        { "Type",                           "wpan.header_ie.type", FT_UINT16, BASE_DEC, VALS(ieee802154_ie_types),
                IEEE802154_HEADER_IE_TYPE_MASK, NULL, HFILL }},

        { &hf_ieee802154_header_ie_id,
        { "Id",                             "wpan.header_ie.id", FT_UINT16, BASE_HEX, VALS(ieee802154_header_ie_names),
                IEEE802154_HEADER_IE_ID_MASK, NULL, HFILL }},

        { &hf_ieee802154_header_ie_length,
        { "Length",                         "wpan.header_ie.length", FT_UINT16, BASE_DEC, NULL,
                IEEE802154_HEADER_IE_LENGTH_MASK, NULL, HFILL }},

        { &hf_ieee802154_header_ie_tlv,
          { "IE Header",                    "wpan.header_ie_tlv", FT_UINT16, BASE_HEX, NULL, 0x0, NULL, HFILL}},

        { &hf_ieee802154_header_termination,
        { "Header Termination",        "wpan.header_ie_termination", FT_UINT16, BASE_HEX, NULL, 0x0, NULL, HFILL}},

        /

        { &hf_ieee802154_time_correction,
        { "Time correction",                "wpan.time_correction", FT_BYTES, BASE_NONE, NULL, 0x0, NULL, HFILL }},

        { &hf_ieee802154_nack,
        { "Non-acknowledgement",            "wpan.nack", FT_BOOLEAN, 16, NULL, 0x8000,
            NULL, HFILL }},

        { &hf_ieee802154_time_correction_value,
        { "Time correction",                "wpan.time_correction_value", FT_INT16, BASE_DEC, NULL, 0x0FFF,
            NULL, HFILL }},

        /

        { &hf_ieee802154_payload_ie,
        { "Payload IE",                     "wpan.payload_ie", FT_NONE, BASE_NONE, NULL, 0x0, NULL, HFILL }},

        { &hf_ieee802154_mlme,
        { "MLME IE",                        "wpan.mlme", FT_NONE, BASE_NONE, NULL, 0x0, NULL, HFILL }},

        { &hf_ieee802154_timeslot_ie,
        { "Timeslot IE",                    "wpan.timeslot", FT_NONE, BASE_NONE, NULL, 0x0, NULL, HFILL }},

        { &hf_ieee802154_enhanced_beacon_filter_ie,
        { "Enhanced Beacon Filter IE",      "wpan.enhanced_beacon_filter", FT_NONE, BASE_NONE, NULL, 0x0, NULL, HFILL }},

        { &hf_ieee802154_payload_ie_tlv,
        { "Payload IE TLV",                         "wpan.payload_ie_tlv", FT_UINT16, BASE_HEX, NULL,
            0x0, NULL, HFILL }},

        { &hf_ieee802154_payload_ie_type,
        { "Type",                           "wpan.payload_ie.type", FT_UINT16, BASE_DEC, VALS(ieee802154_ie_types),
                IEEE802154_PAYLOAD_IE_TYPE_MASK, NULL, HFILL }},

        { &hf_ieee802154_payload_ie_id,
        { "Id",                             "wpan.payload_ie.id", FT_UINT16, BASE_HEX, VALS(ieee802154_payload_ie_names),
                IEEE802154_PAYLOAD_IE_ID_MASK, NULL, HFILL }},

        { &hf_ieee802154_payload_ie_length,
        { "Length",                         "wpan.payload_ie.length", FT_UINT16, BASE_DEC, NULL,
                IEEE802154_PAYLOAD_IE_LENGTH_MASK, NULL, HFILL }},

        { &hf_ieee802154_psie_short,
        { "Payload Sub IE (short)",         "wpan.payload_sub_ie.short", FT_UINT16, BASE_HEX, NULL,
            0x0, NULL, HFILL }},

        { &hf_ieee802154_psie_type_short,
        { "Type",                           "wpan.payload_sub_ie.type_short", FT_UINT16, BASE_DEC, VALS(ieee802154_psie_types),
                IEEE802154_PSIE_TYPE_MASK, NULL, HFILL }},

        { &hf_ieee802154_psie_id_short,
        { "Sub Id (Short)",                 "wpan.payload_sub_ie.id_short", FT_UINT16, BASE_HEX, VALS(ieee802154_psie_names),
                IEEE802154_PSIE_ID_MASK_SHORT, NULL, HFILL }},

        { &hf_ieee802154_psie_length_short,
        { "Length",                         "wpan.payload_sub_ie.length_short", FT_UINT16, BASE_DEC, NULL,
                IEEE802154_PSIE_LENGTH_MASK_SHORT, NULL, HFILL }},

        { &hf_ieee802154_psie_long,
        { "Payload Sub IE (long)",         "wpan.payload_sub_ie.long", FT_UINT16, BASE_HEX, NULL,
            0x0, NULL, HFILL }},

        { &hf_ieee802154_psie_type_long,
        { "Type",                           "wpan.payload_sub_ie.type_long", FT_UINT16, BASE_DEC, VALS(ieee802154_psie_types),
                IEEE802154_PSIE_TYPE_MASK, NULL, HFILL }},

        { &hf_ieee802154_psie_id_long,
        { "Sub Id (Long)",                  "wpan.payload_sub_ie.id_long", FT_UINT16, BASE_HEX, VALS(ieee802154_psie_names),
                IEEE802154_PSIE_ID_MASK_LONG, NULL, HFILL }},

        { &hf_ieee802154_psie_length_long,
        { "Length",                         "wpan.payload_sub_ie.length_long", FT_UINT16, BASE_DEC, NULL,
                IEEE802154_PSIE_LENGTH_MASK_LONG, NULL, HFILL }},

        { &hf_ieee802154_psie_eb_filter,
        { "Enhanced Beacon Filter",         "wpan.payload_sub_ie.eb_filter", FT_UINT8, BASE_HEX, NULL,
              0, NULL, HFILL }},

        { &hf_ieee802154_tsch_sync,
          { "Time Synchronization IE",      "wpan.tsch.time_sync", FT_NONE, BASE_NONE, NULL, 0x0,
            NULL, HFILL }},

        { &hf_ieee802154_tsch_asn,
        { "Absolute Slot Number",                  "wpan.tsch.asn", FT_UINT40, BASE_DEC, NULL, 0x0,
            NULL, HFILL }},

        { &hf_ieee802154_tsch_join_metric,
        { "Join Metric",                           "wpan.tsch.join_metric", FT_UINT8, BASE_DEC, NULL, 0x0,
            NULL, HFILL }},

        { &hf_ieee802154_tsch_channel_hopping,
        { "Channel Hopping IE",                    "wpan.channel_hopping", FT_NONE, BASE_NONE, NULL, 0x0,
            NULL, HFILL }},

        { &hf_ieee802154_tsch_slotframe,
        { "Slotframe IE",   "wpan.tsch.slotframe", FT_NONE, BASE_NONE, NULL, 0x0, NULL, HFILL }},

        { &hf_ieee802154_tsch_slotframe_list,
        { "Slotframe info list",   "wpan.tsch.slotframe_list", FT_NONE, BASE_NONE, NULL, 0x0, NULL, HFILL }},

        { &hf_ieee802154_tsch_link_info,
        { "Link Information",   "wpan.tsch.link_info", FT_NONE, BASE_NONE, NULL, 0x0, NULL, HFILL }},

        { &hf_ieee802154_tsch_slotf_link_nb_slotf,
        { "Number of Slotframes",                   "wpan.tsch.slotframe_num", FT_UINT8, BASE_DEC, NULL, 0x0,
            NULL, HFILL }},

        { &hf_ieee802154_tsch_slotf_link_slotf_handle,
        { "Slotframe handle",                       "wpan.tsch.slotframe_handle", FT_UINT8, BASE_DEC, NULL, 0x0,
            NULL, HFILL }},

        { &hf_ieee802154_tsch_slotf_size,
        { "Slotframe size",                       "wpan.tsch.slotframe_size", FT_UINT16, BASE_DEC, NULL, 0x0,
            NULL, HFILL }},

        { &hf_ieee802154_tsch_slotf_link_nb_links,
        { "Number of Links",                       "wpan.tsch.nb_links", FT_UINT8, BASE_DEC, NULL, 0x0,
            NULL, HFILL }},

        { &hf_ieee802154_tsch_slotf_link_timeslot,
        { "Timeslot",                       "wpan.tsch.timeslot", FT_UINT16, BASE_DEC, NULL, 0x0,
            NULL, HFILL }},

        { &hf_ieee802154_tsch_slotf_link_channel_offset,
        { "Channel Offset",                       "wpan.tsch.channel_offset", FT_UINT16, BASE_DEC, NULL, 0x0,
            NULL, HFILL }},

        { &hf_ieee802154_tsch_slotf_link_options,
        { "Link Options",                       "wpan.tsch.link_options", FT_UINT8, BASE_DEC, NULL, 0x0,
            NULL, HFILL }},

        { &hf_ieee802154_tsch_hopping_sequence_id,
        { "Hopping Sequence ID",                   "wpan.tsch.hopping_sequence_id", FT_UINT8, BASE_HEX, NULL, 0x0,
            NULL, HFILL }},

        { &hf_ieee802154_psie_eb_filter_pjoin,
        { "Permit Join Filter",             "wpan.payload_sub_ie.eb_filter.pjoin", FT_BOOLEAN, 8, TFS(&tfs_enabled_disabled),
              IEEE802154_MLME_PSIE_EB_FLT_PJOIN, NULL, HFILL }},

        { &hf_ieee802154_psie_eb_filter_lqi,
        { "LQI Filter",                     "wpan.payload_sub_ie.eb_filter.lqi", FT_BOOLEAN, 8, TFS(&tfs_enabled_disabled),
            IEEE802154_MLME_PSIE_EB_FLT_LQI, NULL, HFILL }},

        { &hf_ieee802154_psie_eb_filter_lqi_min,
        { "Minimum LQI",                    "wpan.payload_sub_ie.eb_filter.lqi_minimum", FT_UINT8, BASE_DEC, NULL,
             0x0, NULL, HFILL }},

        { &hf_ieee802154_psie_eb_filter_percent,
        { "Probability to Respond",         "wpan.payload_sub_ie.eb_filter.contains_prob", FT_BOOLEAN, 8, TFS(&tfs_enabled_disabled),
            IEEE802154_MLME_PSIE_EB_FLT_PERCENT, NULL, HFILL }},

        { &hf_ieee802154_psie_eb_filter_percent_prob,
        { "Response Probability Percentage", "wpan.payload_sub_ie.eb_filter.prob", FT_UINT8, BASE_DEC, NULL,
                 0x0, NULL, HFILL }},

        { &hf_ieee802154_psie_eb_filter_attr_id,
        { "Requested Attribute Length",      "wpan.payload_sub_ie.eb_filter.attr_id", FT_UINT8, BASE_DEC, NULL,
            IEEE802154_MLME_PSIE_EB_FLT_ATTR_LEN, NULL, HFILL }},

        { &hf_ieee802154_psie_eb_filter_attr_id_bitmap,
        { "Attribute ID Bitmap",             "wpan.payload_sub_ie.eb_filter.attr_id_bits", FT_UINT24, BASE_HEX, NULL,
                0x0, NULL, HFILL }},

        { &hf_ieee802154_payload_ie_data,
        { "Data",                            "wpan.payload_ie.data", FT_BYTES, BASE_NONE, NULL, 0x0, NULL, HFILL }},

        { &hf_ieee802154_payload_ie_vendor_oui,
        { "Vendor OUI",                      "wpan.payload_ie.vendor_oui", FT_UINT24, BASE_HEX, NULL,
            0x0, NULL, HFILL }},

        { &hf_ieee802154_mlme_ie_data,
        { "Data",                            "wpan.mlme_sub_ie.data", FT_BYTES, BASE_NONE, NULL, 0x0, NULL, HFILL }},

        /

        { &hf_ieee802154_p_ie_ietf_sub_id,
        { "Sub-ID", "wpan.ietf_ie.sub_id", FT_UINT8, BASE_DEC, NULL, 0,
          NULL, HFILL }},

        /

        { &hf_ieee802154_6top_version,
        { "6P Version", "wpan.6top_version", FT_UINT8, BASE_DEC, NULL, IETF_6TOP_VERSION,
          NULL, HFILL }},

        { &hf_ieee802154_6top_type,
          { "Type", "wpan.6top_type", FT_UINT8, BASE_HEX, VALS(ietf_6top_types), IETF_6TOP_TYPE,
          NULL, HFILL }},

        { &hf_ieee802154_6top_flags_reserved,
        { "Reserved", "wpan.6top_flags_reserved", FT_UINT8, BASE_HEX, NULL, IETF_6TOP_FLAGS_RESERVED,
          NULL, HFILL }},

        { &hf_ieee802154_6top_code,
        { "Code",  "wpan.6top_code", FT_UINT8, BASE_HEX, NULL, 0x0,
          NULL, HFILL }},

        { &hf_ieee802154_6top_sfid,
        { "SFID (6top Scheduling Function ID)", "wpan.6top_sfid", FT_UINT8, BASE_HEX, NULL, 0x0,
          NULL, HFILL }},

        { &hf_ieee802154_6top_seqnum,
        { "SeqNum", "wpan.6top_seqnum", FT_UINT8, BASE_DEC, NULL, IETF_6TOP_SEQNUM,
          NULL, HFILL }},

        { &hf_ieee802154_6top_gab,
        { "GAB", "wpan.6top_gab", FT_UINT8, BASE_DEC, VALS(ietf_6top_generation_numbers), IETF_6TOP_GAB,
          NULL, HFILL }},

        { &hf_ieee802154_6top_gba,
        { "GBA", "wpan.6top_gba", FT_UINT8, BASE_DEC, VALS(ietf_6top_generation_numbers), IETF_6TOP_GBA,
          NULL, HFILL }},

        { &hf_ieee802154_6top_metadata,
        { "Metadata", "wpan.6top_metadata", FT_UINT16, BASE_HEX, NULL, 0x0,
          NULL, HFILL }},

        { &hf_ieee802154_6top_cell_options,
          { "Cell Options", "wpan.6top_cell_options", FT_UINT8, BASE_HEX, VALS(ietf_6top_cell_options), 0x0,
          NULL, HFILL }},

        { &hf_ieee802154_6top_cell_option_tx,
        { "Transmit (TX) Cell", "wpan.6top_cell_option_tx", FT_UINT8, BASE_HEX, NULL, IETF_6TOP_CELL_OPTION_TX,
          NULL, HFILL }},

        { &hf_ieee802154_6top_cell_option_rx,
        { "Receive (RX) Cell", "wpan.6top_cell_option_rx", FT_UINT8, BASE_HEX, NULL, IETF_6TOP_CELL_OPTION_RX,
          NULL, HFILL }},

        { &hf_ieee802154_6top_cell_option_shared,
        { "SHARED Cell", "wpan.6top_cell_option_shared", FT_UINT8, BASE_HEX, NULL, IETF_6TOP_CELL_OPTION_SHARED,
          NULL, HFILL }},

        { &hf_ieee802154_6top_cell_option_reserved,
        { "Reserved", "wpan.6top_cell_option_reserved", FT_UINT8, BASE_HEX, NULL, IETF_6TOP_CELL_OPTION_RESERVED,
          NULL, HFILL }},

        { &hf_ieee802154_6top_num_cells,
        { "Number of Cells", "wpan.6top_num_cells", FT_UINT16, BASE_DEC, NULL, 0x0,
          NULL, HFILL }},

        { &hf_ieee802154_6top_reserved,
        { "Reserved", "wpan.6top_reserved", FT_UINT8, BASE_HEX, NULL, 0x0,
          NULL, HFILL }},

        { &hf_ieee802154_6top_offset,
        { "Offset", "wpan.6top_offset", FT_UINT16, BASE_DEC, NULL, 0x0,
          NULL, HFILL }},

        { &hf_ieee802154_6top_max_num_cells,
        { "Maximum Number of Requested Cells", "wpan.6top_max_num_cells", FT_UINT16, BASE_DEC, NULL, 0x0,
          NULL, HFILL }},

        { &hf_ieee802154_6top_slot_offset,
        { "Slot Offset", "wpan.6top_cell_slot_offset", FT_UINT16, BASE_HEX, NULL, 0x0,
          NULL, HFILL }},

        { &hf_ieee802154_6top_channel_offset,
        { "Channel Offset", "wpan.6top_channel_offset", FT_UINT16, BASE_HEX, NULL, 0x0,
          NULL, HFILL }},

        /

        { &hf_ieee802154_cmd_id,
        { "Command Identifier",         "wpan.cmd", FT_UINT8, BASE_HEX, VALS(ieee802154_cmd_names), 0x0,
            NULL, HFILL }},

        /

        { &hf_ieee802154_cinfo_alt_coord,
        { "Alternate PAN Coordinator",  "wpan.cinfo.alt_coord", FT_BOOLEAN, 8, NULL, IEEE802154_CMD_CINFO_ALT_PAN_COORD,
            "Whether this device can act as a PAN coordinator or not.", HFILL }},

        { &hf_ieee802154_cinfo_device_type,
        { "Device Type",                "wpan.cinfo.device_type", FT_BOOLEAN, 8, TFS(&tfs_cinfo_device_type), IEEE802154_CMD_CINFO_DEVICE_TYPE,
            "Whether this device is RFD (reduced-function device) or FFD (full-function device).", HFILL }},

        { &hf_ieee802154_cinfo_power_src,
        { "Power Source",               "wpan.cinfo.power_src", FT_BOOLEAN, 8, TFS(&tfs_cinfo_power_src), IEEE802154_CMD_CINFO_POWER_SRC,
            "Whether this device is operating on AC/mains or battery power.", HFILL }},

        { &hf_ieee802154_cinfo_idle_rx,
        { "Receive On When Idle",       "wpan.cinfo.idle_rx", FT_BOOLEAN, 8, NULL, IEEE802154_CMD_CINFO_IDLE_RX,
            "Whether this device can receive packets while idle or not.", HFILL }},

        { &hf_ieee802154_cinfo_sec_capable,
        { "Security Capability",        "wpan.cinfo.sec_capable", FT_BOOLEAN, 8, NULL, IEEE802154_CMD_CINFO_SEC_CAPABLE,
            "Whether this device is capable of receiving encrypted packets.", HFILL }},

        { &hf_ieee802154_cinfo_alloc_addr,
        { "Allocate Address",           "wpan.cinfo.alloc_addr", FT_BOOLEAN, 8, NULL, IEEE802154_CMD_CINFO_ALLOC_ADDR,
            "Whether this device wishes to use a 16-bit short address instead of its IEEE 802.15.4 64-bit long address.", HFILL }},

        /

        { &hf_ieee802154_assoc_addr,
        { "Short Address",              "wpan.asoc.addr", FT_UINT16, BASE_HEX, NULL, 0x0,
            "The short address that the device should assume. An address of 0xfffe indicates that the device should use its IEEE 64-bit long address.", HFILL }},

        { &hf_ieee802154_assoc_status,
        { "Association Status",         "wpan.assoc.status", FT_UINT8, BASE_HEX, NULL, 0x0,
            NULL, HFILL }},

        { &hf_ieee802154_disassoc_reason,
        { "Disassociation Reason",      "wpan.disassoc.reason", FT_UINT8, BASE_HEX, NULL, 0x0,
            NULL, HFILL }},

        /

        { &hf_ieee802154_realign_pan,
        { "PAN ID",                     "wpan.realign.pan", FT_UINT16, BASE_HEX, NULL, 0x0,
            "The PAN identifier the coordinator wishes to use for future communication.", HFILL }},

        { &hf_ieee802154_realign_caddr,
        { "Coordinator Short Address",  "wpan.realign.addr", FT_UINT16, BASE_HEX, NULL, 0x0,
            "The 16-bit address the coordinator wishes to use for future communication.", HFILL }},

        { &hf_ieee802154_realign_channel,
        { "Logical Channel",            "wpan.realign.channel", FT_UINT8, BASE_DEC, NULL, 0x0,
            "The logical channel the coordinator wishes to use for future communication.", HFILL }},

        { &hf_ieee802154_realign_addr,
        { "Short Address",              "wpan.realign.addr", FT_UINT16, BASE_HEX, NULL, 0x0,
            "A short-address that the orphaned device shall assume if applicable.", HFILL }},

        { &hf_ieee802154_realign_channel_page,
        { "Channel Page",               "wpan.realign.channel_page", FT_UINT8, BASE_DEC, NULL, 0x0,
            "The logical channel page the coordinator wishes to use for future communication.", HFILL }},

        { &hf_ieee802154_gtsreq_len,
        { "GTS Length",                 "wpan.gtsreq.length", FT_UINT8, BASE_DEC, NULL, IEEE802154_CMD_GTS_REQ_LEN,
            "Number of superframe slots the device is requesting.", HFILL }},

        { &hf_ieee802154_gtsreq_dir,
        { "GTS Direction",              "wpan.gtsreq.direction", FT_BOOLEAN, 8, TFS(&tfs_gtsreq_dir), IEEE802154_CMD_GTS_REQ_DIR,
            "The direction of traffic in the guaranteed timeslot.", HFILL }},

        { &hf_ieee802154_gtsreq_type,
        { "Characteristic Type",        "wpan.gtsreq.type", FT_BOOLEAN, 8, TFS(&tfs_gtsreq_type), IEEE802154_CMD_GTS_REQ_TYPE,
            "Whether this request is to allocate or deallocate a timeslot.", HFILL }},

        /

        { &hf_ieee802154_beacon_order,
        { "Beacon Interval",            "wpan.beacon_order", FT_UINT16, BASE_DEC, NULL, IEEE802154_BEACON_ORDER_MASK,
            "Specifies the transmission interval of the beacons.", HFILL }},

        { &hf_ieee802154_superframe_order,
        { "Superframe Interval",        "wpan.superframe_order", FT_UINT16, BASE_DEC, NULL,
            IEEE802154_SUPERFRAME_ORDER_MASK,
            "Specifies the length of time the coordinator will interact with the PAN.", HFILL }},

        { &hf_ieee802154_cap,
        { "Final CAP Slot",             "wpan.cap", FT_UINT16, BASE_DEC, NULL, IEEE802154_SUPERFRAME_CAP_MASK,
            "Specifies the final superframe slot used by the CAP.", HFILL }},

        { &hf_ieee802154_superframe_battery_ext,
        { "Battery Extension",          "wpan.battery_ext", FT_BOOLEAN, 16, NULL, IEEE802154_BATT_EXTENSION_MASK,
            "Whether transmissions may not extend past the length of the beacon frame.", HFILL }},

        { &hf_ieee802154_superframe_coord,
        { "PAN Coordinator",            "wpan.bcn_coord", FT_BOOLEAN, 16, NULL, IEEE802154_SUPERFRAME_COORD_MASK,
            "Whether this beacon frame is being transmitted by the PAN coordinator or not.", HFILL }},

        { &hf_ieee802154_assoc_permit,
        { "Association Permit",         "wpan.assoc_permit", FT_BOOLEAN, 16, NULL, IEEE802154_ASSOC_PERMIT_MASK,
            "Whether this PAN is accepting association requests or not.", HFILL }},

        { &hf_ieee802154_gts_count,
        { "GTS Descriptor Count",       "wpan.gts.count", FT_UINT8, BASE_DEC, NULL, 0x0,
            "The number of GTS descriptors present in this beacon frame.", HFILL }},

        { &hf_ieee802154_gts_permit,
        { "GTS Permit",                 "wpan.gts.permit", FT_BOOLEAN, BASE_NONE, NULL, 0x0,
            "Whether the PAN coordinator is accepting GTS requests or not.", HFILL }},

        { &hf_ieee802154_gts_direction,
        { "Direction",                  "wpan.gts.direction", FT_BOOLEAN, BASE_NONE, TFS(&ieee802154_gts_direction_tfs), 0x0,
            "A flag defining the direction of the GTS Slot.", HFILL }},

        { &hf_ieee802154_gts_address,
        { "Address",                  "wpan.gts.address", FT_UINT16, BASE_HEX, NULL, 0x0,
            NULL, HFILL }},

        { &hf_ieee802154_pending16,
        { "Address",                    "wpan.pending16", FT_UINT16, BASE_HEX, NULL, 0x0,
            "Device with pending data to receive.", HFILL }},

        { &hf_ieee802154_pending64,
        { "Address",                    "wpan.pending64", FT_EUI64, BASE_NONE, NULL, 0x0,
            "Device with pending data to receive.", HFILL }},

        /
        { &hf_ieee802154_aux_security_header,
        { "Auxiliary Security Header", "wpan.aux_sec.hdr", FT_NONE, BASE_NONE, NULL,
            0x0, "The Auxiliary Security Header of the frame", HFILL }},

        { &hf_ieee802154_aux_sec_security_level,
        { "Security Level", "wpan.aux_sec.sec_level", FT_UINT8, BASE_HEX, VALS(ieee802154_sec_level_names),
            IEEE802154_AUX_SEC_LEVEL_MASK, "The Security Level of the frame", HFILL }},

        { &hf_ieee802154_aux_sec_security_control,
        { "Security Control Field", "wpan.aux_sec.security_control_field", FT_UINT8, BASE_HEX, NULL,
            0x0, NULL, HFILL }},

        { &hf_ieee802154_aux_sec_key_id_mode,
        { "Key Identifier Mode", "wpan.aux_sec.key_id_mode", FT_UINT8, BASE_HEX, VALS(ieee802154_key_id_mode_names),
            IEEE802154_AUX_KEY_ID_MODE_MASK,
            "The scheme to use by the recipient to lookup the key in its key table", HFILL }},

        { &hf_ieee802154_aux_sec_frame_counter_suppression,
        { "Frame Counter Suppression", "wpan.aux_sec.frame_counter_suppression", FT_BOOLEAN, 8, NULL,
            IEEE802154_AUX_FRAME_COUNTER_SUPPRESSION_MASK,
            "Whether the frame counter is omitted from the Auxiliary Security Header", HFILL }},

        { &hf_ieee802154_aux_sec_asn_in_nonce,
        { "ASN in Nonce", "wpan.aux_sec.asn_in_nonce", FT_BOOLEAN, 8, NULL,
            IEEE802154_AUX_ASN_IN_NONCE_MASK,
            "Whether the ASN is used to generate the nonce instead of the frame counter", HFILL }},

        { &hf_ieee802154_aux_sec_reserved,
        { "Reserved", "wpan.aux_sec.reserved", FT_UINT8, BASE_HEX, NULL, IEEE802154_AUX_CTRL_RESERVED_MASK,
            NULL, HFILL }},

        { &hf_ieee802154_aux_sec_frame_counter,
        { "Frame Counter", "wpan.aux_sec.frame_counter", FT_UINT32, BASE_DEC, NULL, 0x0,
            "Frame counter of the originator of the protected frame", HFILL }},

        { &hf_ieee802154_aux_sec_key_source,
        { "Key Source", "wpan.aux_sec.key_source", FT_UINT64, BASE_HEX, NULL, 0x0,
            "Key Source for processing of the protected frame", HFILL }},

        { &hf_ieee802154_aux_sec_key_source_bytes,
        { "Key Source", "wpan.aux_sec.key_source.bytes", FT_BYTES, BASE_NONE, NULL, 0x0,
            "Key Source for processing of the protected frame", HFILL }},

        { &hf_ieee802154_aux_sec_key_index,
        { "Key Index", "wpan.aux_sec.key_index", FT_UINT8, BASE_HEX, NULL, 0x0,
            "Key Index for processing of the protected frame", HFILL }},

        { &hf_ieee802154_mic,
        { "Decrypted MIC", "wpan.mic", FT_BYTES, BASE_NONE, NULL, 0x0,
            "The Decrypted MIC", HFILL }},

        { &hf_ieee802154_key_number,
        { "Key Number", "wpan.key_number", FT_UINT8, BASE_DEC, NULL, 0x0,
            "Key number used to decode", HFILL }},

        /
        { &hf_ieee802154_sec_frame_counter,
        { "Frame Counter", "wpan.sec_frame_counter", FT_UINT32, BASE_HEX, NULL, 0x0,
            "Frame counter of the originator of the protected frame (802.15.4-2003)", HFILL }},

        { &hf_ieee802154_sec_key_sequence_counter,
        { "Key Sequence Counter", "wpan.sec_key_sequence_counter", FT_UINT8, BASE_HEX, NULL, 0x0,
            "Key Sequence counter of the originator of the protected frame (802.15.4-2003)", HFILL }},

        /

        { &hf_zboss_page,
        { "Page", "wpan.zboss.page", FT_UINT8, BASE_DEC_HEX, VALS(zboss_page_names), 0xFE,
            "IEEE802.15.4 page number", HFILL } },

        { &hf_zboss_channel,
        { "Channel", "wpan.zboss.channel", FT_UINT8, BASE_DEC, NULL, 0x0,
            "Channel number", HFILL }},

        { &hf_zboss_direction,
        { "ZBOSS Direction", "wpan.zboss.direction", FT_UINT8, BASE_HEX, VALS(zboss_direction_names), 0x01,
            "ZBOSS Packet Direction", HFILL }},

        { &hf_zboss_trace_number,
        { "Trace number", "wpan.zboss.trace", FT_UINT32, BASE_DEC, NULL, 0x0,
            "Trace item number", HFILL }},
    };

    /
    static gint *ett[] = {
        &ett_ieee802154_nonask_phy,
        &ett_ieee802154_nonask_phy_phr,
        &ett_ieee802154,
        &ett_ieee802154_fcf,
        &ett_ieee802154_auxiliary_security,
        &ett_ieee802154_aux_sec_control,
        &ett_ieee802154_aux_sec_key_id,
        &ett_ieee802154_fcs,
        &ett_ieee802154_cmd,
        &ett_ieee802154_superframe,
        &ett_ieee802154_gts,
        &ett_ieee802154_gts_direction,
        &ett_ieee802154_gts_descriptors,
        &ett_ieee802154_pendaddr,
        &ett_ieee802154_header,
        &ett_ieee802154_header_ie,
        &ett_ieee802154_h_ie_payload,
        &ett_ieee802154_payload,
        &ett_ieee802154_payload_ie,
        &ett_ieee802154_tsch_timeslot,
        &ett_ieee802154_tsch_synch,
        &ett_ieee802154_channel_hopping,
        &ett_ieee802154_mlme,
        &ett_ieee802154_mlme_payload,
        &ett_ieee802154_mlme_payload_data,
        &ett_ieee802154_psie_short,
        &ett_ieee802154_psie_short_bitmap,
        &ett_ieee802154_psie_long,
        &ett_ieee802154_psie_long_bitmap,
        &ett_ieee802154_psie_enh_beacon_flt,
        &ett_ieee802154_psie_enh_beacon_flt_bitmap,
        &ett_ieee802154_psie_slotframe_link_slotframes,
        &ett_ieee802154_zigbee,
        &ett_ieee802154_zboss,
        &ett_ieee802154_p_ie_6top,
        &ett_ieee802154_p_ie_6top_version_type,
        &ett_ieee802154_p_ie_6top_seqnum_gab_gba,
        &ett_ieee802154_p_ie_6top_cell_options,
        &ett_ieee802154_p_ie_6top_cell_list,
        &ett_ieee802154_p_ie_6top_cell
    };

    static ei_register_info ei[] = {
        { &ei_ieee802154_invalid_addressing, { "wpan.invalid_addressing", PI_MALFORMED, PI_WARN,
                "Invalid Addressing", EXPFILL }},
#if 0
        { &ei_ieee802154_invalid_panid_compression, { "wpan.invalid_panid_compression", PI_MALFORMED, PI_ERROR,
                "Invalid Setting for PAN ID Compression", EXPFILL }},
#endif
        { &ei_ieee802154_invalid_panid_compression2, { "wpan.seqno_supression_fv2_invalid",  PI_MALFORMED, PI_WARN,
                "Invalid Pan ID Compression and addressing combination for Frame Version 2", EXPFILL }},
        { &ei_ieee802154_dst, { "wpan.dst_invalid", PI_MALFORMED, PI_ERROR,
                "Invalid Destination Address Mode", EXPFILL }},
        { &ei_ieee802154_src, { "wpan.src_invalid", PI_MALFORMED, PI_ERROR,
                "Invalid Source Address Mode", EXPFILL }},
        { &ei_ieee802154_frame_ver,  { "wpan.frame_version_unknown", PI_MALFORMED, PI_ERROR,
                "Frame Version Unknown Cannot Dissect", EXPFILL }},
#if 0
        { &ei_ieee802154_frame_type, { "wpan.frame_type_unknown", PI_MALFORMED, PI_ERROR,
                "Frame Type Unknown Cannot Dissect", EXPFILL }},
#endif
        { &ei_ieee802154_decrypt_error, { "wpan.decrypt_error", PI_UNDECODED, PI_WARN,
                "Decryption error", EXPFILL }},
        { &ei_ieee802154_fcs, { "wpan.fcs.bad", PI_CHECKSUM, PI_WARN,
                "Bad FCS", EXPFILL }},
        { &ei_ieee802154_seqno_suppression, { "wpan.seqno_supression_invalid",  PI_MALFORMED, PI_WARN,
                "Sequence Number Suppression invalid for 802.15.4-2003 and 2006", EXPFILL }},
        { &ei_ieee802154_6top_unsupported_type, { "wpan.6top_unsupported_type", PI_PROTOCOL, PI_WARN,
                "Unsupported Type of Message", EXPFILL }},
        { &ei_ieee802154_6top_unsupported_command, { "wpan.6top_unsupported_command", PI_PROTOCOL, PI_WARN,
                "Unsupported 6Top command", EXPFILL }},
        { &ei_ieee802154_time_correction_error, { "wpan.time_correction.error", PI_PROTOCOL, PI_WARN,
                "Incorrect value. Reference: IEEE-802.15.4-2015. Table 7-8: Values of the Time Sync Info field for ACK with timing Information", EXPFILL}},
        { &ei_ieee802154_6top_unsupported_return_code, { "wpan.6top_unsupported_code", PI_PROTOCOL, PI_WARN,
                "Unsupported 6Top return code", EXPFILL }},
        { &ei_ieee802154_unsupported_element_id, { "wpan.header_ie_unsupported_element_id", PI_PROTOCOL, PI_WARN,
                                                         "Unsupported Header IE element ID", EXPFILL }},
    };

    /
    module_t *ieee802154_module;
    expert_module_t* expert_ieee802154;

    static uat_field_t addr_uat_flds[] = {
        UAT_FLD_HEX(addr_uat,addr16,"Short Address",
                "16-bit short address in hexadecimal."),
        UAT_FLD_HEX(addr_uat,pan,"PAN Identifier",
                "16-bit PAN identifier in hexadecimal."),
        UAT_FLD_BUFFER(addr_uat,eui64,"EUI-64",
                "64-bit extended unique identifier."),
        UAT_END_FIELDS
    };

    static uat_field_t key_uat_flds[] = {
        UAT_FLD_CSTRING(key_uat,pref_key,"Decryption key",
                "128-bit decryption key in hexadecimal format"),
        UAT_FLD_DEC(key_uat,key_index,"Decryption key index",
                "Key index in decimal format"),
        UAT_FLD_VS(key_uat, hash_type, "Key hash", ieee802154_key_hash_vals, "Specifies which hash scheme is used to derived the key"),
        UAT_END_FIELDS
    };

    static build_valid_func     ieee802154_da_build_value[1] = {ieee802154_da_value};
    static decode_as_value_t    ieee802154_da_values = {ieee802154_da_prompt, 1, ieee802154_da_build_value};
    static decode_as_t          ieee802154_da = {
        IEEE802154_PROTOABBREV_WPAN, "PAN", IEEE802154_PROTOABBREV_WPAN_PANID,
        1, 0, &ieee802154_da_values, NULL, NULL,
        decode_as_default_populate_list, decode_as_default_reset, decode_as_default_change, NULL
    };

    /
    register_init_routine(proto_init_ieee802154);
    register_cleanup_routine(proto_cleanup_ieee802154);

    /
    proto_ieee802154 = proto_register_protocol("IEEE 802.15.4 Low-Rate Wireless PAN", "IEEE 802.15.4",
           IEEE802154_PROTOABBREV_WPAN);
    proto_ieee802154_nonask_phy = proto_register_protocol("IEEE 802.15.4 Low-Rate Wireless PAN non-ASK PHY",
            "IEEE 802.15.4 non-ASK PHY", "wpan-nonask-phy");
    proto_zboss = proto_register_protocol("ZBOSS IEEE 802.15.4 dump",
                                          "ZBOSS dump", "wpan-zboss");

    /
    proto_register_field_array(proto_ieee802154, hf, array_length(hf));
    proto_register_field_array(proto_ieee802154, hf_phy, array_length(hf_phy));

    proto_register_subtree_array(ett, array_length(ett));

    expert_ieee802154 = expert_register_protocol(proto_ieee802154);
    expert_register_field_array(expert_ieee802154, ei, array_length(ei));

    ieee802_15_4_short_address_type = address_type_dissector_register("AT_IEEE_802_15_4_SHORT", "IEEE 802.15.4 16-bit short address",
                                        ieee802_15_4_short_address_to_str, ieee802_15_4_short_address_str_len, NULL, NULL, ieee802_15_4_short_address_len, NULL, NULL);

    /
    ieee802154_module = prefs_register_protocol(proto_ieee802154,
                                   proto_reg_handoff_ieee802154);
    prefs_register_uint_preference(ieee802154_module, "802154_ethertype",
                                   "802.15.4 Ethertype (in hex)",
                                   "(Hexadecimal) Ethertype used to indicate IEEE 802.15.4 frame.",
                                   16, &ieee802154_ethertype);
    prefs_register_bool_preference(ieee802154_module, "802154_cc24xx",
                                   "TI CC24xx FCS format",
                                   "Set if the FCS field is in TI CC24xx format.",
                                   &ieee802154_cc24xx);
    prefs_register_bool_preference(ieee802154_module, "802154_fcs_ok",
                                   "Dissect only good FCS",
                                   "Dissect payload only if FCS is valid.",
                                   &ieee802154_fcs_ok);

    /
    static_addr_uat = uat_new("Static Addresses",
            sizeof(static_addr_t),      /
            "802154_addresses",         /
            TRUE,                       /
            &static_addrs,              /
            &num_static_addrs,          /
            UAT_AFFECTS_DISSECTION,     /
            NULL,                       /
            NULL,                       /
            addr_uat_update_cb,         /
            NULL,                       /
            NULL,                       /
            NULL,                       /
            addr_uat_flds);             /
    prefs_register_uat_preference(ieee802154_module, "static_addr",
                "Static Addresses",
                "A table of static address mappings between 16-bit short addressing and EUI-64 addresses",
                static_addr_uat);

    /
    ieee802154_key_uat = uat_new("Keys",
            sizeof(ieee802154_key_t),   /
            "ieee802154_keys",          /
            TRUE,                       /
            &ieee802154_keys,           /
            &num_ieee802154_keys,       /
            UAT_AFFECTS_DISSECTION,     /
            NULL,                       /
            ieee802154_key_copy_cb,     /
            ieee802154_key_update_cb,   /
            ieee802154_key_free_cb,     /
            ieee802154_key_post_update_cb, /
            NULL,                       /
            key_uat_flds);              /
    prefs_register_uat_preference(ieee802154_module, "ieee802154_keys",
                "Decryption Keys",
                "Decryption key configuration data",
                ieee802154_key_uat);

    /
    prefs_register_obsolete_preference(ieee802154_module, "802154_key");

    prefs_register_enum_preference(ieee802154_module, "802154_sec_suite",
                                   "Security Suite (802.15.4-2003)",
                                   "Specifies the security suite to use for 802.15.4-2003 secured frames"
                                   " (only supported suites are listed). Option ignored for 802.15.4-2006"
                                   " and unsecured frames.",
                                   &ieee802154_sec_suite, ieee802154_2003_sec_suite_enums, FALSE);

    prefs_register_bool_preference(ieee802154_module, "802154_extend_auth",
                                   "Extend authentication data (802.15.4-2003)",
                                   "Set if the manufacturer extends the authentication data with the"
                                   " security header. Option ignored for 802.15.4-2006 and unsecured frames.",
                                   &ieee802154_extend_auth);

    /
    panid_dissector_table = register_dissector_table(IEEE802154_PROTOABBREV_WPAN_PANID, "IEEE 802.15.4 PANID", proto_ieee802154, FT_UINT16, BASE_HEX);
    ieee802154_heur_subdissector_list = register_heur_dissector_list(IEEE802154_PROTOABBREV_WPAN, proto_ieee802154);
    ieee802154_beacon_subdissector_list = register_heur_dissector_list(IEEE802154_PROTOABBREV_WPAN_BEACON, proto_ieee802154);

    /
    ieee802154_handle = register_dissector(IEEE802154_PROTOABBREV_WPAN, dissect_ieee802154, proto_ieee802154);
    ieee802154_nofcs_handle = register_dissector("wpan_nofcs", dissect_ieee802154_nofcs, proto_ieee802154);
    register_dissector("wpan_cc24xx", dissect_ieee802154_cc24xx, proto_ieee802154);
    ieee802154_nonask_phy_handle = register_dissector("wpan-nonask-phy", dissect_ieee802154_nonask_phy, proto_ieee802154_nonask_phy);

    /
    mac_key_hash_handlers = wmem_tree_new(wmem_epan_scope());

    /
    register_decode_as(&ieee802154_da);
} /
