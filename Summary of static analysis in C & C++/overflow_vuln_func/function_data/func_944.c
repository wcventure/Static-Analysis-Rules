#ifdef HAVE_LIBGCRYPT
static void
esp_sa_remove_white(const gchar *sa, gchar **sa_bis)
{  
  int i = 0;
  int cpt = 0;
  gchar sa_tmp[strlen(sa)];

  if((sa == NULL) || (strcmp(sa, "") == 0))
    {
      *sa_bis = NULL;
      return;
    }
  else
    for(i = 0; (unsigned int) i < strlen(sa); i++)
      {

	if((sa[i] != ' ') && (sa[i] != '\t')) 
	  {	    
	    sa_tmp[cpt] = sa[i];
	    cpt ++;
	  }
      }
  sa_tmp[cpt] = '\0';

  *sa_bis = (gchar *)g_malloc((cpt +1) * sizeof(gchar));
  memcpy(*sa_bis,sa_tmp,cpt);
  (*sa_bis)[cpt] = '\0';
}  
