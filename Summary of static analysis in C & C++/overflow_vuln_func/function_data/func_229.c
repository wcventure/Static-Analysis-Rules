void
proto_register_pana(void)
{
       static hf_register_info hf[] = {
               { &hf_pana_response_in,
                       { "Response In", "pana.response_in",
                       FT_FRAMENUM, BASE_NONE, NULL, 0x0,
                       "The response to this PANA request is in this frame", HFILL }
               },
               { &hf_pana_response_to,
                       { "Request In", "pana.response_to",
                       FT_FRAMENUM, BASE_NONE, NULL, 0x0,
                       "This is a response to the PANA request in this frame", HFILL }
               },
               { &hf_pana_time,
                       { "Time", "pana.time",
                       FT_RELATIVE_TIME, BASE_NONE, NULL, 0x0,
                       "The time between the Call and the Reply", HFILL }
               },
               { &hf_pana_reserved_type,
                       { "PANA Reserved", "pana.reserved",
                       FT_UINT16, BASE_HEX, NULL, 0x0,
                       NULL, HFILL }
               },
               { &hf_pana_length_type,
                       { "PANA Message Length", "pana.length",
                       FT_UINT16, BASE_DEC, NULL, 0x0,
                       NULL, HFILL }
               },


               { &hf_pana_flags,
                       { "Flags", "pana.flags",
                       FT_UINT8, BASE_HEX, NULL, 0x0,
                       NULL, HFILL }
               },
               { &hf_pana_flag_r,
                       { "Request", "pana.flags.r",
                       FT_BOOLEAN, 16, TFS(&tfs_set_notset), PANA_FLAG_R,
                       NULL, HFILL }
               },
               { &hf_pana_flag_s,
                       { "Start", "pana.flags.s",
                       FT_BOOLEAN, 16, TFS(&tfs_set_notset), PANA_FLAG_S,
                       NULL, HFILL }
               },
               { &hf_pana_flag_c,
                       { "Complete","pana.flags.c",
                       FT_BOOLEAN, 16, TFS(&tfs_set_notset), PANA_FLAG_C,
                       NULL, HFILL }
               },
               { &hf_pana_flag_a,
                       { "Auth","pana.flags.a",
                       FT_BOOLEAN, 16, TFS(&tfs_set_notset), PANA_FLAG_A,
                       NULL, HFILL }
               },
               { &hf_pana_flag_p,
                       { "Ping","pana.flags.p",
                       FT_BOOLEAN, 16, TFS(&tfs_set_notset), PANA_FLAG_P,
                       NULL, HFILL }
               },
               { &hf_pana_flag_i,
                       { "IP Reconfig","pana.flags.i",
                       FT_BOOLEAN, 16, TFS(&tfs_set_notset), PANA_FLAG_I,
                       NULL, HFILL }
               },

               { &hf_pana_msg_type,
                       { "PANA Message Type", "pana.type",
                       FT_UINT16, BASE_DEC, NULL, 0x0,
                       NULL, HFILL }
               },
               { &hf_pana_session_id,
                       { "PANA Session ID", "pana.sid",
                       FT_UINT32, BASE_HEX, NULL, 0x0,
                       NULL, HFILL }
               },
               { &hf_pana_seqnumber,
                       { "PANA Sequence Number", "pana.seq",
                       FT_UINT32, BASE_HEX, NULL, 0x0,
                       NULL, HFILL }
               },


               { &hf_pana_avp_code,
                       { "AVP Code", "pana.avp.code",
                       FT_UINT16, BASE_DEC, NULL, 0x0,
                       NULL, HFILL }
               },
               { &hf_pana_avp_length,
                       { "AVP Length", "pana.avp.length",
                       FT_UINT16, BASE_DEC, NULL, 0x0,
                       NULL, HFILL }
               },
               { &hf_pana_avp_flags,
                       { "AVP Flags", "pana.avp.flags",
                       FT_UINT16, BASE_HEX, NULL, 0x0,
                       NULL, HFILL }
               },
               { &hf_pana_avp_flag_v,
                       { "Vendor", "pana.avp.flags.v",
                       FT_BOOLEAN, 16, TFS(&tfs_set_notset), PANA_AVP_FLAG_V,
                       NULL, HFILL }
               },
               { &hf_pana_avp_reserved,
                       { "AVP Reserved", "pana.avp.reserved",
                       FT_UINT16, BASE_HEX, NULL, 0x0,
                       NULL, HFILL }
               },
               { &hf_pana_avp_vendorid,
                       { "AVP Vendor ID", "pana.avp.vendorid",
                       FT_UINT32, BASE_HEX, NULL, 0x0,
                       NULL, HFILL }
               },


               { &hf_pana_avp_data_uint64,
                       { "Value", "pana.avp.data.uint64",
                       FT_UINT64, BASE_HEX, NULL, 0x0,
                       NULL, HFILL }
               },
               { &hf_pana_avp_data_int64,
                       { "Value", "pana.avp.data.int64",
                       FT_INT64, BASE_DEC, NULL, 0x0,
                       NULL, HFILL }
               },
               { &hf_pana_avp_data_uint32,
                       { "Value", "pana.avp.data.uint32",
                       FT_UINT32, BASE_HEX, NULL, 0x0,
                       NULL, HFILL }
               },
               { &hf_pana_avp_data_int32,
                       { "Value", "pana.avp.data.int32",
                       FT_INT32, BASE_DEC, NULL, 0x0,
                       NULL, HFILL }
               },
               { &hf_pana_avp_data_bytes,
                       { "Value", "pana.avp.data.bytes",
                       FT_BYTES, BASE_NONE, NULL, 0x0,
                       NULL, HFILL }
               },
               { &hf_pana_avp_data_string,
                       { "Value", "pana.avp.data.string",
                       FT_STRING, BASE_NONE, NULL, 0x0,
                       NULL, HFILL }
               },
               { &hf_pana_avp_data_enumerated,
                       { "Value", "pana.avp.data.enum",
                       FT_INT32, BASE_DEC, NULL, 0x0,
                       NULL, HFILL }
               }

       };

       /
       static gint *ett[] = {
               &ett_pana,
               &ett_pana_flags,
               &ett_pana_avp,
               &ett_pana_avp_info,
               &ett_pana_avp_flags
       };

       /
       proto_pana = proto_register_protocol("Protocol for carrying Authentication for Network Access",
           "PANA", "pana");

       /
       proto_register_field_array(proto_pana, hf, array_length(hf));
       proto_register_subtree_array(ett, array_length(ett));

}
