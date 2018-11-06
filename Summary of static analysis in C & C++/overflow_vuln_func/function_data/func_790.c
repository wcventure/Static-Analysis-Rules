static void
fTimeTag (tvbuff_t *tvb, proto_tree *tree, guint *offset, guint8 *label, guint32 lvt)
{
	guint32 year, month, day, weekday;

	(*offset)++;
	year = tvb_get_guint8(tvb, (*offset));
	month = tvb_get_guint8(tvb, (*offset)+1);
	day = tvb_get_guint8(tvb, (*offset)+2);
	weekday = tvb_get_guint8(tvb, (*offset)+3);
	if ((year == 255) && (day == 255) && (month == 255) && (weekday == 255))
		proto_tree_add_text(tree, tvb, *offset, lvt, "%sany", LABEL(label));
	else
		proto_tree_add_text(tree, tvb, *offset, lvt, "%s%d:%02d:%02d.%d %s = %02d:%02d:%02d.%d", LABEL(label), year > 12 ? year -12 : year, month, day, weekday, year > 12 ? "P.M." : "A.M.", year, month, day, weekday);
	(*offset)+=lvt;
}
