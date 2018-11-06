static gchar *dissect_thales_mts_id (tvbuff_t *tvb, gint offset, gint length)
{
  /
  if (length >= 7 && length <= 22) {
    return ep_strdup_printf ("%s/%08X%04X", 
                             dissect_7bit_string (tvb, offset, length - 6),
                             tvb_get_ntohl (tvb, offset + length - 6),
                             tvb_get_ntohs (tvb, offset + length - 2));
  }

  return ILLEGAL_FORMAT;
}
