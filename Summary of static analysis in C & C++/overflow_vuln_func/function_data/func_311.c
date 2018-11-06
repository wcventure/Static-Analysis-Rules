static int
dissect_nfs2_status(tvbuff_t *tvb, int offset, proto_tree *tree, guint32 *status)
{
	guint32 stat;
	proto_item *stat_item = NULL;

	stat = tvb_get_ntohl(tvb, offset+0);

	if (tree) {
		proto_tree_add_uint(tree, hf_nfs2_status, tvb, offset+0, 4, stat);
		stat_item = proto_tree_add_uint(tree, hf_nfs_status, tvb, offset+0, 4, stat);
		PROTO_ITEM_SET_HIDDEN(stat_item);
	}

	offset += 4;

	if (status) 
		*status = stat;

	return offset;
}
