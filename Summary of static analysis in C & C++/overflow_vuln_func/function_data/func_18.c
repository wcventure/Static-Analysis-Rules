static void decode_tlv(proto_tree *tree, tvbuff_t *tvb, guint offset, guint length)
{
	proto_item *property_item, *value_item;
	proto_tree *property_tree, *value_tree;
	guint8 property_type;
	guint16 property_length, property_value_index;
/

	/
	property_item = proto_tree_add_text(tree,
		tvb,
		offset,
		length,
		"NOE Message Body");
	property_tree = proto_item_add_subtree(property_item, ett_body);

	while(length > 0)
	{
		property_type = tvb_get_guint8(tvb, offset);
		proto_tree_add_int_format_value(property_tree, hf_noe_pcode, tvb, offset, 1,
			property_type, "%s (0x%02x)", val_to_str(property_type, val_str_props, "Unknown"), property_type);
		offset++;
		length--;

		if(property_type >= P_ARRAY)
		{
			proto_tree_add_item(property_item, hf_noe_aindx, tvb, offset, 1, ENC_NA);
			offset++;
			length--;
		}

		property_length = tvb_get_guint8(tvb, offset);
		if(property_length & 0x80)
		{
			property_length = tvb_get_ntohs(tvb, offset);
			property_length &= 0x7fff;
			proto_tree_add_uint(property_tree, hf_noe_psize, tvb, offset, 2,
				tvb_get_guint8(tvb, offset) * 256 + tvb_get_guint8(tvb, offset+1));
			offset += 2;
			length -= 2;
		}
		else
		{
			proto_tree_add_uint(property_tree, hf_noe_psize, tvb, offset, 1,
				tvb_get_guint8(tvb, offset));
			offset++;
			length--;
		}
		
		switch(property_length)
		{
		case 0:
			{
				break;
			}
		case 1:
			{
				proto_tree_add_text(property_item,
					tvb,
					offset,
					1,
					"Value: %d",
					tvb_get_guint8(tvb, offset));
				offset++;
				length--;
				break;
			}
		case 2:
			{
				guint16 property_value;
				property_value = tvb_get_ntohs(tvb, offset);
				proto_tree_add_text(property_item,
					tvb,
					offset,
					2,
					"Value: %d",
					property_value);
				offset += 2;
				length -= 2;
				break;
			}
		case 3:
			{
				guint32 property_value;
				property_value = tvb_get_ntoh24(tvb, offset);
				proto_tree_add_text(property_item,
					tvb,
					offset,
					3,
					"Value: %u",
					property_value);
				offset += 3;
				length -= 3;
				break;
			}
		case 4:
			{
				guint32 property_value;
				property_value = tvb_get_ntohl(tvb, offset);
				proto_tree_add_text(property_item,
					tvb,
					offset,
					4,
					"Value: %u",
					property_value);
				offset += 4;
				length -= 4;
				break;
			}
		default:
			{
				/
				value_item = proto_tree_add_text(property_item,
					tvb,
					offset,
					property_length,
					"Value (> 4 bytes)");

				value_tree = proto_item_add_subtree(value_item, ett_value);

				property_value_index = 0;
				while(property_length > 0)
				{
					property_value_index++;
					proto_tree_add_text(value_tree,
						tvb,
						offset,
						1,
						"Value %5d: 0x%02x",
						property_value_index, tvb_get_guint8(tvb, offset));
					offset++;
					length--;
					property_length--;
				}
				break;
			}
		}
	}
}
