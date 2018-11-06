static void listOfColorItem(tvbuff_t *tvb, int *offsetp, proto_tree *t, int hf,
    int length)
{
      proto_item *ti = proto_tree_add_item(t, hf, tvb, *offsetp, length * 8, little_endian);
      proto_tree *tt = proto_item_add_subtree(ti, ett_x11_list_of_color_item);
      while(length--) {
	    proto_item *tti;
	    proto_tree *ttt;
	    unsigned do_red_green_blue;
	    guint16 red, green, blue;
	    char buffer[1024];
	    char *bp;
	    const char *sep;

	    if (*offsetp + 12 > next_offset) {
		/
		return;
	    }
	    red = VALUE16(tvb, *offsetp + 4);
	    green = VALUE16(tvb, *offsetp + 6);
	    blue = VALUE16(tvb, *offsetp + 8);
	    do_red_green_blue = VALUE8(tvb, *offsetp + 10);

	    bp = buffer + sprintf(buffer, "colorItem: ");
	    sep = "";
	    if (do_red_green_blue & 0x1) {
		bp += sprintf(bp, "red = %d", red);
		sep = ", ";
	    }
	    if (do_red_green_blue & 0x2) {
		bp += sprintf(bp, "%sgreen = %d", sep, green);
		sep = ", ";
	    }
	    if (do_red_green_blue & 0x4)
		bp += sprintf(bp, "%sblue = %d", sep, blue);

	    tti = proto_tree_add_none_format(tt, hf_x11_coloritem, tvb, *offsetp, 12, "%s", buffer);
	    ttt = proto_item_add_subtree(tti, ett_x11_color_item);
	    proto_tree_add_item(ttt, hf_x11_coloritem_pixel, tvb, *offsetp, 4, little_endian);
	    *offsetp += 4;
	    proto_tree_add_item(ttt, hf_x11_coloritem_red, tvb, *offsetp, 2, little_endian);
	    *offsetp += 2;
	    proto_tree_add_item(ttt, hf_x11_coloritem_green, tvb, *offsetp, 2, little_endian);
	    *offsetp += 2;
	    proto_tree_add_item(ttt, hf_x11_coloritem_blue, tvb, *offsetp, 2, little_endian);
	    *offsetp += 2;
	    colorFlags(tvb, offsetp, ttt);
	    proto_tree_add_item(ttt, hf_x11_coloritem_unused, tvb, *offsetp, 1, little_endian);
	    *offsetp += 1;
      }
}
