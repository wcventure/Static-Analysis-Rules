static void
channelised_fill_vc_id_string(char* out_string, int maxstrlen, sdh_g707_format_t* in_fmt)
{
  int i = 0;
  int cur_len = 0, print_index = 0;
  guint8 is_printed = 0;
  static char* g_vc_size_strings[] =  {
                  "unknown", /
                  "VC3", /
                  "VC4", /
                  "VC4-4c", /
                  "VC4-16c", /
                  "VC4-64c", /
                  "VC4-256c", /};

  print_index = g_snprintf(out_string, maxstrlen,"%s(",g_vc_size_strings[in_fmt->m_vc_size]);

  if (in_fmt->m_sdh_line_rate <= 0 )
  {
    /
    for (i = (DECHAN_MAX_AUG_INDEX -1); i >= 0; i--)
    {
      if ( (in_fmt->m_vc_index_array[i] > 0) || (is_printed) )
      {
        cur_len = g_snprintf(out_string+print_index, maxstrlen,"%s%d",((is_printed)?", ":""),     in_fmt->m_vc_index_array[i]);
        print_index += cur_len;
        is_printed = 1;
      }
    }

  }
  else
  {
    for (i = in_fmt->m_sdh_line_rate - 2; i >= 0; i--)
    {
      cur_len = g_snprintf(out_string+print_index, maxstrlen,"%s%d",((is_printed)?", ":""),     in_fmt->m_vc_index_array[i]);
      print_index += cur_len;
      is_printed = 1;
    }
  }
  if ( ! is_printed )
  {
    /
    for ( i =0; i < in_fmt->m_vc_size - 2; i++)
    {
      cur_len = g_snprintf(out_string+print_index, maxstrlen,"%s0",((is_printed)?", ":""));
      print_index += cur_len;
      is_printed = 1;
    }
  }
  g_snprintf(out_string+print_index, maxstrlen, ")");
  return;
}
