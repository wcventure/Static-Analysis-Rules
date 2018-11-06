static int
dissect_nfs4_status(tvbuff_t *tvb, int offset, proto_tree *tree, guint32 *status)
{
	guint32 stat;
	proto_item *stat_item = NULL;

	stat = tvb_get_ntohl(tvb, offset+0);

	proto_tree_add_uint(tree, hf_nfs4_status, tvb, offset+0, 4, stat);
	stat_item = proto_tree_add_uint(tree, hf_nfs_status, tvb, offset+0, 4, stat);
	PROTO_ITEM_SET_HIDDEN(stat_item);

	if (status) 
		*status = stat;

	return offset + 4;
}
