static int
dissect_createmode3(tvbuff_t *tvb, int offset, proto_tree* tree, guint32* mode)
{
	guint32 mode_value;

	mode_value = tvb_get_ntohl(tvb, offset + 0);
	if (tree) 
		proto_tree_add_uint(tree, hf_nfs3_createmode, tvb, offset+0, 4, mode_value);
	offset += 4;

	*mode = mode_value;
	return offset;
}
