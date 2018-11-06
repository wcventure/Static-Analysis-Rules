cksum_status_t
calc_checksum( tvbuff_t *tvb, int offset, guint len, guint checksum) {
  const gchar *buffer;
  guint   available_len;
  const guint8 *p;
  guint32 c0, c1;
  guint   seglen;
  guint   i;

  if ( 0 == checksum )
    return( NO_CKSUM );

  available_len = tvb_length_remaining( tvb, offset );
  if ( available_len < len )
    return( DATA_MISSING );

  buffer = tvb_get_ptr( tvb, offset, len );

  /
  p = buffer;
  c0 = 0;
  c1 = 0;
  while (len != 0) {
    seglen = len;
    if (seglen > 5803)
      seglen = 5803;
    for (i = 0; i < seglen; i++) {
      c0 = c0 + *(p++);
      c1 += c0;
    }

    c0 = c0 % 255;
    c1 = c1 % 255;

    len -= seglen;
  }
  if (c0 != 0 || c1 != 0)
    return( CKSUM_NOT_OK );	/
  else
    return( CKSUM_OK );
}
