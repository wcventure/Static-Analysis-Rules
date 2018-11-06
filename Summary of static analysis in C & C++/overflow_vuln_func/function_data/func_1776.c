static void
dissect_svc_unavail(tvbuff_t * tvb, proto_tree * tree, guint16 pos, guint16 length) {

  proto_item *svc_unavail_it;
  proto_tree *svc_unavail_tree;
  svc_unavail_it = proto_tree_add_item (tree,
                            hf_docsis_tlv_svc_unavail,
			    tvb, pos, length, FALSE);
  svc_unavail_tree = proto_item_add_subtree(svc_unavail_it, ett_docsis_tlv_svc_unavail );
  proto_tree_add_item (svc_unavail_tree,
                       hf_docsis_tlv_svc_unavail_classid, tvb,
	   		pos, 1, FALSE);
  proto_tree_add_item (svc_unavail_tree,
	           hf_docsis_tlv_svc_unavail_type, tvb,
		   pos+1, 1, FALSE);
  proto_tree_add_item (svc_unavail_tree,
	           hf_docsis_tlv_svc_unavail_code, tvb,
		   pos+2, 1, FALSE);

}
