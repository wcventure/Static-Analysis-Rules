gint
dissect_epl_sdo_command_write_multiple_by_index(proto_tree *epl_tree, tvbuff_t *tvb, packet_info *pinfo, gint offset, guint8 segmented  _U_, gboolean response)
{
	gint pyldoffset, dataoffset;
	guint8 subindx, padding;
	guint16 indx;
	guint32 size, offsetincrement, datalength;
	proto_item* item;
	proto_item *sdo_data_tree;
	gboolean lastentry = FALSE;

	/
	pyldoffset = 8;

	if (!response)
	{   /

		col_append_str(pinfo->cinfo, COL_INFO, "Write Multiple Parameter by Index" );

		while ( !lastentry && tvb_reported_length_remaining(tvb, offset) > 0 )
		{
			offsetincrement = tvb_get_letohl(tvb, offset);

			/
			padding = tvb_get_guint8 ( tvb, offset + 7 ) & 0x03;

			if ((guint32)(offset-pyldoffset) >= offsetincrement)
				break;
			datalength = offsetincrement - ( offset - pyldoffset );

			/
			size = datalength - 8 - padding;

			if ( offsetincrement == 0 )
			{
				datalength = tvb_reported_length_remaining ( tvb, offset );
				size = tvb_reported_length_remaining ( tvb, offset ) - ( pyldoffset );
				lastentry = TRUE;
			}

			item = proto_tree_add_item(epl_tree, hf_epl_asnd_sdo_cmd_data, tvb, offset, datalength, ENC_NA);
			sdo_data_tree = proto_item_add_subtree(item, ett_epl_sdo_data);

			dataoffset = offset + 4;

			if (segmented <= EPL_ASND_SDO_CMD_SEGMENTATION_INITIATE_TRANSFER)
			{
				indx = tvb_get_letohs(tvb, dataoffset);
				proto_tree_add_uint(sdo_data_tree, hf_epl_asnd_sdo_cmd_data_index, tvb, dataoffset, 2, indx);
				proto_item_append_text(item, ": 0x%04X", indx );

				dataoffset += 2;
				subindx = tvb_get_guint8(tvb, dataoffset);
				proto_tree_add_uint(sdo_data_tree, hf_epl_asnd_sdo_cmd_data_subindex, tvb, dataoffset, 1, subindx);
				proto_item_append_text(item, "/0x%02X ", subindx );

				dataoffset += 1;
				proto_tree_add_uint(sdo_data_tree, hf_epl_asnd_sdo_cmd_data_padding, tvb, dataoffset, 1, padding);
				dataoffset += 1;
			}

			if ( size > 4 )
				size = size - padding;

			dissect_epl_payload ( sdo_data_tree, tvb, pinfo, dataoffset, size );

			offset += datalength;
		}
	}
	else
	{
		/
		col_append_str(pinfo->cinfo, COL_INFO, "Response");
	}
	return offset;
}
