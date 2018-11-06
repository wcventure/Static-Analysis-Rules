static int
dissect_handleList(tvbuff_t *tvb, int offset, packet_info *pinfo _U_,
                   proto_tree *tree)
{

	int numHandles;
	int listLength;
	int i;
	proto_item *handleListItem;
	proto_tree *handleListTree;

	numHandles = tvb_get_ntohl(tvb, offset);
	listLength = 4;

	for (i = 0; i < numHandles; i++) {
		listLength += (4 + tvb_get_ntohl(tvb, offset + listLength));
	}

	handleListItem =  proto_tree_add_text(tree, tvb, offset, listLength,
	                                      "Handle List");
	handleListTree = proto_item_add_subtree(handleListItem,
	                                        ett_fmp_notify_hlist);

	offset = dissect_rpc_uint32(tvb,  handleListTree,
	                            hf_fmp_handleListLen, offset);

	for (i = 0; i <= numHandles; i++) {
		offset = dissect_rpc_data(tvb, handleListTree,
		                          hf_fmp_fmpFHandle, offset);/
	}

	return offset;
}
