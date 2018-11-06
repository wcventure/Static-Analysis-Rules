static void listOfTextItem(tvbuff_t *tvb, int *offsetp, proto_tree *t, int hf,
    int sizeIs16)
{
      int allocated = 0;
      char *s = NULL;
      proto_item *ti;
      proto_tree *tt;
      guint32 fid;

      /
      
      int scanning_offset = *offsetp; /
      int l;                            /
      int n = 0;                        /

      while(scanning_offset < next_offset) {
	    l = tvb_get_guint8(tvb, scanning_offset);
	    scanning_offset++;
	    if (!l) break;
	    n++;
	    scanning_offset += l == 255 ? 4 : l + (sizeIs16 ? l : 0) + 1;
      }

      ti = proto_tree_add_item(t, hf, tvb, *offsetp, scanning_offset - *offsetp, little_endian);
      tt = proto_item_add_subtree(ti, ett_x11_list_of_text_item);

      /
      CLEANUP_PUSH(g_free, s);

      while(n--) {
	    unsigned l = VALUE8(tvb, *offsetp);
	    if (l == 255) { /
		  fid = tvb_get_ntohl(tvb, *offsetp + 1);
		  proto_tree_add_uint(tt, hf_x11_textitem_font, tvb, *offsetp, 5, fid);
		  *offsetp += 5;
	    } else { /
		  proto_item *tti;
		  proto_tree *ttt;
		  gint8 delta = VALUE8(tvb, *offsetp + 1);
		  if (sizeIs16) l += l;
		  if ((unsigned) allocated < l + 1) {
			/
			g_free(s);
			s = g_malloc(l + 1);
			allocated = l + 1;
		  }
		  stringCopy(s, tvb_get_ptr(tvb, *offsetp + 2, l), l);
		  tti = proto_tree_add_none_format(tt, hf_x11_textitem_string, tvb, *offsetp, l + 2,
						       "textitem (string): delta = %d, \"%s\"",
						       delta, s);
		  ttt = proto_item_add_subtree(tti, ett_x11_text_item);
		  proto_tree_add_item(ttt, hf_x11_textitem_string_delta, tvb, *offsetp + 1, 1, little_endian);
		  if (sizeIs16)
			string16_with_buffer_preallocated(tvb, ttt, hf_x11_textitem_string_string16, 
							  hf_x11_textitem_string_string16_bytes,
							  *offsetp + 2, l,
							   &s, &allocated);
		  else
			proto_tree_add_string_format(ttt, hf_x11_textitem_string_string8, tvb, 
						     *offsetp + 2, l, s, "\"%s\"", s);
		  *offsetp += l + 2;
	    }
      }

      /
      CLEANUP_CALL_AND_POP;
}
