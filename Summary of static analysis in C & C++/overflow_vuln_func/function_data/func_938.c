#ifdef HAVE_LIBGCRYPT
static gboolean 
get_full_ipv4_addr(char* ipv4_addr_expanded, char *ipv4_addr)
{
  char addr_byte_string_tmp[4];
  char addr_byte_string[4];

  guint addr_byte = 0;
  guint i = 0;
  guint j = 0;
  guint k = 0;
  guint cpt = 0;
  gboolean done_flag = FALSE;
  
  if((ipv4_addr == NULL) || (strcmp(ipv4_addr, "") == 0))  return done_flag;

  if((strlen(ipv4_addr) == 1) && (ipv4_addr[0] == IPSEC_SA_WILDCARDS_ANY))
    {
      for(i = 0; i <= IPSEC_STRLEN_IPV4; i++)
	{
	  ipv4_addr_expanded[i] = IPSEC_SA_WILDCARDS_ANY;
	}
      ipv4_addr_expanded[IPSEC_STRLEN_IPV4] = '\0';
      done_flag = TRUE;
    }

  else {
    j = 0;
    cpt = 0;
    k = 0;
    while((done_flag == FALSE) && (j <= strlen(ipv4_addr)) && (cpt < IPSEC_STRLEN_IPV4))
      {      
	if(j == strlen(ipv4_addr))
	  {
	    addr_byte_string_tmp[k] = '\0';
	    if((strlen(addr_byte_string_tmp) == 1) && (addr_byte_string_tmp[0] == IPSEC_SA_WILDCARDS_ANY))
	      {
		for(i = 0; i < 2; i++)
		  {
		    ipv4_addr_expanded[cpt] = IPSEC_SA_WILDCARDS_ANY;
		    cpt ++;
		  }
	      }
	    else
	      {
		sscanf(addr_byte_string_tmp,"%u",&addr_byte);
		if(addr_byte < 16) g_snprintf(addr_byte_string,4,"0%X",addr_byte); 
		else g_snprintf(addr_byte_string,4,"%X",addr_byte);	    
		for(i = 0; i < strlen(addr_byte_string); i++)
		  {
		    ipv4_addr_expanded[cpt] = addr_byte_string[i];
		    cpt ++;
		  }
	      }
	    done_flag = TRUE;
	  }
	
	else if(ipv4_addr[j] == '.')
	  {
	    addr_byte_string_tmp[k] = '\0';
	    if((strlen(addr_byte_string_tmp) == 1) && (addr_byte_string_tmp[0] == IPSEC_SA_WILDCARDS_ANY))
	      {
		for(i = 0; i < 2; i++)
		  {
		    ipv4_addr_expanded[cpt] = IPSEC_SA_WILDCARDS_ANY;
		    cpt ++;
		  }
	      }
	    else
	      {
		sscanf(addr_byte_string_tmp,"%u",&addr_byte);
		if(addr_byte < 16) g_snprintf(addr_byte_string,4,"0%X",addr_byte);	    
		else g_snprintf(addr_byte_string,4,"%X",addr_byte);	    
		for(i = 0; i < strlen(addr_byte_string); i++)
		  {
		    ipv4_addr_expanded[cpt] = addr_byte_string[i];
		    cpt ++;
		  }
	      }
	    k = 0;
	    j++;
	  }
	else
	  {
	    if(k >= 3) 
	      {
		/
		addr_byte_string_tmp[0] = ipv4_addr[j];
		k = 1;
		j++;
	      }
	    else 
	      {
		addr_byte_string_tmp[k] = ipv4_addr[j];
		k++;
		j++;
	      }
	  }
	
      }

    ipv4_addr_expanded[cpt] = '\0';
  }

  return done_flag;
}
