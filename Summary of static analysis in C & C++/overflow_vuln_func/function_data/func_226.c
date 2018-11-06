static void
dissect_avps(tvbuff_t *tvb, packet_info *pinfo, proto_tree *avp_tree)
{

       gint offset;
       guint16 avp_code;
       guint16 avp_flags;
       guint16 avp_length;
       guint16 avp_type;
       guint32 vendor_id;
       guint16 avp_hdr_length;
       guint16 avp_data_length;
       guint16 padding;

       guint16 buffer_length;
       int bad_avp = FALSE;

       tvbuff_t *group_tvb;
       tvbuff_t *eap_tvb;
       proto_item *single_avp_item;
       proto_tree *single_avp_tree;
       proto_item *avp_group_item;
       proto_tree *avp_group_tree;
       proto_item *avp_eap_item;
       proto_tree *avp_eap_tree;

       offset = 0;
       buffer_length = tvb_reported_length(tvb);

       /
       while (buffer_length > 0) {
               avp_code = tvb_get_ntohs(tvb, offset);
               avp_flags = tvb_get_ntohs(tvb, offset + 2);
               avp_length = tvb_get_ntohs(tvb, offset + 4);

               /
               if (avp_flags & PANA_AVP_FLAG_V) {
                       vendor_id = tvb_get_ntohl(tvb, 8);
                       avp_hdr_length = 12;
               } else {
                       vendor_id = 0;
                       avp_hdr_length = 8;
               }

               /
               avp_type = pana_avp_get_type(avp_code, vendor_id);

               /
               if (avp_length < avp_hdr_length) {
                       single_avp_item = proto_tree_add_text(avp_tree, tvb, offset, avp_length,
                                                             "%s (%s) length: %d bytes (shorter than header length %d)",
                                                             val_to_str(avp_code, avp_code_names, "Unknown (%d)"),
                                                             val_to_str(avp_type, avp_type_names, "Unknown (%d)"),
                                                             avp_length,
                                                             avp_hdr_length);

                       single_avp_tree = proto_item_add_subtree(single_avp_item, ett_pana_avp_info);

                       if (single_avp_tree != NULL) {
                               /
                               proto_tree_add_uint_format_value(single_avp_tree, hf_pana_avp_code, tvb,
                                                                offset, 2, avp_code, "%s (%u)",
                                                                val_to_str(avp_code, avp_code_names, "Unknown (%d)"),
                                                                avp_code);
                               offset += 2;
                               /
                               dissect_pana_avp_flags(single_avp_tree, tvb, offset, avp_flags);
                               offset += 2;
                               /
                               proto_tree_add_item(single_avp_tree, hf_pana_avp_length, tvb, offset, 2, FALSE);
                               offset += 2;
                       }
                       return;
               }

               /
               if (avp_flags & PANA_AVP_FLAG_RES) bad_avp = TRUE;

               /
               padding = (4 - (avp_length % 4)) % 4;

               single_avp_item = proto_tree_add_text(avp_tree, tvb, offset, avp_length + padding,
                                                               "%s (%s) length: %d bytes (%d padded bytes)",
                                                               val_to_str(avp_code, avp_code_names, "Unknown (%d)"),
                                                               val_to_str(avp_type, avp_type_names, "Unknown (%d)"),
                                                               avp_length,
                                                               avp_length + padding);

               single_avp_tree = proto_item_add_subtree(single_avp_item, ett_pana_avp_info);

               /
               avp_data_length = avp_length - avp_hdr_length;

               if (single_avp_tree != NULL) {
                       /
                       proto_tree_add_uint_format_value(single_avp_tree, hf_pana_avp_code, tvb,
                                                       offset, 2, avp_code, "%s (%u)",
                                                       val_to_str(avp_code, avp_code_names, "Unknown (%d)"),
                                                       avp_code);
               }
               offset += 2;
               if (single_avp_tree != NULL) {
                       /
                       dissect_pana_avp_flags(single_avp_tree, tvb, offset, avp_flags);
               }
               offset += 2;
               if (single_avp_tree != NULL) {
                       /
                       proto_tree_add_item(single_avp_tree, hf_pana_avp_length, tvb, offset, 2, FALSE);
               }
               offset += 2;
               if (single_avp_tree != NULL) {
                       /
                       proto_tree_add_item(single_avp_tree, hf_pana_avp_reserved, tvb, offset, 2, FALSE);
               }
               offset += 2;
               if (avp_flags & PANA_AVP_FLAG_V) {
                       if (single_avp_tree != NULL) {
                               /
                               proto_tree_add_item(single_avp_tree, hf_pana_avp_vendorid, tvb, offset, 4, FALSE);
                       }
                       offset += 4;
               }
               if (avp_flags & PANA_AVP_FLAG_V) {
                       /
                       switch(avp_type) {
                               case PANA_GROUPED: {
                                       avp_group_item = proto_tree_add_text(single_avp_tree,
                                                                         tvb, offset, avp_data_length,
                                                                         "Grouped AVP");
                                       avp_group_tree = proto_item_add_subtree(avp_group_item, ett_pana_avp);
                                       group_tvb = tvb_new_subset(tvb, offset,
                                                                       MIN(avp_data_length, tvb_length(tvb)-offset), avp_data_length);
                                       if (avp_group_tree != NULL) {
                                               dissect_avps(group_tvb, pinfo, avp_group_tree);
                                       }
                                       break;
                               }
                               case PANA_UTF8STRING: {
                                       const guint8 *data;
                                       data = tvb_get_ptr(tvb, offset, avp_data_length);
                                       proto_tree_add_string_format(single_avp_tree, hf_pana_avp_data_string, tvb,
                                                       offset, avp_data_length, data,
                                                       "UTF8String: %*.*s",
                                                       avp_data_length, avp_data_length, data);
                                       break;
                               }
                               case PANA_OCTET_STRING: {
                                       proto_tree_add_bytes_format(single_avp_tree, hf_pana_avp_data_bytes, tvb,
                                                       offset, avp_data_length,
                                                   tvb_get_ptr(tvb, offset, avp_data_length),
                                                       "Hex Data Highlighted Below");
                                       break;
                               }
                               case PANA_INTEGER32: {
                                       proto_tree_add_item(single_avp_tree, hf_pana_avp_data_int32, tvb,
                                                       offset, 4, FALSE);
                                       break;
                               }
                               case PANA_UNSIGNED32: {
                                       proto_tree_add_item(single_avp_tree, hf_pana_avp_data_uint32, tvb,
                                                       offset, 4, FALSE);
                                       break;
                               }
                               case PANA_INTEGER64: {
                                       proto_tree_add_item(single_avp_tree, hf_pana_avp_data_int64, tvb,
                                                       offset, 8, FALSE);
                                       break;
                               }
                               case PANA_UNSIGNED64: {
                                       proto_tree_add_item(single_avp_tree, hf_pana_avp_data_uint64, tvb,
                                                       offset, 8, FALSE);
                                       break;
                               }
                               case PANA_ENUMERATED: {
                                       proto_tree_add_item(single_avp_tree, hf_pana_avp_data_enumerated, tvb,
                                                       offset, 4, FALSE);
                                       break;
                               }
                               case PANA_RESULT_CODE: {
                                       proto_tree_add_text(single_avp_tree, tvb, offset, avp_data_length,
                                                               "Value: %d (%s)",
                                                               tvb_get_ntohl(tvb, offset),
                                                               val_to_str(tvb_get_ntohs(tvb, offset), avp_code_names, "Unknown (%d)"));
                                       break;
                               }
                               case PANA_EAP: {
                                       avp_eap_item = proto_tree_add_text(single_avp_tree,
                                                                         tvb, offset, avp_data_length,
                                                                         "AVP Value (EAP packet)");
                                       avp_eap_tree = proto_item_add_subtree(avp_eap_item, ett_pana_avp);
                                       eap_tvb = tvb_new_subset(tvb, offset, avp_data_length, avp_data_length);
                                       if (eap_handle != NULL) {
                                               call_dissector(eap_handle, eap_tvb, pinfo, avp_eap_tree);
                                       }
                                       break;
                               }
                       }
               }
               offset += avp_data_length + padding;

               /
               buffer_length -=  avp_length + padding;
       }

}
