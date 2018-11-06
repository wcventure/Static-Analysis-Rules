static void dissect_uaudp(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree)
{
	/
	if (use_sys_ip) {
		/
		if (memcmp((pinfo->src).data, sys_ip, 4*sizeof(guint8)) == 0)
		{
			_dissect_uaudp(tvb, pinfo, tree, SYS_TO_TERM);
			return;
		}
		else if (memcmp((pinfo->dst).data, sys_ip, 4*sizeof(guint8)) == 0)
		{
			_dissect_uaudp(tvb, pinfo, tree, TERM_TO_SYS);
			return;
		}
	}

	/
	if (find_terminal_port(pinfo->srcport))
	{
		_dissect_uaudp(tvb, pinfo, tree, TERM_TO_SYS);
		return;
	}
	else if (find_terminal_port(pinfo->destport))
	{
		_dissect_uaudp(tvb, pinfo, tree, SYS_TO_TERM);
		return;
	}

	_dissect_uaudp(tvb, pinfo, tree, DIR_UNKNOWN);
}
