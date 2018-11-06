static void
dissect_cip_multiple_service_packet_req(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, proto_item * item, int offset)
{
   proto_item *mult_serv_item;
   proto_tree *mult_serv_tree;
   int i, num_services, serv_offset;
   cip_req_info_t *cip_req_info, *mr_single_req_info;
   mr_mult_req_info_t *mr_mult_req_info = NULL;

   /
   num_services = tvb_get_letohs( tvb, offset);
   proto_tree_add_item(tree, hf_cip_sc_mult_serv_pack_num_services, tvb, offset, 2, ENC_LITTLE_ENDIAN);

   /
   cip_req_info = (cip_req_info_t*)p_get_proto_data( pinfo->fd, proto_cip );
   if ( cip_req_info )
   {
      if ( cip_req_info->pData == NULL )
      {
         mr_mult_req_info = se_alloc(sizeof(mr_mult_req_info_t));
         mr_mult_req_info->service = SC_MULT_SERV_PACK;
         mr_mult_req_info->num_services = 0;
         mr_mult_req_info->requests = se_alloc(sizeof(cip_req_info_t)*num_services);
         cip_req_info->pData = mr_mult_req_info;
      }
      else
      {
         mr_mult_req_info = (mr_mult_req_info_t*)cip_req_info->pData;
         if ( mr_mult_req_info && mr_mult_req_info->num_services != num_services )
            mr_mult_req_info = NULL;
      }
   }

   for( i=0; i < num_services; i++ )
   {
      int serv_length;
      tvbuff_t *next_tvb;

      serv_offset = tvb_get_letohs( tvb, offset+2+(i*2) );

      if (tvb_reported_length_remaining(tvb, serv_offset) <= 0)
      {
         expert_add_info_format(pinfo, item, PI_MALFORMED, PI_WARN, "Multiple Service Packet service invalid offset");
         continue;
      }

      if( i == (num_services-1) )
      {
         /
         serv_length = tvb_reported_length_remaining(tvb, offset)-serv_offset;
      }
      else
      {
         serv_length = tvb_get_letohs( tvb, offset+2+((i+1)*2) ) - serv_offset;
      }

      mult_serv_item = proto_tree_add_text(tree, tvb, offset+serv_offset, serv_length, "Service Packet #%d", i+1 );
      mult_serv_tree = proto_item_add_subtree(mult_serv_item, ett_cip_mult_service_packet );
      proto_tree_add_item(mult_serv_tree, hf_cip_sc_mult_serv_pack_offset, tvb, offset+2+(i*2) , 2, ENC_LITTLE_ENDIAN);

      /

      col_append_str( pinfo->cinfo, COL_INFO, ", ");

      next_tvb = tvb_new_subset(tvb, offset+serv_offset, serv_length, serv_length);

      if ( mr_mult_req_info )
      {
         mr_single_req_info = mr_mult_req_info->requests + i;
         mr_single_req_info->bService = 0;
         mr_single_req_info->dissector = NULL;
         mr_single_req_info->IOILen = 0;
         mr_single_req_info->pIOI = NULL;
         mr_single_req_info->pData = NULL;
         mr_single_req_info->ciaData = NULL;

         dissect_cip_data(mult_serv_tree, next_tvb, 0, pinfo, mr_single_req_info );
      }
      else
      {
         dissect_cip_data(mult_serv_tree, next_tvb, 0, pinfo, NULL );
      }

      /
      mr_mult_req_info->num_services = i;
   }

}
