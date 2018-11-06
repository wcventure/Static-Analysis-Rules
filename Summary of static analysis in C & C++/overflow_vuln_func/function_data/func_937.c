#ifdef HAVE_LIBGCRYPT
static int 
get_full_ipv6_addr(char* ipv6_addr_expanded, char *ipv6_addr)
{
  char suffix[IPSEC_STRLEN_IPV6 + 1];
  char prefix[IPSEC_STRLEN_IPV6 + 1];
  
  int suffix_cpt = 0;
  int suffix_len = 0;
  int prefix_remaining = 0;
  int prefix_len = 0;
  int j = 0;

  
  if((ipv6_addr == NULL) || (strcmp(ipv6_addr, "") == 0))  return -1;
  if((strlen(ipv6_addr) == 1) && (ipv6_addr[0] == IPSEC_SA_WILDCARDS_ANY))
    {
      for(j = 0; j <= IPSEC_STRLEN_IPV6; j++)
	{
	  ipv6_addr_expanded[j] = IPSEC_SA_WILDCARDS_ANY;
	}
      ipv6_addr_expanded[IPSEC_STRLEN_IPV6] = '\0';
      return 0;
    }

  suffix_cpt = get_ipv6_suffix(suffix,ipv6_addr);
  suffix_len = strlen(suffix);

  if(suffix_len <  IPSEC_STRLEN_IPV6)
    {
      char prefix_addr[strlen(ipv6_addr) - suffix_cpt + 1];
      memcpy(prefix_addr,ipv6_addr,strlen(ipv6_addr) - suffix_cpt);
      prefix_addr[strlen(ipv6_addr) - suffix_cpt] = '\0';
      prefix_remaining = get_ipv6_suffix(prefix,prefix_addr);
      prefix_len = strlen(prefix);
      memcpy(ipv6_addr_expanded,prefix,prefix_len);
    }
  
  
  for(j = 0; j <= IPSEC_STRLEN_IPV6 - prefix_len - suffix_len; j++)
    {
      ipv6_addr_expanded[j + prefix_len] = '0';
    }
  
  memcpy(ipv6_addr_expanded + IPSEC_STRLEN_IPV6 - suffix_len, suffix,suffix_len + 1);

  if(suffix_len < IPSEC_STRLEN_IPV6)
    return (prefix_len - prefix_remaining); 
  else 
    return strlen(ipv6_addr) - suffix_cpt;
}
