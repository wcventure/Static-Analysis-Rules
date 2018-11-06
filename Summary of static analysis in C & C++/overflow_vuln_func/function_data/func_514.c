static char *
IP_to_string(gint offset,tvbuff_t *tvb,gboolean little_endian,char buff[])
{
  IPAddress         ip;
  guint8  a = 0, b = 0, c = 0, d = 0; /

  ip = get_guint32(tvb, offset, little_endian);
     /
  a = (ip >> 24);
  b = (ip >> 16) & 0xff;
  c = (ip >>  8) & 0xff;
  d =  ip & 0xff;

  sprintf(buff,"%d.%d.%d.%d", a, b, c, d);
  return(buff);
}
