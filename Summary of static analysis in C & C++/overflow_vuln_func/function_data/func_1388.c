int
imf_find_field_end(tvbuff_t *tvb, int offset, gint max_length, gboolean *last_field)
{

  while(offset < max_length) {

    /
    offset = tvb_find_guint8(tvb, offset, max_length - offset, '\r');

    if(offset != -1) {
      if(tvb_get_guint8(tvb, ++offset) == '\n') {
        /
        /
        switch(tvb_get_guint8(tvb, ++offset)) {
        case '\r':
          /
          if(tvb_get_guint8(tvb, offset + 1) == '\n') {
            if(last_field) {
              *last_field = TRUE;
            }
          }
          return offset;
        case  ' ':
        case '\t':
          /
          break;
        default:
          /
          return offset;
        }
      }
    } else {
      /
      return offset;
    }

  }

  return -1;  /

}
