static void
dissect_lcp_options(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree)
{
  dissect_ip_tcp_options(tvb, 0, tvb_reported_length(tvb), lcp_opts, N_LCP_OPTS, 
            -1, pinfo, tree);
}
