#ifdef HAVE_LIBGCRYPT
static gboolean
esp_sa_parse_addr_len(const gchar *sa, guint index_start, gint *len, guint *index_end)
{  
  guint cpt = 0;
  guint strlen_max = 3;  
  char len_string[strlen_max];
  gboolean done_flag = FALSE;  

  *len = -1;	  

  if((sa == NULL) || (strcmp(sa, "") == 0))  return FALSE;

  if(sa[index_start] == IPSEC_SA_SEPARATOR) 
    {
      *index_end = index_start + 1;
      *len = -1;
      done_flag = TRUE;
    }

  else if(sa[index_start] == IPSEC_SA_ADDR_LEN_SEPARATOR) 
    {
      cpt ++;
      while(((cpt + index_start) < strlen(sa)) && (done_flag == FALSE) && (cpt <= strlen_max + 1))	
	{           
	  if(sa[cpt + index_start] == IPSEC_SA_SEPARATOR) 
	    {
	      if(cpt == 1) 
		{		
		  *index_end = index_start + cpt + 1;
		  *len = -1;
		  done_flag = TRUE;
		  
		}
	      else
		{
		  *index_end = cpt + index_start + 1;
		  len_string[cpt - 1] = '\0';
		  *len = atoi(len_string);
		  done_flag = TRUE;
		}
	    }
      
	  else 
	    {
	      if((cpt == strlen_max) && ((cpt  + index_start) < strlen(sa)) && (sa[cpt + index_start + 1] != IPSEC_SA_ADDR_LEN_SEPARATOR) && (sa[cpt + index_start + 1] != IPSEC_SA_SEPARATOR)) return FALSE;  
	      len_string[cpt -1] = sa[cpt + index_start];
	      cpt ++;
	    }	
	}
    }
  
  return done_flag;
}  
