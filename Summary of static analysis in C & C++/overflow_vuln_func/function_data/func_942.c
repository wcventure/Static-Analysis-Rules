#ifdef HAVE_LIBGCRYPT
static gboolean
esp_sa_parse_protocol_typ(const gchar *sa, guint index_start, gint *pt_protocol_typ, guint *index_end)
{
  guint cpt = 0;
  guint typ_len_max = 4;  
  gchar typ_string[typ_len_max];
  gboolean done_flag = FALSE;  

  *pt_protocol_typ = IPSEC_SA_UNKNOWN;
  if((sa == NULL) || (strcmp(sa, "") == 0))  return FALSE;
  
  while(((cpt + index_start) < strlen(sa)) && (done_flag == FALSE) && (cpt <= typ_len_max) && (sa[cpt + index_start] != IPSEC_SA_SEPARATOR))	
    {     
      typ_string[cpt] = toupper(sa[cpt + index_start]);
      cpt ++;
    }	
  
  if(cpt == 0) done_flag = FALSE;
  else
    {     
      typ_string[cpt] = '\0';
      if(strcmp(typ_string, "IPV6") == 0) 
	{ 
	  *pt_protocol_typ = IPSEC_SA_IPV6;
	  done_flag = TRUE;
	}
      else if (strcmp(typ_string, "IPV4") == 0) 
	{ 
	  *pt_protocol_typ = IPSEC_SA_IPV4;
	  done_flag = TRUE;
	}
      else
	{
	  *pt_protocol_typ = IPSEC_SA_UNKNOWN;
	  done_flag = FALSE;
	}
      
      *index_end = cpt + index_start + 1;
    }


  return done_flag;
}  
