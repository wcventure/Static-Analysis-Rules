static int
channelised_fill_sdh_g707_format(sdh_g707_format_t* in_fmt, guint16 bit_flds, guint8 vc_size, guint8 speed)
{
  int i = 0; /

  if (0 == vc_size)
  {
    /
    return -1;
  }
  in_fmt->m_vc_size = vc_size;
  in_fmt->m_sdh_line_rate = speed;
  memset(&(in_fmt->m_vc_index_array[0]), 0xff, DECHAN_MAX_AUG_INDEX);

  /
  in_fmt->m_vc_index_array[ speed - 1] = 0;
  /
    for (i = (speed - 2); i >= 0; i--)
  {
    guint8 aug_n_index = 0;

    /
    if ( i >= (vc_size - 1))
    {
      /
      aug_n_index = ((bit_flds >> (2 *i))& 0x3) +1;
    }
    else
    {
      aug_n_index = 0;
    }
    in_fmt->m_vc_index_array[i] = aug_n_index;
  }
  return 0;
}
