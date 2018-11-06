static int header_len(tvbuff_t *tvb, int offset)
{
  int i = 0;
  guint8 sc;

  /

  /

  while (1) {

    if (tvb_length_remaining(tvb, offset + i) < 1)
      return i;   /

    if ((sc = tvb_get_guint8(tvb, offset + i)) == 0x0d 
	&& tvb_get_guint8(tvb, offset + i + 1) == 0x0a)
      return i;   /

    if (!global_beep_strict_term && (sc == 0x0d || sc == 0x0a))
      return i;   /
	  
    i++;

  }
}
