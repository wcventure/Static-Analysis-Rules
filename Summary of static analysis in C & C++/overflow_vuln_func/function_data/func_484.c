static tvbuff_t*
remove_escape_chars(tvbuff_t *tvb, int offset, int length)
{
  guint8	*buff;
  int		i;
  int		scanned_len = 0;
  guint8	octet;
  tvbuff_t  *next_tvb;
	
  buff = g_malloc(length);
  i = 0;
  while ( scanned_len < length ){
	  octet = tvb_get_guint8(tvb,offset);
	  if (octet == 0x7d){
		  offset++;
		  scanned_len++;
		  if (scanned_len >= length)
			  break;
		  octet = tvb_get_guint8(tvb,offset);
		  buff[i] = octet ^ 0x20;
	  }else{
		  buff[i]= octet;
	  }
	  offset++;
	  scanned_len++;
	  i++;
  }
  if (i == 0) {
	  g_free(buff);
	  return NULL;
  }
  next_tvb = tvb_new_real_data(buff,i,i);

  /
  tvb_set_free_cb( next_tvb, g_free );

  tvb_set_child_real_data_tvbuff(tvb,next_tvb);
  return next_tvb;

}
