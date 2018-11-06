void get_CDR_octet_seq(tvbuff_t *tvb, gchar **seq, int *offset, guint32 len) {

  guint32 remain_len = tvb_length_remaining(tvb, *offset);

  if (remain_len < len) {
    /
    tvb_ensure_length_remaining(tvb, *offset + remain_len + 1);
  }

  /
  *seq = g_new0(gchar, len + 1);
  tvb_memcpy( tvb, *seq, *offset, len);
  *offset += len;
}
