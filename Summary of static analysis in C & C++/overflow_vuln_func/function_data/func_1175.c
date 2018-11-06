void proto_register_waveagent(void)
{
    static const value_string tcp_states[] = {
        { 0, "Closed" },
        { 1, "Listen" },
        { 2, "SYN Sent" },
        { 3, "SYN received" },
        { 4, "Established" },
        { 5, "FIN Wait 1" },
        { 6, "FIN Wait 2" },
        { 7, "Close Wait" },
        { 8, "Closing" },
        { 9, "Last ACK" },
        { 10, "Time Wait" },
        { 0, NULL },
    };

    static const value_string app_states[] = {
        { 0, "IDLE" },
        { 1, "READY" },
        { 0, NULL },
    };

    static const value_string wa_modes[] = {
        { 0, "In-band" },
        { 1, "Source" },
        { 2, "Sink" },
        { 3, "Loopback" },
        { 0, NULL },
    };

    static const value_string wa_endpointtypes[] = {
        { 0, "Undefined" },
        { 1, "Server" },
        { 2, "Client" },
        { 0, NULL },
    };

    static const value_string binding_levels[] = {
        { 0, "WLAN" },
        { 1, "Ethernet" },
        { 2, "IP" },
        { 3, "UDP" },
        { 4, "TCP" },
        { 5, "FIN Wait 1" },
        { 6, "FIN Wait 2" },
        { 7, "Close Wait" },
        { 8, "Closing" },
        { 9, "Last ACK" },
        { 10, "Time Wait" },
        { 0, NULL },
    };

    static const value_string if_types[] = {
        { ETHERNET_INTERFACE, "Ethernet" },
        { WLAN_INTERFACE, "WLAN" },
        { 0, NULL },
    };

    static const value_string no_yes[] = {
        { 0, "No" },
        { 1, "Yes" },
        { 0, NULL },
    };

    static const value_string ip_types[] = {
        { 0,  "Unspecified" },
        { IPV4_TYPE,  "IPv4" },
        { IPV6_TYPE, "IPv6" },
        { 0, NULL },
    };

    static const value_string if_l3_states[] = {
        { 0, "Uninitialized" },
        { 1, "Disconnected" },
        { 2, "Connected" },
        { 3, "Error" },
        { 0, NULL },
    };

    static const value_string if_wlan_states[] = {
        { 0, "Uninitialized" },
        { 1, "Not ready" },
        { 2, "Connected" },
        { 3, "Ad Hoc network formed" },
        { 4, "Disconnecting" },
        { 5, "Disconnected" },
        { 6, "Associating" },
        { 7, "Discovering" },
        { 8, "Authenticating" },
        { 0, NULL },
    };

    static const value_string if_eth_states[] = {
        { 0, "Uninitialized" },
        { 1, "Not Operational" },
        { 2, "Unreachable" },
        { 3, "Disconnected" },
        { 4, "Connecting" },
        { 5, "Connected" },
        { 6, "Operational" },
        { 7, "Error" },
        { 0, NULL },
    };

    static const value_string bss_modes[] = {
        { 0, "Infrastructure" },
        { 1, "IBSS" },
        { 2, "Unknown" },
        { 0, NULL },
    };

    static const value_string auth_algs[] = {
        { 0,  "Open" },
        { 1,  "Shared Key" },
        { 2,  "WPA" },
        { 4,  "WPA PSK" },
        { 8,  "WPA2" },
        { 16, "WPA2 PSK" },
        { 0, NULL },
    };

    static const value_string cipher_algs[] = {
        { 0,  "None" },
        { 1,  "WEP 40" },
        { 2,  "WEP 104" },
        { 4,  "WEP" },
        { 8,  "TKIP" },
        { 16, "CCMP" },
        { 0, NULL },
    };

    /
    static hf_register_info hf[] = {

    /
        { &hf_waveagent_controlword,
        { "Control Word", "waveagent.cword",
        FT_UINT16, BASE_HEX, VALS(control_words), 0x0,
        NULL, HFILL } },

        { &hf_waveagent_payloadlen,
        { "Payload Length", "waveagent.paylen",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL, HFILL } },
            
        { &hf_waveagent_transnum,
        { "Transaction Number", "waveagent.transnum",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL, HFILL } },
            
        { &hf_waveagent_rtoken,
        { "Reservation Token", "waveagent.rtoken",
        FT_UINT32, BASE_HEX, NULL, 0x0,
        NULL, HFILL } },
            
        { &hf_waveagent_flowid,
        { "Flow ID", "waveagent.flowid",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL, HFILL } },
            
        { &hf_waveagent_capstatus,
        { "Capabilities Status", "waveagent.capstatus",
        FT_UINT32, BASE_HEX, NULL, 0x0,
        NULL, HFILL } },
            
        { &hf_waveagent_protocolversion,
        { "Protocol Version", "waveagent.protocolversion",
        FT_UINT8, BASE_DEC, NULL, 0x0,
        NULL, HFILL } },
            
        { &hf_waveagent_capimpl,
        { "Capabilities Implementation", "waveagent.capimpl",
        FT_UINT32, BASE_HEX, NULL, 0x0,
        NULL, HFILL } },
            
        { &hf_waveagent_sigsequencenum,
        { "Signature Sequence Number", "waveagent.sigsequencenum",
        FT_UINT8, BASE_DEC, NULL, 0x0,
        NULL, HFILL } },
            
        { &hf_waveagent_id,
        { "ID", "waveagent.id",
        FT_STRING, 0, NULL, 0x0,
        NULL, HFILL } },
            
        { &hf_waveagent_bindtag,
        { "Binding Tag", "waveagent.bindtag",
        FT_STRING, 0, NULL, 0x0,
        NULL, HFILL } },
            
        { &hf_waveagent_version,
        { "Version", "waveagent.version",
        FT_STRING, 0, NULL, 0x0,
        NULL, HFILL } },
            
        { &hf_waveagent_brokerip,
        { "Broker IP address", "waveagent.brokerip",
        FT_IPv4, BASE_NONE, NULL, 0x0,
        NULL, HFILL } },
            
        { &hf_waveagent_brokerport,
        { "Broker Port", "waveagent.brokerport",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL, HFILL } },
            
        { &hf_waveagent_bindlevel,
        { "Binding Level", "waveagent.bindlevel",
        FT_UINT32, BASE_DEC, VALS(binding_levels), 0x0,
        NULL, HFILL } },
            
        { &hf_waveagent_bindport,
        { "Binding Port", "waveagent.bindport",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL, HFILL } },

        { &hf_waveagent_ifindex,
        { "Interface Index", "waveagent.ifindex",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL, HFILL } },
    /
            
    /        
        { &hf_waveagent_capabilities2,
        { "Additional Capabilities", "waveagent.capabilities2",
        FT_UINT32, BASE_HEX, NULL, 0x0,
        NULL, HFILL } },

        { &hf_waveagent_numinterfaces,
        { "Number of WA Interfaces", "waveagent.numinterfaces",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL, HFILL } },

        { &hf_waveagent_ifmask,
        { "Mask of Active Interfaces", "waveagent.ifmask",
        FT_UINT32, BASE_HEX, NULL, 0x0,
        NULL, HFILL } },
    /        

    /        
        { &hf_waveagent_commandstatus,
        { "Status of Previous Command", "waveagent.cmdstat",
        FT_INT32, BASE_DEC, NULL, 0x0,
        NULL, HFILL } },
            
        { &hf_waveagent_syserrno,
        { "System Error Number", "waveagent.syserrno",
        FT_INT32, BASE_DEC, NULL, 0x0,
        NULL, HFILL } },
            
        { &hf_waveagent_statusstring,
        { "Status Message", "waveagent.statmsg",
        FT_STRING, 0, NULL, 0x0,
        NULL, HFILL } },
    /        
            
    /
        { &hf_waveagent_rxdatapckts,
        { "Received Data Packets", "waveagent.rxdpkts",
        FT_UINT64, BASE_DEC, NULL, 0x0,
        NULL, HFILL } },
            
        { &hf_waveagent_rxdatabytes,
        { "Received Data Bytes", "waveagent.rxdbytes",
        FT_UINT64, BASE_DEC, NULL, 0x0,
        NULL, HFILL } },
            
        { &hf_waveagent_rxpcktrate,
        { "Received Data Packet Rate (pps)", "waveagent.rxpktrate",
        FT_UINT64, BASE_DEC, NULL, 0x0,
        NULL, HFILL } },
            
        { &hf_waveagent_rxbyterate,
        { "Received Byte Rate", "waveagent.rxbyterate",
        FT_UINT64, BASE_DEC, NULL, 0x0,
        NULL, HFILL } },
            
        { &hf_waveagent_txdatapckts,
        { "Transmitted Data Packets", "waveagent.txdpkts",
        FT_UINT64, BASE_DEC, NULL, 0x0,
        NULL, HFILL } },
            
        { &hf_waveagent_txdatabytes,
        { "Transmitted Data Bytes", "waveagent.txdbytes",
        FT_UINT64, BASE_DEC, NULL, 0x0,
        NULL, HFILL } },
            
        { &hf_waveagent_txpcktrate,
        { "Transmitted Data Packet Rate (pps)", "waveagent.txpktrate",
        FT_UINT64, BASE_DEC, NULL, 0x0,
        NULL, HFILL } },
            
        { &hf_waveagent_txbyterate,
        { "Transmitted Byte Rate", "waveagent.txbyterate",
        FT_UINT64, BASE_DEC, NULL, 0x0,
        NULL, HFILL } },
            
        { &hf_waveagent_looppckts,
        { "Loopback Packets", "waveagent.looppckts",
        FT_UINT64, BASE_DEC, NULL, 0x0,
        NULL, HFILL } },
            
        { &hf_waveagent_loopbytes,
        { "Loopback Bytes", "waveagent.loopbytes",
        FT_UINT64, BASE_DEC, NULL, 0x0,
        NULL, HFILL } },
            
        { &hf_waveagent_rxctlpckts,
        { "Received Control Packets", "waveagent.rxctlpkts",
        FT_UINT64, BASE_DEC, NULL, 0x0,
        NULL, HFILL } },
            
        { &hf_waveagent_rxctlbytes,
        { "Received Control Bytes", "waveagent.rxctlbytes",
        FT_UINT64, BASE_DEC, NULL, 0x0,
        NULL, HFILL } },
            
        { &hf_waveagent_txctlpckts,
        { "Transmitted Control Packets", "waveagent.txctlpkts",
        FT_UINT64, BASE_DEC, NULL, 0x0,
        NULL, HFILL } },
            
        { &hf_waveagent_txctlbytes,
        { "Transmitted Control Bytes", "waveagent.txctlbytes",
        FT_UINT64, BASE_DEC, NULL, 0x0,
        NULL, HFILL } },
            
        { &hf_waveagent_unknowncmds,
        { "Unknown Commands", "waveagent.unkcmds",
        FT_UINT64, BASE_DEC, NULL, 0x0,
        NULL, HFILL } },
            
        { &hf_waveagent_snap,
        { "Time Snap for Counters", "waveagent.snap",
        FT_UINT64, BASE_DEC, NULL, 0x0,
        NULL, HFILL } },
            
        { &hf_waveagent_appstate,
        { "TCP State", "waveagent.state",
        FT_UINT32, BASE_DEC, VALS(tcp_states), 0x0,
        NULL, HFILL } },
            
        { &hf_waveagent_state,
        { "Application State", "waveagent.appstate",
        FT_UINT32, BASE_DEC, VALS(app_states), 0x0,
        NULL, HFILL } },
            
        { &hf_waveagent_rx1pl,
        { "Instances of single packet loss", "waveagent.rx1pl",
        FT_UINT64, BASE_DEC, NULL, 0x0,
        NULL, HFILL } },
            
        { &hf_waveagent_rx2pl,
        { "Instances of 2 sequential packets lost", "waveagent.rx2pl",
        FT_UINT64, BASE_DEC, NULL, 0x0,
        NULL, HFILL } },
            
        { &hf_waveagent_rx3pl,
        { "Instances of 3 sequential packets lost", "waveagent.rx3pl",
        FT_UINT64, BASE_DEC, NULL, 0x0,
        NULL, HFILL } },
            
        { &hf_waveagent_rx4pl,
        { "Instances of 4 sequential packets lost", "waveagent.rx4pl",
        FT_UINT64, BASE_DEC, NULL, 0x0,
        NULL, HFILL } },
            
        { &hf_waveagent_rx5pl,
        { "Instances of 5 sequential packets lost", "waveagent.rx5pl",
        FT_UINT64, BASE_DEC, NULL, 0x0,
        NULL, HFILL } },
            
        { &hf_waveagent_rxoospkts,
        { "Instances of out-of-sequence packets", "waveagent.rxoospkts",
        FT_UINT64, BASE_DEC, NULL, 0x0,
        NULL, HFILL } },
            
        { &hf_waveagent_rxmeanlatency,
        { "Rx Mean latency", "waveagent.rxmeanlatency",
        FT_UINT64, BASE_DEC, NULL, 0x0,
        NULL, HFILL } },
            
        { &hf_waveagent_rxminlatency,
        { "Rx Minimum latency", "waveagent.rxminlatency",
        FT_UINT64, BASE_DEC, NULL, 0x0,
        NULL, HFILL } },
            
        { &hf_waveagent_rxmaxlatency,
        { "Rx Maximum latency", "waveagent.rxmaxlatency",
        FT_UINT64, BASE_DEC, NULL, 0x0,
        NULL, HFILL } },
            
        { &hf_waveagent_jitter,
        { "Jitter (microseconds)", "waveagent.jitter",
        FT_UINT64, BASE_DEC, NULL, 0x0,
        NULL, HFILL } },

        { &hf_waveagent_delayfactor,
        { "Delay Factor", "waveagent.delayfactor",
        FT_UINT64, BASE_DEC, NULL, 0x0,
        NULL, HFILL } },

        { &hf_waveagent_medialossrate,
        { "Media Loss Rate", "waveagent.medialossrate",
        FT_UINT64, BASE_DEC, NULL, 0x0,
        NULL, HFILL } },

        { &hf_waveagent_txstartts,
        { "Timestamp for first Tx flow packet", "waveagent.txstartts",
        FT_UINT64, BASE_DEC, NULL, 0x0,
        NULL, HFILL } },

        { &hf_waveagent_txendts,
        { "Timestamp for last Tx flow packet", "waveagent.txendts",
        FT_UINT64, BASE_DEC, NULL, 0x0,
        NULL, HFILL } },

        { &hf_waveagent_rxstartts,
        { "Timestamp for first Rx flow packet", "waveagent.rxstartts",
        FT_UINT64, BASE_DEC, NULL, 0x0,
        NULL, HFILL } },

        { &hf_waveagent_rxendts,
        { "Timestamp for last Rx flow packet", "waveagent.rxendts",
        FT_UINT64, BASE_DEC, NULL, 0x0,
        NULL, HFILL } },

        { &hf_waveagent_latencysum,
        { "Sum of latencies across all received packets", "waveagent.latencysum",
        FT_UINT64, BASE_DEC, NULL, 0x0,
        NULL, HFILL } },

        { &hf_waveagent_latencycount,
        { "Count of packets included in the latency sum", "waveagent.latencycount",
        FT_UINT64, BASE_DEC, NULL, 0x0,
        NULL, HFILL } },

        { &hf_waveagent_txflowstop,
        { "Timestamp for Tx flow stop message", "waveagent.txflowstop",
        FT_UINT64, BASE_DEC, NULL, 0x0,
        NULL, HFILL } },
    /

    /
        { &hf_waveagent_rxflownum,
        { "Received Flow Number", "waveagent.rxflownum",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL, HFILL } },
            
        { &hf_waveagent_mode,
        { "WaveAgent Mode", "waveagent.trafficmode",
        FT_UINT8, BASE_DEC, VALS(wa_modes), 0x03,
        NULL, HFILL } },
            
        { &hf_waveagent_endpointtype,
        { "WaveAgent Endpoint Type", "waveagent.endpointtype",
        FT_UINT8, BASE_DEC, VALS(wa_endpointtypes), 0x0c,
        NULL, HFILL } },
            
        { &hf_waveagent_remoteport,
        { "Remote port", "waveagent.remoteport",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL, HFILL } },
            
        { &hf_waveagent_remoteaddr,
        { "Remote address", "waveagent.remoteaddr",
        FT_IPv4, BASE_NONE, NULL, 0x0,
        NULL, HFILL } },
            
        { &hf_waveagent_dscp,
        { "DSCP Setting", "waveagent.dscp",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL, HFILL } },
            
        { &hf_waveagent_fsflags,
        { "Flow Setup Flags", "waveagent.fsflags",
        FT_UINT32, BASE_HEX, NULL, 0x0,
        NULL, HFILL } },

        { &hf_waveagent_fscbrflag,
        { "CBR Transmit Mode", "waveagent.fscbrflag",
        FT_BOOLEAN, 4, NULL, 0x01, NULL, HFILL } },

        { &hf_waveagent_fscombinedsetupflag,
        { "Setup, Connect/Listen, Start Combined", "waveagent.fscombinedsetupflag",
        FT_BOOLEAN, 4, NULL, 0x02, NULL, HFILL } },

    /
            
    /
        { &hf_waveagent_payfill,
        { "Payload Fill", "waveagent.payfill",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL, HFILL } },
            
        { &hf_waveagent_paysize,
        { "WaveAgent Payload Size (bytes)", "waveagent.paysize",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL, HFILL } },
            
        { &hf_waveagent_avgrate,
        { "Average Rate (header + payload + trailer bytes/s)", "waveagent.avgrate",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL, HFILL } },
            
        { &hf_waveagent_totalframes,
        { "Total Frames", "waveagent.totalframes",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL, HFILL } },

    /

    /
        { &hf_waveagent_bssidstartindex,
        { "Starting Index of BSSID list for reporting", "waveagent.bssidstartindex",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL, HFILL } },
            
        { &hf_waveagent_bssidstopindex,
        { "Ending Index of BSSID list for reporting", "waveagent.bssidstopindex",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL, HFILL } },
            
    /

    /
        { &hf_waveagent_ifwlanbssid,
        { "WLAN Interface Connected to BSSID", "waveagent.ifwlanbssid",
        FT_ETHER, 0, NULL, 0x0,
        NULL, HFILL } },
            
        { &hf_waveagent_ifwlanssid,
        { "WLAN Interface Connected to SSID", "waveagent.ifwlanssid",
        FT_STRING, 0, NULL, 0x0,
        NULL, HFILL } },
            
        { &hf_waveagent_ifwlanrssi,
        { "WLAN Interface RSSI", "waveagent.ifwlanrssi",
        FT_INT32, BASE_DEC, NULL, 0x0,
        NULL, HFILL } },

        { &hf_waveagent_ifwlannoise,
        { "WLAN Interface Noise Floor (dBm)", "waveagent.ifwlannoise",
        FT_INT32, BASE_DEC, NULL, 0x0,
        NULL, HFILL } },

        { &hf_waveagent_ifphytypes,
        { "WLAN Interface Supported PHY Types", "waveagent.ifphytypes",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL, HFILL } },

        { &hf_waveagent_ifphytypebit0,
        { "11b", "waveagent.ifphytypebit0",
        FT_BOOLEAN, 4, NULL, 0x01, NULL, HFILL } },

        { &hf_waveagent_ifphytypebit1,
        { "11g", "waveagent.ifphytypebit1",
        FT_BOOLEAN, 4, NULL, 0x02, NULL, HFILL } },

        { &hf_waveagent_ifphytypebit2,
        { "11a", "waveagent.ifphytypebit2",
        FT_BOOLEAN, 4, NULL, 0x04, NULL, HFILL } },

        { &hf_waveagent_ifphytypebit3,
        { "11n", "waveagent.ifphytypebit3",
        FT_BOOLEAN, 4, NULL, 0x08, NULL, HFILL } },

        { &hf_waveagent_ifwlanauthentication,
        { "WLAN Interface Authentication Algorithm", "waveagent.ifwlanauthentication",
        FT_UINT32, BASE_DEC, VALS(auth_algs), 0x0,
        NULL, HFILL } },

        { &hf_waveagent_ifwlancipher,
        { "WLAN Interface Encryption/Cipher Algorithm", "waveagent.ifwlancipher",
        FT_UINT32, BASE_DEC, VALS(cipher_algs), 0x0,
        NULL, HFILL } },
    /

    /
        { &hf_waveagent_iftype,
        { "Interface type", "waveagent.iftype",
        FT_UINT32, BASE_DEC, VALS(if_types), 0x0,
        NULL, HFILL } },
            
        { &hf_waveagent_ifdescription,
        { "Name/Description of the adapter", "waveagent.ifdescription",
        FT_STRING, 0, NULL, 0x0,
        NULL, HFILL } },
     
        { &hf_waveagent_ifmacaddr,
        { "Interface MAC Address", "waveagent.ifmacaddr",
        FT_ETHER, 0, NULL, 0x0,
        NULL, HFILL } },
            
        { &hf_waveagent_iflinkspeed,
        { "Interface Link Speed (kbps)", "waveagent.iflinkspeed",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL, HFILL } },
            
        { &hf_waveagent_ifdhcp,
        { "Interface DHCP Enabled", "waveagent.ifdhcp",
        FT_UINT32, BASE_DEC, VALS(no_yes), 0x0,
        NULL, HFILL } },

        { &hf_waveagent_ifiptype,
        { "Interface IP Type", "waveagent.ifiptype",
        FT_UINT32, BASE_DEC, VALS(ip_types), 0x0,
        NULL, HFILL } },
            
        { &hf_waveagent_ifipv4,
        { "Interface Bound to IP Address", "waveagent.ifipv4",
        FT_IPv4, BASE_NONE, NULL, 0x0,
        NULL, HFILL } },
            
        { &hf_waveagent_ifipv6,
        { "Interface Bound to IP Address", "waveagent.ifipv6",
        FT_IPv6, BASE_NONE, NULL, 0x0,
        NULL, HFILL } },
            
        { &hf_waveagent_ifdhcpserver,
        { "Interface DHCP Server Address", "waveagent.ifdhcpserver",
        FT_IPv4, BASE_NONE, NULL, 0x0,
        NULL, HFILL } },
            
        { &hf_waveagent_ifgateway,
        { "Interface Gateway", "waveagent.ifgateway",
        FT_IPv4, BASE_NONE, NULL, 0x0,
        NULL, HFILL } },
            
        { &hf_waveagent_ifdnsserver,
        { "Interface DNS Server Address", "waveagent.ifdnsserver",
        FT_IPv4, BASE_NONE, NULL, 0x0,
        NULL, HFILL } },
            
        { &hf_waveagent_ifethl2status,
        { "Ethernet L2 Interface Status", "waveagent.ifethl2status",
        FT_UINT32, BASE_DEC, VALS(if_eth_states), 0x0,
        NULL, HFILL } },
            
        { &hf_waveagent_ifwlanl2status,
        { "WLAN L2 Interface Status", "waveagent.ifwlanl2status",
        FT_UINT32, BASE_DEC, VALS(if_wlan_states), 0x0,
        NULL, HFILL } },
            
        { &hf_waveagent_ifl3status,
        { "L3 Interface Status", "waveagent.ifl3status",
        FT_UINT32, BASE_DEC, VALS(if_l3_states), 0x0,
        NULL, HFILL } },
            
    /

    /
        { &hf_waveagent_totalbssid,
        { "Number of Found BSSID", "waveagent.totalbssid",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL, HFILL } },

        { &hf_waveagent_returnedbssid,
        { "Number of BSSID Reported in This Response", "waveagent.returnedbssid",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL, HFILL } },

        { &hf_waveagent_scanbssid,
        { "BSSID", "waveagent.scanbssid",
        FT_ETHER, 0, NULL, 0x0,
        NULL, HFILL } },
            
        { &hf_waveagent_scanssid,
        { "SSID", "waveagent.scanssid",
        FT_STRING, 0, NULL, 0x0,
        NULL, HFILL } },
            
        { &hf_waveagent_ifwlansupprates,
        { "Supported Rates", "waveagent.ifwlansupportedrates",
        FT_STRING, BASE_NONE, NULL, 0x0,
        NULL, HFILL } },

        { &hf_waveagent_ifwlancapabilities,
        { "Capabilities field", "waveagent.ifwlancapabilities",
        FT_UINT16, BASE_HEX, NULL, 0x0,
        NULL, HFILL } },

        { &hf_waveagent_ifwlanchannel,
        { "Channel", "waveagent.ifwlanchannel",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL, HFILL } },

        { &hf_waveagent_ifwlanprivacy,
        { "Privacy Enabled", "waveagent.ifwlanprivacy",
        FT_UINT32, BASE_DEC, VALS(no_yes), 0x0,
        NULL, HFILL } },

        { &hf_waveagent_ifwlanbssmode,
        { "BSS Mode", "waveagent.ifwlanbssmode",
        FT_UINT32, BASE_DEC, VALS(bss_modes), 0x0,
        NULL, HFILL } },
    /

    /
        { &hf_waveagent_oidcode,
        { "OID Code", "waveagent.oidcode",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL, HFILL } },

        { &hf_waveagent_oidvalue,
        { "OID Value", "waveagent.oidvalue",
        FT_STRING, 0, NULL, 0x0,
        NULL, HFILL } },
    /

    /
        { &hf_waveagent_destip,
        { "Destination IP", "waveagent.destip",
        FT_IPv4, BASE_NONE, NULL, 0x0,
        NULL, HFILL } },

        { &hf_waveagent_destport,
        { "Destination Port", "waveagent.destport",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL, HFILL } },

        { &hf_waveagent_connectflags,
        { "Connect Flags", "waveagent.connectflags",
        FT_UINT32, BASE_HEX, NULL, 0x0,
        NULL, HFILL } },
    /

    /
        { &hf_waveagent_connecttype,
        { "Connect Type", "waveagent.connecttype",
        FT_UINT32, BASE_HEX, NULL, 0x0,
        NULL, HFILL } },

        { &hf_waveagent_minrssi,
        { "Minimum RSSI", "waveagent.minrssi",
        FT_INT32, BASE_DEC, NULL, 0x0,
        NULL, HFILL } },

        { &hf_waveagent_connecttimeout,
        { "Connect timeout (s)", "waveagent.connecttimeout",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL, HFILL } },

        { &hf_waveagent_connectattempts,
        { "Connect attempts", "waveagent.connectattempt",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL, HFILL } },

        { &hf_waveagent_reason,
        { "Reason", "waveagent.reason",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL, HFILL } },
    /

    /
        { &hf_waveagent_relaydestid,
        { "ID of destination client (assigned by relay server)", "waveagent.relaydestid",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL, HFILL } },

        { &hf_waveagent_relaysrcid,
        { "ID of source client (assigned by relay server)", "waveagent.relaysrcid",
        FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL, HFILL } },

        { &hf_waveagent_relaymessagest,
        { "Relayed WaveAgent Message", "waveagent.relaymessagest",
        FT_NONE, BASE_NONE, NULL, 0x0,
        "This is a relayed WaveAgent message", HFILL } },

/

    };

/
    static gint *ett[] = {
        &ett_waveagent,
        &ett_statechange,
        &ett_phytypes,
        &ett_fsflags,
        &ett_scindex0,
        &ett_scindex1,
        &ett_scindex2,
        &ett_scindex3,
        &ett_scindex4,
        &ett_scindex5,
        &ett_scindex6,
        &ett_scindex7,
        &ett_bss0,
        &ett_bss1,
        &ett_bss2,
        &ett_bss3,
        &ett_bss4,
        &ett_bss5,
        &ett_bss6,
        &ett_bss7,
        &ett_relaymessage,
    };

/
    proto_waveagent = proto_register_protocol("WaveAgent",
        "waveagent", "waveagent");

/
    proto_register_field_array(proto_waveagent, hf, array_length(hf));
    proto_register_subtree_array(ett, array_length(ett));
}
