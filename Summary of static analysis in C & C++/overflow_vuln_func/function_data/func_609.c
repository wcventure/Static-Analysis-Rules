static void
add_cip_data( proto_tree *item_tree, tvbuff_t *tvb, int offset, int item_length, packet_info *pinfo )
{
   proto_item *pi, *rrsci, *ncppi, *ar_item, *temp_item, *temp_item2;
	proto_tree *temp_tree;
   proto_tree *rrsci_tree;
   proto_tree *ncp_tree;
   proto_tree *cmd_data_tree;
	int req_path_size, conn_path_size, mr_req_path_size;
	int temp_data;
	unsigned char gen_stat;
   unsigned char add_stat_size;
   unsigned char temp_byte, route_path_size;
   unsigned char app_rep_size, i;
   int msg_req_siz, num_services, serv_offset;


   /
	rrsci = proto_tree_add_text(item_tree, tvb, offset, 1, "Service: ");
	rrsci_tree = proto_item_add_subtree(rrsci, ett_rrsc);

	/
   proto_tree_add_item(rrsci_tree, hf_enip_ucm_rr,
			tvb, offset, 1, TRUE );

   proto_item_append_text( rrsci, "%s (%s)",
               val_to_str( ( tvb_get_guint8( tvb, offset ) & 0x7F ),
                  encap_sc_vals , "Unknown Service (%x)"),
               val_to_str( ( tvb_get_guint8( tvb, offset ) & 0x80 )>>7,
                  encap_sc_rr, "") );



   /
   if(check_col(pinfo->cinfo, COL_INFO))
   {
      col_append_fstr( pinfo->cinfo, COL_INFO, ", %s",
               val_to_str( ( tvb_get_guint8( tvb, offset ) & 0x7F ),
                  encap_sc_vals , "Unknown Service (%x)") );
   }


	/
	proto_tree_add_item(rrsci_tree, hf_enip_ucm_sc,
			tvb, offset, 1, TRUE );


	if( tvb_get_guint8( tvb, offset ) & 0x80 )
	{
	   /

		/
		gen_stat = tvb_get_guint8( tvb, offset+2 );

		proto_tree_add_item(item_tree, hf_enip_ucm_genstat,
			tvb, offset+2, 1, TRUE );

      /
      temp_data = tvb_get_guint8( tvb, offset+3 );
      proto_tree_add_text( item_tree, tvb, offset+3, 1, "Additional Status Size: %d (word)", temp_data );

		add_stat_size = tvb_get_guint8( tvb, offset+3 )*2;

		if( add_stat_size )
		{
         /
         pi = proto_tree_add_text( item_tree, tvb, offset+4, add_stat_size, "Additional Status:" );

         for( i=0; i < add_stat_size/2; i ++ )
         {
           proto_item_append_text( pi, " 0x%04X", tvb_get_letohs( tvb, offset+4+(i*2) ) );
         }
		}

      /
      if( ( item_length-4-add_stat_size ) != 0 )
      {
         pi = proto_tree_add_text( item_tree, tvb, offset+4+add_stat_size, item_length-4-add_stat_size, "Command Specific data" );
         cmd_data_tree = proto_item_add_subtree( pi, ett_cmd_data );

		   if( gen_stat == CI_GRC_SUCCESS )
   		{
   			/

   			if( ( tvb_get_guint8( tvb, offset ) & 0x7F ) == SC_FWD_OPEN )
            {
               /

               /
               temp_data = tvb_get_letohl( tvb, offset+4+add_stat_size );
               proto_tree_add_text( cmd_data_tree, tvb, offset+4+add_stat_size, 4, "O->T Network Connection ID: 0x%08X", temp_data );

               /
               temp_data = tvb_get_letohl( tvb, offset+4+add_stat_size+4 );
               proto_tree_add_text( cmd_data_tree, tvb, offset+4+add_stat_size+4, 4, "T->O Network Connection ID: 0x%08X", temp_data );

               /
               temp_data = tvb_get_letohs( tvb, offset+4+add_stat_size+8 );
               proto_tree_add_text( cmd_data_tree, tvb, offset+4+add_stat_size+8, 2, "Connection Serial Number: 0x%04X", temp_data );

               /
               proto_tree_add_item( cmd_data_tree, hf_enip_vendors, tvb, offset+4+add_stat_size+10, 2, TRUE);

               /
               temp_data = tvb_get_letohl( tvb, offset+4+add_stat_size+12 );
               proto_tree_add_text( cmd_data_tree, tvb, offset+4+add_stat_size+12, 4, "Originator Serial Number: 0x%08X", temp_data );

               /
               temp_data = tvb_get_letohl( tvb, offset+4+add_stat_size+16 );
               proto_tree_add_text( cmd_data_tree, tvb, offset+4+add_stat_size+16, 4, "O->T API: %dms (0x%08X)", temp_data / 1000, temp_data );

               /
               temp_data = tvb_get_letohl( tvb, offset+4+add_stat_size+20 );
               proto_tree_add_text( cmd_data_tree, tvb, offset+4+add_stat_size+20, 4, "T->O API: %dms (0x%08X)", temp_data / 1000, temp_data );

               /
               app_rep_size = tvb_get_guint8( tvb, offset+4+add_stat_size+24 ) * 2;
               proto_tree_add_text( cmd_data_tree, tvb, offset+4+add_stat_size+24, 1, "Application Reply Size: %d (words)", app_rep_size / 2 );

               /
               temp_byte = tvb_get_guint8( tvb, offset+4+add_stat_size+25 );
               proto_tree_add_text( cmd_data_tree, tvb, offset+4+add_stat_size+25, 1, "Reserved: 0x%02X", temp_byte );

               if( app_rep_size != 0 )
               {
                  /
                  ar_item = proto_tree_add_text( cmd_data_tree, tvb, offset+4+add_stat_size+26, app_rep_size, "Application Reply:" );

                  for( i=0; i < app_rep_size; i++ )
                  {
                    temp_byte = tvb_get_guint8( tvb, offset+4+add_stat_size+26+i );
                    proto_item_append_text(ar_item, " 0x%02X", temp_byte );
                  }

                } /

            } /
   			else if( ( tvb_get_guint8( tvb, offset ) & 0x7F ) == SC_FWD_CLOSE )
            {
               /

               /
               temp_data = tvb_get_letohs( tvb, offset+4+add_stat_size );
               proto_tree_add_text( cmd_data_tree, tvb, offset+4+add_stat_size, 2, "Connection Serial Number: 0x%04X", temp_data );

               /
               proto_tree_add_item( cmd_data_tree, hf_enip_vendors, tvb, offset+4+add_stat_size+2, 2, TRUE);

               /
               temp_data = tvb_get_letohl( tvb, offset+4+add_stat_size+4 );
               proto_tree_add_text( cmd_data_tree, tvb, offset+4+add_stat_size+4, 4, "Originator Serial Number: 0x%08X", temp_data );

               /
               app_rep_size = tvb_get_guint8( tvb, offset+4+add_stat_size+8 ) * 2;
               proto_tree_add_text( cmd_data_tree, tvb, offset+4+add_stat_size+8, 1, "Application Reply Size: %d (words)", app_rep_size / 2 );

               /
               temp_byte = tvb_get_guint8( tvb, offset+4+add_stat_size+9 );
               proto_tree_add_text( cmd_data_tree, tvb, offset+4+add_stat_size+9, 1, "Reserved: 0x%02X", temp_byte );

               if( app_rep_size != 0 )
               {
                  /
                  ar_item = proto_tree_add_text( cmd_data_tree, tvb, offset+4+add_stat_size+10, app_rep_size, "Application Reply:" );

                  for( i=0; i < app_rep_size; i ++ )
                  {
                    temp_byte = tvb_get_guint8( tvb, offset+4+add_stat_size+10+i );
                    proto_item_append_text(ar_item, " 0x%02X", temp_byte );
                  }

                } /

            } /
            else if( ( tvb_get_guint8( tvb, offset ) & 0x7F ) == SC_UNCON_SEND )
            {
               /

               /
               add_byte_array_text_to_proto_tree( cmd_data_tree, tvb, offset+4+add_stat_size, item_length-4-add_stat_size, "Data: " );
            }
            else if( ( tvb_get_guint8( tvb, offset ) & 0x7F ) == SC_MULT_SERV_PACK )
            {
               /

               /
               num_services = tvb_get_letohs( tvb, offset+4+add_stat_size );
               proto_tree_add_text( cmd_data_tree, tvb, offset+4+add_stat_size, 2, "Number of Replies: %d", num_services );

               /
               temp_item = proto_tree_add_text( cmd_data_tree, tvb, offset+2+add_stat_size+4, num_services*2, "Offsets: " );

               for( i=0; i < num_services; i++ )
               {
                  int serv_length;

                  serv_offset = tvb_get_letohs( tvb, offset+6+add_stat_size+(i*2) );

                  if( i == (num_services-1) )
                  {
                     /
                     proto_item_append_text(temp_item, "%d", serv_offset );
                     serv_length = item_length-add_stat_size-serv_offset-4;
                  }
                  else
                  {
                     proto_item_append_text(temp_item, "%d, ", serv_offset );
                     serv_length = tvb_get_letohs( tvb, offset+6+add_stat_size+((i+1)*2) ) - serv_offset;
                  }

                  temp_item2 = proto_tree_add_text( cmd_data_tree, tvb, offset+serv_offset+4, serv_length, "Service Reply #%d", i+1 );
                  temp_tree = proto_item_add_subtree( temp_item2, ett_mult_ser );
                  add_cip_data( temp_tree, tvb, offset+serv_offset+4, serv_length, pinfo );
               }
            } /
            else if( ( tvb_get_guint8( tvb, offset ) & 0x7F ) == SC_GET_ATT_LIST )
            {
               /

               int att_count;

               /
               att_count = tvb_get_letohs( tvb, offset+4+add_stat_size );
               proto_tree_add_text( cmd_data_tree, tvb, offset+4+add_stat_size, 2, "Attribute Count: %d", att_count );

               /
               add_byte_array_text_to_proto_tree( cmd_data_tree, tvb, offset+6+add_stat_size, item_length-6-add_stat_size, "Data: " );

            } /
            else
   			{
   			   /
               add_byte_array_text_to_proto_tree( cmd_data_tree, tvb, offset+4+add_stat_size, item_length-4-add_stat_size, "Data: " );
   		   } /

   	   }
         else
         {
            /

            if( ( ( tvb_get_guint8( tvb, offset ) & 0x7F ) == SC_FWD_OPEN ) ||
                ( ( tvb_get_guint8( tvb, offset ) & 0x7F ) == SC_FWD_CLOSE ) )
            {
               /

               /
               temp_data = tvb_get_letohs( tvb, offset+4+add_stat_size );
               proto_tree_add_text( cmd_data_tree, tvb, offset+4+add_stat_size, 2, "Connection Serial Number: 0x%04X", temp_data );

               /
               proto_tree_add_item( cmd_data_tree, hf_enip_vendors, tvb, offset+4+add_stat_size+2, 2, TRUE);

               /
               temp_data = tvb_get_letohl( tvb, offset+4+add_stat_size+4 );
               proto_tree_add_text( cmd_data_tree, tvb, offset+4+add_stat_size+4, 4, "Originator Serial Number: 0x%08X", temp_data );

               /
               temp_data = tvb_get_guint8( tvb, offset+4+add_stat_size+8 );
               proto_tree_add_text( cmd_data_tree, tvb, offset+4+add_stat_size+8, 1, "Remaining Path Size: %d", temp_data );

               /
               temp_data = tvb_get_guint8( tvb, offset+4+add_stat_size+9 );
               proto_tree_add_text( cmd_data_tree, tvb, offset+4+add_stat_size+9, 1, "Reserved: 0x%02X", temp_data );
            }
            else if( ( tvb_get_guint8( tvb, offset ) & 0x7F ) == SC_UNCON_SEND )
            {
               /

               /
               temp_data = tvb_get_guint8( tvb, offset+4+add_stat_size);
               proto_tree_add_text( cmd_data_tree, tvb, offset+4+add_stat_size, 1, "Remaining Path Size: %d", temp_data );
            }
            else
            {
               /
               add_byte_array_text_to_proto_tree( cmd_data_tree, tvb, offset+4+add_stat_size, item_length-4-add_stat_size, "Data: " );
            }

         } /

      } /

	} /
	else
	{
	   /

	   /
	   req_path_size = tvb_get_guint8( tvb, offset+1 )*2;
	   proto_tree_add_text( item_tree, tvb, offset+1, 1, "Request Path Size: %d (words)", req_path_size/2 );

      /
      pi = proto_tree_add_text(item_tree, tvb, offset+2, req_path_size, "Request Path: ");
      show_epath( tvb, pi, offset+2, req_path_size );

      /
      if( (item_length-req_path_size-2) != 0 )
      {

         pi = proto_tree_add_text( item_tree, tvb, offset+2+req_path_size, item_length-req_path_size-2, "Command Specific Data" );
         cmd_data_tree = proto_item_add_subtree( pi, ett_cmd_data );

         /

         if( tvb_get_guint8( tvb, offset ) == SC_FWD_OPEN )
         {
            /

            /
            temp_byte = tvb_get_guint8( tvb, offset+2+req_path_size );
            proto_tree_add_text( cmd_data_tree, tvb, offset+2+req_path_size, 1, "Priority/Time_tick: 0x%02X", temp_byte );

            /
            temp_data = tvb_get_guint8( tvb, offset+2+req_path_size+1 );
            proto_tree_add_text( cmd_data_tree, tvb, offset+2+req_path_size+1, 1, "Time-out_ticks: %d", temp_data );

            /
            temp_data = ( 1 << ( temp_byte & 0x0F ) ) * temp_data;
            proto_tree_add_text( cmd_data_tree, tvb, offset+2+req_path_size, 2, "Actual Time Out: %dms", temp_data );

            /
            temp_data = tvb_get_letohl( tvb, offset+2+req_path_size+2 );
            proto_tree_add_text( cmd_data_tree, tvb, offset+2+req_path_size+2, 4, "O->T Network Connection ID: 0x%08X", temp_data );

            /
            temp_data = tvb_get_letohl( tvb, offset+2+req_path_size+6 );
            proto_tree_add_text( cmd_data_tree, tvb, offset+2+req_path_size+6, 4, "T->O Network Connection ID: 0x%08X", temp_data );

            /
            temp_data = tvb_get_letohs( tvb, offset+2+req_path_size+10 );
            proto_tree_add_text( cmd_data_tree, tvb, offset+2+req_path_size+10, 2, "Connection Serial Number: 0x%04X", temp_data );

            /
            proto_tree_add_item( cmd_data_tree, hf_enip_vendors, tvb, offset+2+req_path_size+12, 2, TRUE);

            /
            temp_data = tvb_get_letohl( tvb, offset+2+req_path_size+14 );
            proto_tree_add_text( cmd_data_tree, tvb, offset+2+req_path_size+14, 4, "Originator Serial Number: 0x%08X", temp_data );

            /
            temp_data = tvb_get_guint8( tvb, offset+2+req_path_size+18 );
            proto_tree_add_text( cmd_data_tree, tvb, offset+2+req_path_size+18, 1, "Connection Timeout Multiplier: %s (%d)", val_to_str( temp_data, enip_con_time_mult_vals , "Reserved" ), temp_data );

            /
            proto_tree_add_text( cmd_data_tree, tvb, offset+2+req_path_size+19, 3, "Reserved Data" );

            /
            temp_data = tvb_get_letohl( tvb, offset+2+req_path_size+22 );
            proto_tree_add_text( cmd_data_tree, tvb, offset+2+req_path_size+22, 4, "O->T RPI: %dms (0x%08X)", temp_data / 1000, temp_data );

   	      /
   	      temp_data = tvb_get_letohs( tvb, offset+2+req_path_size+26 );
   	      ncppi = proto_tree_add_text(cmd_data_tree, tvb, offset+2+req_path_size+26, 2, "O->T Network Connection Parameters: 0x%04X", temp_data );
   	      ncp_tree = proto_item_add_subtree(ncppi, ett_ncp);

            /
            proto_tree_add_item(ncp_tree, hf_enip_ucm_fwo_own,
   					tvb, offset+2+req_path_size+26, 2, TRUE );
   			proto_tree_add_item(ncp_tree, hf_enip_ucm_fwo_typ,
   					tvb, offset+2+req_path_size+26, 2, TRUE );
            proto_tree_add_item(ncp_tree, hf_enip_ucm_fwo_prio,
   					tvb, offset+2+req_path_size+26, 2, TRUE );
   			proto_tree_add_item(ncp_tree, hf_enip_ucm_fwo_fixed_var,
   					tvb, offset+2+req_path_size+26, 2, TRUE );
   			proto_tree_add_item(ncp_tree, hf_enip_ucm_fwo_con_size,
   					tvb, offset+2+req_path_size+26, 2, TRUE );

            /
            temp_data = tvb_get_letohl( tvb, offset+2+req_path_size+28 );
            proto_tree_add_text( cmd_data_tree, tvb, offset+2+req_path_size+28, 4, "T->O RPI: %dms (0x%08X)", temp_data / 1000, temp_data );

   	      /
   	      temp_data = tvb_get_letohs( tvb, offset+2+req_path_size+32 );
   	      ncppi = proto_tree_add_text(cmd_data_tree, tvb, offset+2+req_path_size+32, 2, "T->O Network Connection Parameters: 0x%04X", temp_data );
   	      ncp_tree = proto_item_add_subtree(ncppi, ett_ncp);

            /
            proto_tree_add_item(ncp_tree, hf_enip_ucm_fwo_own,
   					tvb, offset+2+req_path_size+32, 2, TRUE );
   			proto_tree_add_item(ncp_tree, hf_enip_ucm_fwo_typ,
   					tvb, offset+2+req_path_size+32, 2, TRUE );
            proto_tree_add_item(ncp_tree, hf_enip_ucm_fwo_prio,
   					tvb, offset+2+req_path_size+32, 2, TRUE );
   			proto_tree_add_item(ncp_tree, hf_enip_ucm_fwo_fixed_var,
   					tvb, offset+2+req_path_size+32, 2, TRUE );
   			proto_tree_add_item(ncp_tree, hf_enip_ucm_fwo_con_size,
   					tvb, offset+2+req_path_size+32, 2, TRUE );

            /
            temp_data = tvb_get_guint8( tvb, offset+2+req_path_size+34 );

   	      ncppi = proto_tree_add_text(cmd_data_tree, tvb, offset+2+req_path_size+34, 1, "Transport Type/Trigger: 0x%02X", temp_data );
   	      ncp_tree = proto_item_add_subtree(ncppi, ett_ncp);

            /
            proto_tree_add_item(ncp_tree, hf_enip_ucm_fwo_dir,
   					tvb, offset+2+req_path_size+34, 1, TRUE );

   			proto_tree_add_item(ncp_tree, hf_enip_ucm_fwo_trigg,
   					tvb, offset+2+req_path_size+34, 1, TRUE );

            proto_tree_add_item(ncp_tree, hf_enip_ucm_fwo_class,
   					tvb, offset+2+req_path_size+34, 1, TRUE );

            /
            conn_path_size = tvb_get_guint8( tvb, offset+2+req_path_size+35 )*2;
            proto_tree_add_text( cmd_data_tree, tvb, offset+2+req_path_size+35, 1, "Connection Path Size: %d (words)", conn_path_size / 2 );

            /
            pi = proto_tree_add_text(cmd_data_tree, tvb, offset+2+req_path_size+36, conn_path_size, "Connection Path: ");
            show_epath( tvb, pi, offset+2+req_path_size+36, conn_path_size );
         }
         else if( tvb_get_guint8( tvb, offset ) == SC_FWD_CLOSE )
         {
            /

            /
            temp_byte = tvb_get_guint8( tvb, offset+2+req_path_size );
            proto_tree_add_text( cmd_data_tree, tvb, offset+2+req_path_size, 1, "Priority/Time_tick: 0x%02X", temp_byte );

            /
            temp_data = tvb_get_guint8( tvb, offset+2+req_path_size+1 );
            proto_tree_add_text( cmd_data_tree, tvb, offset+2+req_path_size+1, 1, "Time-out_ticks: %d", temp_data );

            /
            temp_data = ( 1 << ( temp_byte & 0x0F ) ) * temp_data;
            proto_tree_add_text( cmd_data_tree, tvb, offset+2+req_path_size, 2, "Actual Time Out: %dms", temp_data );

            /
            temp_data = tvb_get_letohs( tvb, offset+2+req_path_size+2 );
            proto_tree_add_text( cmd_data_tree, tvb, offset+2+req_path_size+2, 2, "Connection Serial Number: 0x%04X", temp_data );

            /
            proto_tree_add_item( cmd_data_tree, hf_enip_vendors, tvb, offset+2+req_path_size+4, 2, TRUE);

            /
            temp_data = tvb_get_letohl( tvb, offset+2+req_path_size+6 );
            proto_tree_add_text( cmd_data_tree, tvb, offset+2+req_path_size+6, 4, "Originator Serial Number: 0x%08X", temp_data );

            /
            conn_path_size = tvb_get_guint8( tvb, offset+2+req_path_size+10 )*2;
            proto_tree_add_text( cmd_data_tree, tvb, offset+2+req_path_size+10, 1, "Connection Path Size: %d (words)", conn_path_size / 2 );

            /
            temp_byte = tvb_get_guint8( tvb, offset+2+req_path_size+11 );
            proto_tree_add_text( cmd_data_tree, tvb, offset+2+req_path_size+11, 1, "Reserved: 0x%02X", temp_byte );

            /
            pi = proto_tree_add_text(cmd_data_tree, tvb, offset+2+req_path_size+12, conn_path_size, "Connection Path: ");
            show_epath( tvb, pi, offset+2+req_path_size+12, conn_path_size );

         } /
         else if( tvb_get_guint8( tvb, offset ) == SC_UNCON_SEND )
         {
            /

            /
            temp_byte = tvb_get_guint8( tvb, offset+2+req_path_size );
            proto_tree_add_text( cmd_data_tree, tvb, offset+2+req_path_size, 1, "Priority/Time_tick: 0x%02X", temp_byte );

            /
            temp_data = tvb_get_guint8( tvb, offset+2+req_path_size+1 );
            proto_tree_add_text( cmd_data_tree, tvb, offset+2+req_path_size+1, 1, "Time-out_ticks: %d", temp_data );

            /
            temp_data = ( 1 << ( temp_byte & 0x0F ) ) * temp_data;
            proto_tree_add_text( cmd_data_tree, tvb, offset+2+req_path_size, 2, "Actual Time Out: %dms", temp_data );

            /
            msg_req_siz = tvb_get_letohs( tvb, offset+2+req_path_size+2 );
            proto_tree_add_text( cmd_data_tree, tvb, offset+2+req_path_size+2, 2, "Message Request Size: 0x%04X", msg_req_siz );

            /
            temp_item = proto_tree_add_text( cmd_data_tree, tvb, offset+2+req_path_size+4, msg_req_siz, "Message Request" );
            temp_tree = proto_item_add_subtree(temp_item, ett_mes_req );

            /
            temp_data = tvb_get_guint8( tvb, offset+2+req_path_size+4 );
            proto_tree_add_text( temp_tree, tvb, offset+2+req_path_size+4, 1, "Service: %s (0x%02X)", val_to_str( temp_data, encap_sc_vals , "" ), temp_data );

            /
            if(check_col(pinfo->cinfo, COL_INFO))
            {
               col_append_fstr( pinfo->cinfo, COL_INFO, ", %s",
                        val_to_str( ( temp_data & 0x7F ),
                           encap_sc_vals , ", Unknown Service (%x)") );
            }

            /
   		   mr_req_path_size = tvb_get_guint8( tvb, offset+2+req_path_size+5 )*2;
   		   proto_tree_add_text( temp_tree, tvb, offset+2+req_path_size+5, 1, "Request Path Size: %d (words)", mr_req_path_size/2 );

            /
            temp_item = proto_tree_add_text(temp_tree, tvb, offset+2+req_path_size+6, mr_req_path_size, "Request Path: ");
            show_epath( tvb, temp_item, offset+2+req_path_size+6, mr_req_path_size );

            /
            if( ( msg_req_siz-2-mr_req_path_size ) != 0 )
            {
               add_byte_array_text_to_proto_tree( temp_tree, tvb, offset+2+req_path_size+6+mr_req_path_size, msg_req_siz-2-mr_req_path_size, "Request Data: " );
            }

            if( msg_req_siz % 2 )
            {
               /
               proto_tree_add_text( cmd_data_tree, tvb, offset+2+req_path_size+4+msg_req_siz, 1, "Pad Byte (0x%02X)",
                  tvb_get_guint8( tvb, offset+2+req_path_size+4+msg_req_siz ) );
               msg_req_siz++;	/
            }

            /
            route_path_size = tvb_get_guint8( tvb, offset+2+req_path_size+4+msg_req_siz )*2;
            proto_tree_add_text( cmd_data_tree, tvb, offset+2+req_path_size+4+msg_req_siz, 1, "Route Path Size: %d (words)", route_path_size/2 );

            /
            proto_tree_add_text( cmd_data_tree, tvb, offset+2+req_path_size+5+msg_req_siz, 1, "Reserved (0x%02X)",
                tvb_get_guint8( tvb, offset+2+req_path_size+5+msg_req_siz ) );

            /
            temp_item = proto_tree_add_text(cmd_data_tree, tvb, offset+2+req_path_size+6+msg_req_siz, route_path_size, "Route Path");
            show_epath( tvb, temp_item, offset+2+req_path_size+6+msg_req_siz, route_path_size );

         } /
         else if( tvb_get_guint8( tvb, offset ) == SC_MULT_SERV_PACK )
         {
            /

            /
            num_services = tvb_get_letohs( tvb, offset+2+req_path_size );
            proto_tree_add_text( cmd_data_tree, tvb, offset+2+req_path_size, 2, "Number of Services: %d", num_services );

            /
            temp_item = proto_tree_add_text( cmd_data_tree, tvb, offset+2+req_path_size+2, num_services*2, "Offsets: " );

            for( i=0; i < num_services; i++ )
            {
               int serv_length;

               serv_offset = tvb_get_letohs( tvb, offset+4+req_path_size+(i*2) );

               if( i == (num_services-1) )
               {
                  /
                  serv_length = item_length-2-req_path_size-serv_offset;
                  proto_item_append_text(temp_item, "%d", serv_offset );
               }
               else
               {
                  serv_length = tvb_get_letohs( tvb, offset+4+req_path_size+((i+1)*2) ) - serv_offset;
                  proto_item_append_text(temp_item, "%d, ", serv_offset );
               }

               temp_item2 = proto_tree_add_text( cmd_data_tree, tvb, offset+serv_offset+6, serv_length, "Service Packet #%d", i+1 );
               temp_tree = proto_item_add_subtree( temp_item2, ett_mult_ser );
               add_cip_data( temp_tree, tvb, offset+serv_offset+6, serv_length, pinfo );
            }
         } /
         else if( tvb_get_guint8( tvb, offset ) == SC_GET_ATT_LIST )
         {
            /

            int att_count;

            /
            att_count = tvb_get_letohs( tvb, offset+2+req_path_size );
            proto_tree_add_text( cmd_data_tree, tvb, offset+2+req_path_size, 2, "Attribute Count: %d", att_count );

            /
            temp_item = proto_tree_add_text( cmd_data_tree, tvb, offset+2+req_path_size+2, att_count*2, "Attribute List: " );

            for( i=0; i < att_count; i++ )
            {
               if( i == (att_count-1) )
                  proto_item_append_text(temp_item, "%d",tvb_get_letohs( tvb, offset+4+req_path_size+(i*2) ) );
               else
                  proto_item_append_text(temp_item, "%d, ",tvb_get_letohs( tvb, offset+4+req_path_size+(i*2) ) );
            }

         } /
         else
         {
		      /
            add_byte_array_text_to_proto_tree( cmd_data_tree, tvb, offset+2+req_path_size, item_length-req_path_size-2, "Data: " );
         } /

      } /

	} /

} /
