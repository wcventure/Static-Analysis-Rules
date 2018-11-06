static void listOfAtom(tvbuff_t *tvb, int *offsetp, proto_tree *t, int hf,
    int length)
{
      proto_item *ti = proto_tree_add_item(t, hf, tvb, *offsetp, length * 4, little_endian);
      proto_tree *tt = proto_item_add_subtree(ti, ett_x11_list_of_atom);
      while(length--) {
	    if (*offsetp + 4 > next_offset) {
		/
		return;
	    }
	    atom(tvb, offsetp, tt, hf_x11_properties_item);
      }
}
