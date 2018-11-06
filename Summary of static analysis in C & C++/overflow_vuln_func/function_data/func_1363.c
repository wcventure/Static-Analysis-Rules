static void listOfKeysyms(tvbuff_t *tvb, int *offsetp, proto_tree *t, int hf,
    int hf_item, int keycode_count, int keysyms_per_keycode)
{
      proto_item *ti = proto_tree_add_item(t, hf, tvb, *offsetp, keycode_count * keysyms_per_keycode * 4, little_endian);
      proto_tree *tt = proto_item_add_subtree(ti, ett_x11_list_of_keysyms);
      proto_item *tti;
      proto_tree *ttt;
      int i;
      char buffer[128];
      char *bp;

      while(keycode_count--) {
	    if (*offsetp + keysyms_per_keycode * 4 > next_offset) {
		/
		return;
	    }
	    bp = buffer + sprintf(buffer, "keysyms:");
	    for(i = 0; i < keysyms_per_keycode; i++) {
		  bp += sprintf(bp, " %s", keysymString(VALUE32(tvb, *offsetp + i * 4)));
	    }
	    *bp = '\0';
	    tti = proto_tree_add_none_format(tt, hf_item, tvb, *offsetp, keysyms_per_keycode * 4,
						 "%s", buffer);
	    ttt = proto_item_add_subtree(tti, ett_x11_keysym);
	    for(i = keysyms_per_keycode; i; i--) {
		  guint32 v = VALUE32(tvb, *offsetp);
		  proto_tree_add_uint_format(ttt, hf_x11_keysyms_item_keysym, tvb, *offsetp, 4, v,
					     "keysym: 0x%08x (%s)", v, keysymString(v));
		  *offsetp += 4;
	    }
      }
}
