static void dissect_ua_sys_to_term(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree)
{
	_dissect_ua_msg(tvb, pinfo, tree, SYS_TO_TERM);
}
