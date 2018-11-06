static int
mysql_field_add_lestring(tvbuff_t *tvb, int offset, proto_tree *tree, int field)
{
	guint64 lelen;
	guint8 is_null;

	offset += tvb_get_fle(tvb, offset, &lelen, &is_null);
	if(is_null)
		proto_tree_add_string(tree, field, tvb, offset, 4, "NULL");
	else
	{
		proto_tree_add_item(tree, field, tvb, offset, (int)lelen, ENC_NA);
		offset += (int)lelen;
	}
	return offset;
}
