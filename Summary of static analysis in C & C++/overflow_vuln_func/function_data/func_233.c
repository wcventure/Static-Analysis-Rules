static int
dissect_execute_cdb_payload(tvbuff_t *tvb, int offset, packet_info *pinfo, proto_tree *parent_tree,
    const char *name, int hf_len, gboolean isreq)
{
	proto_item* item = NULL;
	proto_tree* tree = NULL;
	guint32 payload_len;
	guint32 payload_len_full;

	payload_len = tvb_get_ntohl(tvb, offset);
	payload_len_full = rpc_roundup(payload_len);

	if (parent_tree) {
		item = proto_tree_add_text(parent_tree, tvb, offset,
				4+payload_len_full, "%s", name);
		tree = proto_item_add_subtree(item,
		    ett_ndmp_execute_cdb_payload);
	}

	proto_tree_add_uint(tree, hf_len, tvb, offset, 4, payload_len);
	offset += 4;

	if ((int) payload_len > 0) {
		tvbuff_t *data_tvb;
		int tvb_len, tvb_rlen;

		tvb_len=tvb_length_remaining(tvb, offset);
		if(tvb_len>(int)payload_len)
	    		tvb_len=payload_len;
		tvb_rlen=tvb_reported_length_remaining(tvb, offset);
		if(tvb_rlen>(int)payload_len)
	    		tvb_rlen=payload_len;
		data_tvb=tvb_new_subset(tvb, offset, tvb_len, tvb_rlen);

		if(ndmp_conv_data->task->itlq){
			dissect_scsi_payload(data_tvb, pinfo, top_tree, isreq,
		 		   ndmp_conv_data->task->itlq, get_itl_nexus(ndmp_conv_data, pinfo, FALSE));
		}
		offset += payload_len_full;
	}

	return offset;
}
