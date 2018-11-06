#ifdef HAVE_LIBGCRYPT
static gboolean
esp_sa_parse_ipv4addr(const gchar *sa, guint index_start, gchar **pt_ipv4addr, guint *index_end)
{
  guint cpt = 0;
  guint strlen_max = 16;
  
  char addr_string[strlen_max];
  gboolean done_flag = FALSE;  

  if((sa == NULL) || (strcmp(sa, "") == 0))  return FALSE;
  
  /
  while(((cpt + index_start) < strlen(sa)) && (done_flag == FALSE) && (cpt <= strlen_max))	
    {   
      if((sa[cpt + index_start] == IPSEC_SA_SEPARATOR)  || (sa[cpt + index_start] == IPSEC_SA_ADDR_LEN_SEPARATOR)) 
	{
	  if(cpt == 0) return FALSE;
	  *index_end = cpt + index_start;
	  addr_string[cpt] = '\0';
	  done_flag = TRUE;
	}
      
      else 
	{
	  if((cpt == strlen_max - 1) && ((cpt  + index_start) < strlen(sa)) && (sa[cpt + index_start + 1] != IPSEC_SA_ADDR_LEN_SEPARATOR) && (sa[cpt + index_start + 1] != IPSEC_SA_SEPARATOR)) return FALSE;  
	  addr_string[cpt] = toupper(sa[cpt + index_start]);
	  cpt ++;
	}	
    }
  
  if(done_flag)
    {
      *pt_ipv4addr = (gchar *)g_malloc((strlen(addr_string) + 1) * sizeof(gchar));
      memcpy(*pt_ipv4addr,addr_string,strlen(addr_string) + 1);
    }

  return done_flag;
}  
