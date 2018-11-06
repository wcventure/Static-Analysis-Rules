static void
dissect_bpkmattr (tvbuff_t * tvb, packet_info * pinfo, proto_tree * tree)
{

  proto_item *it;
  proto_tree *bpkmattr_tree;

  if (tree)
    {
      it =
	proto_tree_add_protocol_format (tree, proto_docsis_bpkmattr, tvb, 0,
					tvb_length_remaining (tvb, 0),
					"BPKM Attributes");
      bpkmattr_tree = proto_item_add_subtree (it, ett_docsis_bpkmattr);
      dissect_attrs (tvb, pinfo, bpkmattr_tree);
    }

}
