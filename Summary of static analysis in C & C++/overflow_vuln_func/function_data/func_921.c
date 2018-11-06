tvbuff_t * dissect_cbs_data(guint8 sms_encoding, tvbuff_t *tvb, proto_tree *tree, packet_info *pinfo, guint16 offset )
{
   tvbuff_t * tvb_out = NULL;
   int			length = tvb_length(tvb) - offset;
   gchar *utf8_text = NULL, *utf8_out;
   guint8 * input_string;
   GIConv cd;
   GError *l_conv_error = NULL;

   switch(sms_encoding){
     case SMS_ENCODING_7BIT:
     case SMS_ENCODING_7BIT_LANG:
     utf8_text = tvb_get_ts_23_038_7bits_string(wmem_packet_scope(), tvb, offset<<3, (length*8)/7);
     utf8_out = g_strdup(utf8_text);
     tvb_out = tvb_new_child_real_data(tvb, utf8_out, (guint)strlen(utf8_out), (guint)strlen(utf8_out));
     tvb_set_free_cb(tvb_out, g_free);
     add_new_data_source(pinfo, tvb_out, "unpacked 7 bit data");
     break;

     case SMS_ENCODING_8BIT:
     tvb_out = tvb_new_subset(tvb, offset, length, length);
     break;

     case SMS_ENCODING_UCS2:
     case SMS_ENCODING_UCS2_LANG:
     input_string = tvb_get_string(wmem_packet_scope(), tvb, offset, length);
     if ((cd = g_iconv_open("UTF-8","UCS-2BE")) != (GIConv) -1)
     {
         utf8_text = g_convert_with_iconv(input_string, length, cd, NULL, NULL, &l_conv_error);
         if(!l_conv_error)
         {
            tvb_out = tvb_new_subset(tvb, offset, length, length);
         }
         else
         proto_tree_add_text(tree, tvb, offset, length, "CBS String: g_convert_with_iconv FAILED");

         g_free(utf8_text);
         g_iconv_close(cd);
     }
     else
     {
            proto_tree_add_text(tree, tvb, offset, length, "CBS String: g_iconv_open FAILED contact wireshark");
     }
     break;

      default:
         proto_tree_add_text(tree, tvb, offset, length, "Unhandled encoding %d of CBS String", sms_encoding);
     break;
   }
   return tvb_out;
}
