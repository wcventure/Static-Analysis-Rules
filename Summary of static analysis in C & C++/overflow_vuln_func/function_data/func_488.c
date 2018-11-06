void
proto_register_iphc_crtp(void)
{
  static hf_register_info hf[] =
  {
    {
      &hf_iphc_crtp_cid16,
      {
        "Context Id", "crtp.cid",
        FT_UINT16, BASE_DEC,
        NULL, 0x0,
        "The context identifier of the compressed packet.", HFILL
      }
    },
    {
      &hf_iphc_crtp_cid8,
      {
        "Context Id", "crtp.cid",
        FT_UINT8, BASE_DEC,
        NULL, 0x0,
        "The context identifier of the compressed packet.", HFILL
      }
    },
    {
      &hf_iphc_crtp_gen,
      {
        "Generation", "crtp.gen",
        FT_UINT8, BASE_DEC,
        NULL, 0x3f,
        "The generation of the compressed packet.", HFILL
      }
    },
    {
      &hf_iphc_crtp_seq,
      {
        "Sequence", "crtp.seq",
        FT_UINT8, BASE_DEC,
        NULL, 0x0f,
        "The sequence of the compressed packet.", HFILL
      }
    },
    {
      &hf_iphc_crtp_fh_flags,
      {
        "Flags", "crtp.flags",
        FT_UINT8, BASE_HEX,
        iphc_crtp_fh_flags, 0xc0,
        "The flags of the full header packet.", HFILL
      }
    },
    {
      &hf_iphc_crtp_cs_flags,
      {
        "Flags", "crtp.flags",
        FT_UINT8, BASE_DEC,
        iphc_crtp_cs_flags, 0x0,
        "The flags of the context state packet.", HFILL
      }
    },
    {
      &hf_iphc_crtp_cs_cnt,
      {
        "Count", "crtp.cnt",
        FT_UINT8, BASE_DEC,
        NULL, 0x0,
        "The count of the context state packet.", HFILL
      }
    },
    {
      &hf_iphc_crtp_cs_invalid,
      {
        "Invalid", "crtp.invalid",
        FT_BOOLEAN, 8,
        NULL, 0x80,
        "The invalid bit of the context state packet.", HFILL
      }
    },
 };
	
  static gint *ett[] = {
    &ett_iphc_crtp,
    &ett_iphc_crtp_hdr,
    &ett_iphc_crtp_info,
  };

  proto_iphc_crtp = proto_register_protocol("CRTP", "CRTP", "crtp");
  proto_register_field_array(proto_iphc_crtp, hf, array_length(hf));
  proto_register_subtree_array(ett, array_length(ett));
}
