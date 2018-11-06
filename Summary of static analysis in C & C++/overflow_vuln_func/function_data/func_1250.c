static void
dissect_h261( tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree )
{
	proto_item *ti            = NULL;
	proto_tree *h261_tree     = NULL;
	unsigned int offset       = 0;

	if ( check_col( pinfo->cinfo, COL_PROTOCOL ) )   {
		col_set_str( pinfo->cinfo, COL_PROTOCOL, "H.261" );
	}
	
	if ( check_col( pinfo->cinfo, COL_INFO) ) {
		col_set_str( pinfo->cinfo, COL_INFO, "H.261 message");
	}

	if ( tree ) {
		ti = proto_tree_add_item( tree, proto_h261, tvb, offset, -1, FALSE );
		h261_tree = proto_item_add_subtree( ti, ett_h261 );
		/
		proto_tree_add_uint( h261_tree, hf_h261_sbit, tvb, offset, 1, tvb_get_guint8( tvb, offset ) >> 5 );
		/
		proto_tree_add_uint( h261_tree, hf_h261_ebit, tvb, offset, 1, ( tvb_get_guint8( tvb, offset )  << 3 ) >> 5 );
		/
		proto_tree_add_boolean( h261_tree, hf_h261_ibit, tvb, offset, 1, tvb_get_guint8( tvb, offset ) & 2 );
		/
		proto_tree_add_boolean( h261_tree, hf_h261_vbit, tvb, offset, 1, tvb_get_guint8( tvb, offset ) & 1 );
		offset++;

		/
		proto_tree_add_uint( h261_tree, hf_h261_gobn, tvb, offset, 1, tvb_get_guint8( tvb, offset ) >> 4 );
		/
		proto_tree_add_uint( h261_tree, hf_h261_mbap, tvb, offset, 1,
		    ( tvb_get_guint8( tvb, offset ) & 15 )
		    + ( tvb_get_guint8( tvb, offset + 1 ) >> 7 ) );
		offset++;

		/
		proto_tree_add_uint( h261_tree, hf_h261_quant, tvb, offset, 1, tvb_get_guint8( tvb, offset ) & 124 );
		/
		proto_tree_add_uint( h261_tree, hf_h261_hmvd, tvb, offset, 1,
		    ( ( tvb_get_guint8( tvb, offset ) << 4) >> 4 )
		     + ( tvb_get_guint8( tvb, offset ) >> 5 ) );
		offset++;

		/
		proto_tree_add_uint( h261_tree, hf_h261_vmvd, tvb, offset, 1, tvb_get_guint8( tvb, offset ) & 31 );
		offset++;

		/
		proto_tree_add_item( h261_tree, hf_h261_data, tvb, offset, tvb_length_remaining( tvb, offset ), FALSE );
	}
}
