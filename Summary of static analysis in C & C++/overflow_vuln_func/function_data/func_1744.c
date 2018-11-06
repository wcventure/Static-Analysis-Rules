gchar *
ip_to_str(const guint8 *ad) {
  gchar *buf;

  buf=ep_alloc(16);
  ip_to_str_buf(ad, buf);
  return buf;
}
