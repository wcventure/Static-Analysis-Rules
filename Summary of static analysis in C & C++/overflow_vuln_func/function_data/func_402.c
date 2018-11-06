void proto_register_olsr(void) {
  static hf_register_info hf[] = {
    { &hf_olsr_packet_len,
      { "Packet Length", "olsr.packet_len",
        FT_UINT16, BASE_DEC, NULL, 0,
        "Packet Length in Bytes", HFILL
      }
    },

    { &hf_olsr_packet_seq_num,
      { "Packet Sequence Number", "olsr.packet_seq_num",
        FT_UINT16, BASE_DEC, NULL, 0,
        NULL, HFILL
      }
    },

    { &hf_olsr_message,
      { "Message", "olsr.message",
        FT_BYTES, BASE_NONE, NULL, 0,
        NULL, HFILL
      }
    },

    { &hf_olsr_message_type,
      { "Message Type", "olsr.message_type",
        FT_UINT8, BASE_DEC, VALS(message_type_vals), 0,
        NULL, HFILL
      }
    },

    { &hf_olsr_message_size,
      { "Message", "olsr.message_size",
        FT_UINT16, BASE_DEC, NULL, 0,
        "Message Size in Bytes", HFILL
      }
    },

    { &hf_olsr_message_seq_num,
      { "Message Sequence Number", "olsr.message_seq_num",
        FT_UINT16, BASE_DEC, NULL, 0,
        NULL, HFILL
      }
    },

    { &hf_olsr_vtime,
      { "Validity Time", "olsr.vtime",
        FT_DOUBLE, BASE_NONE, NULL, 0,
        "Validity Time in seconds", HFILL
      }
    },

    { &hf_olsr_ansn,
      { "Advertised Neighbor Sequence Number (ANSN)", "olsr.ansn",
        FT_UINT16, BASE_DEC, NULL, 0,
        NULL, HFILL
      }
    },

    { &hf_olsr_htime,
      { "Hello Emission Interval", "olsr.htime",
        FT_DOUBLE, BASE_NONE, NULL, 0,
        "Hello emission interval in seconds", HFILL
      }
    },

    { &hf_olsr_willingness,
      { "Willingness to forward messages", "olsr.willingness",
        FT_UINT8, BASE_DEC, VALS(willingness_type_vals), 0,
        NULL, HFILL
      }
    },

    { &hf_olsr_ttl,
      { "TTL", "olsr.ttl",
        FT_UINT8, BASE_DEC, NULL, 0,
        "Time to Live in hops", HFILL
      }
    },

    { &hf_olsr_link_type,
      { "Link Type", "olsr.link_type",
        FT_UINT8, BASE_DEC, VALS(link_type_vals), 0,
        NULL, HFILL
      }
    },

    { &hf_olsr_link_message_size,
      { "Link Message Size", "olsr.link_message_size",
        FT_UINT16, BASE_DEC, NULL, 0,
        "Link Message Size in bytes", HFILL
      }
    },

    { &hf_olsr_hop_count,
      { "Hop Count", "olsr.hop_count",
        FT_UINT8, BASE_DEC, NULL, 0,
        NULL, HFILL
      }
    },

    { &hf_olsr_neighbor,
      { "Neighbor Address", "olsr.neighbor",
        FT_BYTES, BASE_NONE, NULL, 0,
        NULL, HFILL
      }
    },

    { &hf_olsr_origin_addr,
      { "Originator Address", "olsr.origin_addr",
        FT_IPv4, BASE_NONE, NULL, 0,
        NULL, HFILL
      }
    },

    { &hf_olsr_neighbor_addr,
      { "Neighbor Address", "olsr.neighbor_addr",
        FT_IPv4, BASE_NONE, NULL, 0,
        NULL, HFILL
      }
    },

    { &hf_olsr_network_addr,
      { "Network Address", "olsr.network_addr",
        FT_IPv4, BASE_NONE, NULL, 0,
        NULL, HFILL
      }
    },

    { &hf_olsr_interface_addr,
      { "Interface Address", "olsr.interface_addr",
        FT_IPv4, BASE_NONE, NULL, 0,
        NULL, HFILL
      }
    },

    { &hf_olsr_netmask,
      { "Netmask", "olsr.netmask",
        FT_IPv4, BASE_NONE, NULL, 0,
        NULL, HFILL
      }
    },

#if 0
    { &hf_olsr_neighbor6,
      { "Neighbor Address", "olsr.neighbor6",
        FT_BYTES, BASE_NONE, NULL, 0,
        NULL, HFILL
      }
    },
#endif

    { &hf_olsr_origin6_addr,
      { "Originator Address", "olsr.origin6_addr",
        FT_IPv6, BASE_NONE, NULL, 0,
        NULL, HFILL
      }
    },

    { &hf_olsr_neighbor6_addr,
      { "Neighbor Address", "olsr.neighbor6_addr",
        FT_IPv6, BASE_NONE, NULL, 0,
        NULL, HFILL
      }
    },

    { &hf_olsr_network6_addr,
      { "Network Address", "olsr.network6_addr",
        FT_IPv6, BASE_NONE, NULL, 0,
        NULL, HFILL
      }
    },

    { &hf_olsr_interface6_addr,
      { "Interface Address", "olsr.interface6_addr",
        FT_IPv6, BASE_NONE, NULL, 0,
        NULL, HFILL
      }
    },

    { &hf_olsr_netmask6,
      { "Netmask", "olsr.netmask6",
        FT_IPv6, BASE_NONE, NULL, 0,
        NULL, HFILL
      }
    },

    { &hf_olsr_data,
      { "Data", "olsr.data",
        FT_BYTES, BASE_NONE, NULL, 0,
        NULL, HFILL
      }
    },

    { &hf_olsrorg_lq,
      { "LQ", "olsr.lq",
        FT_UINT8, BASE_DEC, NULL, 0,
        "Link quality", HFILL
      }
    },
    { &hf_olsrorg_nlq,
      { "NLQ", "olsr.nlq",
        FT_UINT8, BASE_DEC, NULL, 0,
        "Neighbor link quality", HFILL
      }
    },

    { &hf_olsrorg_ns,
      { "Nameservice", "olsr.ns",
        FT_BYTES, BASE_NONE, NULL, 0,
        NULL, HFILL
      }
    },
    { &hf_olsrorg_ns_version,
      { "Version", "olsr.ns.version",
        FT_UINT16, BASE_DEC, NULL, 0,
        NULL, HFILL
      }
    },
    { &hf_olsrorg_ns_count,
      { "Count", "olsr.ns.count",
        FT_UINT16, BASE_DEC, NULL, 0,
        "Number of nameservice messages", HFILL
      }
    },
    { &hf_olsrorg_ns_type,
      { "Message Type", "olsr.ns.type",
        FT_UINT16, BASE_DEC, VALS(nameservice_type_vals), 0,
        NULL, HFILL
      }
    },
    { &hf_olsrorg_ns_length,
      { "Length", "olsr.ns.length",
        FT_UINT16, BASE_DEC, NULL, 0,
        NULL, HFILL
      }
    },
    { &hf_olsrorg_ns_ip,
      { "Address", "olsr.ns.ip",
        FT_IPv4, BASE_NONE, NULL, 0,
        NULL, HFILL
      }
    },
    { &hf_olsrorg_ns_ip6,
      { "Address", "olsr.ns.ip6",
        FT_IPv6, BASE_NONE, NULL, 0,
        NULL, HFILL
      }
    },
    { &hf_olsrorg_ns_content,
      { "Content", "olsr.ns.content",
        FT_STRING, BASE_NONE, NULL, 0,
        NULL, HFILL
      }
    },

    { &hf_nrlolsr_f1,
      { "NRL MINMAX", "olsr.nrl.minmax",
        FT_UINT8, BASE_DEC, NULL, 0,
        NULL, HFILL
      }
    },
    { &hf_nrlolsr_f2,
      { "NRL SPF", "olsr.nrl.spf",
        FT_UINT8, BASE_DEC, NULL, 0,
        NULL, HFILL
      }
    }
  };

  static gint *ett_base[] = {
    &ett_olsr,
    &ett_olsr_message_linktype,
    &ett_olsr_message_neigh,
    &ett_olsr_message_neigh6,
    &ett_olsr_message_ns
  };

  static ei_register_info ei[] = {
    { &ei_olsr_not_enough_bytes, { "olsr.not_enough_bytes", PI_MALFORMED, PI_ERROR, "Not enough bytes for field", EXPFILL }},
    { &ei_olsrorg_ns_version, { "olsr.ns.version.unknown", PI_PROTOCOL, PI_WARN, "Unknown nameservice protocol version", EXPFILL }},
    { &ei_olsr_data_misaligned, { "olsr.data.misaligned", PI_PROTOCOL, PI_WARN, "Must be aligned on 32 bits", EXPFILL }},
  };

  gint *ett[array_length(ett_base) + (G_MAXUINT8+1)];

  module_t *olsr_module;
  expert_module_t *expert_olsr;
  int i,j;

  memcpy(ett, ett_base, sizeof(ett_base));
  j = array_length(ett_base);
  for (i=0; i<G_MAXUINT8+1; i++) {
    ett_olsr_message[i] = -1;
    ett[j++] = &ett_olsr_message[i];
  }

  proto_olsr = proto_register_protocol("Optimized Link State Routing Protocol", "OLSR", "olsr");

  proto_register_field_array(proto_olsr, hf, array_length(hf));
  proto_register_subtree_array(ett, array_length(ett));
  expert_olsr = expert_register_protocol(proto_olsr);
  expert_register_field_array(expert_olsr, ei, array_length(ei));

  olsr_module = prefs_register_protocol(proto_olsr, NULL);
  prefs_register_bool_preference(olsr_module, "ff_olsrorg",
                                 "Dissect olsr.org messages","Dissect custom olsr.org message types (compatible with rfc routing agents)",
                                 &global_olsr_olsrorg);
  prefs_register_bool_preference(olsr_module, "nrlolsr",
                                 "Dissect NRL-Olsr TC messages", "Dissect custom nrlolsr tc message (incompatible with rfc routing agents)",
                                 &global_olsr_nrlolsr);
}
