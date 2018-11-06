static void
dissect_dect(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree)
{
	guint16			type;
	guint			pkt_len;
	const guint8		*pkt_ptr;
	struct dect_afield	pkt_afield;
	struct dect_bfield	pkt_bfield;

	/
	pkt_len=tvb_length(tvb);

	if(pkt_len<=DECT_PACKET_INFO_LEN)
	{
		col_set_str(pinfo->cinfo, COL_PROTOCOL, "No Data");
		return;
	}

	pkt_ptr=tvb_get_ptr(tvb, DECT_PACKET_INFO_LEN, pkt_len-DECT_PACKET_INFO_LEN);

	/
	pkt_afield.Header=pkt_ptr[0];
	memcpy((char*)(&(pkt_afield.Tail)), (char*)(pkt_ptr+1), 5);
	pkt_afield.RCRC=(((guint16)pkt_ptr[6])<<8)|pkt_ptr[7];

	/
	if(pkt_len>DECT_PACKET_INFO_LEN+2)
		memcpy((char*)(&(pkt_bfield.Data)), (char*)(pkt_ptr+8), pkt_len-5-8);
	else
		memset((char*)(&(pkt_bfield.Data)), 0, 128);
	pkt_bfield.Length=pkt_len-DECT_PACKET_INFO_LEN-8;

	col_set_str(pinfo->cinfo, COL_PROTOCOL, "DECT");

	/
	col_clear(pinfo->cinfo, COL_INFO);
	if(tree)
	{
		proto_item *ti		=NULL;
		proto_item *typeti	=NULL;
		proto_tree *DectTree	=NULL;
		gint offset		=0;

		ti=proto_tree_add_item(tree, proto_dect, tvb, 0, -1, FALSE);

		DectTree=proto_item_add_subtree(ti, ett_dect);
		proto_tree_add_item(DectTree, hf_dect_transceivermode, tvb, offset, 1, FALSE);
		offset++;

		proto_tree_add_item(DectTree, hf_dect_channel, tvb, offset, 1, FALSE);
		offset++;

		proto_tree_add_item(DectTree, hf_dect_slot, tvb, offset, 2, FALSE);
		offset+=2;

		proto_tree_add_item(DectTree, hf_dect_framenumber, tvb, offset, 1, FALSE);
		offset++;

		proto_tree_add_item(DectTree, hf_dect_rssi, tvb, offset, 1, FALSE);
		offset++;

		proto_tree_add_item(DectTree, hf_dect_preamble, tvb, offset, 3, FALSE);
		offset+=3;

		typeti=proto_tree_add_item(DectTree, hf_dect_type, tvb, offset, 2, FALSE);

		type=tvb_get_ntohs(tvb, offset);
		offset+=2;

		switch(type) {
		case 0x1675:
			col_set_str(pinfo->cinfo, COL_PROTOCOL, "DECT PP");
			proto_item_append_text(typeti, " Phone Packet");
			dissect_decttype(DECT_PACKET_PP, &pkt_afield, &pkt_bfield, pinfo, pkt_ptr, tvb, ti, DectTree);
			break;
		case 0xe98a:
			col_set_str(pinfo->cinfo, COL_PROTOCOL, "DECT RFP");
			proto_item_append_text(typeti, " Station Packet");
			dissect_decttype(DECT_PACKET_FP, &pkt_afield, &pkt_bfield, pinfo, pkt_ptr, tvb, ti, DectTree);
			break;
		default:
			col_set_str(pinfo->cinfo, COL_PROTOCOL, "DECT Unk");
			proto_item_append_text(typeti, " Unknown Packet");
			break;
		}
	}
}
