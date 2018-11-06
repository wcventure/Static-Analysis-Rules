static const char *
wpa_cipher_idx2str(guint idx)
{
  if (idx < sizeof(wpa_cipher_str)/sizeof(wpa_cipher_str[0]))
    return wpa_cipher_str[idx];
  return "UNKNOWN";
}
