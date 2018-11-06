void proto_register_zbee_zcl_appl_ctrl
  (
    void
  )
{
    guint i, j;

    static hf_register_info hf[] = {

        { &hf_zbee_zcl_appl_ctrl_attr_id,
            { "Attribute", "zbee_zcl_general.applctrl.attr_id", FT_UINT16, BASE_HEX, VALS(zbee_zcl_appl_ctrl_attr_names),
            0x0, NULL, HFILL } },

        { &hf_zbee_zcl_appl_ctrl_time_mm,
            { "Minutes", "zbee_zcl_general.applctrl.time.mm", FT_UINT16, BASE_DEC, NULL, ZBEE_ZCL_APPL_CTRL_TIME_MM,
            NULL, HFILL } },

        { &hf_zbee_zcl_appl_ctrl_time_encoding_type,
            { "Encoding Type", "zbee_zcl_general.applctrl.time.encoding_type", FT_UINT16, BASE_HEX, VALS(zbee_zcl_appl_ctrl_time_encoding_type_names),
            ZBEE_ZCL_APPL_CTRL_TIME_ENCOD_TYPE, NULL, HFILL } },

        { &hf_zbee_zcl_appl_ctrl_time_hh,
            { "Hours", "zbee_zcl_general.applctrl.time.hh", FT_UINT16, BASE_DEC, NULL, ZBEE_ZCL_APPL_CTRL_TIME_HH,
            NULL, HFILL } },

        { &hf_zbee_zcl_appl_ctrl_srv_tx_cmd_id,
            { "Command", "zbee_zcl_general.applctrl.cmd.srv_tx.id", FT_UINT8, BASE_HEX, VALS(zbee_zcl_appl_ctrl_srv_tx_cmd_names),
            0x0, NULL, HFILL } },

        { &hf_zbee_zcl_appl_ctrl_srv_rx_cmd_id,
            { "Command", "zbee_zcl_general.applctrl.cmd.srv_rx.id", FT_UINT8, BASE_HEX, VALS(zbee_zcl_appl_ctrl_srv_rx_cmd_names),
            0x0, NULL, HFILL } },

        { &hf_zbee_zcl_appl_ctrl_appl_status,
            { "Appliance Status", "zbee_zcl_general.applctrl.status", FT_UINT8, BASE_HEX, VALS(zbee_zcl_appl_ctrl_appl_status_names),
            0x0, NULL, HFILL } },

        { &hf_zbee_zcl_appl_ctrl_rem_en_flags,
            { "Remote Enable Flags", "zbee_zcl_general.applctrl.remenflags", FT_UINT8, BASE_HEX, VALS(zbee_zcl_appl_ctrl_rem_flags_names),
            ZBEE_ZCL_APPL_CTRL_REM_EN_FLAGS_FLAGS, NULL, HFILL } },

        { &hf_zbee_zcl_appl_ctrl_status2,
            { "Appliance Status 2", "zbee_zcl_general.applctrl.status2", FT_UINT8, BASE_HEX, VALS(zbee_zcl_appl_ctrl_status2_names),
            ZBEE_ZCL_APPL_CTRL_REM_EN_FLAGS_STATUS2, NULL, HFILL } },

        { &hf_zbee_zcl_appl_ctrl_status2_array,
            { "Appliance Status 2", "zbee_zcl_general.applctrl.status2.array", FT_UINT24, BASE_HEX, NULL,
            0x00, NULL, HFILL } },

        { &hf_zbee_zcl_appl_ctrl_exec_cmd_id,
            { "Command", "zbee_zcl_general.applctrl.execcmd.id", FT_UINT8, BASE_HEX, VALS(zbee_zcl_appl_ctrl_exec_cmd_names),
            0x0, NULL, HFILL } },

        { &hf_zbee_zcl_appl_ctrl_attr_func_id,
            { "ID", "zbee_zcl_general.applctrl.attr_func.id", FT_UINT16, BASE_HEX, VALS(zbee_zcl_appl_ctrl_attr_names),
            0x0, NULL, HFILL } },

        { &hf_zbee_zcl_appl_ctrl_attr_func_data_type,
            { "Data Type", "zbee_zcl_general.applctrl.attr_func.datatype", FT_UINT8, BASE_HEX, VALS(zbee_zcl_short_data_type_names),
            0x0, NULL, HFILL } },

        { &hf_zbee_zcl_appl_ctrl_warning_id,
            { "Warning", "zbee_zcl_general.applctrl.ovrlwarning.id", FT_UINT8, BASE_HEX, VALS(zbee_zcl_appl_ctrl_ovrl_warning_names),
            0x0, NULL, HFILL } }

    };

    /
    gint *ett[ZBEE_ZCL_APPL_CTRL_NUM_ETT];

    ett[0] = &ett_zbee_zcl_appl_ctrl;
    ett[1] = &ett_zbee_zcl_appl_ctrl_flags;
    ett[2] = &ett_zbee_zcl_appl_ctrl_time;

    /
    for ( i = 0, j = ZBEE_ZCL_APPL_CTRL_NUM_GENERIC_ETT; i < ZBEE_ZCL_APPL_CTRL_NUM_FUNC_ETT; i++, j++) {
        ett_zbee_zcl_appl_ctrl_func[i] = -1;
        ett[j] = &ett_zbee_zcl_appl_ctrl_func[i];
    }

    /
    proto_zbee_zcl_appl_ctrl = proto_register_protocol("ZigBee ZCL Appliance Control", "ZCL Appliance Control", ZBEE_PROTOABBREV_ZCL_APPLCTRL);
    proto_register_field_array(proto_zbee_zcl_appl_ctrl, hf, array_length(hf));
    proto_register_subtree_array(ett, array_length(ett));

    /
    register_dissector(ZBEE_PROTOABBREV_ZCL_APPLCTRL, dissect_zbee_zcl_appl_ctrl, proto_zbee_zcl_appl_ctrl);

} /
