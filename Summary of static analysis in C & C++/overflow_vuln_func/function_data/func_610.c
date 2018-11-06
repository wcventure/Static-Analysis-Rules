static void
show_cdf( int encap_service, tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, int offset )
{
   proto_item *temp_item, *ri, *ci;
   proto_item *sockaddr_item;
	proto_tree *temp_tree, *cip_tree, *item_tree, *sockaddr_tree;
	int temp_data, item_count, item_length, item, i;
	char temp_char;
	unsigned char name_length;

	/
	item_count = tvb_get_letohs( tvb, offset );
   ri = proto_tree_add_text( tree, tvb, offset, 2, "Item Count: %d", item_count );
	cip_tree = proto_item_add_subtree(ri, ett_cip);

	while( item_count-- )
	{
		/
		ci = proto_tree_add_item(cip_tree, hf_enip_cpf_typeid, tvb, offset+2, 2, TRUE );
		item_tree = proto_item_add_subtree(ci, ett_cpf);

		/
      temp_data = tvb_get_letohs( tvb, offset+4 );
      proto_tree_add_text( item_tree, tvb, offset+4, 2, "Length: %d", temp_data );

		item        = tvb_get_letohs( tvb, offset+2 );
		item_length = tvb_get_letohs( tvb, offset+4 );

		if( item_length )
		{
		   /

			switch( item )
			{
			   case CONNECTION_BASED:

			      /
			      proto_tree_add_text( item_tree, tvb, offset+6, 4, "Connection Identifier: 0x%08X", tvb_get_letohl( tvb, offset + 6 )  );

			      /
			      if(check_col(pinfo->cinfo, COL_INFO))
	            {
                  col_append_fstr(pinfo->cinfo, COL_INFO,
				         ", CONID: 0x%08X",
				         tvb_get_letohl( tvb, offset+6 ) );
				   }

			      break;

			   case UNCONNECTED_MSG:

					/
					add_cip_data( item_tree, tvb, offset+6, item_length, pinfo );

					break;

            case CONNECTION_TRANSPORT:

               if( encap_service == SEND_UNIT_DATA )
               {
                  /

                  /
                  proto_tree_add_text( item_tree, tvb, offset+6, 2, "Sequence Count: 0x%04X", tvb_get_letohs( tvb, offset+6 ) );

                  /
                  add_cip_data( item_tree, tvb, offset+8, item_length-2, pinfo );

                  /

   			      /
               }
               else
               {
                  /
                  add_byte_array_text_to_proto_tree( item_tree, tvb, offset+6, item_length, "Data: " );

               } /

               break;


            case LIST_IDENTITY_RESP:

               /
               temp_data = tvb_get_letohs( tvb, offset+6 );
               proto_tree_add_text( item_tree, tvb, offset+6, 2, "Encapsulation Version: %d", temp_data );

               /
               sockaddr_item = proto_tree_add_text( item_tree, tvb, offset+8, 16, "Socket Address");
               sockaddr_tree = proto_item_add_subtree( sockaddr_item, ett_sockadd );

               /
               proto_tree_add_item(sockaddr_tree, hf_enip_cpf_lir_sinfamily,
							tvb, offset+8, 2, FALSE );

               /
               proto_tree_add_item(sockaddr_tree, hf_enip_cpf_lir_sinport,
							tvb, offset+10, 2, FALSE );

               /
               proto_tree_add_item(sockaddr_tree, hf_enip_cpf_lir_sinaddr,
							tvb, offset+12, 4, FALSE );

               /
               proto_tree_add_item(sockaddr_tree, hf_enip_cpf_lir_sinzero,
							tvb, offset+16, 8, FALSE );

               /
               proto_tree_add_item(item_tree, hf_enip_vendors,
							tvb, offset+24, 2, TRUE );

               /
               proto_tree_add_item(item_tree, hf_enip_cpf_lir_devtype,
							tvb, offset+26, 2, TRUE );

               /
               proto_tree_add_item(item_tree, hf_enip_cpf_lir_prodcode,
							tvb, offset+28, 2, TRUE );

               /
               temp_data = tvb_get_letohs( tvb, offset+30 );
               proto_tree_add_text( item_tree, tvb, offset+30, 2, "Revision: v.%d.%02d", temp_data & 0xFF, ( temp_data & 0xFF00 ) >> 8 );

               /
               proto_tree_add_item(item_tree, hf_enip_cpf_lir_status,
							tvb, offset+32, 2, TRUE );

               /
               proto_tree_add_item(item_tree, hf_enip_cpf_lir_sernbr,
							tvb, offset+34, 4, TRUE );

               /
               proto_tree_add_item(item_tree, hf_enip_cpf_lir_namelength,
							tvb, offset+38, 1, TRUE );

               /
               name_length = tvb_get_guint8( tvb, offset+38 );

               /
               proto_tree_add_item(item_tree, hf_enip_cpf_lir_name,
							tvb, offset+39, name_length, TRUE );

               /
               proto_tree_add_item(item_tree, hf_enip_cpf_lir_state,
							tvb, offset+name_length+39, 1, TRUE );
               break;


            case SOCK_ADR_INFO_OT:
            case SOCK_ADR_INFO_TO:

               /
               proto_tree_add_item(item_tree, hf_enip_cpf_lir_sinfamily,
							tvb, offset+6, 2, FALSE );

               /
               proto_tree_add_item(item_tree, hf_enip_cpf_lir_sinport,
							tvb, offset+8, 2, FALSE );

               /
               proto_tree_add_item(item_tree, hf_enip_cpf_lir_sinaddr,
							tvb, offset+10, 4, FALSE );

               /
               proto_tree_add_item( item_tree, hf_enip_cpf_lir_sinzero,
							tvb, offset+14, 8, FALSE );
				   break;


            case SEQ_ADDRESS:
               proto_tree_add_item(item_tree, hf_enip_cpf_sat_connid,
							tvb, offset+6, 4, TRUE );

               proto_tree_add_item(item_tree, hf_enip_cpf_sat_seqnum,
							tvb, offset+10, 4, TRUE );

               /

               if(check_col(pinfo->cinfo, COL_INFO))
	            {
	               col_clear(pinfo->cinfo, COL_INFO);

                  col_add_fstr(pinfo->cinfo, COL_INFO,
				         "Connection:  ID=0x%08X, SEQ=%010d",
				         tvb_get_letohl( tvb, offset+6 ),
				         tvb_get_letohl( tvb, offset+10 ) );
				   }

				   break;

            case LIST_SERVICES_RESP:

               /
               temp_data = tvb_get_letohs( tvb, offset+6 );
               proto_tree_add_text( item_tree, tvb, offset+6, 2, "Encapsulation Version: %d", temp_data );

               /
               temp_data = tvb_get_letohs( tvb, offset+8 );
               temp_item = proto_tree_add_text(item_tree, tvb, offset+8, 2, "Capability Flags: 0x%04X", temp_data );
               temp_tree = proto_item_add_subtree(temp_item, ett_lsrcf);

               proto_tree_add_item(temp_tree, hf_enip_cpf_lsr_tcp,
                  tvb, offset+8, 2, TRUE );
      		   proto_tree_add_item(temp_tree, hf_enip_cpf_lsr_udp,
      			   tvb, offset+8, 2, TRUE );

               /
               temp_item = proto_tree_add_text( item_tree, tvb, offset+10, 16, "Name Of Service: " );

               for( i=0; i<16; i++ )
               {
                    temp_char = tvb_get_guint8( tvb, offset+10+i );

                    if( temp_char == 0 )
                     break;

                    proto_item_append_text(temp_item, "%c", temp_char );
               }
               break;


				default:

               add_byte_array_text_to_proto_tree( item_tree, tvb, offset+6, item_length, "Data: " );
               break;

			} /

		} /

		offset = offset + item_length + 4;

	} /

} /
