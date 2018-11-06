static gboolean
guess_byte_ordering(tvbuff_t *tvb, packet_info *pinfo)
{
      /

      int le, be, decision, decisionToCache;

      gboolean is_reply = (pinfo->srcport == pinfo->match_port);
      address *addr = is_reply ? &pinfo->net_dst : &pinfo->net_src;
      gint32 cache = GPOINTER_TO_INT(g_tree_lookup(byte_ordering_cache, addr));
      if (cache) return cache > 0 ? TRUE : FALSE;
      if (is_reply) return TRUE; /

      le = x_endian_match(tvb, tvb_get_letohs);
      be = x_endian_match(tvb, tvb_get_ntohs);

      /
      if (le == be) {
	    /
	    if (!tvb_bytes_exist(tvb, 0, 4))
		  /
		  decision = TRUE;
	    else
		  decision = tvb_get_letohs(tvb, 2) <= tvb_get_ntohs(tvb, 2);
      } else
	  decision = le >= be;

      decisionToCache = (le < 0 && be > 0) || (le > 0 && be < 0);
      if (decisionToCache) {
	    /
	    int address_length;
	    char *address_data;
	    address *cached;
	    if (addr -> type == AT_IPv4) {
		  address_length = 4;
		  address_data = g_mem_chunk_alloc(ipv4_chunk);
	    } else if (addr -> type == AT_IPv6) {
		  address_length = 16;
		  address_data = g_mem_chunk_alloc(ipv6_chunk);
	    } else {
		  address_length = addr -> len;
		  address_data = g_malloc(address_length);
	    }
	    cached = g_mem_chunk_alloc(address_chunk);
	    memcpy(address_data, addr -> data, address_length);
	    SET_ADDRESS(cached, addr -> type, addr -> len, address_data);
	    g_tree_insert(byte_ordering_cache, cached, GINT_TO_POINTER(decision ? 1 : -1));
      }
	    
      /
      return decision;
}
