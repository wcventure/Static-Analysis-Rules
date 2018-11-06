void
ipv4_addr_set_netmask_bits(ipv4_addr *ipv4, guint new_nmask_bits)
{
/
	ipv4->nmask = create_nmask(new_nmask_bits);
}
