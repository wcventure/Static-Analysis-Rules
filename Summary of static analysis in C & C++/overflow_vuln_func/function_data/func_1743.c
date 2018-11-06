void
ipv4_addr_str_buf(const ipv4_addr *ipv4, gchar *buf)
{
	guint32	ipv4_host_order = g_htonl(ipv4->addr);
	ip_to_str_buf((guint8*)&ipv4_host_order, buf);
}
