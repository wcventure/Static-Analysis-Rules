static void
dissect_snmpv3_kickstart(tvbuff_t * tvb, proto_tree *tree, guint16 start, guint16 len) {
  proto_item *snmpv3_it;
  proto_tree *snmpv3_tree;
  guint8 type, length;
  guint16 pos = start;

  snmpv3_it = proto_tree_add_item (tree,
                            hf_docsis_tlv_snmpv3_kick,
			    tvb, start, len, FALSE);
  snmpv3_tree = proto_item_add_subtree(snmpv3_it, ett_docsis_tlv_snmpv3_kick);

  while (pos < (start + len)) {
    type = tvb_get_guint8 (tvb, pos++);
    length = tvb_get_guint8 (tvb, pos++);
    switch (type) {
	case SNMPV3_SEC_NAME:
  	  proto_tree_add_item (snmpv3_tree,
	           hf_docsis_tlv_snmpv3_kick_name, tvb,
		   pos, length, FALSE);
	  break;
	case SNMPV3_MGR_PUB_NUM:
  	  proto_tree_add_item (snmpv3_tree,
	           hf_docsis_tlv_snmpv3_kick_publicnum, tvb,
		   pos, length, FALSE);
	  break;
    }  /
    pos += length;
  }   /
}
