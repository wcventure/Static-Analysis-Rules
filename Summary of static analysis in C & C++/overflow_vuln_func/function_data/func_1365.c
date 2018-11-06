static void listOfRectangle(tvbuff_t *tvb, int *offsetp, proto_tree *t, int hf,
    int length)
{
      proto_item *ti = proto_tree_add_item(t, hf, tvb, *offsetp, length * 8, little_endian);
      proto_tree *tt = proto_item_add_subtree(ti, ett_x11_list_of_rectangle);
      while(length--) {
	    gint16 x, y;
	    unsigned width, height;
	    proto_item *tti;
	    proto_tree *ttt;

	    if (*offsetp + 8 > next_offset) {
		/
		return;
	    }
	    x = VALUE16(tvb, *offsetp);
	    y = VALUE16(tvb, *offsetp + 2);
	    width = VALUE16(tvb, *offsetp + 4);
	    height = VALUE16(tvb, *offsetp + 6);

	    tti = proto_tree_add_none_format(tt, hf_x11_rectangle, tvb, *offsetp, 8, 
						 "rectangle: %dx%d+%d+%d", width, height, x, y);
	    ttt = proto_item_add_subtree(tti, ett_x11_rectangle);
	    proto_tree_add_int(ttt, hf_x11_rectangle_x, tvb, *offsetp, 2, x);
	    *offsetp += 2;
	    proto_tree_add_int(ttt, hf_x11_rectangle_y, tvb, *offsetp, 2, y);
	    *offsetp += 2;
	    proto_tree_add_uint(ttt, hf_x11_rectangle_width, tvb, *offsetp, 2, width);
	    *offsetp += 2;
	    proto_tree_add_uint(ttt, hf_x11_rectangle_height, tvb, *offsetp, 2, height);
	    *offsetp += 2;
      }
}
