static gint
dissect_bfield(gboolean dect_packet_type _U_, guint8 a_header,
	struct dect_bfield *pkt_bfield, packet_info *pinfo _U_, const guint8 *pkt_ptr _U_,
	tvbuff_t *tvb, proto_item *ti _U_, proto_tree *DectTree, gint offset, proto_tree *ColumnsTree)
{
	guint8 xcrc, xcrclen;
	guint16 blen;
	gint oldoffset, fn;
	char *bfield_str;
	char *bfield_short_str;
	proto_item *bfieldti        = NULL;
	proto_tree *BField          = NULL;

	proto_item *bfdescrdatati   = NULL;
	proto_tree *BFDescrData	    = NULL;

	/
	switch((a_header&0x0E)>>1)
	{
	case 0:
	case 1:
	case 3:
	case 5:
	case 6:
		blen=40;
		xcrclen=4;

		bfield_short_str="Full Slot";
		bfield_str="Full Slot (320 bit data, 4 bit xcrc)";
		break;
	case 2:
		blen=100;
		xcrclen=4;

		bfield_short_str="Double Slot";
		bfield_str="Double Slot (800 bit data, 4 bit xcrc)";
		break;
	case 4:
		blen=10;
		xcrclen=4;

		bfield_short_str="Half Slot";
		bfield_str="Half Slot (80 bit data, 4 bit xcrc)";
		break;
	case 7:
	default:
		blen=0;
		xcrclen=0;

		bfield_short_str="No B-Field";
		bfield_str="No B-Field";
		break;

	}

	proto_tree_add_string(ColumnsTree, hf_dect_cc_BField, tvb, offset, 1, bfield_short_str);

	if(blen)
	{
		bfieldti		= proto_tree_add_item(DectTree, hf_dect_B, tvb, offset, 40, FALSE);
		BField			= proto_item_add_subtree(bfieldti, ett_bfield);

		proto_tree_add_none_format(BField, hf_dect_B_Data, tvb, offset, 40, "%s", bfield_str);

		bfdescrdatati	= proto_tree_add_item(BField, hf_dect_B_DescrambledData, tvb, offset, 40, FALSE);
		BFDescrData		= proto_item_add_subtree(bfdescrdatati, ett_bfdescrdata);
	}

	oldoffset=offset;

	if(blen<=pkt_bfield->Length)
	{
		guint16 x, y;
		for(fn=0;fn<8;fn++)
		{
			guint16 bytecount=0;

			offset=oldoffset;

			proto_tree_add_none_format(BFDescrData, hf_dect_B_fn, tvb, offset, 0, "Framenumber %u/%u", fn, fn+8);
			for(x=0;x<blen;x+=16)
			{
				/
				emem_strbuf_t *string;
				string = ep_strbuf_new(NULL);
				for(y=0;y<16;y++)
				{
					if((x+y)>=blen)
						break;

					ep_strbuf_append_printf(string,"%.2x ", pkt_bfield->Data[x+y]^scrt[fn][bytecount%31]);
					bytecount++;
				}
				proto_tree_add_none_format(BFDescrData, hf_dect_B_Data, tvb, offset, y, "Data: %s", string->str);
				offset+=y;
			}
		}
	}
	else
		proto_tree_add_none_format(BField, hf_dect_B_Data, tvb, offset, 0, "Data too Short");

	xcrc=calc_xcrc(pkt_bfield->Data, 83);

	if((blen+1)<=pkt_bfield->Length)
	{
		if(xcrc!=(pkt_bfield->Data[40]&0xf0))
			/
			proto_tree_add_uint_format(bfieldti, hf_dect_B_XCRC, tvb, offset, 1, 0, "X-CRC Error (Calc:%.2x, Recv:%.2x)",xcrc, pkt_bfield->Data[40]&0xf0);
		else
			/
			proto_tree_add_uint_format(bfieldti, hf_dect_B_XCRC, tvb, offset, 1, 1, "X-CRC Match (Calc:%.2x, Recv:%.2x)", xcrc, pkt_bfield->Data[40]&0xf0);
	}
	else
		proto_tree_add_uint_format(bfieldti, hf_dect_B_XCRC, tvb, offset, 1, 0, "No X-CRC logged (Calc:%.2x)", xcrc);

	return offset;
}
