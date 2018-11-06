static void
dissect_reqxmit_policy (tvbuff_t * tvb, proto_tree * tree, guint16 start)
{
  guint32 value;
  proto_item *it;
  proto_tree *pol_tree;

  value = tvb_get_ntohl (tvb, start);
  it =
    proto_tree_add_item (tree, hf_docsis_tlv_sflow_reqxmit_pol, tvb, start, 4,
			 FALSE);
  pol_tree = proto_item_add_subtree (it, ett_docsis_tlv_reqxmitpol);

  if (value & 0x01)
    proto_tree_add_text (pol_tree, tvb, start, 4,
			 "Service flow MUST NOT use \"all CMs\" broadcast request opportunities");
  if (value & 0x02)
    proto_tree_add_text (pol_tree, tvb, start, 4,
			 "Service flow MUST NOT use priority multicast request opportunities");
  if (value & 0x04)
    proto_tree_add_text (pol_tree, tvb, start, 4,
			 "Service flow MUST NOT use Request/Data opportunities for requests");
  if (value & 0x08)
    proto_tree_add_text (pol_tree, tvb, start, 4,
			 "Service flow MUST NOT use Request/Data opportunities for data");
  if (value & 0x10)
    proto_tree_add_text (pol_tree, tvb, start, 4,
			 "Service flow MUST NOT use piggy back requests with data");
  if (value & 0x20)
    proto_tree_add_text (pol_tree, tvb, start, 4,
			 "Service flow MUST NOT concatenate data");
  if (value & 0x40)
    proto_tree_add_text (pol_tree, tvb, start, 4,
			 "Service flow MUST NOT fragment data");
  if (value & 0x80)
    proto_tree_add_text (pol_tree, tvb, start, 4,
			 "Service flow MUST NOT suppress payload headers");
  if (value & 0x100)
    proto_tree_add_text (pol_tree, tvb, start, 4,
			 "Service flow MUST drop packets that do not fit in the UGS size");
}
