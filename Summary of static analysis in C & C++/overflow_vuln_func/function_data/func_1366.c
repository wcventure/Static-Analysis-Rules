static void listOfSegment(tvbuff_t *tvb, int *offsetp, proto_tree *t, int hf,
    int length)
{
      proto_item *ti = proto_tree_add_item(t, hf, tvb, *offsetp, length * 8, little_endian);
      proto_tree *tt = proto_item_add_subtree(ti, ett_x11_list_of_segment);
      while(length--) {
	    gint16 x1, y1, x2, y2;
	    proto_item *tti;
	    proto_tree *ttt;

	    if (*offsetp + 8 > next_offset) {
		/
		return;
	    }
	    x1 = VALUE16(tvb, *offsetp);
	    y1 = VALUE16(tvb, *offsetp + 2);
	    x2 = VALUE16(tvb, *offsetp + 4);
	    y2 = VALUE16(tvb, *offsetp + 6);

	    tti = proto_tree_add_none_format(tt, hf_x11_segment, tvb, *offsetp, 8, 
						 "segment: (%d,%d)-(%d,%d)", x1, y1, x2, y2);
	    ttt = proto_item_add_subtree(tti, ett_x11_segment);
	    proto_tree_add_item(ttt, hf_x11_segment_x1, tvb, *offsetp, 2, little_endian);
	    *offsetp += 2;
	    proto_tree_add_item(ttt, hf_x11_segment_y1, tvb, *offsetp, 2, little_endian);
	    *offsetp += 2;
	    proto_tree_add_item(ttt, hf_x11_segment_x2, tvb, *offsetp, 2, little_endian);
	    *offsetp += 2;
	    proto_tree_add_item(ttt, hf_x11_segment_y2, tvb, *offsetp, 2, little_endian);
	    *offsetp += 2;
      }
}
