static void listOfKeycode(tvbuff_t *tvb, int *offsetp, proto_tree *t, int hf,
    int length)
{
      char buffer[1024];
      proto_item *ti = proto_tree_add_item(t, hf, tvb, *offsetp, length * 8, little_endian);
      proto_tree *tt = proto_item_add_subtree(ti, ett_x11_list_of_keycode);

      while(length--) {
	    char *bp = buffer;
	    const char **m;
	    int i;

	    if (*offsetp + 8 > next_offset) {
		/
		return;
	    }
	    for(i = 8, m = modifiers; i; i--, m++) {
		u_char c = tvb_get_guint8(tvb, *offsetp);
		*offsetp += 1;
		if (c)
		    bp += sprintf(bp, "  %s=%d", *m, c);
	    }

	    proto_tree_add_bytes_format(tt, hf_x11_keycodes_item, tvb, *offsetp - 8, 8, tvb_get_ptr(tvb, *offsetp - 8, 8),	"item: %s", buffer);
      }
}
