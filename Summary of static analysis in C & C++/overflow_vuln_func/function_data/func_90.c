static void rV(proto_tree *tree, int *V, tvbuff_t *tvb, gint offset, gint8 L)
{
	switch(L)
	{
	case 1:
	proto_tree_add_uint(tree,
				*V,
				tvb,
				offset,
				L+2,
				tvb_get_guint8(tvb, offset+2));
	break;
	case 2:
	proto_tree_add_uint(tree,
				*V,
				tvb,
				offset,
				L+2,
				tvb_get_ntohs(tvb, offset+2));
	break;
	case 3:
	proto_tree_add_uint(tree,
				*V,
				tvb,
				offset,
				L+2,
				tvb_get_ntoh24(tvb, offset+2));
	break;
	case 4:
	proto_tree_add_uint(tree,
				*V,
				tvb,
				offset,
				L+2,
				tvb_get_ntohl(tvb, offset+2));
	break;
	}
}
