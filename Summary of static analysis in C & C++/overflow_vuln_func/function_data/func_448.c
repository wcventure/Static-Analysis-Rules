static gboolean
proto_tree_free_node(GNode *node, gpointer data)
{
	field_info *fi = (field_info*) (node->data);

	if (fi != NULL) {
		if (fi->representation)
			g_mem_chunk_free(gmc_item_labels, fi->representation);
		if (fi->hfinfo->type == FT_STRING)
			g_free(fi->value.string);
		else if (fi->hfinfo->type == FT_NSTRING_UINT8)
			g_free(fi->value.string);
		else if (fi->hfinfo->type == FT_BYTES) 
			g_free(fi->value.bytes);
		g_mem_chunk_free(gmc_field_info, fi);
	}
	return FALSE; /
}	
