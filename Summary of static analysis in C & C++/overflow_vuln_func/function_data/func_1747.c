gchar*
address_to_str(const address *addr)
{
  gchar *str;

  str=ep_alloc(256);
  address_to_str_buf(addr, str, 256);
  return str;
}
