static void
dissect_pana_pdu(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree)
{

       proto_tree *pana_tree=NULL;
       proto_tree *avp_tree=NULL;
       proto_item *ti=NULL;
       proto_item *avp_item=NULL;
       tvbuff_t *avp_tvb;
       guint16 flags = 0;
       guint16 msg_type;
       gint16 msg_length;
       gint16 avp_length;
       guint32 session_id;
       guint32 seq_num;
       conversation_t *conversation;
       pana_conv_info_t *pana_info;
       pana_transaction_t *pana_trans;
       int offset = 0;


       /
       msg_length = tvb_get_ntohs(tvb, 2);
       flags = tvb_get_ntohs(tvb, 4);
       msg_type = tvb_get_ntohs(tvb, 6);
       session_id = tvb_get_ntohl(tvb, 8);
       seq_num = tvb_get_ntohl(tvb, 12);
       avp_length = msg_length-16;

       /
       col_set_str(pinfo->cinfo, COL_PROTOCOL, "PANA");

       if (check_col(pinfo->cinfo, COL_INFO)) {
               col_add_fstr(pinfo->cinfo, COL_INFO, "Type %s-%s",
			    val_to_str(msg_type, msg_type_names, "Unknown (%d)"),
			    val_to_str(flags & PANA_FLAG_R, msg_subtype_names, "Unknown (%d)"));
       }

       /
       if (tree) {
               ti = proto_tree_add_item(tree, proto_pana, tvb, 0, -1, FALSE);
               pana_tree = proto_item_add_subtree(ti, ett_pana);
       }


       /
       conversation = find_or_create_conversation(pinfo);

       /
       pana_info = conversation_get_proto_data(conversation, proto_pana);
       if (!pana_info) {
               /
               pana_info = se_alloc(sizeof(pana_conv_info_t));
               pana_info->pdus=se_tree_create_non_persistent(EMEM_TREE_TYPE_RED_BLACK, "pana_pdus");

               conversation_add_proto_data(conversation, proto_pana, pana_info);
       }
       if(!pinfo->fd->flags.visited){
               if(flags&PANA_FLAG_R){
                      /
                      pana_trans=se_alloc(sizeof(pana_transaction_t));
                      pana_trans->req_frame=pinfo->fd->num;
                      pana_trans->rep_frame=0;
                      pana_trans->req_time=pinfo->fd->abs_ts;
                      se_tree_insert32(pana_info->pdus, seq_num, (void *)pana_trans);
               } else {
                      pana_trans=se_tree_lookup32(pana_info->pdus, seq_num);
                      if(pana_trans){
                              pana_trans->rep_frame=pinfo->fd->num;
                      }
               }
       } else {
               pana_trans=se_tree_lookup32(pana_info->pdus, seq_num);
       }
       if(!pana_trans){
               /
               pana_trans=ep_alloc(sizeof(pana_transaction_t));
               pana_trans->req_frame=0;
               pana_trans->rep_frame=0;
               pana_trans->req_time=pinfo->fd->abs_ts;
       }


       /
       if(flags&PANA_FLAG_R){
               /
               if(pana_trans->rep_frame){
                       proto_item *it;

                       it=proto_tree_add_uint(pana_tree, hf_pana_response_in, tvb, 0, 0, pana_trans->rep_frame);
                       PROTO_ITEM_SET_GENERATED(it);
               }
       } else {
               /
               if(pana_trans->req_frame){
                       proto_item *it;
                       nstime_t ns;

                       it=proto_tree_add_uint(pana_tree, hf_pana_response_to, tvb, 0, 0, pana_trans->req_frame);
                       PROTO_ITEM_SET_GENERATED(it);

                       nstime_delta(&ns, &pinfo->fd->abs_ts, &pana_trans->req_time);
                       it=proto_tree_add_time(pana_tree, hf_pana_time, tvb, 0, 0, &ns);
                       PROTO_ITEM_SET_GENERATED(it);
               }
       }



       /
       proto_tree_add_item(pana_tree, hf_pana_reserved_type, tvb, offset, 2, FALSE);
       offset += 2;

       /
       proto_tree_add_item(pana_tree, hf_pana_length_type, tvb, offset, 2, FALSE);
       offset += 2;

       /
       dissect_pana_flags(pana_tree, tvb, offset, flags);
       offset += 2;

       /
       proto_tree_add_uint_format_value(pana_tree, hf_pana_msg_type, tvb,
                           offset, 2, msg_type, "%s-%s (%d)",
                           val_to_str(msg_type, msg_type_names, "Unknown (%d)"),
			   val_to_str(flags & PANA_FLAG_R, msg_subtype_names, "Unknown (%d)"),
			   msg_type);
       offset += 2;

       /
       proto_tree_add_item(pana_tree, hf_pana_session_id, tvb, offset, 4, FALSE);
       offset += 4;

       /
       proto_tree_add_item(pana_tree, hf_pana_seqnumber, tvb, offset, 4, FALSE);
       offset += 4;

       /
       if(avp_length>0){
               avp_tvb = tvb_new_subset(tvb, offset, avp_length, avp_length);
               avp_item = proto_tree_add_text(pana_tree, tvb, offset, avp_length, "Attribute Value Pairs");
               avp_tree = proto_item_add_subtree(avp_item, ett_pana_avp);

               if (avp_tree != NULL) {
                       dissect_avps(avp_tvb, pinfo, avp_tree);
               }
        }
}
