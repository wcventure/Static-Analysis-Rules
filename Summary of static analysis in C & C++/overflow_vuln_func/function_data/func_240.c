static guint16 
get_nstring(tvbuff_t *tvb, gint offset, guint8* cbuf)
{
  guint16 len;
  guint8* ptr;
  gint i;
  /
  len = tvb_get_ntohs(tvb, offset);
  /
  if (len == 0xffff) {
    cbuf[0] = '\0';
    len = 0;
  } else {
    ptr = tvb_get_ptr(tvb, offset+2, len);
    for(i=0; i<len; i++) {
      /
      cbuf[i] = ptr[i];
    }
    cbuf[i] = '\0';
    len++;
  }
  return len;
}
