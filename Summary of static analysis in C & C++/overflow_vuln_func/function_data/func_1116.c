static void
dissect_bat_ase_Encapsulated_Application_Information(tvbuff_t *parameter_tvb, packet_info *pinfo, proto_tree *parameter_tree, gint offset)
{ 
	gint		length = tvb_reported_length_remaining(parameter_tvb, offset), list_end;
	tvbuff_t	*next_tvb;
	proto_tree	*bat_ase_tree, *bat_ase_element_tree, *bat_ase_iwfa_tree;
	proto_item	*bat_ase_item, *bat_ase_element_item, *bat_ase_iwfa_item;
	guint8 identifier,compatibility_info,content, BCTP_Indicator_field_1, BCTP_Indicator_field_2;
	guint8 sdp_length, tempdata, element_no, number_of_indicators;
	guint8 iwfa[32], diagnostic_len;
	guint8 length_ind_len;
	guint tempdata16;
	guint content_len, length_indicator;
	guint duration;
	guint diagnostic;
	guint32 bncid, Local_BCU_ID;
	element_no = 0;

	bat_ase_item = proto_tree_add_text(parameter_tree,parameter_tvb,
					   offset, -1,
"Bearer Association Transport (BAT) Application Service Element (ASE) Encapsulated Application Information:");
	bat_ase_tree = proto_item_add_subtree(bat_ase_item , ett_bat_ase);

	proto_tree_add_text(bat_ase_tree, parameter_tvb, offset, -1, 
		"BAT ASE Encapsulated Application Information, (%u byte%s length)", length, plurality(length, "", "s"));
	while(tvb_reported_length_remaining(parameter_tvb, offset) > 0){
		element_no = element_no + 1;
		identifier = tvb_get_guint8(parameter_tvb, offset);

		/
		offset = offset + 1;
		tempdata = tvb_get_guint8(parameter_tvb, offset);
		if ( tempdata & 0x80 ) {
			length_indicator = tempdata & 0x7f;
			length_ind_len = 1;	
		}
		else {
			offset = offset + 1;
			tempdata16 = ( tempdata & 0x7f );
			length_indicator = tvb_get_guint8(parameter_tvb, offset)& 0x0f;
			length_indicator = length_indicator << 7;
			length_indicator = length_indicator + tempdata16;
			length_ind_len = 2;
		}

		bat_ase_element_item = proto_tree_add_text(bat_ase_tree,parameter_tvb,
			( offset - length_ind_len),(length_indicator + 2),"BAT ASE Element %u, Identifier: %s",element_no,
					val_to_str(identifier,bat_ase_list_of_Identifiers_vals,"unknown (%u)"));
		bat_ase_element_tree = proto_item_add_subtree(bat_ase_element_item , 
			ett_bat_ase_element);
		if ( identifier != CODEC ) {
		/
		/
		proto_tree_add_uint(bat_ase_element_tree , hf_bat_ase_identifier , parameter_tvb, 
			offset - length_ind_len, 1, identifier );
		proto_tree_add_uint(bat_ase_element_tree , hf_length_indicator  , parameter_tvb, 
			offset - length_ind_len + 1, length_ind_len, length_indicator );
		
		offset = offset + 1;
		compatibility_info = tvb_get_guint8(parameter_tvb, offset);
		proto_tree_add_uint(bat_ase_element_tree, hf_Instruction_ind_for_general_action , parameter_tvb, offset, 1, compatibility_info );
		proto_tree_add_boolean(bat_ase_element_tree, hf_Send_notification_ind_for_general_action , parameter_tvb, offset, 1, compatibility_info );
		proto_tree_add_uint(bat_ase_element_tree, hf_Instruction_ind_for_pass_on_not_possible , parameter_tvb, offset, 1, compatibility_info );
		proto_tree_add_boolean(bat_ase_element_tree, hf_Send_notification_ind_for_pass_on_not_possible , parameter_tvb, offset, 1, compatibility_info );
		proto_tree_add_boolean(bat_ase_element_tree, hf_isup_extension_ind , parameter_tvb, offset, 1, compatibility_info );
		offset = offset + 1;
		}
		content_len = length_indicator - 1 ; /
	
		/
		switch ( identifier ){

			case ACTION_INDICATOR :

				content = tvb_get_guint8(parameter_tvb, offset);
				proto_tree_add_uint(bat_ase_element_tree, hf_Action_Indicator , parameter_tvb, offset, 1, content );
				proto_item_append_text(bat_ase_element_item, " - %s",
						 val_to_str(content,bat_ase_action_indicator_field_vals, "unknown (%u)"));
				offset = offset + 1;
				break;                         	
			case BACKBONE_NETWORK_CONNECTION_IDENTIFIER :   	

				bncid = tvb_get_ntohl(parameter_tvb, offset);
				switch ( content_len ){
				case 1:
						bncid = bncid & 0x000000ff;
						break;  
				case 2:
						bncid = bncid & 0x0000ffff;
						break;  
				case 3:
						bncid = bncid & 0x00ffffff;
						break;  
				case 4:;  
				default:;
				}
				proto_tree_add_uint_format(bat_ase_element_tree, hf_bncid, parameter_tvb, offset, content_len, bncid, "BNCId: 0x%08x", bncid);
				proto_item_append_text(bat_ase_element_item, " - 0x%08x",bncid);
				offset = offset + content_len;

			break;
			case INTERWORKING_FUNCTION_ADDRESS :           	
				tvb_memcpy(parameter_tvb, iwfa, offset, content_len);        	
				bat_ase_iwfa_item = proto_tree_add_bytes(bat_ase_element_tree, hf_bat_ase_biwfa, parameter_tvb, offset, content_len,
							    iwfa);
				bat_ase_iwfa_tree = proto_item_add_subtree(bat_ase_iwfa_item , ett_bat_ase_iwfa);
				dissect_nsap(parameter_tvb, offset, content_len, bat_ase_iwfa_tree);

				offset = offset + content_len;
			break;
			case CODEC_LIST :    
				list_end = offset + content_len;
				while ( offset < ( list_end - 1 )) { 
				identifier = tvb_get_guint8(parameter_tvb, offset);			      	
				offset = offset + 1;
				tempdata = tvb_get_guint8(parameter_tvb, offset);
				if ( tempdata & 0x80 ) {
					length_indicator = tempdata & 0x7f;	
				}
				else {
					offset = offset +1;
					length_indicator = tvb_get_guint8(parameter_tvb, offset);
					length_indicator = length_indicator << 7;
					length_indicator = length_indicator & ( tempdata & 0x7f );
				}
				offset = dissect_codec(parameter_tvb, bat_ase_element_tree, length_indicator , offset, identifier);
				}
			break;
			case CODEC :       
				/                       	
				offset = dissect_codec(parameter_tvb, bat_ase_element_tree, length_indicator , offset, identifier);
				break;/
			case BAT_COMPATIBILITY_REPORT :                	
				tempdata = tvb_get_guint8(parameter_tvb, offset);
				proto_tree_add_uint(bat_ase_element_tree, hf_BAT_ASE_Comp_Report_Reason, parameter_tvb, offset, 1, tempdata );
				offset = offset + 1;

				diagnostic_len = content_len - 1;
				while ( diagnostic_len > 0 ) {
					tempdata = tvb_get_guint8(parameter_tvb, offset);
					proto_tree_add_uint(bat_ase_element_tree, hf_BAT_ASE_Comp_Report_ident, parameter_tvb, offset, 1, tempdata );
					offset = offset + 1;
					diagnostic = tvb_get_letohs(parameter_tvb, offset);
					proto_tree_add_uint(bat_ase_element_tree, hf_BAT_ASE_Comp_Report_diagnostic, parameter_tvb, offset, 2, diagnostic);
					offset = offset + 2;
					diagnostic_len = diagnostic_len - 3;
				}
			break;
			case BEARER_NETWORK_CONNECTION_CHARACTERISTICS :
				tempdata = tvb_get_guint8(parameter_tvb, offset);
				proto_tree_add_uint(bat_ase_element_tree, hf_characteristics , parameter_tvb,
						 offset, 1, tempdata );
				proto_item_append_text(bat_ase_element_item, " - %s",
						 val_to_str(tempdata,bearer_network_connection_characteristics_vals, "unknown (%u)"));

				offset = offset + content_len;
			break;
/
/

			case BEARER_CONTROL_INFORMATION :              	
				BCTP_Indicator_field_1 = tvb_get_guint8(parameter_tvb, offset);
				proto_tree_add_uint(bat_ase_element_tree, hf_BCTP_Version_Indicator, 
					parameter_tvb, offset, 1, BCTP_Indicator_field_1 );
				
				proto_tree_add_boolean(bat_ase_element_tree, hf_BVEI,
					parameter_tvb, offset, 1, BCTP_Indicator_field_1 );
				offset = offset + 1;

				BCTP_Indicator_field_2 = tvb_get_guint8(parameter_tvb, offset);
				
				proto_tree_add_uint(bat_ase_element_tree, hf_Tunnelled_Protocol_Indicator ,
					parameter_tvb, offset, 1, BCTP_Indicator_field_2 );
				
				proto_tree_add_boolean(bat_ase_element_tree, hf_TPEI,
					parameter_tvb, offset, 1, BCTP_Indicator_field_2 );
				offset = offset + 1;

				sdp_length = ( length_indicator ) - 3;

				next_tvb = tvb_new_subset(parameter_tvb, offset, sdp_length, sdp_length);
				call_dissector(sdp_handle, next_tvb, pinfo, bat_ase_element_tree);
				offset = offset + sdp_length;


			break;
			case BEARER_CONTROL_TUNNELLING :              	

				tempdata = tvb_get_guint8(parameter_tvb, offset);
				proto_tree_add_boolean(bat_ase_element_tree, hf_bearer_control_tunneling , parameter_tvb, offset, 1, ( tempdata & 0x01 ) );
				if ( tempdata & 0x01 )
					proto_item_append_text(bat_ase_element_item, " - Tunnelling to be used ");

				offset = offset + content_len;
			break;
			case BEARER_CONTROL_UNIT_IDENTIFIER :          	
				tempdata = tvb_get_guint8(parameter_tvb, offset);
				proto_tree_add_text(bat_ase_element_tree, parameter_tvb, offset, 1, "Network ID Length indicator= %u",tempdata);
				offset = offset +1;
				if ( tempdata > 0 ) {
					offset = offset +1;
				
/
					proto_tree_add_text(bat_ase_element_tree, parameter_tvb, offset, tempdata , "Network ID: %s",
							    tvb_bytes_to_str(parameter_tvb, offset, tempdata));
					offset = offset + tempdata;
				} /

				Local_BCU_ID = tvb_get_letohl(parameter_tvb, offset);
				proto_tree_add_uint_format(bat_ase_element_tree, hf_Local_BCU_ID , parameter_tvb, offset, 4, Local_BCU_ID , "Local BCU ID : 0x%08x", Local_BCU_ID );
				offset = offset + 4;
			break;
			case SIGNAL :          	
				tempdata = tvb_get_guint8(parameter_tvb, offset);
				proto_tree_add_uint(bat_ase_element_tree, hf_bat_ase_signal , parameter_tvb, offset, 1, tempdata );
				offset = offset + 1;
				if ( content_len > 1){
				duration = tvb_get_letohs(parameter_tvb, offset);
				proto_tree_add_uint(bat_ase_element_tree, hf_bat_ase_duration , parameter_tvb, offset, 2, duration );
				offset = offset + 2;
				}
			break;
			case BEARER_REDIRECTION_CAPABILITY :            	
				tempdata = tvb_get_guint8(parameter_tvb, offset);
				proto_tree_add_boolean(bat_ase_element_tree, hf_late_cut_trough_cap_ind , parameter_tvb, offset, 1, tempdata );
				offset = offset + content_len;
			break;
			case BEARER_REDIRECTION_INDICATORS :
				number_of_indicators = 0;
				while ( number_of_indicators < content_len ) {        	
					tempdata = tvb_get_guint8(parameter_tvb, offset);
					proto_tree_add_uint(bat_ase_element_tree, hf_bat_ase_bearer_redir_ind , parameter_tvb, offset, 1, tempdata );
					offset = offset + 1;
					number_of_indicators = number_of_indicators + 1;
				}
			break;
			case SIGNAL_TYPE :                                  	
				tempdata = tvb_get_guint8(parameter_tvb, offset);
				proto_tree_add_uint(bat_ase_element_tree, hf_bat_ase_signal , parameter_tvb, offset, 1, tempdata );
				offset = offset + content_len;
			break;
			case DURATION :
				duration = tvb_get_letohs(parameter_tvb, offset);
				proto_tree_add_uint(bat_ase_element_tree, hf_bat_ase_duration , parameter_tvb, offset, 2, duration );
				offset = offset + content_len;
			break;
			default :
				proto_tree_add_text(bat_ase_element_tree, parameter_tvb, offset,content_len , "Default ?, (%u byte%s length)", (content_len), plurality(content_len, "", "s"));
				offset = offset + content_len;
			}                                	
  	} 

}
