static char *
get_NtpTime(gint offset,tvbuff_t *tvb,gboolean little_endian,char buff[])
{
  NtpTime         ntpTime;
  float           time;

  /
  ntpTime.seconds  =  get_guint32(tvb, offset, little_endian);
  ntpTime.fraction =  get_guint32(tvb, (offset + 4), little_endian);
  time = (float) ntpTime.seconds + (ntpTime.fraction / 2^(32));

  sprintf(buff,"%f", time);
  return(buff);
}
