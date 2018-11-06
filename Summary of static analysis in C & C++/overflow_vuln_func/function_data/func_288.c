gint8 get_CDR_wchar(tvbuff_t *tvb, gchar **seq, int *offset, MessageHeader * header) {
  
  guint32 slength;
  gchar *raw_wstring;

  /

  *seq = NULL; /
  slength = 2; /

  if (header->GIOP_version.minor > 1) /
    slength = get_CDR_octet(tvb,offset);

  if (slength > 0) {
    /
    get_CDR_octet_seq(tvb, &raw_wstring, offset, slength);

    /
    *seq = make_printable_string(raw_wstring, slength);

    g_free(raw_wstring);
  }

  /
  if (header->GIOP_version.minor < 2)
    slength = -slength;

  return slength;		/

}
