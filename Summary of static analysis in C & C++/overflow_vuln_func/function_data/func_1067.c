static guint
dissect_rdm_pd_nack_reason(tvbuff_t *tvb, guint offset, proto_tree *tree, guint8 cc, guint8 len _U_)
{
	switch(cc) {
	case RDM_CC_GET_COMMAND_RESPONSE:
	case RDM_CC_SET_COMMAND_RESPONSE:
		proto_tree_add_item(tree, hf_rdm_pd_nack_reason, tvb, offset, 2, ENC_BIG_ENDIAN);
		offset +=  2;
		break;
	}

	return offset;
}
