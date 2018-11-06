static int
dissect_stable_how(tvbuff_t *tvb, int offset, proto_tree* tree, int hfindex)
{
	guint32 stable_how;

	stable_how = tvb_get_ntohl(tvb,offset+0);
	if (tree) 
		proto_tree_add_uint(tree, hfindex, tvb,	offset, 4, stable_how);
	offset += 4;

	return offset;
}
