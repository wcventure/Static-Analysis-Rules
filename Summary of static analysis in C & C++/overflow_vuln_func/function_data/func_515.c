static char *
port_to_string(gint offset,tvbuff_t *tvb,gboolean little_endian,char buff[])
{
  Port port = get_guint32(tvb, offset, little_endian);
            /

  if (port == PORT_INVALID)
    sprintf(buff,"PORT_INVALID");
  else
    sprintf(buff,"0x%X",port);

  return(buff);
}
