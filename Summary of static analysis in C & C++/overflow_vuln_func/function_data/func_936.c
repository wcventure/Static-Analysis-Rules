#ifdef HAVE_LIBGCRYPT 
static int get_ipv6_suffix(char* ipv6_suffix, char *ipv6_address)
{
  char suffix[IPSEC_STRLEN_IPV6 + 1];
  int cpt = 0;
  int cpt_suffix = 0;
  int cpt_seg = 0;
  int j =0;
  int ipv6_len = 0;
  gboolean found = FALSE;
  
  ipv6_len = strlen(ipv6_address);
  if(ipv6_len  == 0) 
    {
      /
      found = TRUE;	     
    }
  else 
    {      
      while ( (cpt_suffix < IPSEC_STRLEN_IPV6) && (ipv6_len - cpt -1 >= 0) && (found == FALSE))
	{
	  if(ipv6_address[ipv6_len - cpt - 1] == ':')
	    {
	      /
	      for(j = cpt_seg; j < 4; j++)
		{
		  suffix[IPSEC_STRLEN_IPV6 -1 -cpt_suffix] = '0';
		  cpt_suffix ++;
		}
	      cpt_seg = 0;
	      
	      if(ipv6_len - cpt - 1 == 0)
		{
		  /
		  found = TRUE;	      
		}	  
	      else 
		if(ipv6_address[ipv6_len - cpt - 2] == ':')
		  {
		    /
		    cpt +=2;
		    found = TRUE;	     
		  }
	  
		else
		  {
		    cpt++;
		  }	  	  
	    }
	  else
	    {	  
	      suffix[IPSEC_STRLEN_IPV6 -1 -cpt_suffix] = toupper(ipv6_address[ipv6_len - cpt - 1]);
	      cpt_seg ++;
	      cpt_suffix ++;
	      cpt++;
	    }    
	}      

      if(cpt_suffix % 4 != 0)
	{
	  for(j = cpt_seg; j < 4; j++)
	    {
	      suffix[IPSEC_STRLEN_IPV6 -1 -cpt_suffix] = '0';
	      cpt_suffix ++;
	    }
	  cpt_seg = 0;
	}

    }
  
  for(j = 0 ; j < cpt_suffix ; j ++)
    {	
      suffix[j] = suffix[j + IPSEC_STRLEN_IPV6 - cpt_suffix] ;
    }
    
  suffix[j] = '\0';  
  memcpy(ipv6_suffix,suffix,j + 1);
  return cpt;
}
