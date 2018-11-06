#ifdef HAVE_LIBGCRYPT
static gboolean
esp_sa_parse_spi(const gchar *sa, guint index_start, gchar **pt_spi, guint *index_end)
{
  guint cpt = 0;
  guint spi_len_max = 10;
  guint32 spi = 0;
  guint i = 0;

  gchar spi_string[spi_len_max];
  gchar spi_string_tmp[spi_len_max];
  gboolean done_flag = FALSE;  

  if((sa == NULL) || (strcmp(sa, "") == 0))  return FALSE;
  
  while(((cpt + index_start) < strlen(sa)) && (done_flag == FALSE) && (cpt <= spi_len_max))	
    {     
      spi_string[cpt] = toupper(sa[cpt + index_start]);
      cpt ++;
    }	

  if(cpt == 0) done_flag = FALSE;
  else
    {     
      spi_string[cpt] = '\0';
      if((cpt >= 2) &&
	 (spi_string[0] == '0') &&
	 (spi_string[1] == 'X'))
	{
	  for(i = 0; i <= cpt - 2; i++) spi_string_tmp[i] = spi_string[i+2];
	  sscanf(spi_string_tmp,"%x",&spi);
	  g_snprintf(spi_string,spi_len_max,"%i",spi);	    
	}
      
      *index_end = cpt + index_start - 1;
      *pt_spi = (gchar *)g_malloc((strlen(spi_string) + 1) * sizeof(gchar));
      memcpy(*pt_spi, spi_string, strlen(spi_string) + 1);

      done_flag = TRUE;
    }

  return done_flag;
}  
