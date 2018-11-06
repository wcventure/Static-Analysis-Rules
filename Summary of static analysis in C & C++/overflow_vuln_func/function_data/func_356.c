int
read_prefs_file(const char *pf_path, FILE *pf, pref_set_pair_cb pref_set_pair_fct)
{
  enum { START, IN_VAR, PRE_VAL, IN_VAL, IN_SKIP };
  int       got_c, state = START;
  static guint  cur_pref_val_size;
  static gchar* cur_val = NULL;
  static guint  cur_pref_var_size;
  static gchar* cur_var = NULL;
  gboolean  got_val = FALSE;
  gint      var_len = 0, val_len = 0, fline = 1, pline = 1;
  gchar     hint[] = "(saving your preferences once should remove this warning)";

  if (! cur_val) {
	  cur_pref_val_size = 128;
	  cur_val = g_malloc(cur_pref_val_size);
  }
  
  if (! cur_var) {
	  cur_pref_var_size = 128;
	  cur_var = g_malloc(cur_pref_var_size);
  }
  
  while ((got_c = getc(pf)) != EOF) {
    if (got_c == '\n') {
      state = START;
      fline++;
      continue;
    }
    if (var_len >= cur_pref_var_size) {
		cur_pref_var_size *= 2;
		cur_var = g_realloc(cur_var,cur_pref_val_size);
      continue;
    }
    if (val_len >= cur_pref_val_size) {
		cur_pref_val_size *= 2;
		cur_val = g_realloc(cur_val,cur_pref_val_size);
      continue;
    }

    switch (state) {
      case START:
        if (isalnum(got_c)) {
          if (var_len > 0) {
            if (got_val) {
              cur_var[var_len] = '\0';
              cur_val[val_len] = '\0';
              switch (pref_set_pair_fct(cur_var, cur_val)) {

	      case PREFS_SET_SYNTAX_ERR:
                g_warning ("%s line %d: Syntax error %s", pf_path, pline, hint);
                break;

	      case PREFS_SET_NO_SUCH_PREF:
                g_warning ("%s line %d: No such preference \"%s\" %s", pf_path,
				pline, cur_var, hint);
                break;

	      case PREFS_SET_OBSOLETE:
	        /
                break;
              }
            } else {
              g_warning ("%s line %d: Incomplete preference %s", pf_path, pline, hint);
            }
          }
          state      = IN_VAR;
          got_val    = FALSE;
          cur_var[0] = got_c;
          var_len    = 1;
          pline = fline;
        } else if (isspace(got_c) && var_len > 0 && got_val) {
          state = PRE_VAL;
        } else if (got_c == '#') {
          state = IN_SKIP;
        } else {
          g_warning ("%s line %d: Malformed line %s", pf_path, fline, hint);
        }
        break;
      case IN_VAR:
        if (got_c != ':') {
          cur_var[var_len] = got_c;
          var_len++;
        } else {
          state   = PRE_VAL;
          val_len = 0;
          got_val = TRUE;
        }
        break;
      case PRE_VAL:
        if (!isspace(got_c)) {
          state = IN_VAL;
          cur_val[val_len] = got_c;
          val_len++;
        }
        break;
      case IN_VAL:
        if (got_c != '#')  {
          cur_val[val_len] = got_c;
          val_len++;
        } else {
          while (isspace((guchar)cur_val[val_len]) && val_len > 0)
            val_len--;
          state = IN_SKIP;
        }
        break;
    }
  }
  if (var_len > 0) {
    if (got_val) {
      cur_var[var_len] = '\0';
      cur_val[val_len] = '\0';
      switch (pref_set_pair_fct(cur_var, cur_val)) {

      case PREFS_SET_SYNTAX_ERR:
        g_warning ("%s line %d: Syntax error %s", pf_path, pline, hint);
        break;

      case PREFS_SET_NO_SUCH_PREF:
        g_warning ("%s line %d: No such preference \"%s\" %s", pf_path,
			pline, cur_var, hint);
        break;

      case PREFS_SET_OBSOLETE:
	/
        break;
      }
    } else {
      g_warning ("%s line %d: Incomplete preference %s", pf_path, pline, hint);
    }
  }
  if (ferror(pf))
    return errno;
  else
    return 0;
}
