static void listOfPoint(tvbuff_t *tvb, int *offsetp, proto_tree *t, int hf, int length)
{
      proto_item *ti = proto_tree_add_item(t, hf, tvb, *offsetp, length * 4, little_endian);
      proto_tree *tt = proto_item_add_subtree(ti, ett_x11_list_of_point);
      while(length--) {
	    gint16 x, y;
	    proto_item *tti;
	    proto_tree *ttt;

	    if (*offsetp + 4 > next_offset) {
		/
		return;
	    }
	    x = VALUE16(tvb, *offsetp);
	    y = VALUE16(tvb, *offsetp + 2);

	    tti = proto_tree_add_none_format(tt, hf_x11_point, tvb, *offsetp, 4, "point: (%d,%d)", x, y);
	    ttt = proto_item_add_subtree(tti, ett_x11_point);
	    proto_tree_add_int(ttt, hf_x11_point_x, tvb, *offsetp, 2, x);
	    *offsetp += 2;
	    proto_tree_add_int(ttt, hf_x11_point_y, tvb, *offsetp, 2, y);
	    *offsetp += 2;
      }
}
