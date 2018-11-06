static guint
dissect_rdm_pd_queued_message(tvbuff_t *tvb, guint offset, proto_tree *tree, guint8 cc, guint8 len _U_)
{
	switch(cc) {
	case RDM_CC_GET_COMMAND:
		proto_tree_add_item(tree, hf_rdm_pd_queued_message_status, tvb,
			offset, 1, ENC_BIG_ENDIAN);
		offset += 1;
		break;
	}

	return offset;
}
