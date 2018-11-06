void
proto_register_ajp13(void)
{                 
  static hf_register_info hf[] = {
    { &hf_ajp13_magic,
      { "Magic",  "ajp13.magic", FT_BYTES, BASE_HEX, NULL, 0x0, "Magic Number" }
    },
    { &hf_ajp13_len,
      { "Length",  "ajp13.len", FT_UINT16, BASE_DEC, NULL, 0x0, "Data Length" }
    },
    { &hf_ajp13_code,
      { "Code",  "ajp13.code", FT_STRING, BASE_DEC, NULL, 0x0, "Type Code" }
    },
    { &hf_ajp13_method,
      { "Method",  "ajp13.method", FT_STRING, BASE_DEC, NULL, 0x0, "HTTP Method" }
    },
    { &hf_ajp13_ver,
      { "Version",  "ajp13.ver", FT_STRING, BASE_DEC, NULL, 0x0, "HTTP Version" }
    },
    { &hf_ajp13_uri,
      { "URI",  "ajp13.uri", FT_STRING, BASE_DEC, NULL, 0x0, "HTTP URI" }
    },
    { &hf_ajp13_raddr,
      { "RADDR",  "ajp13.raddr", FT_STRING, BASE_DEC, NULL, 0x0, "Remote Address" }
    },
    { &hf_ajp13_rhost,
      { "RHOST",  "ajp13.rhost", FT_STRING, BASE_DEC, NULL, 0x0, "Remote Host" }
    },
    { &hf_ajp13_srv,
      { "SRV",  "ajp13.srv", FT_STRING, BASE_DEC, NULL, 0x0, "Server" }
    },
    { &hf_ajp13_port,
      { "PORT",  "ajp13.port", FT_UINT16, BASE_DEC, NULL, 0x0, "Port" }
    },
    { &hf_ajp13_sslp,
      { "SSLP",  "ajp13.sslp", FT_UINT8, BASE_DEC, NULL, 0x0, "Is SSL?" }
    },
    { &hf_ajp13_nhdr,
      { "NHDR",  "ajp13.nhdr", FT_UINT16, BASE_DEC, NULL, 0x0, "Num Headers" }
    },
    { &hf_ajp13_hname,
      { "HNAME",  "ajp13.hname", FT_STRING, BASE_DEC, NULL, 0x0, "Header Name" }
    },
    { &hf_ajp13_hval,
      { "HVAL",  "ajp13.hval", FT_STRING, BASE_DEC, NULL, 0x0, "Header Value" }
    },
    { &hf_ajp13_rlen,
      { "RLEN",  "ajp13.rlen", FT_UINT16, BASE_DEC, NULL, 0x0, "Requested Length" }
    },
    { &hf_ajp13_reusep,
      { "REUSEP",  "ajp13.reusep", FT_UINT8, BASE_DEC, NULL, 0x0, "Reuse Connection?" }
    },
    { &hf_ajp13_rstatus,
      { "RSTATUS",  "ajp13.rstatus", FT_UINT16, BASE_DEC, NULL, 0x0, "HTTP Status Code" }
    },
    { &hf_ajp13_rsmsg,
      { "RSMSG",  "ajp13.rmsg", FT_STRING, BASE_DEC, NULL, 0x0, "HTTP Status Message" }
    },
    { &hf_ajp13_data,
      { "Data",  "ajp13.data", FT_STRING, BASE_DEC, NULL, 0x0, "Data" }
    },
  };

  static gint *ett[] = {
    &ett_ajp13,
  };

  /
  /

  /
  proto_ajp13 = proto_register_protocol("Apache JServ Protocol v1.3", "AJP13", "ajp13");

  proto_register_field_array(proto_ajp13, hf, array_length(hf));
  proto_register_subtree_array(ett, array_length(ett));
}
