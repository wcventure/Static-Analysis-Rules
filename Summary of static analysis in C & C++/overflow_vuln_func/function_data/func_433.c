gchar* oid_to_str_buf(const guint8 *oid, gint oid_len, gchar *buf, int buf_len) {
  gint i;
  guint8 byte;
  guint32 subid0, subid;
  gboolean is_first;
  gchar *bufp;

  bufp = buf; subid = 0; is_first = TRUE;
  for (i=0; i<oid_len; i++){
    byte = oid[i];
    if ((bufp - buf) > (buf_len - OID_STR_LIMIT)) {
      bufp += g_snprintf(bufp, buf_len-(bufp-buf), ".>>>");
      break;
    }
    subid <<= 7;
    subid |= byte & 0x7F;
    if (byte & 0x80) {
      continue;
    }
    if (is_first) {
      subid0 = 0;
      if (subid >= 40) { subid0++; subid-=40; }
      if (subid >= 40) { subid0++; subid-=40; }
      bufp += g_snprintf(bufp, buf_len-(bufp-buf), "%u.%u", subid0, subid);
      is_first = FALSE;
    } else {
      bufp += g_snprintf(bufp, buf_len-(bufp-buf), ".%u", subid);
    }
    subid = 0;
  }
  *bufp = '\0';

  return buf;
}
