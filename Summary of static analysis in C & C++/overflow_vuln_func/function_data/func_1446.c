static void
proto_mpeg_descriptor_dissect_parental_rating(tvbuff_t *tvb, guint offset, proto_tree *tree)
{
	proto_tree_add_item(tree, hf_mpeg_descr_parental_rating_country_code, tvb, offset, 3, ENC_ASCII|ENC_NA);
	offset += 3;

	proto_tree_add_item(tree, hf_mpeg_descr_parental_rating_rating, tvb, offset, 1, ENC_NA);
}
