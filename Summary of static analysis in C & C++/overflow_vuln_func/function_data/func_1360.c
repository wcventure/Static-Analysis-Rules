static void listOfCard32(tvbuff_t *tvb, int *offsetp, proto_tree *t, int hf,
    int hf_item, int length)
{
      proto_item *ti = proto_tree_add_item(t, hf, tvb, *offsetp, length * 4, little_endian);
      proto_tree *tt = proto_item_add_subtree(ti, ett_x11_list_of_card32);
      while(length--) {
	    if (*offsetp + 4 > next_offset) {
		/
		return;
	    }
	    proto_tree_add_uint(tt, hf_item, tvb, *offsetp, 4, VALUE32(tvb, *offsetp));
	    *offsetp += 4;
      }
}
