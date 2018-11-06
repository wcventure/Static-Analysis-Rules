guint16 dissect_cbs_message_identifier(tvbuff_t *tvb, proto_tree *tree, guint16 offset)
{
   guint16 msg_id;
   const char *msg_id_string = NULL;

   msg_id = tvb_get_ntohs(tvb, offset);
   msg_id_string = try_val_to_str(msg_id, message_id_values);
   if (msg_id_string == NULL)
   {
      if (msg_id < 1000)
      {
         msg_id_string = "Message ID to be allocated by GSMA";
      }
      else if (msg_id < 4096)
      {
         msg_id_string = "Message ID intended for standardization in future versions of 3GPP TS 23.041";
      }
      else if (msg_id < 4224)
      {
         msg_id_string = "Message ID reserved for Cell Broadcast Data Download (unsecured) to the SIM ";
      }
      else if (msg_id < 4352)
      {
         msg_id_string = "Message ID reserved for Cell Broadcast Data Download (secured) to the SIM ";
      }
      else if (msg_id < 4360)
      {
         msg_id_string = "ETWS CBS Message Identifier for future extension";
      }
      else if (msg_id < 4400)
      {
         msg_id_string = "CMAS CBS Message Identifier for future extension";
      }
      else if (msg_id < 6400)
      {
         msg_id_string = "CBS Message Identifier for future PWS use";
      }
      else if (msg_id < 40960)
      {
         msg_id_string = "Intended for standardization in future versions of 3GPP TS 23.041";
      }
      else if (msg_id < 43500)
      {
         msg_id_string = "Message ID in PLMN operator specific range";
      }
      else if (msg_id < 43530)
      {
         msg_id_string = "Traffic Information Traffic Master UK";
      }
      else if (msg_id < 43585)
      {
         msg_id_string = "Traffic information Mannesmann Telecommerce";
      }
      else if (msg_id < 45056)
      {
         msg_id_string = "Message ID in PLMN operator specific range";
      }
      else
      {
         msg_id_string = "Message ID intended as PLMN operator specific range in future versions of 3GPP TS 23.041";
      }
   }
   proto_tree_add_uint_format_value(tree, hf_gsm_cbs_message_identifier, tvb, offset, 2, msg_id, "%s (%d)", msg_id_string, msg_id);
   return 2;
}
