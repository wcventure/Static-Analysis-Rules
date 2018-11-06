static char *
host_id_to_string(gint offset,tvbuff_t *tvb, char buff[])
{
  guint32       hostId = tvb_get_ntohl(tvb, offset);
  /

  sprintf(buff,"0x%X", hostId);
  return(buff);
}
