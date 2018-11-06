static int
jxta_hostlist_packet(void *pit, packet_info *pinfo _U_, epan_dissect_t *edt _U_, const void *vip)
{
	hostlist_table *hosts = (hostlist_table *) pit;
	const jxta_tap_header *jxtahdr = vip;

	/
	add_hostlist_table_data(hosts, &jxtahdr->src_address, 0, TRUE, 1, jxtahdr->size, SAT_NONE, PT_NONE);
	add_hostlist_table_data(hosts, &jxtahdr->dest_address, 0, FALSE, 1, jxtahdr->size, SAT_NONE, PT_NONE);
	return 1;
}
