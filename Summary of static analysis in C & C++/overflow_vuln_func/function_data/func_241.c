static void
display_rsp(tvbuff_t *tvb, packet_info *pinfo, proto_tree *ajp13_tree)
{
  gchar* msg_code = NULL;
  int pos = 0;
  guint8 mcode = 0;
  char mcode_buf[1024];
  int i;

  /

  /
  if (ajp13_tree)
    proto_tree_add_item(ajp13_tree, hf_ajp13_magic, tvb, pos, 2, 0);
  pos+=2;

  /
  if (ajp13_tree)
    proto_tree_add_item(ajp13_tree, hf_ajp13_len,   tvb, pos, 2, 0);
  pos+=2;

  /
  mcode = tvb_get_guint8(tvb, pos);
  msg_code = val_to_str(mcode, mtype_codes, "UNKNOWN");
  sprintf(mcode_buf, "(%d) %s", mcode, msg_code);
  if (ajp13_tree)
    proto_tree_add_string(ajp13_tree, hf_ajp13_code, tvb, pos, 1, mcode_buf);
  pos+=1;

  if(check_col(pinfo->cinfo, COL_INFO))
    col_append_str(pinfo->cinfo, COL_INFO, msg_code);

  if (mcode == 5) {

    guint8 len = tvb_get_guint8(tvb, pos);
    if (ajp13_tree)
      proto_tree_add_item(ajp13_tree, hf_ajp13_reusep, tvb, pos, 1, 0);
    pos+=1;

  } else if (mcode == 4) {

    guint8 rsmsg_bytes[8*1024]; /
    guint16 rsmsg_len;
    guint16 nhdr;
    guint16 rcode_num;

    /
    rcode_num = tvb_get_ntohs(tvb, pos);
    if (ajp13_tree)
      proto_tree_add_item(ajp13_tree, hf_ajp13_rstatus, tvb, pos, 2, 0);
    pos+=2;
    if(check_col(pinfo->cinfo, COL_INFO))
      col_append_fstr(pinfo->cinfo, COL_INFO, ":%d", rcode_num);

    /
    rsmsg_len = get_nstring(tvb, pos, rsmsg_bytes);
    pos+=2;
    if (ajp13_tree)
      proto_tree_add_item(ajp13_tree, hf_ajp13_rsmsg, tvb, pos, rsmsg_len, 0);
    pos+=rsmsg_len;
    /
    if(check_col(pinfo->cinfo, COL_INFO))
      col_append_fstr(pinfo->cinfo, COL_INFO, " %s", rsmsg_bytes);
    
    /
    nhdr = tvb_get_ntohs(tvb, pos);
    if (ajp13_tree)
      proto_tree_add_item(ajp13_tree, hf_ajp13_nhdr, tvb, pos, 2, 0);
    pos+=2;

    /
    for(i=0; i<nhdr; i++) {

      guint8 hcd;
      guint8 hid;
      char hval[8192];
      guint16 hval_len;
      int orig_pos = pos;
      gchar* hname = NULL;
      int dp = 0;
      int cl = 0;
      guint8 hname_bytes[1024];
      gchar hname_value[8*1024];

      /
      hcd = tvb_get_guint8(tvb, pos);
      /
      if (hcd == 0xA0) {
        pos+=1;
        hid = tvb_get_guint8(tvb, pos);
        pos+=1;
        /
        hname = val_to_str(hid, rsp_header_codes, "UNKNOWN");
        /
        if (hid == 0x08)
          cl = 1;
      } else {
        int hname_len = get_nstring(tvb, pos, hname_bytes);
        /
        pos+=hname_len+2;
        hname = (gchar*)hname_bytes; /
      }

      /
      dp = pos-orig_pos;
      /

      /

      /
      orig_pos = pos;
      hval_len = get_nstring(tvb, pos, hval);
      /
      pos+=hval_len+2;
      dp = pos - orig_pos;
      if (ajp13_tree) {
        sprintf(hname_value, "%s : %s", hname, hval);
        proto_tree_add_string(ajp13_tree, hf_ajp13_hval, tvb, orig_pos, dp, hname_value);
      }
    }

  } else if (mcode == 6) {
    guint16 len = tvb_get_ntohs(tvb, pos);
    if (ajp13_tree)
      proto_tree_add_item(ajp13_tree, hf_ajp13_rlen, tvb, pos, 2, 0);
    pos+=2;

  } else {
    /
    if (ajp13_tree)
      proto_tree_add_item(ajp13_tree, hf_ajp13_data,  tvb, pos+2, -1, 0);
  }
}
