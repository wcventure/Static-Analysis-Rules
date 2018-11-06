void
proto_reg_handoff_ber(void)
{
        dissector_handle_t ber_handle;

	
	oid_add_from_string("itu-t","0");
	oid_add_from_string("iso","1");
	oid_add_from_string("joint-iso-itu-t","2");
	oid_add_from_string("asn1","2.1");
	oid_add_from_string("basic-encoding","2.1.1");

	ber_handle = create_dissector_handle(dissect_ber, proto_ber);
	dissector_add("wtap_encap", WTAP_ENCAP_BER, ber_handle);
}
