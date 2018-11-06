static gboolean
find_tcap_subdisector(tvbuff_t *tvb, asn1_ctx_t *actx, proto_tree *tree){
	proto_item *item;

	/
	if(ansi_tcap_private.d.pdu == 1){
		/
		save_invoke_data(actx->pinfo, tree, tvb);
	}else{
		/
		if(find_saved_invokedata(actx->pinfo, tree, tvb)){
			if(ansi_tcap_private.d.OperationCode == 0){
				/
				item = proto_tree_add_int(tree, hf_ansi_tcap_national, tvb, 0, 0, ansi_tcap_private.d.OperationCode_national);
			}else{
				item = proto_tree_add_int(tree, hf_ansi_tcap_private, tvb, 0, 0, ansi_tcap_private.d.OperationCode_private);
			}
			PROTO_ITEM_SET_GENERATED(item);
			ansi_tcap_private.d.OperationCode_item = item;
		}
	}
	if(ansi_tcap_private.d.OperationCode == 0){
		/
		item = proto_tree_add_text(tree, tvb, 0, -1, 
			"Dissector for ANSI TCAP NATIONAL code:%u not implemented. Contact Wireshark developers if you want this supported",
			ansi_tcap_private.d.OperationCode_national);
		PROTO_ITEM_SET_GENERATED(item);
		return FALSE;
	}else if(ansi_tcap_private.d.OperationCode == 1){
		/
		if((ansi_tcap_private.d.OperationCode_private & 0x0900) != 0x0900){
			item = proto_tree_add_text(tree, tvb, 0, -1,
				"Dissector for ANSI TCAP PRIVATE code:%u not implemented. Contact Wireshark developers if you want this supported",
				ansi_tcap_private.d.OperationCode_private);
			PROTO_ITEM_SET_GENERATED(item);
			return FALSE;
		}
	}
	/
	call_dissector(ansi_map_handle, tvb, actx->pinfo, tcap_top_tree);

	return TRUE;
}
