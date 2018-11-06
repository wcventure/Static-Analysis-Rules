static void
dissect_ospf_lls_data_block(tvbuff_t *tvb, int offset, proto_tree *tree, 
		guint8 version)
{
    proto_tree *ospf_lls_data_block_tree;
    proto_item *ti;
    guint16 ospf_lls_len;
    int orig_offset = offset;

    ospf_lls_len = tvb_get_ntohs(tvb, offset + 2);
    ti = proto_tree_add_text(tree, tvb, offset, -1, "OSPF LLS Data Block");
    ospf_lls_data_block_tree = proto_item_add_subtree(ti, 
		    ett_ospf_lls_data_block);

    if (version != OSPF_VERSION_2)
	    return;

    /
    proto_tree_add_text(ospf_lls_data_block_tree, tvb, offset, 2, 
		    "Checksum: 0x%04x", tvb_get_ntohs(tvb, offset));
    proto_tree_add_text(ospf_lls_data_block_tree, tvb, offset + 2, 2,
		    "LLS Data Length: %d bytes", ospf_lls_len * 4);
    
    offset += 4;
    while (orig_offset + ospf_lls_len * 4 > offset)
	offset = dissect_ospf_lls_tlv (tvb, offset, ospf_lls_data_block_tree);
}
