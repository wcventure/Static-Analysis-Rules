void
proto_register_skinny(void)
{

  /
  static hf_register_info hf[] = {
    { &hf_skinny_data_length,
      { "Data length", "skinny.data_length",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        "Number of bytes in the data portion.",
        HFILL }
    },
    { &hf_skinny_hdr_version,
      { "Header version", "skinny.hdr_version",
        FT_UINT32, BASE_HEX, VALS(header_version), 0x0,
        NULL,
        HFILL }
    },
    /
    { &hf_skinny_messageid,
      { "Message ID", "skinny.messageid",
        FT_UINT32, BASE_HEX|BASE_EXT_STRING, &message_id_ext, 0x0,
        "The function requested/done with this message.",
        HFILL }
    },

    { &hf_skinny_deviceName,
      { "Device name", "skinny.deviceName",
        FT_STRING, BASE_NONE, NULL, 0x0,
        "The device name of the phone.",
        HFILL }
    },

    { &hf_skinny_stationUserId,
      { "Station user ID", "skinny.stationUserId",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_stationInstance,
      { "Station instance", "skinny.stationInstance",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_deviceType,
      { "Device type", "skinny.deviceType",
        FT_UINT32, BASE_DEC|BASE_EXT_STRING, &deviceTypes_ext, 0x0,
        "DeviceType of the station.",
        HFILL }
    },

    { &hf_skinny_maxStreams,
      { "Max streams", "skinny.maxStreams",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        "32 bit unsigned integer indicating the maximum number of simultansous RTP duplex streams that the client can handle.",
        HFILL }
    },

    { &hf_skinny_activeStreams,
      { "Active Streams", "skinny.activeStreams",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_phoneFeatures,
      { "Phone Features", "skinny.phoneFeatures",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_maxButtons,
      { "Maximum number of Buttons", "skinny.maxButtons",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_macAddress,
      { "Mac Address", "skinny.macAddress",
        FT_STRING, BASE_NONE, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_loadInfo,
      { "Load Information / Firmware", "skinny.loadInfo",
        FT_STRING, BASE_NONE, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_stationIpPort,
      { "Station ip port", "skinny.stationIpPort",
        FT_UINT16, BASE_DEC, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_stationKeypadButton,
      { "Keypad button", "skinny.stationKeypadButton",
        FT_UINT32, BASE_HEX|BASE_EXT_STRING, &keypadButtons_ext, 0x0,
        "The button pressed on the phone.",
        HFILL }
    },

    { &hf_skinny_calledPartyNumber,
      { "Called party number", "skinny.calledParty",
        FT_STRING, BASE_NONE, NULL, 0x0,
        "The number called.",
        HFILL }
    },

    { &hf_skinny_stimulus,
      { "Stimulus", "skinny.stimulus",
        FT_UINT32, BASE_HEX|BASE_EXT_STRING, &deviceStimuli_ext, 0x0,
        "Reason for the device stimulus message.",
        HFILL }
    },

    { &hf_skinny_stimulusInstance,
      { "Stimulus instance", "skinny.stimulusInstance",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_lineNumber,
      { "Line number", "skinny.lineNumber",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_speedDialNumber,
      { "Speed-dial number", "skinny.speedDialNumber",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        "Which speed dial number",
        HFILL }
    },

    { &hf_skinny_capCount,
      { "Capabilities count", "skinny.capCount",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        "How many capabilities",
        HFILL }
    },

    { &hf_skinny_payloadCapability,
      { "Payload capability", "skinny.payloadCapability",
        FT_UINT32, BASE_DEC|BASE_EXT_STRING, &mediaPayloads_ext, 0x0,
        "The payload capability for this media capability structure.",
        HFILL }
    },

    { &hf_skinny_maxFramesPerPacket,
      { "Max frames per packet", "skinny.maxFramesPerPacket",
        FT_UINT16, BASE_DEC, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_alarmSeverity,
      { "Alarm severity", "skinny.alarmSeverity",
        FT_UINT32, BASE_DEC|BASE_EXT_STRING, &alarmSeverities_ext, 0x0,
        "The severity of the reported alarm.",
        HFILL }
    },

    { &hf_skinny_alarmParam1,
      { "Alarm param 1", "skinny.alarmParam1",
        FT_UINT32, BASE_HEX, NULL, 0x0,
        "An as yet undecoded param1 value from the alarm message",
        HFILL }
    },

    { &hf_skinny_alarmParam2,
      { "Alarm param 2", "skinny.alarmParam2",
        FT_IPv4, BASE_NONE, NULL, 0x0,
        "This is the second alarm parameter i think it's an ip address",
        HFILL }
    },

    { &hf_skinny_receptionStatus,
      { "Reception status", "skinny.receptionStatus",
        FT_UINT32, BASE_DEC, VALS(multicastMediaReceptionStatus), 0x0,
        "The current status of the multicast media.",
        HFILL }
    },

    { &hf_skinny_passThruPartyID,
      { "Pass-thru party ID", "skinny.passThruPartyID",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_ORCStatus,
      { "Opened receive-channel status", "skinny.openReceiveChannelStatus",
        FT_UINT32, BASE_DEC, VALS(openReceiveChanStatus), 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_IPVersion,
      { "IP Version", "skinny.ipversion",
        FT_UINT32, BASE_DEC, VALS(ipVersion), 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_ipAddress,
      { "IP address", "skinny.ipAddress",
        FT_IPv4, BASE_NONE, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_ipV6Address,
      { "IPv6 address", "skinny.ipv6Address",
        FT_IPv6, BASE_NONE, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_portNumber,
      { "Port number", "skinny.portNumber",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_statsProcessingType,
      { "Stats processing type", "skinny.statsProcessingType",
        FT_UINT32, BASE_DEC, VALS(statsProcessingTypes), 0x0,
        "What do do after you send the stats.",
        HFILL }
    },

    { &hf_skinny_callIdentifier,
      { "Call identifier", "skinny.callIdentifier",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_visibility,
      { "Visibility", "skinny.visibility",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_packetsSent,
      { "Packets sent", "skinny.packetsSent",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_octetsSent,
      { "Octets sent", "skinny.octetsSent",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_packetsRecv,
      { "Packets Received", "skinny.packetsRecv",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_octetsRecv,
      { "Octets received", "skinny.octetsRecv",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_packetsLost,
      { "Packets lost", "skinny.packetsLost",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_latency,
      { "Latency(ms)", "skinny.latency",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        "Average packet latency during the call.",
        HFILL }
    },

    { &hf_skinny_jitter,
      { "Jitter", "skinny.jitter",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        "Average jitter during the call.",
        HFILL }
    },

    { &hf_skinny_directoryNumber,
      { "Directory number", "skinny.directoryNumber",
        FT_STRING, BASE_NONE, NULL, 0x0,
        "The number we are reporting statistics for.",
        HFILL }
    },

    { &hf_skinny_lineInstance,
      { "Line instance", "skinny.lineInstance",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        "The display call plane associated with this call.",
        HFILL }
    },

    { &hf_skinny_softKeyEvent,
      { "Soft-key event", "skinny.softKeyEvent",
        FT_UINT32, BASE_DEC|BASE_EXT_STRING, &softKeyEvents_ext, 0x0,
        "Which softkey event is being reported.",
        HFILL }
    },

    { &hf_skinny_keepAliveInterval,
      { "Keep-alive interval", "skinny.keepAliveInterval",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        "How often are keep alives exchanges between the client and the call manager.",
        HFILL }
    },

    { &hf_skinny_secondaryKeepAliveInterval,
      { "Secondary keep-alive interval", "skinny.secondaryKeepAliveInterval",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        "How often are keep alives exchanges between the client and the secondary call manager.",
        HFILL }
    },

    { &hf_skinny_dateTemplate,
      { "Date template", "skinny.dateTemplate",
        FT_STRING, BASE_NONE, NULL, 0x0,
        "The display format for the date/time on the phone.",
        HFILL }
    },

    { &hf_skinny_buttonOffset,
      { "Button offset", "skinny.buttonOffset",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        "Offset is the number of the first button referenced by this message.",
        HFILL }
    },

    { &hf_skinny_buttonCount,
      { "Buttons count", "skinny.buttonCount",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        "Number of (VALID) button definitions in this message.",
        HFILL }
    },

    { &hf_skinny_totalButtonCount,
      { "Total buttons count", "skinny.totalButtonCount",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        "The total number of buttons defined for this phone.",
        HFILL }
    },

    { &hf_skinny_buttonInstanceNumber,
      { "Instance number", "skinny.buttonInstanceNumber",
        FT_UINT8, BASE_HEX|BASE_EXT_STRING, &keypadButtons_ext, 0x0,
        "The button instance number for a button or the StationKeyPad value, repeats allowed.",
        HFILL }
    },

    { &hf_skinny_buttonDefinition,
      { "Button definition", "skinny.buttonDefinition",
        FT_UINT8, BASE_HEX|BASE_EXT_STRING, &buttonDefinitions_ext, 0x0,
        "The button type for this instance (ie line, speed dial, ....",
        HFILL }
    },

    { &hf_skinny_softKeyOffset,
      { "Soft-Key offset", "skinny.softKeyOffset",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        "The offset for the first soft key in this message.",
        HFILL }
    },

    { &hf_skinny_softKeyCount,
      { "Soft-keys count", "skinny.softKeyCount",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        "The number of valid softkeys in this message.",
        HFILL }
    },

    { &hf_skinny_totalSoftKeyCount,
      { "Total soft-keys count", "skinny.totalSoftKeyCount",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        "The total number of softkeys for this device.",
        HFILL }
    },

    { &hf_skinny_softKeyLabel,
      { "Soft-key label", "skinny.softKeyLabel",
        FT_STRING, BASE_NONE, NULL, 0x0,
        "The text label for this soft key.",
        HFILL }
    },

    { &hf_skinny_softKeySetOffset,
      { "Soft-key-set offset", "skinny.softKeySetOffset",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        "The offset for the first soft key set in this message.",
        HFILL }
    },

    { &hf_skinny_softKeySetCount,
      { "Soft-key-sets count", "skinny.softKeySetCount",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        "The number of valid softkey sets in this message.",
        HFILL }
    },

    { &hf_skinny_totalSoftKeySetCount,
      { "Total soft-key-sets count", "skinny.totalSoftKeySetCount",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        "The total number of softkey sets for this device.",
        HFILL }
    },

    { &hf_skinny_softKeyTemplateIndex,
      { "Soft-key template index", "skinny.softKeyTemplateIndex",
        FT_UINT8, BASE_DEC|BASE_EXT_STRING, &softKeyEvents_ext, 0x0,
        "Array of size 16 8-bit unsigned ints containing an index into the softKeyTemplate.",
        HFILL }
    },

    { &hf_skinny_softKeyInfoIndex,
      { "Soft-key info index", "skinny.softKeyInfoIndex",
        FT_UINT16, BASE_DEC|BASE_EXT_STRING, &softKeyIndexes_ext, 0x0,
        "Array of size 16 16-bit unsigned integers containing an index into the soft key description information.",
        HFILL }
    },

    { &hf_skinny_softKeySetDescription,
      { "Soft-key set description", "skinny.softKeySetDescription",
        FT_UINT8, BASE_DEC|BASE_EXT_STRING, &keySetNames_ext, 0x0,
        "A text description of what this softkey when this softkey set is displayed",
        HFILL }
    },

    { &hf_skinny_softKeyMap,
      { "Soft-key map","skinny.softKeyMap",
        FT_UINT16, BASE_HEX, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_softKey0,
      { "SoftKey0", "skinny.softKeyMap.0",
        FT_BOOLEAN, 16, TFS(&softKeyMapValues), SKINNY_SOFTKEY0,
        NULL,
        HFILL }
    },

    { &hf_skinny_softKey1,
      { "SoftKey1", "skinny.softKeyMap.1",
        FT_BOOLEAN, 16, TFS(&softKeyMapValues), SKINNY_SOFTKEY1,
        NULL,
        HFILL }
    },

    { &hf_skinny_softKey2,
      { "SoftKey2", "skinny.softKeyMap.2",
        FT_BOOLEAN, 16, TFS(&softKeyMapValues), SKINNY_SOFTKEY2,
        NULL,
        HFILL }
    },

    { &hf_skinny_softKey3,
      { "SoftKey3", "skinny.softKeyMap.3",
        FT_BOOLEAN, 16, TFS(&softKeyMapValues), SKINNY_SOFTKEY3,
        NULL,
        HFILL }
    },

    { &hf_skinny_softKey4,
      { "SoftKey4", "skinny.softKeyMap.4",
        FT_BOOLEAN, 16, TFS(&softKeyMapValues), SKINNY_SOFTKEY4,
        NULL,
        HFILL }
    },

    { &hf_skinny_softKey5,
      { "SoftKey5", "skinny.softKeyMap.5",
        FT_BOOLEAN, 16, TFS(&softKeyMapValues), SKINNY_SOFTKEY5,
        NULL,
        HFILL }
    },

    { &hf_skinny_softKey6,
      { "SoftKey6", "skinny.softKeyMap.6",
        FT_BOOLEAN, 16, TFS(&softKeyMapValues), SKINNY_SOFTKEY6,
        NULL,
        HFILL }
    },

    { &hf_skinny_softKey7,
      { "SoftKey7", "skinny.softKeyMap.7",
        FT_BOOLEAN, 16, TFS(&softKeyMapValues), SKINNY_SOFTKEY7,
        NULL,
        HFILL }
    },

    { &hf_skinny_softKey8,
      { "SoftKey8", "skinny.softKeyMap.8",
        FT_BOOLEAN, 16, TFS(&softKeyMapValues), SKINNY_SOFTKEY8,
        NULL,
        HFILL }
    },

    { &hf_skinny_softKey9,
      { "SoftKey9", "skinny.softKeyMap.9",
        FT_BOOLEAN, 16, TFS(&softKeyMapValues), SKINNY_SOFTKEY9,
        NULL,
        HFILL }
    },

    { &hf_skinny_softKey10,
      { "SoftKey10", "skinny.softKeyMap.10",
        FT_BOOLEAN, 16, TFS(&softKeyMapValues), SKINNY_SOFTKEY10,
        NULL,
        HFILL }
    },

    { &hf_skinny_softKey11,
      { "SoftKey11", "skinny.softKeyMap.11",
        FT_BOOLEAN, 16, TFS(&softKeyMapValues), SKINNY_SOFTKEY11,
        NULL,
        HFILL }
    },

    { &hf_skinny_softKey12,
      { "SoftKey12", "skinny.softKeyMap.12",
        FT_BOOLEAN, 16, TFS(&softKeyMapValues), SKINNY_SOFTKEY12,
        NULL,
        HFILL }
    },

    { &hf_skinny_softKey13,
      { "SoftKey13", "skinny.softKeyMap.13",
        FT_BOOLEAN, 16, TFS(&softKeyMapValues), SKINNY_SOFTKEY13,
        NULL,
        HFILL }
    },

    { &hf_skinny_softKey14,
      { "SoftKey14", "skinny.softKeyMap.14",
        FT_BOOLEAN, 16, TFS(&softKeyMapValues), SKINNY_SOFTKEY14,
        NULL,
        HFILL }
    },

    { &hf_skinny_softKey15,
      { "SoftKey15", "skinny.softKeyMap.15",
        FT_BOOLEAN, 16, TFS(&softKeyMapValues), SKINNY_SOFTKEY15,
        NULL,
        HFILL }
    },

    { &hf_skinny_lampMode,
      { "Lamp mode", "skinny.lampMode",
        FT_UINT32, BASE_DEC|BASE_EXT_STRING, &stationLampModes_ext, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_messageTimeOutValue,
      { "Message time-out", "skinny.messageTimeOutValue",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        "The timeout in seconds for this message",
        HFILL }
    },

    { &hf_skinny_displayMessage,
      { "Display message", "skinny.displayMessage",
        FT_STRING, BASE_NONE, NULL, 0x0,
        "The message displayed on the phone.",
        HFILL }
    },

    { &hf_skinny_lineDirNumber,
      { "Line directory number", "skinny.lineDirNumber",
        FT_STRING, BASE_NONE, NULL, 0x0,
        "The directory number for this line.",
        HFILL }
    },

    { &hf_skinny_lineFullyQualifiedDisplayName,
      { "Fully qualified display name", "skinny.fqdn",
        FT_STRING, BASE_NONE, NULL, 0x0,
        "The full display name for this line.",
        HFILL }
    },

    { &hf_skinny_lineDisplayName,
      { "Display name", "skinny.displayName",
        FT_STRING, BASE_NONE, NULL, 0x0,
        "The display name for this line.",
        HFILL }
    },

    { &hf_skinny_speedDialDirNumber,
      { "Speed-dial number", "skinny.speedDialDirNum",
        FT_STRING, BASE_NONE, NULL, 0x0,
        "the number to dial for this speed dial.",
        HFILL }
    },

    { &hf_skinny_speedDialDisplayName,
      { "Speed-dial display", "skinny.speedDialDisplay",
        FT_STRING, BASE_NONE, NULL, 0x0,
        "The text to display for this speed dial.",
        HFILL }
    },

    { &hf_skinny_dateYear,
      { "Year", "skinny.year",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        "The current year",
        HFILL }
    },

    { &hf_skinny_dateMonth,
      { "Month", "skinny.month",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        "The current month",
        HFILL }
    },

    { &hf_skinny_dayOfWeek,
      { "Day of week", "skinny.dayOfWeek",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        "The day of the week",
        HFILL }
    },

    { &hf_skinny_dateDay,
      { "Day", "skinny.day",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        "The day of the current month",
        HFILL }
    },

    { &hf_skinny_dateHour,
      { "Hour", "skinny.hour",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        "Hour of the day",
        HFILL }
    },

    { &hf_skinny_dateMinute,
      { "Minute", "skinny.minute",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_dateSeconds,
      { "Seconds", "skinny.dateSeconds",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_dateMilliseconds,
      { "Milliseconds", "skinny.dateMilliseconds",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_timeStamp,
      { "Timestamp", "skinny.timeStamp",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        "Time stamp for the call reference",
        HFILL }
    },
    { &hf_skinny_callState,
      { "Call state", "skinny.callState",
        FT_UINT32, BASE_DEC|BASE_EXT_STRING, &skinny_stationCallStates_ext, 0x0,
        "The D channel call state of the call",
        HFILL }
    },

    { &hf_skinny_deviceTone,
      { "Tone", "skinny.deviceTone",
        FT_UINT32, BASE_HEX|BASE_EXT_STRING, &skinny_deviceTones_ext, 0x0,
        "Which tone to play",
        HFILL }
    },

    { &hf_skinny_callingPartyName,
      { "Calling party name", "skinny.callingPartyName",
        FT_STRING, BASE_NONE, NULL, 0x0,
        "The passed name of the calling party.",
        HFILL }
    },

    { &hf_skinny_callingPartyNumber,
      { "Calling party number", "skinny.callingParty",
        FT_STRING, BASE_NONE, NULL, 0x0,
        "The passed number of the calling party.",
        HFILL }
    },

    { &hf_skinny_calledPartyName,
      { "Called party name", "skinny.calledPartyName",
        FT_STRING, BASE_NONE, NULL, 0x0,
        "The name of the party we are calling.",
        HFILL }
    },

    { &hf_skinny_callType,
      { "Call type", "skinny.callType",
        FT_UINT32, BASE_DEC, VALS(skinny_callTypes), 0x0,
        "What type of call, in/out/etc",
        HFILL }
    },

    { &hf_skinny_originalCalledPartyName,
      { "Original called party name", "skinny.originalCalledPartyName",
        FT_STRING, BASE_NONE, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_originalCalledParty,
      { "Original called party number", "skinny.originalCalledParty",
        FT_STRING, BASE_NONE, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_ringType,
      { "Ring type", "skinny.ringType",
        FT_UINT32, BASE_HEX|BASE_EXT_STRING, &skinny_ringTypes_ext, 0x0,
        "What type of ring to play",
        HFILL }
    },

    { &hf_skinny_ringMode,
      { "Ring mode", "skinny.ringMode",
        FT_UINT32, BASE_HEX, VALS(skinny_ringModes), 0x0,
        "What mode of ring to play",
        HFILL }
    },

    { &hf_skinny_speakerMode,
      { "Speaker", "skinny.speakerMode",
        FT_UINT32, BASE_HEX, VALS(skinny_speakerModes), 0x0,
        "This message sets the speaker mode on/off",
        HFILL }
    },

    { &hf_skinny_remoteIpAddr,
      { "Remote IP address", "skinny.remoteIpAddr",
        FT_IPv4, BASE_NONE, NULL, 0x0,
        "The remote end ip address for this stream",
        HFILL }
    },

    { &hf_skinny_remotePortNumber,
      { "Remote port", "skinny.remotePortNumber",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        "The remote port number listening for this stream",
        HFILL }
    },

    { &hf_skinny_millisecondPacketSize,
      { "MS/packet", "skinny.millisecondPacketSize",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        "The number of milliseconds of conversation in each packet",
        HFILL }
    },

    { &hf_skinny_precedenceValue,
      { "Precedence", "skinny.precedenceValue",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_silenceSuppression,
      { "Silence suppression", "skinny.silenceSuppression",
        FT_UINT32, BASE_HEX, VALS(skinny_silenceSuppressionModes), 0x0,
        "Mode for silence suppression",
        HFILL }
    },

    { &hf_skinny_g723BitRate,
      { "G723 bitrate", "skinny.g723BitRate",
        FT_UINT32, BASE_DEC, VALS(skinny_g723BitRates), 0x0,
        "The G723 bit rate for this stream/JUNK if not g723 stream",
        HFILL }
    },

    { &hf_skinny_conferenceID,
      { "Conference ID", "skinny.conferenceID",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_deviceResetType,
      { "Reset type", "skinny.deviceResetType",
        FT_UINT32, BASE_DEC, VALS(skinny_deviceResetTypes), 0x0,
        "How the devices it to be reset (reset/restart)",
        HFILL }
    },

    { &hf_skinny_echoCancelType,
      { "Echo-cancel type", "skinny.echoCancelType",
        FT_UINT32, BASE_DEC, VALS(skinny_echoCancelTypes), 0x0,
        "Is echo cancelling enabled or not",
        HFILL }
    },

    { &hf_skinny_deviceUnregisterStatus,
      { "Unregister status", "skinny.deviceUnregisterStatus",
        FT_UINT32, BASE_DEC, VALS(skinny_deviceUnregisterStatusTypes), 0x0,
        "The status of the device unregister request (*CAN* be refused)",
        HFILL }
    },

    { &hf_skinny_hookFlashDetectMode,
      { "Hook flash mode", "skinny.hookFlashDetectMode",
        FT_UINT32, BASE_DEC, VALS(skinny_hookFlashDetectModes), 0x0,
        "Which method to use to detect that a hook flash has occurred",
        HFILL }
    },

    { &hf_skinny_detectInterval,
      { "HF Detect Interval", "skinny.detectInterval",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        "The number of milliseconds that determines a hook flash has occurred",
        HFILL }
    },

    { &hf_skinny_headsetMode,
      { "Headset mode", "skinny.headsetMode",
        FT_UINT32, BASE_DEC, VALS(skinny_headsetModes), 0x0,
        "Turns on and off the headset on the set",
        HFILL }
    },

    { &hf_skinny_microphoneMode,
      { "Microphone mode", "skinny.microphoneMode",
        FT_UINT32, BASE_DEC, VALS(skinny_microphoneModes), 0x0,
        "Turns on and off the microphone on the set",
        HFILL }
    },

    { &hf_skinny_activeForward,
      { "Active forward", "skinny.activeForward",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        "This is non zero to indicate that a forward is active on the line",
        HFILL }
    },

    { &hf_skinny_forwardAllActive,
      { "Forward all", "skinny.forwardAllActive",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        "Forward all calls",
        HFILL }
    },

    { &hf_skinny_forwardBusyActive,
      { "Forward busy", "skinny.forwardBusyActive",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        "Forward calls when busy",
        HFILL }
    },

    { &hf_skinny_forwardNoAnswerActive,
      { "Forward no answer", "skinny.forwardNoAnswerActive",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        "Forward only when no answer",
        HFILL }
    },

    { &hf_skinny_forwardNumber,
      { "Forward number", "skinny.forwardNumber",
        FT_STRING, BASE_NONE, NULL, 0x0,
        "The number to forward calls to.",
        HFILL }
    },

    { &hf_skinny_userName,
      { "Username", "skinny.userName",
        FT_STRING, BASE_NONE, NULL, 0x0,
        "Username for this device.",
        HFILL }
    },

    { &hf_skinny_serverName,
      { "Server name", "skinny.serverName",
        FT_STRING, BASE_NONE, NULL, 0x0,
        "The server name for this device.",
        HFILL }
    },

    { &hf_skinny_numberLines,
      { "Number of lines", "skinny.numberLines",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        "How many lines this device has",
        HFILL }
    },

    { &hf_skinny_numberSpeedDials,
      { "Number of speed-dials", "skinny.numberSpeedDials",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        "The number of speed dials this device has",
        HFILL }
    },

    { &hf_skinny_sessionType,
      { "Session type", "skinny.sessionType",
        FT_UINT32, BASE_DEC|BASE_EXT_STRING, &skinny_sessionTypes_ext, 0x0,
        "The type of this session.",
        HFILL }
    },

    { &hf_skinny_version,
      { "Version", "skinny.version",
        FT_STRING, BASE_NONE, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_mediaEnunciationType,
      { "Enunciation type", "skinny.mediaEnunciationType",
        FT_UINT32, BASE_DEC, VALS(skinny_mediaEnunciationTypes), 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_serverIdentifier,
      { "Server identifier", "skinny.serverIdentifier",
        FT_STRING, BASE_NONE, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_serverListenPort,
      { "Server port", "skinny.serverListenPort",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_serverIpAddress,
      { "Server IP address", "skinny.serverIpAddress",
        FT_IPv4, BASE_NONE, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_multicastPort,
      { "Multicast port", "skinny.multicastPort",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_multicastIpAddress,
      { "Multicast IP address", "skinny.multicastIpAddress",
        FT_IPv4, BASE_NONE, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_tokenRejWaitTime,
      { "Retry wait time", "skinny.tokenRejWaitTime",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_unknown,
      { "Unknown data", "skinny.unknown",
        FT_UINT32, BASE_HEX, NULL, 0x0,
        "Place holder for unknown data.",
        HFILL }
    },

    { &hf_skinny_rawData,
      { "Unknown raw data", "skinny.rawData",
        FT_BYTES, BASE_NONE, NULL, 0x0,
        "Place holder for unknown raw data.",
        HFILL }
    },

    { &hf_skinny_xmlData,
      { "XML data", "skinny.xmlData",
        FT_STRING, BASE_NONE, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_numberOfInServiceStreams,
      { "Number of in-service streams", "skinny.numberOfInServiceStreams",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_maxStreamsPerConf,
      { "Max streams per conf", "skinny.maxStreamsPerConf",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_numberOfOutOfServiceStreams,
      { "Number of out-of-service streams", "skinny.numberOfOutOfServiceStreams",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_applicationID,
      { "Application ID", "skinny.applicationID",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        "Application ID.",
        HFILL }
    },

    { &hf_skinny_transactionID,
      { "Transaction ID", "skinny.transactionID",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_serviceNum,
      { "Service number", "skinny.serviceNum",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_serviceURLIndex,
      { "Service URL index", "skinny.serviceURLIndex",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_featureIndex,
      { "Feature index", "skinny.featureIndex",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_createConfResults,
      { "Create conf results", "skinny.createConfResults",
        FT_UINT32, BASE_DEC, VALS(skinny_createConfResults), 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_modifyConfResults,
      { "Modify conf results", "skinny.modifyConfResults",
        FT_UINT32, BASE_DEC|BASE_EXT_STRING, &skinny_modifyConfResults_ext, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_deleteConfResults,
      { "Delete conf results", "skinny.deleteConfResults",
        FT_UINT32, BASE_DEC, VALS(skinny_deleteConfResults), 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_addParticipantResults,
      { "Add participant results", "skinny.addParticipantResults",
        FT_UINT32, BASE_DEC|BASE_EXT_STRING, &skinny_addParticipantResults_ext, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_passThruData,
      { "Pass-thru data", "skinny.passThruData",
        FT_UINT8, BASE_HEX, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_auditParticipantResults,
      { "Audit participant results", "skinny.auditParticipantResults",
        FT_UINT32, BASE_DEC, VALS(skinny_auditParticipantResults), 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_last,
      { "Last", "skinny.last",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_numberOfEntries,
      { "Number of entries", "skinny.numberOfEntries",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_participantEntry,
      { "Participant entry", "skinny.participantEntry",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_resourceTypes,
      { "ResourceType", "skinny.resourceTypes",
        FT_UINT32, BASE_DEC, VALS(skinny_resourceTypes), 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_numberOfReservedParticipants,
      { "Number of reserved participants", "skinny.numberOfReservedParticipants",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_numberOfActiveParticipants,
      { "Number of active participants", "skinny.numberOfActiveParticipants",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_appID,
      { "Application ID", "skinny.appID",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_appData,
      { "Application data", "skinny.appData",
        FT_UINT8, BASE_HEX, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_appConfID,
      { "Application conf ID", "skinny.appConfID",
        FT_UINT8, BASE_HEX, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_sequenceFlag,
      { "Sequence flag", "skinny.sequenceFlag",
        FT_UINT32, BASE_DEC, VALS(skinny_sequenceFlags), 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_displayPriority,
      { "Display priority", "skinny.displayPriority",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_appInstanceID,
      { "Application instance ID", "skinny.appInstanceID",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_routingID,
      { "Routing ID", "skinny.routingID",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_audioCapCount,
      { "Audio cap count", "skinny.audioCapCount",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_videoCapCount,
      { "Video cap count", "skinny.videoCapCount",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_dataCapCount,
      { "Data cap count", "skinny.dataCapCount",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_RTPPayloadFormat,
      { "RTP payload format", "skinny.RTPPayloadFormat",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_customPictureFormatCount,
      { "Custom picture format count", "skinny.customPictureFormatCount",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_pictureWidth,
      { "Picture width", "skinny.pictureWidth",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_pictureHeight,
      { "Picture height", "skinny.pictureHeight",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_pixelAspectRatio,
      { "Pixel aspect ratio", "skinny.pixelAspectRatio",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_clockConversionCode,
      { "Clock conversion code", "skinny.clockConversionCode",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_clockDivisor,
      { "Clock divisor", "skinny.clockDivisor",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_activeStreamsOnRegistration,
      { "Active streams on registration", "skinny.activeStreamsOnRegistration",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_maxBW,
      { "Max BW", "skinny.maxBW",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_serviceResourceCount,
      { "Service resource count", "skinny.serviceResourceCount",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_layoutCount,
      { "Layout count", "skinny.layoutCount",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_layout,
      { "Layout", "skinny.layout",
        FT_UINT32, BASE_DEC|BASE_EXT_STRING, &skinny_Layouts_ext, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_maxConferences,
      { "Max conferences", "skinny.maxConferences",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_activeConferenceOnRegistration,
      { "Active conference on registration", "skinny.activeConferenceOnRegistration",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_transmitOrReceive,
      { "Transmit or receive", "skinny.transmitOrReceive",
        FT_UINT32, BASE_DEC, VALS(skinny_transmitOrReceive), 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_levelPreferenceCount,
      { "Level preference count", "skinny.levelPreferenceCount",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_transmitPreference,
      { "Transmit preference", "skinny.transmitPreference",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_format,
      { "Format", "skinny.format",
        FT_UINT32, BASE_DEC|BASE_EXT_STRING, &skinny_formatTypes_ext, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_maxBitRate,
      { "Max bitrate", "skinny.maxBitRate",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_minBitRate,
      { "Min bitrate", "skinny.minBitRate",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_MPI,
      { "MPI", "skinny.MPI",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_serviceNumber,
      { "Service number", "skinny.serviceNumber",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_temporalSpatialTradeOffCapability,
      { "Temporal spatial trade off capability", "skinny.temporalSpatialTradeOffCapability",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_stillImageTransmission,
      { "Still image transmission", "skinny.stillImageTransmission",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_h263_capability_bitfield,
      { "H263 capability bitfield", "skinny.h263_capability_bitfield",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_annexNandWFutureUse,
      { "Annex N and W future use", "skinny.annexNandWFutureUse",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_modelNumber,
      { "Model number", "skinny.modelNumber",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_bandwidth,
      { "Bandwidth", "skinny.bandwidth",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_protocolDependentData,
      { "Protocol dependent data", "skinny.protocolDependentData",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_priority,
      { "Priority", "skinny.priority",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_payloadDtmf,
      { "Payload DTMF", "skinny.payloadDtmf",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        "RTP payload type.",
        HFILL }
    },

    { &hf_skinny_featureID,
      { "Feature ID", "skinny.featureID",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_featureTextLabel,
      { "Feature text label", "skinny.featureTextLabel",
        FT_STRING, BASE_NONE, NULL, 0x0,
        "The feature label text that is displayed on the phone.",
        HFILL }
    },

    { &hf_skinny_featureStatus,
      { "Feature status", "skinny.featureStatus",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_notify,
      { "Notify", "skinny.notify",
        FT_STRING, BASE_NONE, NULL, 0x0,
        "The message notify text that is displayed on the phone.",
        HFILL }
    },

    { &hf_skinny_endOfAnnAck,
      { "End of ann. ack", "skinny.endOfAnnAck",
        FT_UINT32, BASE_DEC, VALS(skinny_endOfAnnAck), 0x0,
        "End of announcement ack.",
        HFILL }
    },

    { &hf_skinny_annPlayMode,
      { "Ann. play mode", "skinny.annPlayMode",
        FT_UINT32, BASE_DEC, VALS(skinny_annPlayMode), 0x0,
        "Announcement play mode.",
        HFILL }
    },

    { &hf_skinny_annPlayStatus,
      { "Ann. play status", "skinny.annPlayStatus",
        FT_UINT32, BASE_DEC, VALS(skinny_annPlayStatus), 0x0,
        "Announcement play status.",
        HFILL }
    },

    { &hf_skinny_locale,
      { "Locale", "skinny.locale",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        "User locale ID.",
        HFILL }
    },

    { &hf_skinny_country,
      { "Country", "skinny.country",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        "Country ID (Network locale).",
        HFILL }
    },

    { &hf_skinny_matrixConfPartyID,
      { "Matrix conf party ID", "skinny.matrixConfPartyID",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        "Existing conference parties.",
        HFILL }
    },

    { &hf_skinny_hearingConfPartyMask,
      { "Hearing conf party mask", "skinny.hearingConfPartyMask",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        "Bit mask of conference parties to hear media received on this stream.  Bit0 = matrixConfPartyID[0], Bit1 = matrixConfPartiID[1].",
        HFILL }
    },

    { &hf_skinny_serviceURL,
      { "Service URL value", "skinny.serviceURL",
        FT_STRING, BASE_NONE, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_serviceURLDisplayName,
      { "Service URL display name", "skinny.serviceURLDisplayName",
        FT_STRING, BASE_NONE, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_callSelectStat,
      { "Call select stat", "skinny.callSelectStat",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_isConferenceCreator,
      { "Is conference creator", "skinny.isConferenceCreator",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_payload_rfc_number,
      { "Payload RFC number", "skinny.payload_rfc_number",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_payloadType,
      { "Payload type", "skinny.payloadType",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_bitRate,
      { "Bitrate", "skinny.bitRate",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_pictureFormatCount,
      { "Picture format count", "skinny.pictureFormatCount",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_confServiceNum,
      { "Conf service number", "skinny.confServiceNum",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        "Conference service number.",
        HFILL }
    },

    { &hf_skinny_DSCPValue,
      { "DSCP value", "skinny.DSCPValue",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_miscCommandType,
      { "Misc command type", "skinny.miscCommandType",
        FT_UINT32, BASE_DEC|BASE_EXT_STRING, &skinny_miscCommandType_ext, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_temporalSpatialTradeOff,
      { "Temporal spatial trade-off", "skinny.temporalSpatialTradeOff",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_firstGOB,
      { "First GOB", "skinny.firstGOB",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_numberOfGOBs,
      { "Number of GOBs", "skinny.numberOfGOBs",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_firstMB,
      { "First MB", "skinny.firstMB",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_numberOfMBs,
      { "Number of MBs", "skinny.numberOfMBs",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_pictureNumber,
      { "Picture number", "skinny.pictureNumber",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_longTermPictureIndex,
      { "Long-term picture index", "skinny.longTermPictureIndex",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_recoveryReferencePictureCount,
      { "Recovery-reference picture count", "skinny.recoveryReferencePictureCount",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_lastRedirectingPartyName,
      { "Last redirecting party name", "skinny.lastRedirectingPartyName",
        FT_STRING, BASE_NONE, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_lastRedirectingParty,
      { "Last redirecting party", "skinny.lastRedirectingParty",
        FT_STRING, BASE_NONE, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_cgpnVoiceMailbox,
      { "Calling party voice mailbox", "skinny.cgpnVoiceMailbox",
        FT_STRING, BASE_NONE, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_cdpnVoiceMailbox,
      { "Called party voice mailbox", "skinny.cdpnVoiceMailbox",
        FT_STRING, BASE_NONE, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_originalCdpnVoiceMailbox,
      { "Original called party voice mailbox", "skinny.originalCdpnVoiceMailbox",
        FT_STRING, BASE_NONE, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_lastRedirectingVoiceMailbox,
      { "Last redirecting voice mailbox", "skinny.lastRedirectingVoiceMailbox",
        FT_STRING, BASE_NONE, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_originalCdpnRedirectReason,
      { "Original called party redirect reason", "skinny.originalCdpnRedirectReason",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_lastRedirectingReason,
      { "Last redirecting reason", "skinny.lastRedirectingReason",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_callInstance,
      { "Call instance", "skinny.callInstance",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_callSecurityStatus,
      { "Call security status", "skinny.callSecurityStatus",
        FT_UINT32, BASE_DEC, VALS(skinny_callSecurityStatusTypes), 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_accessory,
      { "Phone accessories", "skinny.accessories",
        FT_UINT32, BASE_DEC|BASE_EXT_STRING, &skinny_accessories_ext, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_accessoryState,
      { "Phone accessory state", "skinny.accessoryState",
        FT_UINT32, BASE_DEC|BASE_EXT_STRING, &skinny_accessoryStates_ext, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_partyPIRestrictionBits_CallingPartyName,
      { "RestrictCallingPartyName", "skinny.partyPIRestrictionBits.CallingPartyName",
        FT_BOOLEAN, 32, TFS(&tfs_yes_no), 0x01,
        NULL, HFILL }
    },

    { &hf_skinny_partyPIRestrictionBits_CallingPartyNumber,
      { "RestrictCallingPartyNumber", "skinny.partyPIRestrictionBits.CallingPartyNumber",
        FT_BOOLEAN, 32, TFS(&tfs_yes_no), 0x02,
        NULL, HFILL }
    },

    { &hf_skinny_partyPIRestrictionBits_CalledPartyName,
      { "RestrictCalledPartyName", "skinny.partyPIRestrictionBits.CalledPartyName",
        FT_BOOLEAN, 32, TFS(&tfs_yes_no), 0x04,
        NULL, HFILL }
    },

    { &hf_skinny_partyPIRestrictionBits_CalledPartyNumber,
      { "RestrictCalledPartyNumber", "skinny.partyPIRestrictionBits.CalledPartyNumber",
        FT_BOOLEAN, 32, TFS(&tfs_yes_no), 0x08,
        NULL, HFILL }
    },

    { &hf_skinny_partyPIRestrictionBits_OriginalCalledPartyName,
      { "RestrictOriginalCalledPartyName", "skinny.partyPIRestrictionBits.OriginalCalledPartyName",
        FT_BOOLEAN, 32, TFS(&tfs_yes_no), 0x10,
        NULL, HFILL }
    },

    { &hf_skinny_partyPIRestrictionBits_OriginalCalledPartyNumber,
      { "RestrictOriginalCalledPartyNumber", "skinny.partyPIRestrictionBits.OriginalCalledPartyNumber",
        FT_BOOLEAN, 32, TFS(&tfs_yes_no), 0x20,
        NULL, HFILL }
    },

    { &hf_skinny_partyPIRestrictionBits_LastRedirectPartyName,
      { "RestrictLastRedirectPartyName", "skinny.partyPIRestrictionBits.LastRedirectPartyName",
        FT_BOOLEAN, 32, TFS(&tfs_yes_no), 0x40,
        NULL, HFILL }
    },

    { &hf_skinny_partyPIRestrictionBits_LastRedirectPartyNumber,
      { "RestrictLastRedirectPartyNumber", "skinny.partyPIRestrictionBits.LastRedirectPartyNumber",
        FT_BOOLEAN, 32, TFS(&tfs_yes_no), 0x80,
        NULL, HFILL }
    },

    { &hf_skinny_directoryIndex,
      { "Directory index", "skinny.directoryIndex",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_directoryPhoneNumber,
      { "Directory phone number", "skinny.directoryPhoneNumber",
        FT_STRING, BASE_NONE, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_qualityStats,
      { "Quality Statistics", "skinny.qualityStats",
        FT_STRING, BASE_NONE, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_speedDialStatus,
      { "Speeddial Status", "skinny.speedDialStatus",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL,
        HFILL }
    },


    { &hf_skinny_speedDialType,
      { "Speeddial Type", "skinny.speedDialType",
        FT_UINT32, BASE_DEC, VALS(buttonDefinitions), 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_startMediaTransmissionStatus,
      { "Start MediaTransmission Status Type", "skinny.startMediaTransmissionStatus",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL,
        HFILL }
    },

    { &hf_skinny_startMultiMediaTransmissionStatus,
      { "Start MultiMediaTransmission Status Type", "skinny.startMultiMediaTransmissionStatus",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL,
        HFILL }
    },


  };

  /
  static gint *ett[] = {
    &ett_skinny,
    &ett_skinny_tree,
    &ett_skinny_softKeyMap,
  };

  module_t *skinny_module;

  /
  proto_skinny = proto_register_protocol("Skinny Client Control Protocol",
                                         "SKINNY", "skinny");

  /
  proto_register_field_array(proto_skinny, hf, array_length(hf));
  proto_register_subtree_array(ett, array_length(ett));

  skinny_module = prefs_register_protocol(proto_skinny, NULL);
  prefs_register_bool_preference(skinny_module, "desegment",
    "Reassemble SCCP messages spanning multiple TCP segments",
    "Whether the SCCP dissector should reassemble messages spanning multiple TCP segments."
    " To use this option, you must also enable"
    " \"Allow subdissectors to reassemble TCP streams\" in the TCP protocol settings.",
    &skinny_desegment);

  skinny_tap = register_tap("skinny");
}
