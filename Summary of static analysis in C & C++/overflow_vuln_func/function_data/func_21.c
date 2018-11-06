static void decode_mtd(proto_tree *tree,
					   tvbuff_t *tvb,
					   packet_info *pinfo,
					   guint8 method,
					   guint offset,
					   guint length)
{
	guint8 class = tvb_get_guint8(tvb, offset);

	proto_tree_add_int_format_value(tree,
		hf_noe_class,
		tvb,
		offset,
		1,
		class,
		"%s (%d)",
		val_to_str(class, val_str_class, "Unknown"),
		class);

	/
	if (check_col(pinfo->cinfo, COL_INFO))
		col_append_fstr(pinfo->cinfo, COL_INFO, " %s",
		val_to_str(class, val_str_class, "Unknown"));
	/
	proto_item_append_text(tree, ", %s",
		val_to_str(class, val_str_class, "Unknown"));

	offset++;
	length--;

	if(class >= C_DYNAMIC)
	{
		proto_tree_add_uint(tree, hf_noe_objectid, tvb, offset, 2, tvb_get_ntohs(tvb, offset));
		offset += 2;
		length -= 2;
	}

	switch(method)
	{
	case METHOD_INSERT_ITEM:
		{
			proto_tree_add_uint(tree,
				hf_noe_method_index,
				tvb,
				offset,
				1,
				tvb_get_guint8(tvb, offset));
			offset += 1;
			length -= 1;
			if(length > 0)
				decode_tlv(tree, tvb, offset, length);
			break;
		}
	case METHOD_DELETE_ITEM:
		{
			proto_tree_add_uint(tree,
				hf_noe_method_index,
				tvb,
				offset,
				1,
				tvb_get_guint8(tvb, offset));
			break;
		}
	case METHOD_GET_PROPERTY:
		{
			decode_getproperty_tlv(tree, tvb, offset, length);
			break;
		}
	default:
		{
			if(length > 0)
				decode_tlv(tree, tvb, offset, length);
			break;
		}
	}
}
