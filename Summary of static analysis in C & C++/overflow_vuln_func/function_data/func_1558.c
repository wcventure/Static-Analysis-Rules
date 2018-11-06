static void
channelised_fill_vc_id_string(emem_strbuf_t* out_string, sdh_g707_format_t* in_fmt)
{
  int      i;
  gboolean is_printed  = FALSE;

  static char* g_vc_size_strings[] = {
    "unknown",  /
    "VC3",      /
    "VC4",      /
    "VC4-4c",   /
    "VC4-16c",  /
    "VC4-64c",  /
    "VC4-256c", /};

  ep_strbuf_printf(out_string, "%s(",
                           (in_fmt->m_vc_size < array_length(g_vc_size_strings)) ?
                           g_vc_size_strings[in_fmt->m_vc_size] : g_vc_size_strings[0] );

  if (in_fmt->m_sdh_line_rate <= 0 )
  {
    /
    for (i = (DECHAN_MAX_AUG_INDEX -1); i >= 0; i--)
    {
      if ((in_fmt->m_vc_index_array[i] > 0) || (is_printed) )
      {
        ep_strbuf_append_printf(out_string, "%s%d",
                                  ((is_printed)?", ":""),
                                  in_fmt->m_vc_index_array[i]);
        is_printed = TRUE;
      }
    }

  }
  else
  {
    for (i = in_fmt->m_sdh_line_rate - 2; i >= 0; i--)
    {
      ep_strbuf_append_printf(out_string, "%s%d",
                                ((is_printed)?", ":""),
                                in_fmt->m_vc_index_array[i]);
      is_printed = TRUE;
    }
  }
  if ( ! is_printed )
  {
    /
    for ( i =0; i < in_fmt->m_vc_size - 2; i++)
    {
      ep_strbuf_append_printf(out_string, "%s0",
                                ((is_printed)?", ":""));
      is_printed = TRUE;
    }
  }
  ep_strbuf_append_c(out_string, ')');
  return;
}
