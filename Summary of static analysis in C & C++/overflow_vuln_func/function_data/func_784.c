static int
dissect_camel_DateAndTime(gboolean implicit_tag _U_, tvbuff_t *tvb, int offset, packet_info *pinfo _U_, proto_tree *tree, int hf_index _U_) {


/

/

  guint8 digit_pair;
  guint8 i = 0, curr_offset; 
  char *time = (char *)(calloc (2*7 + 5 + 1, sizeof(char))); 
  
  char c[ 2*7 + 5] = ""; /
  time[ 2*7 + 5 +1 ] = '\0';
  /
    
  for (curr_offset = 0; curr_offset < 7 ; curr_offset++)    
  /
  {
      digit_pair = tvb_get_guint8(tvb, curr_offset);
      
      proto_tree_add_uint(tree,
                          hf_digit,
                          tvb,
                          curr_offset,
                          1,
                          digit_pair & 0x0F);

      proto_tree_add_uint(tree,
                          hf_digit,
                          tvb,
                          curr_offset,
                          1,
                          digit_pair & 0xF0);
			  
      
      c[i] = camel_number_to_char( digit_pair & 0x0F);
      i++;
      c[i] = camel_number_to_char( digit_pair & 0xF0);
      i++;
  }
  
  /
  
  time[0] = c[9];
  time[1] = c[8];
  time[2] = ':';
  time[3] = c[10];
  time[4] = c[11];
  time[5] = ':';
  time[6] = c[12];
  time[7] = c[13];
  time[8] = ';';
  if ( EUROPEAN_DATE == date_format) /
  {
    time[9] = c[6]; /
    time[10] = c[7];
    time[11] = '/'; 
    time[12] = c[4]; /
    time[13] = c[5];
  }
  else /
  {
    time[9] = c[4]; /
    time[10] = c[5];
    time[11] = '/'; 
    time[12] = c[6]; /
    time[13] = c[7];
  }
  time[14] = '/';
  time[15] = c[0];
  time[16] = c[1];
  time[17] = c[2];
  time[18] = c[3];
 
/
 
  proto_tree_add_string(tree, 
		      hf_index, 
		      tvb,
		      0, 
		      7, 
		      time);
  free (time); 
  return 7; /


  return offset;
}
