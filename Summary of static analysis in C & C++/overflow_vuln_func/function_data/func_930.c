void
proto_register_meta(void)
{
    static hf_register_info hf[] = {
        /
        { &hf_meta_schema, { "Schema", "meta.schema", FT_UINT16, BASE_DEC, VALS(meta_schema_vals), 0, NULL, HFILL } },
        { &hf_meta_hdrlen, { "Header Length", "meta.hdrlen", FT_UINT16, BASE_DEC, NULL, 0, NULL, HFILL } },
        { &hf_meta_proto, { "Protocol", "meta.proto", FT_UINT16, BASE_DEC, VALS(meta_proto_vals), 0, NULL, HFILL } },
        { &hf_meta_reserved, { "Reserved", "meta.reserved", FT_UINT16, BASE_HEX, NULL, 0, NULL, HFILL } },

        /
        { &hf_meta_item, { "Unknown Item", "meta.item", FT_NONE, BASE_NONE, NULL, 0, NULL, HFILL } },
        { &hf_meta_item_id, { "Item ID", "meta.item.id", FT_UINT16, BASE_HEX, VALS(meta_id_vals), 0x0, NULL, HFILL } },
        { &hf_meta_item_type, { "Item Type", "meta.item.type", FT_UINT8, BASE_HEX, VALS(meta_type_vals), 0x0, NULL, HFILL } },
        { &hf_meta_item_len, { "Item Length", "meta.item.len", FT_UINT8, BASE_DEC, NULL, 0, NULL, HFILL } },
        { &hf_meta_item_data, { "Item Data", "meta.item.data", FT_BYTES, BASE_NONE, NULL, 0x0, NULL, HFILL } },

        /
        { &hf_meta_item_direction, { "Direction", "meta.direction", FT_UINT8, BASE_DEC, VALS(meta_direction_vals), 0, NULL, HFILL } },
        { &hf_meta_item_ts, { "Timestamp", "meta.timestamp", FT_UINT64, BASE_DEC, NULL, 0, NULL, HFILL } },
        { &hf_meta_item_phylinkid, { "Physical Link ID", "meta.phylinkid", FT_UINT16, BASE_DEC, NULL, 0, NULL, HFILL } },
        { &hf_meta_item_nsapi, { "NSAPI", "meta.nsapi", FT_UINT8, BASE_DEC, NULL, 0, NULL, HFILL } },
        { &hf_meta_item_imsi_digits, { "IMSI digits", "meta.imsi_digits", FT_STRINGZ, BASE_NONE, NULL, 0, NULL, HFILL } },
        { &hf_meta_item_imsi_value, { "IMSI value", "meta.imsi_value", FT_UINT64, BASE_HEX, NULL, 0, NULL, HFILL } },
        { &hf_meta_item_imei_digits, { "IMEI digits", "meta.imei_digits", FT_STRINGZ, BASE_NONE, NULL, 0, NULL, HFILL } },
        { &hf_meta_item_imei_value, { "IMEI value", "meta.imei_value", FT_UINT64, BASE_HEX, NULL, 0, NULL, HFILL } },
        { &hf_meta_item_signaling, { "Signaling", "meta.signaling", FT_BOOLEAN, BASE_NONE, NULL, 0, NULL, HFILL } },
        { &hf_meta_item_incomplete, { "Incomplete", "meta.incomplete", FT_BOOLEAN, BASE_NONE, NULL, 0, NULL, HFILL } },
        { &hf_meta_item_deciphered, { "Deciphered", "meta.deciphered", FT_BOOLEAN, BASE_NONE, NULL, 0, NULL, HFILL } },
        { &hf_meta_item_apn, { "APN", "meta.apn", FT_STRINGZ, BASE_NONE, NULL, 0, NULL, HFILL } },
        { &hf_meta_item_rat, { "RAT", "meta.rat", FT_UINT8, BASE_DEC, NULL, 0, NULL, HFILL } },
        { &hf_meta_item_aal5proto, { "AAL5 Protocol Type", "meta.aal5proto", FT_UINT8, BASE_DEC, VALS(meta_aal5proto_vals), 0, NULL, HFILL } },
        { &hf_meta_item_cell, { "Mobile Cell", "meta.cell", FT_UINT64, BASE_HEX, NULL, 0, NULL, HFILL } },

        { &hf_meta_item_localdevid, { "Local Device ID", "meta.localdevid", FT_UINT16, BASE_DEC, NULL, 0, NULL, HFILL } },
        { &hf_meta_item_remotedevid, { "Remote Device ID", "meta.remotedevid", FT_UINT16, BASE_DEC, NULL, 0, NULL, HFILL } },
        { &hf_meta_item_tapgroupid, { "Tap Group ID", "meta.tapgroupid", FT_UINT16, BASE_DEC, NULL, 0, NULL, HFILL } },
        { &hf_meta_item_tlli, { "TLLI", "meta.tlli", FT_UINT32, BASE_DEC, NULL, 0, NULL, HFILL } },
        { &hf_meta_item_calling, { "Calling Station ID", "meta.calling", FT_STRINGZ, BASE_NONE, NULL, 0, NULL, HFILL } },
        { &hf_meta_item_called, { "Called Station ID", "meta.called", FT_STRINGZ, BASE_NONE, NULL, 0, NULL, HFILL } },
    };

    static gint *ett[] = {
        &ett_meta,
        &ett_meta_item,
        &ett_meta_cell,
        &ett_meta_imsi,
        &ett_meta_imei
    };

    proto_meta = proto_register_protocol("Metadata", "META", "meta");
    register_dissector("meta", dissect_meta, proto_meta);

    proto_register_field_array(proto_meta, hf, array_length(hf));
    proto_register_subtree_array(ett, array_length(ett));

    meta_dissector_table = register_dissector_table("meta.proto",
            "META protocol", FT_UINT16, BASE_DEC);
}
