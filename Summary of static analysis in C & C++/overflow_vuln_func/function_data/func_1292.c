static tvbuff_t *
dissect_6lowpan_bc0(tvbuff_t *tvb, packet_info *pinfo _U_, proto_tree *tree)
{
    guint8              seqnum;
    proto_tree *        bcast_tree;
    proto_item *        ti;

    /
    if (tree) {
        ti = proto_tree_add_text(tree, tvb, 0, sizeof(guint16), "Broadcast Header");
        bcast_tree = proto_item_add_subtree(ti, ett_6lowpan_bcast);

	/
        proto_tree_add_bits_item(bcast_tree, hf_6lowpan_pattern, tvb, 0, LOWPAN_PATTERN_BC0_BITS, FALSE);

	/
	seqnum = tvb_get_guint8(tvb, sizeof(guint8));
        proto_tree_add_uint(bcast_tree, hf_6lowpan_bcast_seqnum, tvb, sizeof(guint8), sizeof(guint8), seqnum);
    }

    /
    return tvb_new_subset(tvb, sizeof(guint16), -1, -1);
} /
