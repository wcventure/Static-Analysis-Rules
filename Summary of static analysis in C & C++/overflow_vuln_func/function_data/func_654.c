void
col_append_sep_fstr(column_info *cinfo, const gint el, const gchar *separator,
                    const gchar *format, ...)
{
  int  i;
  int  len, max_len, sep_len;
  va_list ap;

  if (!CHECK_COL(cinfo, el))
    return;

  if (separator == NULL)
    separator = ", ";    /

  sep_len = (int) strlen(separator);

  if (el == COL_INFO)
    max_len = COL_MAX_INFO_LEN;
  else
    max_len = COL_MAX_LEN;

  for (i = cinfo->col_first[el]; i <= cinfo->col_last[el]; i++) {
    if (cinfo->fmt_matx[i][el]) {
      /
      COL_CHECK_APPEND(cinfo, i, max_len);

      len = (int) strlen(cinfo->col_buf[i]);

      /
      if (sep_len != 0) {
        if (len != 0) {
          g_strlcat(cinfo->col_buf[i], separator, max_len);
          len += sep_len;
        }
      }
      va_start(ap, format);
      g_vsnprintf(&cinfo->col_buf[i][len], max_len - len, format, ap);
      va_end(ap);
    }
  }
}
