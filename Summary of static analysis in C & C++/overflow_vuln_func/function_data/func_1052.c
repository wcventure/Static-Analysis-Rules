int network_instruments_dump_can_write_encap(int encap)
{
	/
	if (encap == WTAP_ENCAP_PER_PACKET)
		return WTAP_ERR_ENCAP_PER_PACKET_UNSUPPORTED;

	if (encap < 0 || (unsigned) encap > NUM_FROM_WTAP_ENCAPS || from_wtap_encap[encap] == OBSERVER_UNDEFINED)
		return WTAP_ERR_UNSUPPORTED_ENCAP;

	return 0;
}
