static gint
spotlight_dissect_query_loop(tvbuff_t *tvb, proto_tree *tree, gint offset, guint64 cpx_query_type, gint count, gint toc_offset, guint encoding)
{
	gint j;
	gint subquery_count;
	gint toc_index;
	guint64 query_data64;
	gint query_length;
	guint64 query_type;
	guint64 complex_query_type;

	proto_item *item_query;
	proto_tree *sub_tree;

	/
	while ((offset < (toc_offset - 8)) && (count > 0)) {
		query_data64 = spotlight_ntoh64(tvb, offset, encoding);
		query_length = (query_data64 & 0xffff) * 8;
		if (query_length == 0) {
			/
			break;
		}
		query_type = (query_data64 & 0xffff0000) >> 16;
		
		switch (query_type) {
		case SQ_TYPE_COMPLEX:
			toc_index = (gint)((query_data64 >> 32) - 1);
			query_data64 = spotlight_ntoh64(tvb, toc_offset + toc_index * 8, encoding);
			complex_query_type = (query_data64 & 0xffff0000) >> 16;

			switch (complex_query_type) {
			case SQ_CPX_TYPE_ARRAY:
			case SQ_CPX_TYPE_DICT:
				subquery_count = (gint)(query_data64 >> 32);
				item_query = proto_tree_add_text(tree, tvb, offset, query_length,
								 "%s, toc index: %u, children: %u",
								 spotlight_get_cpx_qtype_string(complex_query_type),
								 toc_index + 1,
								 subquery_count);
				break;
			case SQ_CPX_TYPE_STRING:
				subquery_count = 1;
				query_data64 = spotlight_ntoh64(tvb, offset + 8, encoding);
				query_length = (query_data64 & 0xffff) * 8;
				item_query = proto_tree_add_text(tree, tvb, offset, query_length + 8,
								 "%s, toc index: %u, string: '%s'",
								 spotlight_get_cpx_qtype_string(complex_query_type),
								 toc_index + 1,
								 tvb_get_ephemeral_string(tvb, offset + 16, query_length - 8));
				break;
			default:
				subquery_count = 1;
				item_query = proto_tree_add_text(tree, tvb, offset, query_length,
								 "type: %s (%s), toc index: %u, children: %u",
								 spotlight_get_qtype_string(query_type),
								 spotlight_get_cpx_qtype_string(complex_query_type),
								 toc_index + 1,
								 subquery_count);
				break;
			}
			
			sub_tree = proto_item_add_subtree(item_query, ett_afp_spotlight_query_line);
			offset += 8;
			offset = spotlight_dissect_query_loop(tvb, sub_tree, offset, complex_query_type, subquery_count, toc_offset, encoding);
			break;
		case SQ_TYPE_NULL:
			subquery_count = (gint)(query_data64 >> 32);
			j = 0;
			while (j++ < subquery_count) {
				proto_tree_add_text(tree, tvb, offset, 8, "null");
			}
			count -= subquery_count;
			offset += query_length;
			break;
		case SQ_TYPE_BOOL:
			item_query = proto_tree_add_text(tree, tvb, offset, query_length, "bool: %s",
							 (query_data64 >> 32) ? "true" : "false");
			offset += query_length;
			break;
		case SQ_TYPE_INT64:
			item_query = proto_tree_add_text(tree, tvb, offset, 8, "int64");
			sub_tree = proto_item_add_subtree(item_query, ett_afp_spotlight_query_line);
			j = spotlight_int64(tvb, sub_tree, offset, encoding) - 1;
			count -= j;
			offset += query_length;
			break;
		case SQ_TYPE_FLOAT:
			item_query = proto_tree_add_text(tree, tvb, offset, 8, "float");
			sub_tree = proto_item_add_subtree(item_query, ett_afp_spotlight_query_line);
			j = spotlight_float(tvb, sub_tree, offset, encoding) - 1;
			count -= j;
			offset += query_length;
			break;
		case SQ_TYPE_DATA:
			switch (cpx_query_type) {
			case SQ_CPX_TYPE_STRING:
				proto_tree_add_text(tree, tvb, offset, query_length, "string: '%s'",
						    tvb_get_ephemeral_string(tvb, offset + 8, query_length - 8));
				break;
			case SQ_CPX_TYPE_FILEMETA:
				item_query = proto_tree_add_text(tree, tvb, offset, query_length, "filemeta");
				sub_tree = proto_item_add_subtree(item_query, ett_afp_spotlight_query_line);
				(void)dissect_spotlight(tvb, sub_tree, offset + 8);
				break;
			}
			offset += query_length;
			break;
		case SQ_TYPE_CNIDS:
			item_query = proto_tree_add_text(tree, tvb, offset, query_length, "CNID Array");
			sub_tree = proto_item_add_subtree(item_query, ett_afp_spotlight_query_line);
			spotlight_CNID_array(tvb, sub_tree, offset + 8, encoding);
			offset += query_length;
			break;
		default:
			item_query = proto_tree_add_text(tree, tvb, offset, query_length, "type: %s",
							 spotlight_get_qtype_string(query_type));
			offset += query_length;
			break;
		}
		count--;
	}
	
	return offset;
}
