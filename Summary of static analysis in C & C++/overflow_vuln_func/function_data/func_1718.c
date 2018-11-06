extern int dissect_codec_mode(proto_tree *tree, tvbuff_t *tvb, int offset, int len) {
	guint8 tempdata;
	proto_tree *scs_item, *acs_item;
	proto_tree *scs_tree, *acs_tree;
	
	
	tempdata = tvb_get_guint8(tvb, offset);
	proto_tree_add_uint(tree, hf_Organization_Identifier , tvb, offset, 1, tempdata );
	switch ( tempdata ){
		case ITU_T :
			offset = offset + 1;
			tempdata = tvb_get_guint8(tvb, offset);
			proto_tree_add_uint(tree, hf_codec_type , tvb, offset, 1, tempdata );
			offset = offset + 1;
			switch ( tempdata ) {
				case G_711_64_A :
				case G_711_64_U	:		
				case G_711_56_A	:		
				case G_711_56_U	: 		
				case G_722_SB_ADPCM :	
				case G_723_1 :				
				case G_723_1_Annex_A :
					/	
					break;
				case G_726_ADPCM :					
				case G_727_Embedded_ADPCM :
					/
					if ( len > 2 ) {	
						tempdata = tvb_get_guint8(tvb, offset);
						proto_tree_add_text(tree, tvb, offset, 1, "Configuration data : 0x%x", tempdata);
						offset = offset + 1;
					}
					break;	
				case G_728 :							
				case G_729_CS_ACELP :		
				case G_729_Annex_B :
					/
					if ( len > 2 ) {	
						tempdata = tvb_get_guint8(tvb, offset);
						proto_tree_add_text(tree, tvb, offset, 1 , "Configuration data : 0x%x", tempdata);
						offset = offset + 1;
					}
					break;
				default:
					break;
					
			}
				break;	
		case ETSI:
			offset = offset + 1;
			tempdata = tvb_get_guint8(tvb, offset);
			proto_tree_add_uint(tree, hf_etsi_codec_type , tvb, offset, 1, tempdata );
			if ( len > 2 )	{
				offset = offset + 1;
				tempdata = tvb_get_guint8(tvb, offset);

				acs_item = proto_tree_add_item(tree, hf_active_code_set, tvb, offset, 1, TRUE);
				acs_tree = proto_item_add_subtree(acs_item,ett_acs);
				proto_tree_add_item(acs_tree, hf_active_code_set_12_2, tvb, offset, 1, TRUE);
				proto_tree_add_item(acs_tree, hf_active_code_set_10_2, tvb, offset, 1, TRUE);
				proto_tree_add_item(acs_tree, hf_active_code_set_7_95, tvb, offset, 1, TRUE);
				proto_tree_add_item(acs_tree, hf_active_code_set_7_40, tvb, offset, 1, TRUE);
				proto_tree_add_item(acs_tree, hf_active_code_set_6_70, tvb, offset, 1, TRUE);
				proto_tree_add_item(acs_tree, hf_active_code_set_5_90, tvb, offset, 1, TRUE);
				proto_tree_add_item(acs_tree, hf_active_code_set_5_15, tvb, offset, 1, TRUE);
				proto_tree_add_item(acs_tree, hf_active_code_set_4_75, tvb, offset, 1, TRUE);
				
			}
			if ( len > 3 )	{
				offset = offset + 1;
				tempdata = tvb_get_guint8(tvb, offset);
				
				scs_item = proto_tree_add_item(tree, hf_supported_code_set, tvb, offset, 1, TRUE);
				scs_tree = proto_item_add_subtree(scs_item,ett_scs);
				proto_tree_add_item(scs_tree, hf_supported_code_set_12_2, tvb, offset, 1, TRUE);
				proto_tree_add_item(scs_tree, hf_supported_code_set_10_2, tvb, offset, 1, TRUE);
				proto_tree_add_item(scs_tree, hf_supported_code_set_7_95, tvb, offset, 1, TRUE);
				proto_tree_add_item(scs_tree, hf_supported_code_set_7_40, tvb, offset, 1, TRUE);
				proto_tree_add_item(scs_tree, hf_supported_code_set_6_70, tvb, offset, 1, TRUE);
				proto_tree_add_item(scs_tree, hf_supported_code_set_5_90, tvb, offset, 1, TRUE);
				proto_tree_add_item(scs_tree, hf_supported_code_set_5_15, tvb, offset, 1, TRUE);
				proto_tree_add_item(scs_tree, hf_supported_code_set_4_75, tvb, offset, 1, TRUE);
			}
			if ( len > 4 )	{
				offset = offset + 1;
				proto_tree_add_item(tree, hf_optimisation_mode, tvb, offset, 1, TRUE);
				proto_tree_add_item(tree, hf_max_codec_modes, tvb, offset, 1, TRUE);
			}
			offset = offset + 1;
			break;
		default:
			offset = offset + 1;
			tempdata = tvb_get_guint8(tvb, offset);
			proto_tree_add_text(tree, tvb, offset, len ,
								"Unknown organisation Identifier ( Non ITU-T/ETSI codec ) %u", tempdata);
			offset = offset + len - 1;
			break;
	}
	/
	
	return offset;
}
