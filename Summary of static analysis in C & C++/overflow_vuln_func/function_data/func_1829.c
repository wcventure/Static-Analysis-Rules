void
proto_register_ndps(void)
{
	static hf_register_info hf_ndps[] = {
		{ &hf_ndps_record_mark,
		{ "Record Mark",		"ndps.record_mark", FT_UINT16, BASE_HEX, NULL, 0x0,
			"Record Mark", HFILL }},

        { &hf_ndps_packet_type,
        { "Packet Type",    "ndps.packet_type",
          FT_UINT32,    BASE_HEX,   VALS(ndps_packet_types),   0x0,
          "Packet Type", HFILL }},

        { &hf_ndps_length,
        { "Record Length",    "ndps.record_length",
           FT_UINT16,    BASE_DEC,   NULL,   0x0,
           "Record Length", HFILL }},
        
        { &hf_ndps_xid,
        { "Exchange ID",    "ndps.xid",
           FT_UINT32,    BASE_HEX,   NULL,   0x0,
           "Exchange ID", HFILL }},

        { &hf_ndps_rpc_version,
        { "RPC Version",    "ndps.rpc_version",
           FT_UINT32,    BASE_HEX,   NULL,   0x0,
           "RPC Version", HFILL }},

        { &hf_spx_ndps_program,
        { "NDPS Program Number",    "spx.ndps_program",
          FT_UINT32,    BASE_HEX,   VALS(spx_ndps_program_vals),   0x0,
          "NDPS Program Number", HFILL }},
	
        { &hf_spx_ndps_version,
        { "Program Version",    "spx.ndps_version",
          FT_UINT32,    BASE_DEC,   NULL,   0x0,
          "Program Version", HFILL }}, 
    
        { &hf_ndps_error,
        { "NDPS Error",    "spx.ndps_error",
          FT_UINT32,    BASE_HEX,   NULL,   0x0,
          "NDPS Error", HFILL }},

        { &hf_ndps_other_error_string,
        { "Extended Error String",    "ndps.ext_err_string",
          FT_STRING,    BASE_NONE,   NULL,   0x0,
          "Extended Error String", HFILL }},
        
        { &hf_spx_ndps_func_print,
        { "Print Program",    "spx.ndps_func_print",
          FT_UINT32,    BASE_HEX,   VALS(spx_ndps_print_func_vals),   0x0,
          "Print Program", HFILL }},
        
        { &hf_spx_ndps_func_notify,
        { "Notify Program",    "spx.ndps_func_notify",
          FT_UINT32,    BASE_HEX,   VALS(spx_ndps_notify_func_vals),   0x0,
          "Notify Program", HFILL }},
        
        { &hf_spx_ndps_func_delivery,
        { "Delivery Program",    "spx.ndps_func_delivery",
          FT_UINT32,    BASE_HEX,   VALS(spx_ndps_deliver_func_vals),   0x0,
          "Delivery Program", HFILL }},
        
        { &hf_spx_ndps_func_registry,
        { "Registry Program",    "spx.ndps_func_registry",
          FT_UINT32,    BASE_HEX,   VALS(spx_ndps_registry_func_vals),   0x0,
          "Registry Program", HFILL }},
        
        { &hf_spx_ndps_func_resman,
        { "ResMan Program",    "spx.ndps_func_resman",
          FT_UINT32,    BASE_HEX,   VALS(spx_ndps_resman_func_vals),   0x0,
          "ResMan Program", HFILL }},
        
        { &hf_spx_ndps_func_broker,
        { "Broker Program",    "spx.ndps_func_broker",
          FT_UINT32,    BASE_HEX,   VALS(spx_ndps_broker_func_vals),   0x0,
          "Broker Program", HFILL }},
        
        { &hf_ndps_num_objects,
        { "Number of Objects",    "ndps.num_objects",
          FT_UINT32,    BASE_DEC,   NULL,   0x0,
          "Number of Objects", HFILL }},

        { &hf_ndps_num_attributes,
        { "Number of Attributes",    "ndps.num_attributes",
          FT_UINT32,    BASE_DEC,   NULL,   0x0,
          "Number of Attributes", HFILL }},

        { &hf_ndps_sbuffer,
        { "Server",    "ndps.sbuffer",
          FT_UINT32,    BASE_DEC,   NULL,   0x0,
          "Server", HFILL }},
        
        { &hf_ndps_rbuffer,
        { "Connection",    "ndps.rbuffer",
          FT_UINT32,    BASE_DEC,   NULL,   0x0,
          "Connection", HFILL }},

        { &hf_ndps_user_name,
        { "Trustee Name",    "ndps.user_name",
          FT_STRING,    BASE_NONE,   NULL,   0x0,
          "Trustee Name", HFILL }},
        
        { &hf_ndps_broker_name,
        { "Broker Name",    "ndps.broker_name",
          FT_STRING,    BASE_NONE,   NULL,   0x0,
          "Broker Name", HFILL }},

        { &hf_ndps_num_results,
        { "Number of Results",    "ndps.num_results",
          FT_UINT32,    BASE_DEC,   NULL,   0x0,
          "Number of Results", HFILL }},

        { &hf_ndps_num_options,
        { "Number of Options",    "ndps.num_options",
          FT_UINT32,    BASE_DEC,   NULL,   0x0,
          "Number of Options", HFILL }},

        { &hf_ndps_num_jobs,
        { "Number of Jobs",    "ndps.num_jobs",
          FT_UINT32,    BASE_DEC,   NULL,   0x0,
          "Number of Jobs", HFILL }},

        { &hf_ndps_pa_name,
        { "Printer Name",    "ndps.pa_name",
          FT_STRING,    BASE_NONE,   NULL,   0x0,
          "Printer Name", HFILL }},
        
        { &hf_ndps_tree,
        { "Tree",    "ndps.tree",
          FT_STRING,    BASE_NONE,   NULL,   0x0,
          "Tree", HFILL }},

        { &hf_ndps_reqframe,
        { "Request Frame",    "ndps.reqframe",
          FT_FRAMENUM,  BASE_NONE,   NULL,   0x0,
          "Request Frame", HFILL }},

        { &hf_ndps_error_val,
        { "Return Status",    "ndps.error_val",
          FT_UINT32,    BASE_HEX,   VALS(ndps_error_types),   0x0,
          "Return Status", HFILL }},

        { &hf_ndps_object,
        { "Object ID",    "ndps.object",
          FT_UINT32,    BASE_HEX,   NULL,   0x0,
          "Object ID", HFILL }},

        { &hf_ndps_cred_type,
        { "Credential Type",    "ndps.cred_type",
          FT_UINT32,    BASE_HEX,   VALS(ndps_credential_enum),   0x0,
          "Credential Type", HFILL }},

        { &hf_ndps_server_name,
        { "Server Name",    "ndps.server_name",
          FT_STRING,    BASE_DEC,   NULL,   0x0,
          "Server Name", HFILL }},

        { &hf_ndps_connection,
        { "Connection",    "ndps.connection",
          FT_UINT16,    BASE_DEC,   NULL,   0x0,
          "Connection", HFILL }},

        { &hf_ndps_ext_error,
        { "Extended Error Code",    "ndps.ext_error",
          FT_UINT32,    BASE_HEX,   NULL,   0x0,
          "Extended Error Code", HFILL }},

        { &hf_ndps_auth_null,
        { "Auth Null",    "ndps.auth_null",
          FT_BYTES,    BASE_NONE,   NULL,   0x0,
          "Auth Null", HFILL }},

        { &hf_ndps_rpc_accept,
        { "RPC Accept or Deny",    "ndps.rpc_acc",
          FT_UINT32,    BASE_HEX,   VALS(true_false),   0x0,
          "RPC Accept or Deny", HFILL }},

        { &hf_ndps_rpc_acc_stat,
        { "RPC Accept Status",    "ndps.rpc_acc_stat",
          FT_UINT32,    BASE_HEX,   VALS(accept_stat),   0x0,
          "RPC Accept Status", HFILL }},
        
        { &hf_ndps_rpc_rej_stat,
        { "RPC Reject Status",    "ndps.rpc_rej_stat",
          FT_UINT32,    BASE_HEX,   VALS(reject_stat),   0x0,
          "RPC Reject Status", HFILL }},
        
        { &hf_ndps_rpc_acc_results,
        { "RPC Accept Results",    "ndps.rpc_acc_res",
          FT_UINT32,    BASE_HEX,   NULL,   0x0,
          "RPC Accept Results", HFILL }},

        { &hf_ndps_problem_type,
        { "Problem Type",    "ndps.rpc_prob_type",
          FT_UINT32,    BASE_HEX,   VALS(error_type_enum),   0x0,
          "Problem Type", HFILL }},
    
        { &hf_security_problem_type,
        { "Security Problem",    "ndps.rpc_sec_prob",
          FT_UINT32,    BASE_HEX,   VALS(security_problem_enum),   0x0,
          "Security Problem", HFILL }},

        { &hf_service_problem_type,
        { "Service Problem",    "ndps.rpc_serv_prob",
          FT_UINT32,    BASE_HEX,   VALS(service_problem_enum),   0x0,
          "Service Problem", HFILL }},
        
        { &hf_access_problem_type,
        { "Access Problem",    "ndps.rpc_acc_prob",
          FT_UINT32,    BASE_HEX,   VALS(access_problem_enum),   0x0,
          "Access Problem", HFILL }},
        
        { &hf_printer_problem_type,
        { "Printer Problem",    "ndps.rpc_print_prob",
          FT_UINT32,    BASE_HEX,   VALS(printer_problem_enum),   0x0,
          "Printer Problem", HFILL }},
        
        { &hf_selection_problem_type,
        { "Selection Problem",    "ndps.rpc_sel_prob",
          FT_UINT32,    BASE_HEX,   VALS(selection_problem_enum),   0x0,
          "Selection Problem", HFILL }},
        
        { &hf_doc_access_problem_type,
        { "Document Access Problem",    "ndps.rpc_doc_acc_prob",
          FT_UINT32,    BASE_HEX,   VALS(doc_access_problem_enum),   0x0,
          "Document Access Problem", HFILL }},
        
        { &hf_attribute_problem_type,
        { "Attribute Problem",    "ndps.rpc_attr_prob",
          FT_UINT32,    BASE_HEX,   VALS(attribute_problem_enum),   0x0,
          "Attribute Problem", HFILL }},

        { &hf_update_problem_type,
        { "Update Problem",    "ndps.rpc_update_prob",
          FT_UINT32,    BASE_HEX,   VALS(update_problem_enum),   0x0,
          "Update Problem", HFILL }},
        
        { &hf_obj_id_type,
        { "Object ID Type",    "ndps.rpc_obj_id_type",
          FT_UINT32,    BASE_HEX,   VALS(obj_identification_enum),   0x0,
          "Object ID Type", HFILL }},

        { &hf_oid_struct_size,
        { "OID Struct Size",    "ndps.rpc_oid_struct_size",
          FT_UINT16,    BASE_DEC,   NULL,   0x0,
          "OID Struct Size", HFILL }},
        
        { &hf_object_name,
        { "Object Name",    "ndps.ndps_object_name",
          FT_STRING,    BASE_NONE,   NULL,   0x0,
          "Object Name", HFILL }},

        { &hf_ndps_document_number,
        { "Document Number",    "ndps.ndps_doc_num",
          FT_UINT32,    BASE_HEX,   NULL,   0x0,
          "Document Number", HFILL }},

        { &hf_ndps_doc_content,
        { "Document Content",    "ndps.ndps_doc_content",
          FT_UINT32,    BASE_HEX,   NULL,   0x0,
          "Document Content", HFILL }},

        { &hf_ndps_nameorid,
        { "Name or ID Type",    "ndps.ndps_nameorid",
          FT_UINT32,    BASE_HEX,   VALS(nameorid_enum),   0x0,
          "Name or ID Type", HFILL }},

        { &hf_local_object_name,
        { "Local Object Name",    "ndps.ndps_loc_object_name",
          FT_STRING,    BASE_NONE,   NULL,   0x0,
          "Local Object Name", HFILL }},

        { &hf_printer_name,
        { "Printer Name",    "ndps.ndps_printer_name",
          FT_STRING,    BASE_NONE,   NULL,   0x0,
          "Printer Name", HFILL }},

        { &hf_ndps_qualified_name,
        { "Qualified Name Type",    "ndps.ndps_qual_name_type",
          FT_UINT32,    BASE_HEX,   VALS(qualified_name_enum),   0x0,
          "Qualified Name Type", HFILL }},

        { &hf_ndps_qualified_name2,
        { "Qualified Name Type",    "ndps.ndps_qual_name_type2",
          FT_UINT32,    BASE_HEX,   VALS(qualified_name_enum2),   0x0,
          "Qualified Name Type", HFILL }},
        
        { &hf_ndps_item_count,
        { "Number of Items",    "ndps.ndps_item_count",
          FT_UINT32,    BASE_DEC,   NULL,   0x0,
          "Number of Items", HFILL }},

        { &hf_ndps_num_passwords,
        { "Number of Passwords",    "ndps.num_passwords",
          FT_UINT32,    BASE_DEC,   NULL,   0x0,
          "Number of Passwords", HFILL }},

        { &hf_ndps_num_servers,
        { "Number of Servers",    "ndps.num_servers",
          FT_UINT32,    BASE_DEC,   NULL,   0x0,
          "Number of Servers", HFILL }},

        { &hf_ndps_num_locations,
        { "Number of Locations",    "ndps.num_locations",
          FT_UINT32,    BASE_DEC,   NULL,   0x0,
          "Number of Locations", HFILL }},

        { &hf_ndps_num_areas,
        { "Number of Areas",    "ndps.num_areas",
          FT_UINT32,    BASE_DEC,   NULL,   0x0,
          "Number of Areas", HFILL }},

        { &hf_ndps_num_address_items,
        { "Number of Address Items",    "ndps.num_address_items",
          FT_UINT32,    BASE_DEC,   NULL,   0x0,
          "Number of Address Items", HFILL }},

        { &hf_ndps_num_job_categories,
        { "Number of Job Categories",    "ndps.num_job_categories",
          FT_UINT32,    BASE_DEC,   NULL,   0x0,
          "Number of Job Categories", HFILL }},

        { &hf_ndps_num_page_selects,
        { "Number of Page Select Items",    "ndps.num_page_selects",
          FT_UINT32,    BASE_DEC,   NULL,   0x0,
          "Number of Page Select Items", HFILL }},

        { &hf_ndps_num_page_informations,
        { "Number of Page Information Items",    "ndps.num_page_informations",
          FT_UINT32,    BASE_DEC,   NULL,   0x0,
          "Number of Page Information Items", HFILL }},

        { &hf_ndps_num_names,
        { "Number of Names",    "ndps.num_names",
          FT_UINT32,    BASE_DEC,   NULL,   0x0,
          "Number of Names", HFILL }},

        { &hf_ndps_num_categories,
        { "Number of Categories",    "ndps.num_categories",
          FT_UINT32,    BASE_DEC,   NULL,   0x0,
          "Number of Categories", HFILL }},

        { &hf_ndps_num_colorants,
        { "Number of Colorants",    "ndps.num_colorants",
          FT_UINT32,    BASE_DEC,   NULL,   0x0,
          "Number of Colorants", HFILL }},

        { &hf_ndps_num_events,
        { "Number of Events",    "ndps.num_events",
          FT_UINT32,    BASE_DEC,   NULL,   0x0,
          "Number of Events", HFILL }},

        { &hf_ndps_num_args,
        { "Number of Arguments",    "ndps.num_argss",
          FT_UINT32,    BASE_DEC,   NULL,   0x0,
          "Number of Arguments", HFILL }},
        
        { &hf_ndps_num_transfer_methods,
        { "Number of Transfer Methods",    "ndps.num_transfer_methods",
          FT_UINT32,    BASE_DEC,   NULL,   0x0,
          "Number of Transfer Methods", HFILL }},
        
        { &hf_ndps_num_doc_types,
        { "Number of Document Types",    "ndps.num_doc_types",
          FT_UINT32,    BASE_DEC,   NULL,   0x0,
          "Number of Document Types", HFILL }},
        
        { &hf_ndps_num_destinations,
        { "Number of Destinations",    "ndps.num_destinations",
          FT_UINT32,    BASE_DEC,   NULL,   0x0,
          "Number of Destinations", HFILL }},
        
        { &hf_ndps_qualifier,
        { "Qualifier",    "ndps.ndps_qual",
          FT_UINT32,    BASE_HEX,   NULL,   0x0,
          "Qualifier", HFILL }},

        { &hf_ndps_lib_error,
        { "Library Error",    "ndps.ndps_lib_error",
          FT_UINT32,    BASE_HEX,   VALS(ndps_error_types),   0x0,
          "Library Error", HFILL }},

        { &hf_ndps_other_error,
        { "Other Error",    "ndps.ndps_other_error",
          FT_UINT32,    BASE_HEX,   VALS(ndps_error_types),   0x0,
          "Other Error", HFILL }},

        { &hf_ndps_other_error_2,
        { "Other Error 2",    "ndps.ndps_other_error_2",
          FT_UINT32,    BASE_HEX,   VALS(ndps_error_types),   0x0,
          "Other Error 2", HFILL }},

        { &hf_ndps_session,
        { "Session Handle",    "ndps.ndps_session",
          FT_UINT32,    BASE_HEX,   NULL,   0x0,
          "Session Handle", HFILL }},

        { &hf_ndps_abort_flag,
        { "Abort?",    "ndps.ndps_abort",
          FT_BOOLEAN,    BASE_NONE,   NULL,   0x0,
          "Abort?", HFILL }},

        { &hf_obj_attribute_type,
        { "Value Syntax",    "ndps.ndps_attrib_type",
          FT_UINT32,    BASE_HEX,   VALS(ndps_attribute_enum),   0x0,
          "Value Syntax", HFILL }},

        { &hf_ndps_attribute_value,
        { "Value",    "ndps.attribue_value",
          FT_UINT32,    BASE_HEX,   NULL,   0x0,
          "Value", HFILL }},

        { &hf_ndps_lower_range,
        { "Lower Range",    "ndps.lower_range",
          FT_UINT32,    BASE_HEX,   NULL,   0x0,
          "Lower Range", HFILL }},

        { &hf_ndps_upper_range,
        { "Upper Range",    "ndps.upper_range",
          FT_UINT32,    BASE_HEX,   NULL,   0x0,
          "Upper Range", HFILL }},

        { &hf_ndps_n64,
        { "Value",    "ndps.n64",
          FT_BYTES,    BASE_NONE,   NULL,   0x0,
          "Value", HFILL }},

        { &hf_ndps_lower_range_n64,
        { "Lower Range",    "ndps.lower_range_n64",
          FT_BYTES,    BASE_NONE,   NULL,   0x0,
          "Lower Range", HFILL }},

        { &hf_ndps_upper_range_n64,
        { "Upper Range",    "ndps.upper_range_n64",
          FT_BYTES,    BASE_NONE,   NULL,   0x0,
          "Upper Range", HFILL }},

        { &hf_ndps_attrib_boolean,
        { "Value?",    "ndps.ndps_attrib_boolean",
          FT_BOOLEAN,    BASE_NONE,   NULL,   0x0,
          "Value?", HFILL }},

        { &hf_ndps_realization,
        { "Realization Type",    "ndps.ndps_realization",
          FT_UINT32,    BASE_HEX,   VALS(ndps_realization_enum),   0x0,
          "Realization Type", HFILL }},

        { &hf_ndps_xdimension_n64,
        { "X Dimension",    "ndps.xdimension_n64",
          FT_BYTES,    BASE_NONE,   NULL,   0x0,
          "X Dimension", HFILL }},

        { &hf_ndps_ydimension_n64,
        { "Y Dimension",    "ndps.xdimension_n64",
          FT_BYTES,    BASE_NONE,   NULL,   0x0,
          "Y Dimension", HFILL }},

        { &hf_ndps_dim_value,
        { "Dimension Value Type",    "ndps.ndps_dim_value",
          FT_UINT32,    BASE_HEX,   VALS(ndps_dim_value_enum),   0x0,
          "Dimension Value Type", HFILL }},

        { &hf_ndps_dim_flag,
        { "Dimension Flag",    "ndps.ndps_dim_falg",
          FT_UINT32,    BASE_HEX,   NULL,   0x0,
          "Dimension Flag", HFILL }},

        { &hf_ndps_xydim_value,
        { "XY Dimension Value Type",    "ndps.ndps_xydim_value",
          FT_UINT32,    BASE_HEX,   VALS(ndps_xydim_value_enum),   0x0,
          "XY Dimension Value Type", HFILL }},

        { &hf_ndps_location_value,
        { "Location Value Type",    "ndps.ndps_location_value",
          FT_UINT32,    BASE_HEX,   VALS(ndps_location_value_enum),   0x0,
          "Location Value Type", HFILL }},

        { &hf_ndps_xmin_n64,
        { "Minimum X Dimension",    "ndps.xmin_n64",
          FT_BYTES,    BASE_NONE,   NULL,   0x0,
          "Minimum X Dimension", HFILL }},

        { &hf_ndps_xmax_n64,
        { "Maximum X Dimension",    "ndps.xmax_n64",
          FT_BYTES,    BASE_NONE,   NULL,   0x0,
          "Maximum X Dimension", HFILL }},

        { &hf_ndps_ymin_n64,
        { "Minimum Y Dimension",    "ndps.ymin_n64",
          FT_BYTES,    BASE_NONE,   NULL,   0x0,
          "Minimum Y Dimension", HFILL }},

        { &hf_ndps_ymax_n64,
        { "Maximum Y Dimension",    "ndps.ymax_n64",
          FT_BYTES,    BASE_NONE,   NULL,   0x0,
          "Maximum Y Dimension", HFILL }},

        { &hf_ndps_edge_value,
        { "Edge Value",    "ndps.ndps_edge_value",
          FT_UINT32,    BASE_HEX,   VALS(ndps_edge_value_enum),   0x0,
          "Edge Value", HFILL }},

        { &hf_ndps_cardinal_or_oid,
        { "Cardinal or OID",    "ndps.ndps_car_or_oid",
          FT_UINT32,    BASE_HEX,   VALS(ndps_card_or_oid_enum),   0x0,
          "Cardinal or OID", HFILL }},

        { &hf_ndps_cardinal_name_or_oid,
        { "Cardinal Name or OID",    "ndps.ndps_car_name_or_oid",
          FT_UINT32,    BASE_HEX,   VALS(ndps_card_name_or_oid_enum),   0x0,
          "Cardinal Name or OID", HFILL }},

        { &hf_ndps_integer_or_oid,
        { "Integer or OID",    "ndps.ndps_integer_or_oid",
          FT_UINT32,    BASE_HEX,   VALS(ndps_integer_or_oid_enum),   0x0,
          "Integer or OID", HFILL }},

        { &hf_ndps_profile_id,
        { "Profile ID",    "ndps.ndps_profile_id",
          FT_UINT32,    BASE_HEX,   NULL,   0x0,
          "Profile ID", HFILL }},

        { &hf_ndps_persistence,
        { "Persistence",    "ndps.ndps_persistence",
          FT_UINT32,    BASE_HEX,   VALS(ndps_persistence_enum),   0x0,
          "Persistence", HFILL }},

        { &hf_ndps_language_count,
        { "Number of Languages",    "ndps.ndps_language_count",
          FT_UINT32,    BASE_DEC,   NULL,   0x0,
          "Number of Languages", HFILL }},

        { &hf_ndps_language_id,
        { "Language ID",    "ndps.ndps_lang_id",
          FT_UINT32,    BASE_HEX,   NULL,   0x0,
          "Language ID", HFILL }},

        { &hf_address_type,
        { "Address Type",    "ndps.ndps_address_type",
          FT_UINT32,    BASE_HEX,   VALS(ndps_address_type_enum),   0x0,
          "Address Type", HFILL }},

        { &hf_ndps_address,
        { "Address",    "ndps.ndps_address",
          FT_UINT32,    BASE_HEX,   VALS(ndps_address_enum),   0x0,
          "Address", HFILL }},

        { &hf_ndps_add_bytes,
        { "Address Bytes",    "ndps.add_bytes",
          FT_BYTES,    BASE_NONE,   NULL,   0x0,
          "Address Bytes", HFILL }},

        { &hf_ndps_event_type,
        { "Event Type",    "ndps.ndps_event_type",
          FT_UINT32,    BASE_HEX,   NULL,   0x0,
          "Event Type", HFILL }},

        { &hf_ndps_event_object_identifier,
        { "Event Object Type",    "ndps.ndps_event_object_identifier",
          FT_UINT32,    BASE_HEX,   VALS(ndps_event_object_enum),   0x0,
          "Event Object Type", HFILL }},

        { &hf_ndps_octet_string,
        { "Octet String",    "ndps.octet_string",
          FT_BYTES,    BASE_NONE,   NULL,   0x0,
          "Octet String", HFILL }},

        { &hf_ndps_scope,
        { "Scope",    "ndps.scope",
          FT_UINT32,    BASE_HEX,   NULL,   0x0,
          "Scope", HFILL }},

        { &hf_address_len,
        { "Address Length",    "ndps.addr_len",
          FT_UINT32,    BASE_DEC,   NULL,   0x0,
          "Address Length", HFILL }},

        { &hf_ndps_net,
        { "IPX Network",    "ndps.net",
          FT_IPXNET,    BASE_NONE,   NULL,   0x0,
          "Scope", HFILL }},

        { &hf_ndps_node,
        { "Node",    "ndps.node",
          FT_ETHER,    BASE_NONE,   NULL,   0x0,
          "Node", HFILL }},

        { &hf_ndps_socket,
        { "IPX Socket",    "ndps.socket",
          FT_UINT16,    BASE_HEX,   NULL,   0x0,
          "IPX Socket", HFILL }},

        { &hf_ndps_port,
        { "IP Port",    "ndps.port",
          FT_UINT16,    BASE_DEC,   NULL,   0x0,
          "IP Port", HFILL }},

        { &hf_ndps_ip,
        { "IP Address",    "ndps.ip",
          FT_IPv4,    BASE_DEC,   NULL,   0x0,
          "IP Address", HFILL }},
        
        { &hf_ndps_server_type,
        { "NDPS Server Type",    "ndps.ndps_server_type",
          FT_UINT32,    BASE_HEX,   VALS(ndps_server_type_enum),   0x0,
          "NDPS Server Type", HFILL }},

        { &hf_ndps_num_services,
        { "Number of Services",    "ndps.ndps_num_services",
          FT_UINT32,    BASE_DEC,   NULL,   0x0,
          "Number of Services", HFILL }},
        
        { &hf_ndps_service_type,
        { "NDPS Service Type",    "ndps.ndps_service_type",
          FT_UINT32,    BASE_HEX,   VALS(ndps_service_type_enum),   0x0,
          "NDPS Service Type", HFILL }},
    
        { &hf_ndps_service_enabled,
        { "Service Enabled?",    "ndps.ndps_service_enabled",
          FT_BOOLEAN,    BASE_NONE,   NULL,   0x0,
          "Service Enabled?", HFILL }},

        { &hf_ndps_method_name,
        { "Method Name",    "ndps.method_name",
          FT_STRING,    BASE_NONE,   NULL,   0x0,
          "Method Name", HFILL }},

        { &hf_ndps_method_ver,
        { "Method Version",    "ndps.method_ver",
          FT_STRING,    BASE_NONE,   NULL,   0x0,
          "Method Version", HFILL }},

        { &hf_ndps_file_name,
        { "File Name",    "ndps.file_name",
          FT_STRING,    BASE_NONE,   NULL,   0x0,
          "File Name", HFILL }},
        
        { &hf_ndps_admin_submit,
        { "Admin Submit Flag?",    "ndps.admin_submit_flag",
          FT_BOOLEAN,    BASE_NONE,   NULL,   0x0,
          "Admin Submit Flag?", HFILL }},

        { &hf_ndps_oid,
        { "Object ID",    "ndps.oid",
          FT_BYTES,    BASE_NONE,   NULL,   0x0,
          "Object ID", HFILL }},

        { &hf_ndps_object_op,
        { "Operation",    "ndps.ndps_object_op",
          FT_UINT32,    BASE_HEX,   VALS(ndps_object_op_enum),   0x0,
          "Operation", HFILL }},

        { &hf_answer_time,
        { "Answer Time",    "ndps.answer_time",
          FT_UINT32,    BASE_DEC,   NULL,   0x0,
          "Answer Time", HFILL }},
        
        { &hf_oid_asn1_type,
        { "ASN.1 Type",    "ndps.asn1_type",
          FT_UINT16,    BASE_DEC,   NULL,   0x0,
          "ASN.1 Type", HFILL }},

        { &hf_ndps_item_ptr,
        { "Item Pointer",    "ndps.item_ptr",
          FT_BYTES,    BASE_DEC,   NULL,   0x0,
          "Item Pointer", HFILL }},

        { &hf_ndps_len,
        { "Length",    "ndps.ndps_len",
          FT_UINT16,    BASE_DEC,   NULL,   0x0,
          "Length", HFILL }},
     
        { &hf_limit_enc,
        { "Limit Encountered",    "ndps.ndps_limit_enc",
          FT_UINT32,    BASE_HEX,   VALS(ndps_limit_enc_enum),   0x0,
          "Limit Encountered", HFILL }},
        
        { &hf_ndps_delivery_add_count,
        { "Number of Delivery Addresses",    "ndps.delivery_add_count",
          FT_UINT32,    BASE_DEC,   NULL,   0x0,
          "Number of Delivery Addresses", HFILL }},
        
        { &hf_ndps_delivery_add_type,
        { "Delivery Address Type",    "ndps.ndps_delivery_add_type",
          FT_UINT32,    BASE_HEX,   VALS(ndps_delivery_add_enum),   0x0,
          "Delivery Address Type", HFILL }},

        { &hf_ndps_criterion_type,
        { "Criterion Type",    "ndps.ndps_criterion_type",
          FT_UINT32,    BASE_HEX,   VALS(ndps_attribute_enum),   0x0,
          "Criterion Type", HFILL }},

        { &hf_ndps_num_ignored_attributes,
        { "Number of Ignored Attributes",    "ndps.num_ignored_attributes",
          FT_UINT32,    BASE_DEC,   NULL,   0x0,
          "Number of Ignored Attributes", HFILL }},

        { &hf_ndps_ignored_type,
        { "Ignored Type",    "ndps.ndps_ignored_type",
          FT_UINT32,    BASE_HEX,   VALS(ndps_attribute_enum),   0x0,
          "Ignored Type", HFILL }},

        { &hf_ndps_num_resources,
        { "Number of Resources",    "ndps.ndps_num_resources",
          FT_UINT32,    BASE_DEC,   NULL,   0x0,
          "Number of Resources", HFILL }},

        { &hf_ndps_resource_type,
        { "Resource Type",    "ndps.ndps_resource_type",
          FT_UINT32,    BASE_HEX,   VALS(ndps_resource_enum),   0x0,
          "Resource Type", HFILL }},
      
        { &hf_ndps_identifier_type,
        { "Identifier Type",    "ndps.ndps_identifier_type",
          FT_UINT32,    BASE_HEX,   VALS(ndps_identifier_enum),   0x0,
          "Identifier Type", HFILL }},

        { &hf_ndps_page_flag,
        { "Page Flag",    "ndps.ndps_page_flag",
          FT_UINT32,    BASE_HEX,   NULL,   0x0,
          "Page Flag", HFILL }},
        
        { &hf_ndps_media_type,
        { "Media Type",    "ndps.ndps_media_type",
          FT_UINT32,    BASE_HEX,   VALS(ndps_media_enum),   0x0,
          "Media Type", HFILL }},

        { &hf_ndps_page_size,
        { "Page Size",    "ndps.ndps_page_size",
          FT_UINT32,    BASE_HEX,   VALS(ndps_page_size_enum),   0x0,
          "Page Size", HFILL }},
     
        { &hf_ndps_direction,
        { "Direction",    "ndps.ndps_direction",
          FT_UINT32,    BASE_HEX,   VALS(ndps_pres_direction_enum),   0x0,
          "Direction", HFILL }},
       
        { &hf_ndps_page_order,
        { "Page Order",    "ndps.ndps_page_order",
          FT_UINT32,    BASE_HEX,   VALS(ndps_page_order_enum),   0x0,
          "Page Order", HFILL }},

        { &hf_ndps_medium_size,
        { "Medium Size",    "ndps.ndps_medium_size",
          FT_UINT32,    BASE_HEX,   VALS(ndps_medium_size_enum),   0x0,
          "Medium Size", HFILL }},

        { &hf_ndps_long_edge_feeds,
        { "Long Edge Feeds?",    "ndps.ndps_long_edge_feeds",
          FT_BOOLEAN,    BASE_NONE,   NULL,   0x0,
          "Long Edge Feeds?", HFILL }},

        { &hf_ndps_inc_across_feed,
        { "Increment Across Feed",    "ndps.inc_across_feed",
          FT_BYTES,    BASE_NONE,   NULL,   0x0,
          "Increment Across Feed", HFILL }},

        { &hf_ndps_size_inc_in_feed,
        { "Size Increment in Feed",    "ndps.size_inc_in_feed",
          FT_BYTES,    BASE_NONE,   NULL,   0x0,
          "Size Increment in Feed", HFILL }},

        { &hf_ndps_page_orientation,
        { "Page Orientation",    "ndps.ndps_page_orientation",
          FT_UINT32,    BASE_HEX,   VALS(ndps_page_orientation_enum),   0x0,
          "Page Orientation", HFILL }},

        { &hf_ndps_numbers_up,
        { "Numbers Up",    "ndps.ndps_numbers_up",
          FT_UINT32,    BASE_HEX,   VALS(ndps_numbers_up_enum),   0x0,
          "Numbers Up", HFILL }},

        { &hf_ndps_xdimension,
        { "X Dimension",    "ndps.ndps_xdimension",
          FT_UINT32,    BASE_HEX,   NULL,   0x0,
          "X Dimension", HFILL }},
        
        { &hf_ndps_ydimension,
        { "Y Dimension",    "ndps.ndps_ydimension",
          FT_UINT32,    BASE_HEX,   NULL,   0x0,
          "Y Dimension", HFILL }},
        
        { &hf_ndps_state_severity,
        { "State Severity",    "ndps.ndps_state_severity",
          FT_UINT32,    BASE_HEX,   VALS(ndps_state_severity_enum),   0x0,
          "State Severity", HFILL }},

        { &hf_ndps_training,
        { "Training",    "ndps.ndps_training",
          FT_UINT32,    BASE_HEX,   VALS(ndps_training_enum),   0x0,
          "Training", HFILL }},

        { &hf_ndps_colorant_set,
        { "Colorant Set",    "ndps.ndps_colorant_set",
          FT_UINT32,    BASE_HEX,   VALS(ndps_colorant_set_enum),   0x0,
          "Colorant Set", HFILL }},

        { &hf_ndps_card_enum_time,
        { "Cardinal, Enum, or Time",    "ndps.ndps_card_enum_time",
          FT_UINT32,    BASE_HEX,   VALS(ndps_card_enum_time_enum),   0x0,
          "Cardinal, Enum, or Time", HFILL }},

        { &hf_ndps_attrs_arg,
        { "List Attribute Operation",    "ndps.ndps_attrs_arg",
          FT_UINT32,    BASE_HEX,   VALS(ndps_attrs_arg_enum),   0x0,
          "List Attribute Operation", HFILL }},
        
        { &hf_ndps_context_len,
        { "Context Length",    "ndps.context_len",
          FT_UINT32,    BASE_DEC,   NULL,   0x0,
          "Context Length", HFILL }},

        { &hf_ndps_context,
        { "Context",    "ndps.context",
          FT_BYTES,    BASE_NONE,   NULL,   0x0,
          "Context", HFILL }},

        { &hf_ndps_filter,
        { "Filter Type",    "ndps.ndps_filter",
          FT_UINT32,    BASE_HEX,   VALS(ndps_filter_enum),   0x0,
          "Filter Type", HFILL }},

        { &hf_ndps_item_filter,
        { "Filter Item Operation",    "ndps.ndps_filter_item",
          FT_UINT32,    BASE_HEX,   VALS(ndps_filter_item_enum),   0x0,
          "Filter Item Operation", HFILL }},
        
        { &hf_ndps_substring_match,
        { "Substring Match",    "ndps.ndps_substring_match",
          FT_UINT32,    BASE_HEX,   VALS(ndps_match_criteria_enum),   0x0,
          "Substring Match", HFILL }},

        { &hf_ndps_time_limit,
        { "Time Limit",    "ndps.ndps_time_limit",
          FT_UINT32,    BASE_DEC,   NULL,   0x0,
          "Time Limit", HFILL }},

        { &hf_ndps_count_limit,
        { "Count Limit",    "ndps.ndps_count_limit",
          FT_UINT32,    BASE_DEC,   NULL,   0x0,
          "Count Limit", HFILL }},

        { &hf_ndps_operator,
        { "Operator Type",    "ndps.ndps_operator",
          FT_UINT32,    BASE_DEC,   VALS(ndps_operator_enum),   0x0,
          "Operator Type", HFILL }},

        { &hf_ndps_password,
        { "Password",    "ndps.password",
          FT_BYTES,    BASE_NONE,   NULL,   0x0,
          "Password", HFILL }},
        
        { &hf_ndps_retrieve_restrictions,
        { "Retrieve Restrictions",    "ndps.ndps_ret_restrict",
          FT_UINT32,    BASE_DEC,   NULL,   0x0,
          "Retrieve Restrictions", HFILL }},
    
        { &hf_ndps_bind_security_option_count,
        { "Number of Bind Security Options",    "ndps.ndps_bind_security_count",
          FT_UINT32,    BASE_DEC,   NULL,   0x0,
          "Number of Bind Security Options", HFILL }},

        { &hf_bind_security,
        { "Bind Security Options",    "ndps.ndps_bind_security",
          FT_UINT32,    BASE_DEC,   NULL,   0x0,
          "Bind Security Options", HFILL }},
        
        { &hf_ndps_max_items,
        { "Maximum Items in List",    "ndps.ndps_max_items",
          FT_UINT32,    BASE_DEC,   NULL,   0x0,
          "Maximum Items in List", HFILL }},

        { &hf_ndps_status_flags,
        { "Status Flag",    "ndps.ndps_status_flags",
          FT_UINT32,    BASE_DEC,   NULL,   0x0,
          "Status Flag", HFILL }},

        { &hf_ndps_resource_list_type,
        { "Resource Type",    "ndps.ndps_resource_type",
          FT_UINT32,    BASE_DEC,   VALS(ndps_resource_type_enum),   0x0,
          "Resource Type", HFILL }},

        { &hf_os_count,
        { "Number of OSes",    "ndps.os_count",
          FT_UINT32,    BASE_DEC,   NULL,   0x0,
          "Number of OSes", HFILL }},

        { &hf_os_type,
        { "OS Type",    "ndps.os_type",
          FT_UINT32,    BASE_DEC,   VALS(ndps_os_type_enum),   0x0,
          "OS Type", HFILL }},

        { &hf_ndps_printer_type_count,
        { "Number of Printer Types",    "ndps.printer_type_count",
          FT_UINT32,    BASE_DEC,   NULL,   0x0,
          "Number of Printer Types", HFILL }},

        { &hf_ndps_printer_type,
        { "Printer Type",    "ndps.prn_type",
          FT_STRING,    BASE_NONE,   NULL,   0x0,
          "Printer Type", HFILL }},

        { &hf_ndps_printer_manuf,
        { "Printer Manufacturer",    "ndps.prn_manuf",
          FT_STRING,    BASE_NONE,   NULL,   0x0,
          "Printer Manufacturer", HFILL }},

        { &hf_ndps_inf_file_name,
        { "INF File Name",    "ndps.inf_file_name",
          FT_STRING,    BASE_NONE,   NULL,   0x0,
          "INF File Name", HFILL }},

        { &hf_ndps_vendor_dir,
        { "Vendor Directory",    "ndps.vendor_dir",
          FT_STRING,    BASE_NONE,   NULL,   0x0,
          "Vendor Directory", HFILL }},

        { &hf_banner_type,
        { "Banner Type",    "ndps.banner_type",
          FT_UINT32,    BASE_DEC,   VALS(ndps_banner_type_enum),   0x0,
          "Banner Type", HFILL }},

        { &hf_font_type,
        { "Font Type",    "ndps.font_type",
          FT_UINT32,    BASE_DEC,   VALS(ndps_font_type_enum),   0x0,
          "Font Type", HFILL }},

        { &hf_printer_id,
        { "Printer ID",    "ndps.printer_id",
          FT_BYTES,    BASE_NONE,   NULL,   0x0,
          "Printer ID", HFILL }},

        { &hf_ndps_font_name,
        { "Font Name",    "ndps.font_name",
          FT_STRING,    BASE_NONE,   NULL,   0x0,
          "Font Name", HFILL }},

        { &hf_ndps_return_code,
        { "Return Code",    "ndps.ret_code",
          FT_UINT32,    BASE_HEX,   VALS(ndps_error_types),   0x0,
          "Return Code", HFILL }},

        { &hf_ndps_banner_count,
        { "Number of Banners",    "ndps.banner_count",
          FT_UINT32,    BASE_DEC,   NULL,   0x0,
          "Number of Banners", HFILL }},

        { &hf_ndps_banner_name,
        { "Banner Name",    "ndps.banner_name",
          FT_STRING,    BASE_NONE,   NULL,   0x0,
          "Banner Name", HFILL }},

        { &hf_ndps_font_type_count,
        { "Number of Font Types",    "ndps.font_type_count",
          FT_UINT32,    BASE_DEC,   NULL,   0x0,
          "Number of Font Types", HFILL }},

        { &hf_font_type_name,
        { "Font Type Name",    "ndps.font_type_name",
          FT_STRING,    BASE_NONE,   NULL,   0x0,
          "Font Type Name", HFILL }},

        { &hf_ndps_font_file_count,
        { "Number of Font Files",    "ndps.font_file_count",
          FT_UINT32,    BASE_DEC,   NULL,   0x0,
          "Number of Font Files", HFILL }},

        { &hf_font_file_name,
        { "Font File Name",    "ndps.font_file_name",
          FT_STRING,    BASE_NONE,   NULL,   0x0,
          "Font File Name", HFILL }},

        { &hf_ndps_printer_def_count,
        { "Number of Printer Definitions",    "ndps.printer_def_count",
          FT_UINT32,    BASE_DEC,   NULL,   0x0,
          "Number of Printer Definitions", HFILL }},

        { &hf_ndps_prn_file_name,
        { "Printer File Name",    "ndps.print_file_name",
          FT_STRING,    BASE_NONE,   NULL,   0x0,
          "Printer File Name", HFILL }},

        { &hf_ndps_prn_dir_name,
        { "Printer Directory Name",    "ndps.print_dir_name",
          FT_STRING,    BASE_NONE,   NULL,   0x0,
          "Printer Directory Name", HFILL }},

        { &hf_ndps_def_file_name,
        { "Printer Definition Name",    "ndps.print_def_name",
          FT_STRING,    BASE_NONE,   NULL,   0x0,
          "Printer Definition Name", HFILL }},

        { &hf_ndps_num_win31_keys,
        { "Number of Windows 3.1 Keys",    "ndps.num_win31_keys",
          FT_UINT32,    BASE_DEC,   NULL,   0x0,
          "Number of Windows 3.1 Keys", HFILL }},

        { &hf_ndps_num_win95_keys,
        { "Number of Windows 95 Keys",    "ndps.num_win95_keys",
          FT_UINT32,    BASE_DEC,   NULL,   0x0,
          "Number of Windows 95 Keys", HFILL }},

        { &hf_ndps_num_windows_keys,
        { "Number of Windows Keys",    "ndps.num_windows_keys",
          FT_UINT32,    BASE_DEC,   NULL,   0x0,
          "Number of Windows Keys", HFILL }},

        { &hf_ndps_windows_key,
        { "Windows Key",    "ndps.windows_key",
          FT_STRING,    BASE_NONE,   NULL,   0x0,
          "Windows Key", HFILL }},

        { &hf_archive_type,
        { "Archive Type",    "ndps.archive_type",
          FT_UINT32,    BASE_DEC,   VALS(ndps_archive_enum),   0x0,
          "Archive Type", HFILL }},

        { &hf_archive_file_size,
        { "Archive File Size",    "ndps.archive_size",
          FT_UINT32,    BASE_DEC,   NULL,   0x0,
          "Archive File Size", HFILL }},

        { &hf_ndps_segment_overlap,
          { "Segment overlap",	"ndps.segment.overlap", FT_BOOLEAN, BASE_NONE,
    		NULL, 0x0, "Segment overlaps with other segments", HFILL }},
    
        { &hf_ndps_segment_overlap_conflict,
          { "Conflicting data in segment overlap", "ndps.segment.overlap.conflict",
    	FT_BOOLEAN, BASE_NONE,
    		NULL, 0x0, "Overlapping segments contained conflicting data", HFILL }},
    
        { &hf_ndps_segment_multiple_tails,
          { "Multiple tail segments found", "ndps.segment.multipletails",
    	FT_BOOLEAN, BASE_NONE,
    		NULL, 0x0, "Several tails were found when desegmenting the packet", HFILL }},
    
        { &hf_ndps_segment_too_long_segment,
          { "Segment too long",	"ndps.segment.toolongsegment", FT_BOOLEAN, BASE_NONE,
    		NULL, 0x0, "Segment contained data past end of packet", HFILL }},
    
        { &hf_ndps_segment_error,
          {"Desegmentation error",	"ndps.segment.error", FT_FRAMENUM, BASE_NONE,
    		NULL, 0x0, "Desegmentation error due to illegal segments", HFILL }},
    
        { &hf_ndps_segment,
          { "NDPS Fragment",		"ndps.fragment", FT_FRAMENUM, BASE_NONE,
    		NULL, 0x0, "NDPS Fragment", HFILL }},
    
        { &hf_ndps_segments,
          { "NDPS Fragments",	"ndps.fragments", FT_NONE, BASE_NONE,
    		NULL, 0x0, "NDPS Fragments", HFILL }},

        { &hf_ndps_data,
          { "[Data]",	"ndps.data", FT_NONE, BASE_NONE,
    		NULL, 0x0, "[Data]", HFILL }},

        { &hf_get_status_flag,
        { "Get Status Flag",    "ndps.get_status_flags",
          FT_UINT32,    BASE_DEC,   NULL,   0x0,
          "Get Status Flag", HFILL }},
        
        { &hf_res_type,
        { "Resource Type",    "ndps.res_type",
          FT_UINT32,    BASE_DEC,   VALS(ndps_res_type_enum),   0x0,
          "Resource Type", HFILL }},
              
        { &hf_file_timestamp,
        { "File Time Stamp",    "ndps.file_time_stamp",
          FT_UINT32,    BASE_DEC,   NULL,   0x0,
          "File Time Stamp", HFILL }},

        { &hf_print_arg,
        { "Print Type",    "ndps.print_arg",
          FT_UINT32,    BASE_DEC,   VALS(ndps_print_arg_enum),   0x0,
          "Print Type", HFILL }},

        { &hf_sub_complete,
          { "Submission Complete?",	"ndps.sub_complete", FT_BOOLEAN, BASE_NONE,
    		NULL, 0x0, "Submission Complete?", HFILL }},

        { &hf_doc_content,
        { "Document Content",    "ndps.doc_content",
          FT_UINT32,    BASE_DEC,   VALS(ndps_doc_content_enum),   0x0,
          "Document Content", HFILL }},

        { &hf_ndps_doc_name,
        { "Document Name",    "ndps.doc_name",
          FT_STRING,    BASE_NONE,   NULL,   0x0,
          "Document Name", HFILL }},

        { &hf_local_id,
        { "Local ID",    "ndps.local_id",
          FT_UINT32,    BASE_DEC,   NULL,   0x0,
          "Local ID", HFILL }},

        { &hf_ndps_included_doc_len,
        { "Included Document Length",    "ndps.included_doc_len",
          FT_UINT32,    BASE_DEC,   NULL,   0x0,
          "Included Document Length", HFILL }},

        { &hf_ndps_included_doc,
        { "Included Document",    "ndps.included_doc",
          FT_BYTES,    BASE_NONE,   NULL,   0x0,
          "Included Document", HFILL }},

        { &hf_ndps_ref_name,
        { "Referenced Document Name",    "ndps.ref_doc_name",
          FT_STRING,    BASE_NONE,   NULL,   0x0,
          "Referenced Document Name", HFILL }},

        { &hf_interrupt_job_type,
        { "Interrupt Job Identifier",    "ndps.interrupt_job_type",
          FT_UINT32,    BASE_DEC,   VALS(ndps_interrupt_job_enum),   0x0,
          "Interrupt Job Identifier", HFILL }},

        { &hf_pause_job_type,
        { "Pause Job Identifier",    "ndps.pause_job_type",
          FT_UINT32,    BASE_DEC,   VALS(ndps_pause_job_enum),   0x0,
          "Pause Job Identifier", HFILL }},

        { &hf_ndps_force,
        { "Force?",    "ndps.ndps_force",
          FT_BOOLEAN,    BASE_NONE,   NULL,   0x0,
          "Force?", HFILL }},

        { &hf_resubmit_op_type,
        { "Resubmit Operation Type",    "ndps.resubmit_op_type",
          FT_UINT32,    BASE_DEC,   VALS(ndps_resubmit_op_enum),   0x0,
          "Resubmit Operation Type", HFILL }},

        { &hf_shutdown_type,
        { "Shutdown Type",    "ndps.shutdown_type",
          FT_UINT32,    BASE_DEC,   VALS(ndps_shutdown_enum),   0x0,
          "Shutdown Type", HFILL }},

        { &hf_ndps_supplier_flag,
        { "Supplier Data?",    "ndps.supplier_flag",
          FT_BOOLEAN,    BASE_NONE,   NULL,   0x0,
          "Supplier Data?", HFILL }},

        { &hf_ndps_language_flag,
        { "Language Data?",    "ndps.language_flag",
          FT_BOOLEAN,    BASE_NONE,   NULL,   0x0,
          "Language Data?", HFILL }},

        { &hf_ndps_method_flag,
        { "Method Data?",    "ndps.method_flag",
          FT_BOOLEAN,    BASE_NONE,   NULL,   0x0,
          "Method Data?", HFILL }},
        
        { &hf_ndps_delivery_address_flag,
        { "Delivery Address Data?",    "ndps.delivery_flag",
          FT_BOOLEAN,    BASE_NONE,   NULL,   0x0,
          "Delivery Address Data?", HFILL }},
       
        { &hf_ndps_list_profiles_type,
        { "List Profiles Type",    "ndps.ndps_list_profiles_type",
          FT_UINT32,    BASE_HEX,   VALS(ndps_attrs_arg_enum),   0x0,
          "List Profiles Type", HFILL }},
    
        { &hf_ndps_list_profiles_choice_type,
        { "List Profiles Choice Type",    "ndps.ndps_list_profiles_choice_type",
          FT_UINT32,    BASE_HEX,   VALS(ndps_list_profiles_choice_enum),   0x0,
          "List Profiles Choice Type", HFILL }},
        
        { &hf_ndps_list_profiles_result_type,
        { "List Profiles Result Type",    "ndps.ndps_list_profiles_result_type",
          FT_UINT32,    BASE_HEX,   VALS(ndps_list_profiles_result_enum),   0x0,
          "List Profiles Result Type", HFILL }},
        
        { &hf_ndps_integer_type_flag,
        { "Integer Type Flag",    "ndps.ndps_integer_type_flag",
          FT_UINT32,    BASE_HEX,   NULL,   0x0,
          "Integer Type Flag", HFILL }},
        
        { &hf_ndps_integer_type_value,
        { "Integer Type Value",    "ndps.ndps_integer_type_value",
          FT_UINT32,    BASE_HEX,   NULL,   0x0,
          "Integer Type Value", HFILL }},
        
        { &hf_ndps_continuation_option,
        { "Continuation Option",    "ndps.ndps_continuation_option",
          FT_BYTES,    BASE_HEX,   NULL,   0x0,
          "Continuation Option", HFILL }},

        { &hf_ndps_ds_info_type,
        { "DS Info Type",    "ndps.ndps_ds_info_type",
          FT_UINT32,    BASE_HEX,   VALS(ndps_ds_info_enum),   0x0,
          "DS Info Type", HFILL }},

        { &hf_ndps_guid,
        { "GUID",    "ndps.guid",
          FT_BYTES,    BASE_NONE,   NULL,   0x0,
          "GUID", HFILL }},

        { &hf_ndps_list_services_type,
        { "Services Type",    "ndps.ndps_list_services_type",
          FT_UINT32,    BASE_HEX,   VALS(ndps_list_services_enum),   0x0,
          "Services Type", HFILL }},

        { &hf_ndps_item_bytes,
        { "Item Ptr",    "ndps.item_bytes",
          FT_BYTES,    BASE_NONE,   NULL,   0x0,
          "Item Ptr", HFILL }},

        { &hf_ndps_certified,
        { "Certified",    "ndps.certified",
          FT_BYTES,    BASE_NONE,   NULL,   0x0,
          "Certified", HFILL }},

        { &hf_ndps_attribute_set,
        { "Attribute Set",    "ndps.attribute_set",
          FT_BYTES,    BASE_NONE,   NULL,   0x0,
          "Attribute Set", HFILL }},

        { &hf_ndps_data_item_type,
        { "Item Type",    "ndps.ndps_data_item_type",
          FT_UINT32,    BASE_HEX,   VALS(ndps_data_item_enum),   0x0,
          "Item Type", HFILL }},

        { &hf_info_int,
        { "Integer Value",    "ndps.info_int",
          FT_UINT8,    BASE_HEX,   NULL,   0x0,
          "Integer Value", HFILL }},

        { &hf_info_int16,
        { "16 Bit Integer Value",    "ndps.info_int16",
          FT_UINT16,    BASE_HEX,   NULL,   0x0,
          "16 Bit Integer Value", HFILL }},

        { &hf_info_int32,
        { "32 Bit Integer Value",    "ndps.info_int32",
          FT_UINT32,    BASE_HEX,   NULL,   0x0,
          "32 Bit Integer Value", HFILL }},

        { &hf_info_boolean,
        { "Boolean Value",    "ndps.info_boolean",
          FT_BOOLEAN,    BASE_NONE,   NULL,   0x0,
          "Boolean Value", HFILL }},

        { &hf_info_string,
        { "String Value",    "ndps.info_string",
          FT_STRING,    BASE_NONE,   NULL,   0x0,
          "String Value", HFILL }},

        { &hf_info_bytes,
        { "Byte Value",    "ndps.info_bytes",
          FT_BYTES,    BASE_NONE,   NULL,   0x0,
          "Byte Value", HFILL }},
       
        { &hf_ndps_list_local_servers_type,
        { "Server Type",    "ndps.ndps_list_local_server_type",
          FT_UINT32,    BASE_HEX,   VALS(ndps_list_local_servers_enum),   0x0,
          "Server Type", HFILL }},

        { &hf_ndps_registry_name,
        { "Registry Name",    "ndps.registry_name",
          FT_STRING,    BASE_NONE,   NULL,   0x0,
          "Registry Name", HFILL }},

        { &hf_ndps_client_server_type,
        { "Client/Server Type",    "ndps.ndps_client_server_type",
          FT_UINT32,    BASE_HEX,   VALS(ndps_client_server_enum),   0x0,
          "Client/Server Type", HFILL }},

        { &hf_ndps_session_type,
        { "Session Type",    "ndps.ndps_session_type",
          FT_UINT32,    BASE_HEX,   VALS(ndps_session_type_enum),   0x0,
          "Session Type", HFILL }},

        { &hf_time,
        { "Time",    "ndps.time",
          FT_UINT32,    BASE_DEC,   NULL,   0x0,
          "Time", HFILL }},

        { &hf_ndps_supplier_name,
        { "Supplier Name",    "ndps.supplier_name",
          FT_STRING,    BASE_NONE,   NULL,   0x0,
          "Supplier Name", HFILL }},

        { &hf_ndps_message,
        { "Message",    "ndps.message",
          FT_STRING,    BASE_NONE,   NULL,   0x0,
          "Message", HFILL }},

        { &hf_ndps_delivery_method_count,
        { "Number of Delivery Methods",    "ndps.delivery_method_count",
          FT_UINT32,    BASE_DEC,   NULL,   0x0,
          "Number of Delivery Methods", HFILL }},
        
        { &hf_delivery_method_type,
        { "Delivery Method Type",    "ndps.delivery_method_type",
          FT_UINT32,    BASE_HEX,   VALS(ndps_delivery_method_enum),   0x0,
          "Delivery Method Type", HFILL }},

        { &hf_ndps_get_session_type,
        { "Session Type",    "ndps.ndps_get_session_type",
          FT_UINT32,    BASE_HEX,   VALS(ndps_get_session_type_enum),   0x0,
          "Session Type", HFILL }},

        { &hf_packet_count,
        { "Packet Count",    "ndps.packet_count",
          FT_UINT32,    BASE_DEC,   NULL,   0x0,
          "Packet Count", HFILL }},

        { &hf_last_packet_flag,
        { "Last Packet Flag",    "ndps.last_packet_flag",
          FT_UINT32,    BASE_DEC,   NULL,   0x0,
          "Last Packet Flag", HFILL }},

        { &hf_ndps_get_resman_session_type,
        { "Session Type",    "ndps.ndps_get_resman_session_type",
          FT_UINT32,    BASE_HEX,   VALS(ndps_get_resman_session_type_enum),   0x0,
          "Session Type", HFILL }},

        { &hf_problem_type,
        { "Problem Type",    "ndps.ndps_get_resman_session_type",
          FT_UINT32,    BASE_HEX,   VALS(problem_type_enum),   0x0,
          "Problem Type", HFILL }},

        { &hf_ndps_num_values,
        { "Number of Values",    "ndps.num_values",
          FT_UINT32,    BASE_DEC,   NULL,   0x0,
          "Number of Values", HFILL }},
        
        { &hf_ndps_object_ids_7,
        { "Object ID Definition",    "ndps.objectid_def7",
          FT_NONE,    BASE_HEX,   NULL,
          0x0, "Object ID Definition", HFILL }},
        
        { &hf_ndps_object_ids_8,
        { "Object ID Definition",    "ndps.objectid_def8",
          FT_NONE,    BASE_HEX,   NULL,
          0x0, "Object ID Definition", HFILL }},

        { &hf_ndps_object_ids_9,
        { "Object ID Definition",    "ndps.objectid_def9",
          FT_NONE,    BASE_HEX,   NULL,
          0x0, "Object ID Definition", HFILL }},

        { &hf_ndps_object_ids_10,
        { "Object ID Definition",    "ndps.objectid_def10",
          FT_NONE,    BASE_HEX,   NULL,
          0x0, "Object ID Definition", HFILL }},

        { &hf_ndps_object_ids_11,
        { "Object ID Definition",    "ndps.objectid_def11",
          FT_NONE,    BASE_HEX,   NULL,
          0x0, "Object ID Definition", HFILL }},

        { &hf_ndps_object_ids_12,
        { "Object ID Definition",    "ndps.objectid_def12",
          FT_NONE,    BASE_HEX,   NULL,
          0x0, "Object ID Definition", HFILL }},

        { &hf_ndps_object_ids_13,
        { "Object ID Definition",    "ndps.objectid_def13",
          FT_NONE,    BASE_HEX,   NULL,
          0x0, "Object ID Definition", HFILL }},

        { &hf_ndps_object_ids_14,
        { "Object ID Definition",    "ndps.objectid_def14",
          FT_NONE,    BASE_HEX,   NULL,
          0x0, "Object ID Definition", HFILL }},

        { &hf_ndps_object_ids_15,
        { "Object ID Definition",    "ndps.objectid_def15",
          FT_NONE,    BASE_HEX,   NULL,
          0x0, "Object ID Definition", HFILL }},

        { &hf_ndps_object_ids_16,
        { "Object ID Definition",    "ndps.objectid_def16",
          FT_NONE,    BASE_HEX,   NULL,
          0x0, "Object ID Definition", HFILL }},

        { &hf_ndps_attribute_time,
        { "Time",    "ndps.attribute_time",
          FT_ABSOLUTE_TIME,    BASE_DEC,   NULL,   0x0,
          "Time", HFILL }},

        { &hf_print_security,
        { "Printer Security",    "ndps.print_security",
          FT_UINT32,    BASE_HEX,   VALS(ndps_print_security),   0x0,
          "Printer Security", HFILL }},
        
        { &hf_notify_time_interval,
        { "Notify Time Interval",    "ndps.notify_time_interval",
          FT_UINT32,    BASE_HEX,   NULL,   0x0,
          "Notify Time Interval", HFILL }},

        { &hf_notify_sequence_number,
        { "Notify Sequence Number",    "ndps.notify_seq_number",
          FT_UINT32,    BASE_HEX,   NULL,   0x0,
          "Notify Sequence Number", HFILL }},

        { &hf_notify_lease_exp_time,
        { "Notify Lease Expiration Time",    "ndps.notify_lease_exp_time",
          FT_UINT32,    BASE_HEX,   NULL,   0x0,
          "Notify Lease Expiration Time", HFILL }},

        { &hf_notify_printer_uri,
        { "Notify Printer URI",    "ndps.notify_printer_uri",
          FT_STRING,    BASE_NONE,   NULL,   0x0,
          "Notify Printer URI", HFILL }},

        { &hf_level,
        { "Level",    "ndps.level",
          FT_UINT32,    BASE_HEX,   NULL,   0x0,
          "Level", HFILL }},

        { &hf_interval,
        { "Interval",    "ndps.interval",
          FT_UINT32,    BASE_HEX,   NULL,   0x0,
          "Interval", HFILL }},
    };

	static gint *ett[] = {
		&ett_ndps,
		&ett_ndps_segments,
		&ett_ndps_segment,
	};
	module_t *ndps_module;
	
	proto_ndps = proto_register_protocol("Novell Distributed Print System", "NDPS", "ndps");
	proto_register_field_array(proto_ndps, hf_ndps, array_length(hf_ndps));
	proto_register_subtree_array(ett, array_length(ett));

	ndps_module = prefs_register_protocol(proto_ndps, NULL);
	prefs_register_bool_preference(ndps_module, "desegment_tcp",
	    "Reassemble NDPS messages spanning multiple TCP segments",
	    "Whether the NDPS dissector should reassemble messages spanning multiple TCP segments."
	    " To use this option, you must also enable \"Allow subdissectors to reassemble TCP streams\" in the TCP protocol settings.",
	    &ndps_desegment);
	prefs_register_bool_preference(ndps_module, "desegment_spx",
	    "Reassemble fragmented NDPS messages spanning multiple SPX packets",
	    "Whether the NDPS dissector should reassemble fragmented NDPS messages spanning multiple SPX packets",
	    &ndps_defragment);
	prefs_register_bool_preference(ndps_module, "show_oid",
	    "Display NDPS Details",
	    "Whether or not the NDPS dissector should show object id's and other details",
	    &ndps_show_oids);

	register_init_routine(&ndps_init_protocol);
	register_postseq_cleanup_routine(&ndps_postseq_cleanup);
}
