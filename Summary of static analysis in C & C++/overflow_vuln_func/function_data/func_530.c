static int
jxta_conversation_packet(void *pct, packet_info *pinfo, epan_dissect_t *edt _U_, const void *vip)
{
	const jxta_tap_header *jxtahdr = (const jxta_tap_header *) vip;

	add_conversation_table_data((conversations_table *)pct, 
		&jxtahdr->src_address, 
		&jxtahdr->dest_address, 
		0, 
		0, 
		1, 
		jxtahdr->size, 
		SAT_NONE, 
		PT_NONE);


	return 1;
}
