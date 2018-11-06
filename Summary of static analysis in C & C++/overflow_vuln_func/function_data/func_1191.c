static gboolean
find_saved_invokedata(packet_info *pinfo, proto_tree *tree _U_, tvbuff_t *tvb _U_){
  struct ansi_tcap_invokedata_t *ansi_tcap_saved_invokedata;
  address* src = &(pinfo->src);
  address* dst = &(pinfo->dst);
  guint8 *src_str;
  guint8 *dst_str;
  char *buf;

  buf=ep_alloc(1024);
  src_str = address_to_str(src);
  dst_str = address_to_str(dst);

  /
  src_str = address_to_str(src);
  dst_str = address_to_str(dst);
  strcpy(buf, ansi_tcap_private.TransactionID_str);
  /
  strcat(buf,dst_str);
  strcat(buf,src_str);
  strcat(buf,"\0");
  ansi_tcap_saved_invokedata = g_hash_table_lookup(TransactionId_table, buf);
  if(ansi_tcap_saved_invokedata){
	  ansi_tcap_private.d.OperationCode			 = ansi_tcap_saved_invokedata->OperationCode;
	  ansi_tcap_private.d.OperationCode_national = ansi_tcap_saved_invokedata->OperationCode_national;
	  ansi_tcap_private.d.OperationCode_private  = ansi_tcap_saved_invokedata->OperationCode_private;
	  return TRUE;
  }
  return FALSE;
}
