void
proto_register_reload(void)
{
  module_t *reload_module;
  static hf_register_info hf[] = {
    { &hf_reload_response_in,
      { "Response in",  "reload.response-in", FT_FRAMENUM,
        BASE_NONE, NULL, 0x0, "The response to this RELOAD Request is in this frame", HFILL
      }
    },
    { &hf_reload_response_to,
      { "Request in", "reload.response-to", FT_FRAMENUM,
        BASE_NONE, NULL, 0x0, "This is a response to the RELOAD Request in this frame", HFILL
      }
    },
    { &hf_reload_time,
      { "Time", "reload.time", FT_RELATIVE_TIME,
        BASE_NONE, NULL, 0x0, "The time between the Request and the Response", HFILL
      }
    },
    { &hf_reload_duplicate,
      { "Duplicated original message in", "reload.duplicate", FT_FRAMENUM,
        BASE_NONE, NULL, 0x0, "This is a duplicate of RELOAD message in this frame", HFILL
      }
    },
    { &hf_reload_forwarding,
      { "ForwardingHeader",    "reload.forwarding",  FT_NONE,
        BASE_NONE,  NULL,   0x0,  NULL, HFILL
      }
    },
    { &hf_reload_token,
      { "relo_token (uint32)", "reload.forwarding.token",  FT_UINT32,
        BASE_HEX, NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_overlay,
      { "overlay (uint32)",  "reload.forwarding.overlay",  FT_UINT32,
        BASE_HEX, NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_configuration_sequence,
      { "configuration_sequence (uint16)", "reload.forwarding.configuration_sequence", FT_UINT16,
        BASE_DEC, NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_version,
      { "version (uint8)",  "reload.forwarding.version",  FT_UINT8,
        BASE_HEX, VALS(versions), 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_ttl,
      { "ttl (uint8)",  "reload.forwarding.ttl",  FT_UINT8,
        BASE_DEC, NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_fragment_flag,
      { "fragment (uint32)", "reload.forwarding.fragment", FT_UINT32,
        BASE_HEX, NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_fragment_fragmented,
      { "Fragmented (always set)", "reload.forwarding.fragment.fragmented", FT_BOOLEAN, 1, TFS(&tfs_set_notset), 0x0,
        NULL, HFILL
      }
    },
    { &hf_reload_fragment_last_fragment,
      { "Last Fragment", "reload.forwarding.fragment.last", FT_BOOLEAN, 1, TFS(&tfs_set_notset), 0x0,
        NULL, HFILL
      }
    },
    { &hf_reload_fragment_reserved,
      { "Reserved (always 0)", "reload.forwarding.fragment.reserved", FT_BOOLEAN, 1, NULL, 0x0,
        NULL, HFILL
      }
    },
    { &hf_reload_fragment_offset,
      { "Fragment Offset","reload.forwarding.fragment.offset",  FT_UINT32,
        BASE_DEC, NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_trans_id,
      { "transaction_id (uint32)", "reload.forwarding.trans_id", FT_UINT64,
        BASE_HEX, NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_max_response_length,
      { "max_response_length (uint32)",  "reload.forwarding.max_response_length",  FT_UINT32,
        BASE_DEC, NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_via_list_length,
      { "via_list_length (uint16)",  "reload.forwarding.via_list.length",  FT_UINT16,
        BASE_DEC, NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_destination_list_length,
      { "destination_list_length (uint16)",  "reload.forwarding.destination_list.length",  FT_UINT16,
        BASE_DEC, NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_options_length,
      { "options_length (uint16)", "reload.forwarding.options.length", FT_UINT16,
        BASE_DEC, NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_via_list,
      { "via_list",   "reload.forwarding.via_list", FT_NONE,
        BASE_NONE,  NULL,   0x0,  NULL, HFILL
      }
    },
    { &hf_reload_destination,
      { "Destination",    "reload.destination",  FT_NONE,
        BASE_NONE,  NULL,   0x0,  NULL, HFILL
      }
    },
    { &hf_reload_destination_compressed_id,
      { "compressed_id (uint16)", "reload.forwarding.destination.compressed_id",  FT_UINT16,
        BASE_HEX, NULL,   0x0,  NULL, HFILL
      }
    },
    { &hf_reload_destination_type,
      { "type (DestinationType)",    "reload.forwarding.destination.type",  FT_UINT8,
        BASE_HEX, VALS(destinationtypes), 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_destination_data_node_id,
      { "node_id (NodeId)",    "reload.destination.data.nodeid", FT_BYTES,
        BASE_NONE,  NULL,   0x0,  NULL, HFILL
      }
    },
    { &hf_reload_destination_data_resource_id,
      { "resource_id",    "reload.destination.data.resourceid", FT_NONE,
        BASE_NONE,  NULL,   0x0,  NULL, HFILL
      }
    },
    { &hf_reload_nodeid,
      { "NodeId",    "reload.nodeid", FT_BYTES,
        BASE_NONE,  NULL,   0x0,  NULL, HFILL
      }
    },
    { &hf_reload_resourceid,
      { "ResourceId",    "reload.resource_id", FT_NONE,
        BASE_NONE,  NULL,   0x0,  NULL, HFILL
      }
    },
    { &hf_reload_destination_data_compressed_id,
      { "compressed_id",    "reload.destination.data.compressed_id",  FT_NONE,
        BASE_NONE,  NULL,   0x0,  NULL, HFILL
      }
    },
    { &hf_reload_destination_list,
      { "destination_list",   "reload.forwarding.destination_list", FT_NONE,
        BASE_NONE,  NULL,   0x0,  NULL, HFILL
      }
    },
    { &hf_reload_forwarding_options,
      { "options",    "reload.forwarding.option", FT_NONE,
        BASE_NONE,  NULL,   0x0,  NULL, HFILL
      }
    },
    { &hf_reload_forwarding_option,
      { "ForwardingOption",    "reload.forwarding.option", FT_NONE,
        BASE_NONE,  NULL,   0x0,  NULL, HFILL
      }
    },
    { &hf_reload_forwarding_option_type,
      { "type (ForwardingOptionType)", "reload.forwarding.option.type",  FT_UINT8,
        BASE_DEC, VALS(forwardingoptiontypes),  0x0,  NULL, HFILL
      }
    },
    { &hf_reload_forwarding_option_flags,
      { "flags (uint8)",  "reload.forwarding.option.flags", FT_UINT8,
        BASE_HEX, NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_forwarding_option_flag_ignore_state_keeping,
      { "IGNORE_STATE_KEEPING", "reload.forwarding.option.flag.response_copy", FT_BOOLEAN, 8, TFS(&tfs_set_notset), 0x0,
        NULL, HFILL
      }
    },
    { &hf_reload_forwarding_option_flag_response_copy,
      { "RESPONSE_COPY", "reload.forwarding.option.flag.response_copy", FT_BOOLEAN, 8, TFS(&tfs_set_notset), 0x0,
        NULL, HFILL
      }
    },
    { &hf_reload_forwarding_option_flag_destination_critical,
      { "DESTINATION_CRITICAL", "reload.forwarding.option.flags.destination_critical", FT_BOOLEAN, 8, TFS(&tfs_set_notset), 0x0,
        NULL, HFILL
      }
    },
    { &hf_reload_forwarding_option_flag_forward_critical,
      { "FORWARD_CRITICAL", "reload.forwarding.option.flags.forward_critical", FT_BOOLEAN, 8, TFS(&tfs_set_notset), 0x0,
        NULL, HFILL
      }
    },
    { &hf_reload_attachreqans,
      { "AttachReqAns", "reload.attachreqans",  FT_NONE,
        BASE_NONE,  NULL,   0x0,  NULL, HFILL
      }
    },
    { &hf_reload_ufrag,
      { "ufrag",  "reload.ufrag", FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_password,
      { "password", "reload.password",  FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_role,
      { "role", "reload.role",  FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_icecandidates,
      { "candidates",   "reload.icecandidates", FT_NONE,
        BASE_NONE,  NULL,   0x0,  NULL, HFILL
      }
    },
    { &hf_reload_icecandidate,
      { "IceCandidate",    "reload.icecandidate",  FT_NONE,
        BASE_NONE,  NULL,   0x0,  NULL, HFILL
      }
    },
    { &hf_reload_icecandidate_addr_port,
      { "addr_port",    "reload.icecandidate.addr_port",  FT_NONE,
        BASE_NONE,  NULL,   0x0,  NULL, HFILL
      }
    },
    { &hf_reload_icecandidate_relay_addr,
      { "rel_addr_port",    "reload.icecandidate.relay_addr", FT_NONE,
        BASE_NONE,  NULL,   0x0,  NULL, HFILL
      }
    },
    { &hf_reload_ipaddressport,
      { "IpAddressPort",    "reload.ipaddressport", FT_NONE,
        BASE_NONE,  NULL,   0x0,  NULL, HFILL
      }
    },
    { &hf_reload_ipaddressport_type,
      { "type (AddressType)", "reload.ipaddressport.type",  FT_UINT8,
        BASE_HEX, VALS(ipaddressporttypes), 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_ipv4addrport,
      { "IPv4AddrPort",    "reload.ipv4addrport", FT_NONE,
        BASE_NONE,  NULL,   0x0,  NULL, HFILL
      }
    },
    { &hf_reload_ipv4addr,
      { "addr (uint32)", "reload.ipv4addr",  FT_IPv4,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_ipv6addrport,
      { "IPv6AddrPort",    "reload.ipv6addrport", FT_NONE,
        BASE_NONE,  NULL,   0x0,  NULL, HFILL
      }
    },
    { &hf_reload_ipv6addr,
      { "addr (uint128)", "reload.ipv6addr",  FT_IPv6,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_port,
      { "port (uint16)", "reload.port",  FT_UINT16,
        BASE_DEC, NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_overlaylink_type,
      { "overlay_link (OverlayLinkType)",  "reload.overlaylink.type",  FT_UINT8,
        BASE_DEC, VALS(overlaylinktypes), 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_icecandidate_foundation,
      { "foundation", "reload.icecandidate.foundation", FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_icecandidate_priority,
      { "priority (uint32)", "reload.icecandidate.priority", FT_UINT32,
        BASE_DEC, NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_icecandidate_type,
      { "Ice candidate type", "reload.icecandidate.type", FT_UINT8,
        BASE_DEC, VALS(candtypes),  0x0,  NULL, HFILL
      }
    },
    { &hf_reload_iceextensions,
      { "extensions",    "reload.iceextensions",  FT_NONE,
        BASE_NONE,  NULL,   0x0,  NULL, HFILL
      }
    },
    { &hf_reload_iceextension,
      { "IceExtension",    "reload.iceextension",  FT_NONE,
        BASE_NONE,  NULL,   0x0,  NULL, HFILL
      }
    },
    { &hf_reload_iceextension_name,
      { "name", "reload.iceextension.name", FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_iceextension_value,
      { "value",  "reload.iceextension.value",  FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_sendupdate,
      { "send_update (Boolean)", "reload.sendupdate",  FT_BOOLEAN,
        BASE_DEC, NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_message_contents,
      { "MessageContents",   "reload.message.contents",  FT_NONE,
        BASE_NONE,  NULL,   0x0,  NULL, HFILL
      }
    },
    { &hf_reload_message_code,
      { "message_code (uint16)", "reload.message.code",  FT_UINT16,
        BASE_DEC, NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_message_body,
      { "message_body", "reload.message.body",  FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_message_extensions,
      { "extensions",  "reload.message.extensions", FT_NONE,
        BASE_NONE, NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_message_extension,
      { "MessageExtension",    "reload.message_extension", FT_NONE,
        BASE_NONE,  NULL,   0x0,  NULL, HFILL
      }
    },
    { &hf_reload_message_extension_type,
      { "type (MessageExtensionType)", "reload.message_extension.type",  FT_UINT16,
        BASE_DEC, VALS(messageextensiontypes), 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_message_extension_critical,
      { "critical (Boolean)", "reload.message_extension.critical",  FT_BOOLEAN,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_message_extension_content,
      { "extension_content",  "reload.message_extension.content", FT_BYTES,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_error_response,
      { "ErrorResponse", "reload.error_response",  FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_error_response_code,
      { "error_code (uint16)", "reload.error_response.code", FT_UINT16,
        BASE_DEC, VALS(errorcodes), 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_error_response_info,
      { "error_info", "reload.error_response_info", FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_security_block,
      { "SecurityBlock", "reload.security_block",  FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_certificates,
      { "certificates",  "reload.certificates", FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_certificate_type,
      { "type (CertificateType)", "reload.certificate.type",  FT_UINT8,
        BASE_DEC, VALS(tls_certificate_type), 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_genericcertificate,
      { "GenericCertificate", "reload.genericcertificate",  FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_certificate,
      { "certificate", "reload.certificate",  FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_signature,
      { "signature (Signature)",  "reload.signature", FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_signatureandhashalgorithm,
      { "algorithm (SignatureAndHashAlgorithm)",  "reload.signatureandhashalgorithm", FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_hash_algorithm,
      { "hash (HashAlgorithm)", "reload.hash_algorithm",  FT_UINT8,
        BASE_DEC, VALS(tls_hash_algorithm), 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_signeridentity_value_hash_alg,
      { "hash_alg (HashAlgorithm)", "reload.signeridentityvalue.hash_alg",  FT_UINT8,
        BASE_DEC, VALS(tls_hash_algorithm), 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_signature_algorithm,
      { "signature (SignatureAlgorithm)",  "reload.signature_algorithm", FT_UINT8,
        BASE_DEC, VALS(tls_signature_algorithm),  0x0,  NULL, HFILL
      }
    },
    { &hf_reload_signeridentity,
      { "identity (SignerIdentity)", "reload.signature.identity",  FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_signeridentity_identity,
      { "identity", "reload.signature.identity.identity",  FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_signeridentity_type,
      { "identity_type (SignerIdentityType)",  "reload.signature.identity.type", FT_UINT8,
        BASE_DEC, VALS(signeridentitytypes), 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_signeridentity_value,
      { "SignatureIdentityValue", "reload.signature.identity.value",  FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_signeridentity_value_certificate_hash,
      { "certificate_hash",  "reload.signature.identity.value.certificate_hash", FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_signeridentity_value_certificate_node_id_hash,
      { "certificate_node_id_hash",  "reload.signature.identity.value.certificate_node_id_hash", FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_signature_value,
      { "signature_value",  "reload.signature.value.",  FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_length_uint8,
      { "length (uint8)", "reload.length.8", FT_UINT8,
        BASE_DEC, NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_length_uint16,
      { "length (uint16)", "reload.length.16", FT_UINT16,
        BASE_DEC, NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_length_uint24,
      { "length (uint24)", "reload.length.24", FT_UINT32,
        BASE_DEC, NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_length_uint32,
      { "length (uint32)", "reload.length.32",  FT_UINT32,
        BASE_DEC, NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_opaque,
      { "opaque",  "reload.opaque", FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_opaque_data,
      { "data (bytes)", "reload.opaque.data", FT_BYTES,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_opaque_string,
      { "data (string)", "reload.opaque.string", FT_STRING,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_routequeryreq,
      { "RouteQueryReq",  "reload.routequeryreq", FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_routequeryreq_destination,
      { "destination",  "reload.routequeryreq.destination", FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_overlay_specific,
      { "overlay_specific_data",  "reload.overlay_specific_data", FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_probereq,
      { "ProbeReq", "reload.probereq",  FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_probereq_requested_info,
      { "requested_info", "reload.probereq.requested_info",  FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_probe_information,
      { "ProbeInformation",  "reload.probe_information", FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_probe_information_data,
      { "value (ProbeInformationData)",  "reload.probe_information", FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_probe_information_type,
      { "type (ProbeInformationType)", "reload.probe_information.type", FT_UINT8,
        BASE_HEX, VALS(probeinformationtypes),  0x0,  NULL, HFILL
      }
    },
    { &hf_reload_responsible_set,
      { "responsible_ppb (uint32)",  "reload.responsible_set", FT_UINT32,
        BASE_HEX, NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_num_resources,
      { "num_resources (uint32)",  "reload.num_resources", FT_UINT32,
        BASE_DEC, NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_uptime,
      { "uptime (uint32)", "reload.uptime", FT_UINT32,
        BASE_DEC, NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_probeans,
      { "ProbeAns",  "reload.probeans", FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_probeans_probe_info,
      { "probe_info",  "reload.probeans", FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_appattachreq,
      { "AppAttachReq", "reload.appattachreq", FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_appattachans,
      { "AppAttachAns", "reload.appattachans", FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_application,
      { "application (uint16)", "reload.application", FT_UINT16,
        BASE_DEC, VALS(applicationids), 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_ping_response_id,
      { "response_id (uint64)", "reload.ping.response_id",  FT_UINT64,
        BASE_DEC, NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_ping_time,
      { "time (uint64)", "reload.ping.time", FT_ABSOLUTE_TIME,
        ABSOLUTE_TIME_UTC, NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_storeddata,
      { "StoredData",  "reload.storeddata", FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_storedmetadata,
      { "StoredMetaData",  "reload.storedmetadata", FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_storeddata_storage_time,
      { "storage_time (uint64)", "reload.storeddata.storage_time", FT_ABSOLUTE_TIME,
        ABSOLUTE_TIME_UTC, NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_storeddata_lifetime,
      { "lifetime (uint32)",  "reload.storeddata.lifetime", FT_UINT32,
        BASE_DEC, NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_datavalue,
      { "DataValue",  "reload.datavalue", FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_value,
      { "value",  "reload.value", FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_metadata,
      { "MetaData",  "reload.metadata", FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_datavalue_exists,
      { "exists (Boolean)", "reload.datavalue.exists",  FT_BOOLEAN,
        BASE_DEC, NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_datavalue_value,
      { "value",    "reload.datavaluevalue", FT_NONE,
        BASE_NONE,  NULL,   0x0,  NULL, HFILL
      }
    },
    { &hf_reload_metadata_value_length,
      { "value_length (uint32)",  "reload.metadata.value_length", FT_UINT32,
        BASE_DEC, NULL, 0x0,  NULL, HFILL
      }
    },
    { & hf_reload_metadata_hash_value,
      { "hash_value",  "reload.metadata.hash_value", FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_arrayentry,
      { "ArrayEntry",  "reload.arrayentry", FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_arrayentry_index,
      { "index (uint32)",  "reload.arrayentry.index", FT_UINT32,
        BASE_DEC, NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_arrayentry_value,
      { "value",  "reload.arrayentry.value", FT_NONE,
        BASE_NONE, NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_dictionaryentry,
      { "DictionaryEntry",  "reload.dictionaryentry", FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_dictionarykey,
      { "key (DictionaryKey)",  "reload.dictionarykey", FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_dictionary_value,
      { "value (DataValue)",  "reload.dictionary.value", FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_kinddata,
      { "StoreKindData",  "reload.kinddata", FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_kinddata_kind,
      { "kind (KindId)",  "reload.kinddata.kind", FT_UINT32,
        BASE_DEC,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_statkindresponse,
      { "StatKindResponse",  "reload.statkindresponse", FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_kindid,
      { "KindId",  "reload.kindid",  FT_UINT32,
        BASE_DEC, NULL,  0x0,  NULL, HFILL
      }
    },
    { &hf_reload_kindid_list,
      { "kinds",  "reload.kindid_list", FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_generation_counter,
      { "generation_counter (uint64)", "reload.generation_counter", FT_UINT64,
        BASE_DEC, NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_values,
      { "values",  "reload.kinddata.values_length",  FT_NONE,
        BASE_NONE, NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_storereq,
      { "StoreReq", "reload.storereq", FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_resource,
      { "resource", "reload.resource", FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_store_replica_num,
      { "replica_number (uint8)",  "reload.store.replica_number", FT_UINT8,
        BASE_DEC, NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_store_kind_data,
      { "kind_data",  "reload.store.kind_data",  FT_NONE,
        BASE_NONE, NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_storeans,
      { "StoreAns", "reload.storeans", FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_storeans_kind_responses,
      { "kind_responses", "reload.storeans.kind_responses", FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_storekindresponse,
      { "StoreKindResponse", "reload.storekindresponse", FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_replicas,
      { "replicas", "reload.storekindresponse.replicas", FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_statreq,
      { "StatReq", "reload.statreq", FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_fetchans,
      { "FetchAns", "reload.fetchans", FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_fetchreq,
      { "FetchReq", "reload.fetchreq", FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_fetchreq_specifiers,
      { "specifiers", "reload.fetchreq.specifiers", FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_arrayrange,
      { "ArrayRange", "reload.arrayrange", FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_storeddataspecifier,
      { "StoredDataSpecifier", "reload.storeddataspecifier", FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_storeddataspecifier_indices,
      { "indices", "reload.storeddataspecifier.indices", FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_storeddataspecifier_keys,
      { "indices", "reload.storeddataspecifier.keys", FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_statans,
      { "StatAns",  "reload.statans", FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_findans,
      { "FindAns",  "reload.findans", FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_findkinddata_closest,
      { "closest",  "reload.findkindata.closest", FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_findkinddata,
      { "FindKindData", "reload.findkinddata", FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_fragment_overlap,
      { "Fragment overlap", "reload.fragment.overlap", FT_BOOLEAN, BASE_NONE, NULL, 0x0,
        "Fragment overlaps with other fragments", HFILL
      }
    },

    { &hf_reload_fragment_overlap_conflict,
      { "Conflicting data in fragment overlap", "reload.fragment.overlap.conflict", FT_BOOLEAN, BASE_NONE, NULL, 0x0,
        "Overlapping fragments contained conflicting data", HFILL
      }
    },

    { &hf_reload_fragment_multiple_tails,
      { "Multiple tail fragments found",  "reload.fragment.multipletails", FT_BOOLEAN, BASE_NONE, NULL, 0x0,
        "Several tails were found when defragmenting the packet", HFILL
      }
    },

    { &hf_reload_fragment_too_long_fragment,
      { "Fragment too long",  "reload.fragment.toolongfragment", FT_BOOLEAN, BASE_NONE, NULL, 0x0,
        "Fragment contained data past end of packet", HFILL
      }
    },

    { &hf_reload_fragment_error,
      { "Defragmentation error", "reload.fragment.error", FT_FRAMENUM, BASE_NONE, NULL, 0x0,
        "Defragmentation error due to illegal fragments", HFILL
      }
    },

    { &hf_reload_fragment_count,
      { "Fragment count", "reload.fragment.count", FT_UINT32, BASE_DEC, NULL, 0x0,
        NULL, HFILL
      }
    },

    { &hf_reload_fragment,
      { "RELOAD fragment", "reload.fragment", FT_FRAMENUM, BASE_NONE, NULL, 0x0,
        NULL, HFILL
      }
    },

    { &hf_reload_fragments,
      { "RELOAD fragments", "reload.fragments", FT_NONE, BASE_NONE, NULL, 0x0,
        NULL, HFILL
      }
    },

    { &hf_reload_reassembled_in,
      { "Reassembled RELOAD in frame", "reload.reassembled_in", FT_FRAMENUM, BASE_NONE, NULL, 0x0,
        "This RELOAD packet is reassembled in this frame", HFILL
      }
    },

    { &hf_reload_reassembled_length,
      { "Reassembled RELOAD length", "reload.reassembled.length", FT_UINT32, BASE_DEC, NULL, 0x0,
        "The total length of the reassembled payload", HFILL
      }
    },

    { &hf_reload_configupdatereq,
      { "ConfigUpdateReq",  "reload.configupdatereq.",  FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },

    { &hf_reload_configupdatereq_type,
      { "type (ConfigUpdateType)", "reload.configupdatereq.type", FT_UINT8,
        BASE_DEC, VALS(configupdatetypes),  0x0,  NULL, HFILL
      }
    },

    { &hf_reload_configupdatereq_configdata,
      { "config_data",  "reload.configupdatereq.config_data",  FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },

    { &hf_reload_configupdatereq_kinds,
      { "kinds",  "reload.configupdatereq.kinds",  FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_kinddescription,
      { "KindDescription",  "reload.configupdatereq.kinds",  FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_pingreq,
      { "PingReq",  "reload.padding",  FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_pingans,
      { "PingAns",  "reload.padding",  FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_padding,
      { "padding",  "reload.padding",  FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },

    { &hf_reload_chordupdate,
      { "ChordUpdate",  "reload.chordupdate",  FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_chordupdate_type,
      { "type (ChordUpdateType)", "reload.chordupdate.type", FT_UINT8,
        BASE_DEC, VALS(chordupdatetypes),  0x0,  NULL, HFILL
      }
    },
    { &hf_reload_chordupdate_predecessors,
      { "predecessors",  "reload.chordupdate.predecessors",  FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_chordupdate_successors,
      { "successors",  "reload.chordupdate.predecessors",  FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_chordupdate_fingers,
      { "fingers",  "reload.chordupdate.fingers",  FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_chordroutequeryans,
      { "ChordRouteQueryAns",  "reload.chordroutequeryans",  FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_chordroutequeryans_next_peer,
      { "next_peer (NodeId)",  "reload.chordroutequeryans.nodeid",  FT_BYTES,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_chordleave,
      { "ChordLeaveData",  "reload.chordleavedata",  FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_chordleave_type,
      { "type (ChordLeaveType)", "reload.chordleavedata.type", FT_UINT8,
        BASE_DEC, VALS(chordleavetypes),  0x0,  NULL, HFILL
      }
    },
    { &hf_reload_chordleave_predecessors,
      { "predecessors",  "reload.chordleavedata.predecessors",  FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_chordleave_successors,
      { "successors",  "reload.chordleavedata.predecessors",  FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_turnserver,
      { "TurnServer",  "reload.turnserver",  FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_turnserver_iteration,
      { "iteration (uint8)",  "reload.turnserver.iteration",  FT_UINT8,
        BASE_DEC,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_turnserver_server_address,
      { "server_address",  "reload.turnserver.server_address",  FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_sipregistration,
      { "SipRegistration",  "reload.sipregistration",  FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_sipregistration_type,
      { "type (SipRegistrationType)",  "reload.sipregistration.type",  FT_UINT8,
        BASE_DEC,  VALS(sipregistrationtypes), 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_sipregistration_data,
      { "data (SipRegistrationData)",  "reload.sipregistration.data",  FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_sipregistration_data_uri,
      { "uri",  "reload.sipregistration.data.uri",  FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_sipregistration_data_contact_prefs,
      { "contact_prefs",  "reload.sipregistration.data.contact_prefs",  FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_sipregistration_data_destination_list,
      { "destination_list",  "reload.sipregistration.data.destination_list",  FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_redirserviceprovider,
      { "RedirServiceProvider",  "reload.redirserviceprovider",  FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_redirserviceproviderdata,
      { "data (RedirServiceProviderData)",  "reload.redirserviceprovider.data",  FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_redirserviceproviderdata_serviceprovider,
      { "serviceProvider (NodeId)",  "reload.redirserviceprovider.data.serviceprovider",  FT_BYTES,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_redirserviceproviderdata_namespace,
      { "namespace",  "reload.reload.redirserviceprovider.data.namespace",  FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_redirserviceproviderdata_level,
      { "level (uint16)",  "reload.reload.redirserviceprovider.data.level",  FT_UINT16,
        BASE_DEC,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_redirserviceproviderdata_node,
      { "node (uint16)",  "reload.reload.redirserviceprovider.data.node",  FT_UINT16,
        BASE_DEC,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_self_tuning_data,
      { "SelfTuningData",  "reload.selftuning_data",  FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_self_tuning_data_network_size,
      { "network_size (uint32)",  "reload.selftuning_data.network_size",  FT_UINT32,
        BASE_DEC,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_self_tuning_data_join_rate,
      { "join_rate (uint32)",  "reload.selftuning_data.join_rate",  FT_UINT32,
        BASE_DEC,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_self_tuning_data_leave_rate,
      { "leave_rate (uint32)",  "reload.selftuning_data.leave_rate",  FT_UINT32,
        BASE_DEC,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_findreq,
      { "FindReq",  "reload.findreq",  FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_dmflags,
      { "dMFlags (uint64)",  "reload.dmflags",  FT_UINT64,
        BASE_HEX,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_dmflag_status_info,
      { "STATUS_INFO", "reload.dmflags.status_info", FT_BOOLEAN, 1, TFS(&tfs_set_notset), 0x0,
        NULL, HFILL
      }
    },
    { &hf_reload_dmflag_routing_table_size,
      { "ROUTING_TABLE_SIZE", "reload.dmflags.routing_table_size", FT_BOOLEAN, 1, TFS(&tfs_set_notset), 0x0,
        NULL, HFILL
      }
    },
    { &hf_reload_dmflag_process_power,
      { "PROCESS_POWER", "reload.dmflags.process_power", FT_BOOLEAN, 1, TFS(&tfs_set_notset), 0x0,
        NULL, HFILL
      }
    },
    { &hf_reload_dmflag_bandwidth,
      { "BANDWIDTH", "reload.dmflags.bandwidth", FT_BOOLEAN, 1, TFS(&tfs_set_notset), 0x0,
        NULL, HFILL
      }
    },
    { &hf_reload_dmflag_software_version,
      { "SOFTWARE_VERSION", "reload.dmflags.software_version", FT_BOOLEAN, 1, TFS(&tfs_set_notset), 0x0,
        NULL, HFILL
      }
    },
    { &hf_reload_dmflag_machine_uptime,
      { "MACHINE_UPTIME", "reload.dmflags.machine_uptime", FT_BOOLEAN, 1, TFS(&tfs_set_notset), 0x0,
        NULL, HFILL
      }
    },
    { &hf_reload_dmflag_app_uptime,
      { "APP_UPTIME", "reload.dmflags.app_uptime", FT_BOOLEAN, 1, TFS(&tfs_set_notset), 0x0,
        NULL, HFILL
      }
    },
    { &hf_reload_dmflag_memory_footprint,
      { "MEMORY_FOOTPRINT", "reload.dmflags.memory_footprint", FT_BOOLEAN, 1, TFS(&tfs_set_notset), 0x0,
        NULL, HFILL
      }
    },
    { &hf_reload_dmflag_datasize_stored,
      { "DATASIZE_STORED", "reload.dmflags.datasize_stored", FT_BOOLEAN, 1, TFS(&tfs_set_notset), 0x0,
        NULL, HFILL
      }
    },
    { &hf_reload_dmflag_instances_stored,
      { "INSTANCES_STORED", "reload.dmflags.instances_stored", FT_BOOLEAN, 1, TFS(&tfs_set_notset), 0x0,
        NULL, HFILL
      }
    },
    { &hf_reload_dmflag_messages_sent_rcvd,
      { "MESSAGES_SENT_RCVD", "reload.dmflags.messages_sent_rcvd", FT_BOOLEAN, 1, TFS(&tfs_set_notset), 0x0,
        NULL, HFILL
      }
    },
    { &hf_reload_dmflag_ewma_bytes_sent,
      { "EWMA_BYTES_SENT", "reload.dmflags.ewma_bytes_sent", FT_BOOLEAN, 1, TFS(&tfs_set_notset), 0x0,
        NULL, HFILL
      }
    },
    { &hf_reload_dmflag_ewma_bytes_rcvd,
      { "EWMA_BYTES_RCVD", "reload.dmflags.ewma_bytes_rcvd", FT_BOOLEAN, 1, TFS(&tfs_set_notset), 0x0,
        NULL, HFILL
      }
    },
    { &hf_reload_dmflag_underlay_hop,
      { "UNDERLAY_HOP", "reload.dmflags.underlay_hop", FT_BOOLEAN, 1, TFS(&tfs_set_notset), 0x0,
        NULL, HFILL
      }
    },
    { &hf_reload_dmflag_battery_status,
      { "BATTERY_STATUS", "reload.dmflags.battery_status", FT_BOOLEAN, 1, TFS(&tfs_set_notset), 0x0,
        NULL, HFILL
      }
    },
    { &hf_reload_diagnosticrequest,
      { "DiagnosticRequest",  "reload.diagnosticrequest",  FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_diagnosticresponse,
      { "DiagnosticResponse",  "reload.diagnosticresponse",  FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_diagnosticextension,
      { "DiagnosticExtension",  "reload.diagnosticextension",  FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_diagnosticextension_type,
      { "type (DiagnosticExtensionRequestType)",  "reload.diagnosticextension.type",  FT_UINT16,
        BASE_DEC,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_diagnosticextension_contents,
      { "diagnostic_extension_contents",  "reload.diagnosticextension.contents",  FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_diagnostic_expiration, {
        "expiration (uint64)", "reload.diagnostic.expiration", FT_ABSOLUTE_TIME, ABSOLUTE_TIME_LOCAL,
        NULL, 0x0, NULL, HFILL
      }
    },
    { &hf_reload_diagnosticrequest_timestampinitiated, {
        "timestampInitiated (uint64)", "reload.diagnosticrequest.timestampinitiated",FT_ABSOLUTE_TIME, ABSOLUTE_TIME_LOCAL,
        NULL, 0x0, NULL, HFILL
      }
    },
    { &hf_reload_diagnosticrequest_extensions,
      { "diagnostic_extensions",  "reload.diagnosticrequest.extensions",  FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_pathtrackreq,
      { "PathTrackReq",  "reload.pathtrackreq",  FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_pathtrackreq_destination,
      { "destination (Destination)",  "reload.pathtrackreq.destination",  FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_pathtrackreq_request,
      { "request (DiagnosticRequest)",  "reload.pathtrackreq.request",  FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_diagnosticinfo,
      { "DiagnosticInfo",  "reload.diagnostic.info",  FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_diagnosticinfo_kind,
      { "kind (DiagnosticKindId)",  "reload.diagnostic.kindid",  FT_UINT16,
        BASE_DEC,  VALS(diagnostickindids), 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_diagnosticinfo_congestion_status,
      { "congestion_status (uint8)",  "reload.diagnostic.info.congestion_status",  FT_UINT8,
        BASE_DEC,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_diagnosticinfo_number_peers,
      { "number_peers (uint32)",  "reload.diagnostic.info.number_peers",  FT_UINT32,
        BASE_DEC,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_diagnosticinfo_processing_power,
      { "processing_power (uint32)",  "reload.diagnostic.info.processing_power",  FT_UINT32,
        BASE_DEC,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_diagnosticinfo_bandwidth,
      { "bandwidth (uint32)",  "reload.diagnostic.info.bandwidth",  FT_UINT32,
        BASE_DEC,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_diagnosticinfo_software_version,
      { "software_version (opaque string)",  "reload.diagnostic.info.software_version",  FT_STRING,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_diagnosticinfo_machine_uptime,
      { "machine_uptime (uint64)",  "reload.diagnostic.info.machine_uptime",  FT_UINT64,
        BASE_DEC,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_diagnosticinfo_app_uptime,
      { "app_uptime (uint64)",  "reload.diagnostic.info.app_uptime",  FT_UINT64,
        BASE_DEC,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_diagnosticinfo_memory_footprint,
      { "memory_footprint(uint32)",  "reload.diagnostic.info.memory_footprint",  FT_UINT32,
        BASE_DEC,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_diagnosticinfo_datasize_stored,
      { "datasize_stored (uint64)",  "reload.diagnostic.info.datasize_stored",  FT_UINT64,
        BASE_DEC,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_diagnosticinfo_instances_stored,
      { "instances_stored",  "reload.diagnostic.info.instances_stored",  FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_diagnosticinfo_instancesstored_info,
      { "InstancesStoredInfo)",  "reload.diagnostic.info.instancesstored_info",  FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_diagnosticinfo_instancesstored_instances,
      { "instances (uint64)",  "reload.diagnostic.info.instancesstored_instances",  FT_UINT64,
        BASE_DEC,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_diagnosticinfo_messages_sent_rcvd,
      { "messages_sent_rcvd",  "reload.diagnostic.info.messages_sent_rcvd",  FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_diagnosticinfo_messages_sent_rcvd_info,
      { "MessagesSentRcvdInfo",  "reload.diagnostic.info.messages_sent_rcvd.info",  FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_diagnosticinfo_message_code,
      { "messages_code (uint16)",  "reload.diagnostic.info.message_code",  FT_UINT16,
        BASE_HEX,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_diagnosticinfo_messages_sent,
      { "sent (uint64)",  "reload.diagnostic.info.messages_sent",  FT_UINT64,
        BASE_DEC,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_diagnosticinfo_messages_rcvd,
      { "rcvd (uint64)",  "reload.diagnostic.info.messages_rcvd",  FT_UINT64,
        BASE_DEC,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_diagnosticinfo_ewma_bytes_sent,
      { "ewma_bytes_sent (uint32)",  "reload.diagnostic.info.ewma_bytes_sent",  FT_UINT32,
        BASE_DEC,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_diagnosticinfo_ewma_bytes_rcvd,
      { "ewma_bytes_rcvd (uint32)",  "reload.diagnostic.info.ewma_bytes_rcvd",  FT_UINT32,
        BASE_DEC,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_diagnosticinfo_underlay_hops,
      { "underlay_hops (uint8)",  "reload.diagnostic.info.underlay_hops",  FT_UINT8,
        BASE_DEC,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_diagnosticinfo_battery_status,
      { "battery_status (uint8)",  "reload.diagnostic.info.battery_status",  FT_UINT8,
        BASE_DEC,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_diagnosticresponse_timestampreceived, {
        "timestampReceived (uint64)", "reload.diagnosticresponse.timestampreceived",FT_ABSOLUTE_TIME, ABSOLUTE_TIME_LOCAL,
        NULL, 0x0, NULL, HFILL
      }
    },
    { &hf_reload_diagnosticresponse_hopcounter,
      { "hopCounter (uint8)",  "reload.diagnosticresponse.hopcounter",  FT_UINT8,
        BASE_DEC,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_diagnosticresponse_diagnostic_info_list,
      { "diagnostic_info_list",  "reload.diagnosticresponse.diagnostic_info_list",  FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_pathtrackans,
      { "PathTrackAns",  "reload.pathtrackans",  FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_pathtrackans_next_hop,
      { "next_hop",  "reload.pathtrackans.next_hop",  FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_pathtrackans_response,
      { "response (DiagnosticResponse)",  "reload.pathtrackand.response",  FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_extensiveroutingmodeoption,
      { "ExtensiveRoutingModeOption",  "reload.extensiveroutingmodeoption",  FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_routemode,
      { "routemode (RouteMode)", "reload.routemode", FT_UINT8,
        BASE_DEC, VALS(routemodes),  0x0,  NULL, HFILL
      }
    },
    { &hf_reload_extensiveroutingmode_transport,
      { "transport (OverlayLinkType)",  "reload.extensiveroutingmode.transport",  FT_UINT8,
        BASE_DEC, VALS(overlaylinktypes), 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_extensiveroutingmode_ipaddressport,
      { "ipaddressport (IpAddressPort)",  "reload.extensiveroutingmode.ipaddressport",  FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_extensiveroutingmode_destination,
      { "destination",  "reload.extensiveroutingmode.destination",  FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_joinreq,
      { "JoinReq",  "reload.joinreq",  FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_joinans,
      { "JoinAns",  "reload.joinans",  FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_joinreq_joining_peer_id,
      { "joining_peer_id (NodeId)",  "reload.joinreq.joining_peer_id",  FT_BYTES,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_leavereq,
      { "LeaveReq",  "reload.leavereq",  FT_NONE,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },
    { &hf_reload_leavereq_leaving_peer_id,
      { "leaving_peer_id (NodeId)",  "reload.leavereq.leaving_peer_id",  FT_BYTES,
        BASE_NONE,  NULL, 0x0,  NULL, HFILL
      }
    },

  };

  /
  static gint *ett[] = {
    &ett_reload,
    &ett_reload_forwarding,
    &ett_reload_message,
    &ett_reload_security,
    &ett_reload_fragment_flag,
    &ett_reload_destination,
    &ett_reload_via_list,
    &ett_reload_destination_list,
    &ett_reload_resourceid,
    &ett_reload_forwarding_options,
    &ett_reload_forwarding_option,
    &ett_reload_forwarding_option_flags,
    &ett_reload_forwarding_option_directresponseforwarding,
    &ett_reload_attachreqans,
    &ett_reload_icecandidates,
    &ett_reload_icecandidate,
    &ett_reload_icecandidate_computed_address,
    &ett_reload_iceextensions,
    &ett_reload_iceextension,
    &ett_reload_ipaddressport,
    &ett_reload_ipv4addrport,
    &ett_reload_ipv6addrport,
    &ett_reload_message_contents,
    &ett_reload_message_extensions,
    &ett_reload_message_extension,
    &ett_reload_error_response,
    &ett_reload_security_block,
    &ett_reload_certificates,
    &ett_reload_genericcertificate,
    &ett_reload_signature,
    &ett_reload_signatureandhashalgorithm,
    &ett_reload_signeridentity,
    &ett_reload_signeridentity_identity,
    &ett_reload_signeridentity_value,
    &ett_reload_opaque,
    &ett_reload_message_body,
    &ett_reload_routequeryreq,
    &ett_reload_probereq,
    &ett_reload_probereq_requested_info,
    &ett_reload_probe_information,
    &ett_reload_probe_information_data,
    &ett_reload_probeans,
    &ett_reload_probeans_probe_info,
    &ett_reload_appattach,
    &ett_reload_pingreq,
    &ett_reload_pingans,
    &ett_reload_storeddata,
    &ett_reload_kinddata,
    &ett_reload_values,
    &ett_reload_datavalue,
    &ett_reload_arrayentry,
    &ett_reload_dictionaryentry,
    &ett_reload_storereq,
    &ett_reload_store_kind_data,
    &ett_reload_storeans,
    &ett_reload_storeans_kind_responses,
    &ett_reload_storekindresponse,
    &ett_reload_fetchans,
    &ett_reload_fetchreq,
    &ett_reload_fetchreq_specifiers,
    &ett_reload_storeddataspecifier,
    &ett_reload_storeddataspecifier_indices,
    &ett_reload_storeddataspecifier_keys,
    &ett_reload_statans,
    &ett_reload_findans,
    &ett_reload_findkinddata,
    &ett_reload_fragments,
    &ett_reload_fragment,
    &ett_reload_configupdatereq,
    &ett_reload_configupdatereq_config_data,
    &ett_reload_kinddescription,
    &ett_reload_configupdatereq_kinds,
    &ett_reload_storekindresponse_replicas,
    &ett_reload_nodeid_list,
    &ett_reload_chordupdate,
    &ett_reload_chordroutequeryans,
    &ett_reload_chordleave,
    &ett_reload_turnserver,
    &ett_reload_sipregistration,
    &ett_reload_sipregistration_data,
    &ett_reload_sipregistration_destination_list,
    &ett_reload_dictionaryentry_key,
    &ett_reload_overlay_specific,
    &ett_reload_kindid_list,
    &ett_reload_redirserviceproviderdata,
    &ett_reload_redirserviceprovider,
    &ett_reload_findreq,
    &ett_reload_dmflags,
    &ett_reload_diagnosticextension,
    &ett_reload_diagnosticrequest_extensions,
    &ett_reload_pathtrackreq,
    &ett_reload_diagnosticinfo,
    &ett_reload_diagnosticinfo_instances_stored,
    &ett_reload_diagnosticinfo_instancesstored_info,
    &ett_reload_diagnosticinfo_messages_sent_rcvd,
    &ett_reload_diagnosticinfo_messages_sent_rcvd_info,
    &ett_reload_diagnosticresponse,
    &ett_reload_diagnosticresponse_diagnostic_info_list,
    &ett_reload_pathtrackans,
    &ett_reload_extensiveroutingmodeoption,
    &ett_reload_extensiveroutingmode_destination,
    &ett_reload_joinreq,
    &ett_reload_joinans,
    &ett_reload_leavereq,
  };

  static uat_field_t reloadkindidlist_uats_flds[] = {
    UAT_FLD_DEC(kindidlist_uats,id,"Kind-ID Number","Custom Kind-ID Number"),
    UAT_FLD_CSTRING(kindidlist_uats,name,"Kind-ID Name","Custom Kind-ID Name"),
    UAT_FLD_VS(kindidlist_uats,data_model,"Kind-ID data model",datamodels,"Kind ID data model"),
    UAT_END_FIELDS
  };


  /
  proto_reload = proto_register_protocol("REsource LOcation And Discovery", "RELOAD", "reload");
  register_dissector("reload", dissect_reload_message_no_return, proto_reload);
  /
  proto_register_field_array(proto_reload, hf, array_length(hf));
  proto_register_subtree_array(ett, array_length(ett));

  reload_module = prefs_register_protocol(proto_reload, NULL);

  reloadkindids_uat =
    uat_new("Kind-ID Table",
            sizeof(kind_t),
            "reload_kindids",                     /
            TRUE,                           /
            (void*) &kindidlist_uats,       /
            &nreloadkinds,                   /
            UAT_CAT_GENERAL,                   /
            NULL,                           /
            uat_kindid_copy_cb,
            NULL,
            uat_kindid_record_free_cb,
            NULL,
            reloadkindidlist_uats_flds);


  prefs_register_uat_preference(reload_module, "kindid.table",
                                "Kind ID list",
                                "A table of Kind ID definitions",
                                reloadkindids_uat);

  prefs_register_bool_preference(reload_module, "defragment",
                                 "Reassemble fragmented reload datagrams",
                                 "Whether fragmented RELOAD datagrams should be reassembled",
                                 &reload_defragment);
  prefs_register_uint_preference(reload_module, "nodeid_length",
                                 "NodeId Length",
                                 "Length of the NodeId as defined in the overlay.",
                                 10,
                                 &reload_nodeid_length);
  prefs_register_string_preference(reload_module, "topology_plugin",
                                   "topology plugin", "topology plugin defined in the overlay", &reload_topology_plugin);

  register_init_routine(reload_defragment_init);
}
