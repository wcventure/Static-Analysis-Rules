static int
dissect_ospf_lls_tlv(tvbuff_t *tvb, int offset, proto_tree *tree)
{
    proto_item *ti;
    proto_tree *ospf_lls_tlv_tree;
    guint16 type;
    guint16 length;

    type = tvb_get_ntohs(tvb, offset);
    length = tvb_get_ntohs(tvb, offset + 2);

    ti = proto_tree_add_text(tree, tvb, offset, length + 4, 
		    val_to_str(type, lls_tlv_type_vals, "Unknown TLV"));
    ospf_lls_tlv_tree = proto_item_add_subtree(ti, ett_ospf_lls_tlv);
    
    proto_tree_add_text(ospf_lls_tlv_tree, tvb, offset, 2,
		    "Type: %d", type);
    proto_tree_add_text(ospf_lls_tlv_tree, tvb, offset + 2, 2,
		    "Length: %d", length);
      
    switch(type) {
	case 1:
	    dissect_ospf_bitfield(ospf_lls_tlv_tree, tvb, offset + 4, &bfinfo_lls_ext_options);
	    break;
	case 2:
	    proto_tree_add_text(ospf_lls_tlv_tree, tvb, offset + 4, 4, 
			    "Sequence number 0x%08x", 
			    tvb_get_ntohl(tvb, offset + 4));
	    proto_tree_add_text(ospf_lls_tlv_tree, tvb, offset + 8, length - 4,
			    "Auth Data: %s", 
			    tvb_bytes_to_str(tvb, offset + 8, length - 4));
	    break;
    }

    return offset + length + 4;
}
