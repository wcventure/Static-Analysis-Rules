static void dissect_uasip(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree)
{
	if (use_proxy_ipaddr)
	{
		if (memcmp((pinfo->src).data, proxy_ipaddr, 4*sizeof(guint8)) == 0)
		{
			_dissect_uasip(tvb, pinfo, tree, SYS_TO_TERM);
			return;
		}
		else if (memcmp((pinfo->dst).data, proxy_ipaddr, 4*sizeof(guint8)) == 0)
		{
			_dissect_uasip(tvb, pinfo, tree, TERM_TO_SYS);
			return;
		}
	}
	_dissect_uasip(tvb, pinfo, tree, DIR_UNKNOWN);
}
