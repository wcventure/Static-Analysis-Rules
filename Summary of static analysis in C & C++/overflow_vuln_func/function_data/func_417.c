static int
call_dop_oid_callback(char *base_oid, tvbuff_t *tvb, int offset, packet_info *pinfo, proto_tree *tree, char *col_info)
{
  char binding_param[MAX_OID_STR_LEN];

  g_snprintf(binding_param, MAX_OID_STR_LEN, "%s.%s", base_oid, binding_type ? binding_type : "");	

  if (col_info && (check_col(pinfo->cinfo, COL_INFO))) 
    col_append_fstr(pinfo->cinfo, COL_INFO, " %s", col_info);

  return call_ber_oid_callback(binding_param, tvb, offset, pinfo, tree);
}
