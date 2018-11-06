gchar* oid_to_str(const guint8 *oid, gint oid_len) {
  gchar *buf;

  buf=ep_alloc(MAX_OID_STR_LEN);
  return oid_to_str_buf(oid, oid_len, buf, MAX_OID_STR_LEN);
}
