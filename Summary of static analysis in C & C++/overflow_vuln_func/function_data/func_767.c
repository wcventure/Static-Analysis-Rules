static const char *
wpa_keymgmt_idx2str(guint idx)
{
  if (idx < sizeof(wpa_keymgmt_str)/sizeof(wpa_keymgmt_str[0]))
    return wpa_keymgmt_str[idx];
  return "UNKNOWN";
}
