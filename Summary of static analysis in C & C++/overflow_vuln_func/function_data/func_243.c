static void
display_req_forward(tvbuff_t *tvb, packet_info *pinfo,
                    proto_tree *ajp13_tree,
                    ajp13_conv_data* cd)
{
  int pos = 0;
  guint8 meth;
  guint8 cod;
  guint8 ver[1024];
  guint16 ver_len;
  guint8 uri[4096];
  guint16 uri_len;
  guint8 raddr[4096];
  guint16 raddr_len;
  guint8 rhost[4096];
  guint16 rhost_len;
  guint8 srv[4096];
  guint16 srv_len;
  guint nhdr;
  int i;

  /

  if (ajp13_tree)
    proto_tree_add_item(ajp13_tree, hf_ajp13_magic, tvb, pos, 2, 0);
  pos+=2;

  if (ajp13_tree)
    proto_tree_add_item(ajp13_tree, hf_ajp13_len, tvb, pos, 2, 0);
  pos+=2;

  /
  cod = tvb_get_guint8(tvb, 4);
  if (ajp13_tree) {
    gchar* msg_code = NULL;
    guint8 mcode = 0;
    char mcode_buf[1024];
    msg_code = val_to_str(cod, mtype_codes, "UNKNOWN");
    sprintf(mcode_buf, "(%d) %s", cod, msg_code);
    proto_tree_add_string(ajp13_tree, hf_ajp13_code, tvb, pos, 1, mcode_buf);
  }
  pos+=1;

  /
  {
    gchar* meth_code = NULL;
    guint8 mcode = 0;
    meth = tvb_get_guint8(tvb, pos);
    meth_code = val_to_str(meth, http_method_codes, "UNKNOWN");
    if (ajp13_tree) {
      char mcode_buf[1024];
      sprintf(mcode_buf, "(%d) %s", meth, meth_code);
      proto_tree_add_string(ajp13_tree, hf_ajp13_method, tvb, pos, 1, mcode_buf);
    }
    if(check_col(pinfo->cinfo, COL_INFO))
      col_append_str(pinfo->cinfo, COL_INFO, meth_code);
    pos+=1;
  }

  /
  ver_len = get_nstring(tvb, pos, ver);
  pos+=2; // skip over size 
  if (ajp13_tree)
    proto_tree_add_item(ajp13_tree, hf_ajp13_ver, tvb, pos, ver_len, 0);
  pos=pos+ver_len;  /

  /
  uri_len = get_nstring(tvb, pos, uri);
  pos+=2; // skip over size
  if (ajp13_tree)
    proto_tree_add_item(ajp13_tree, hf_ajp13_uri, tvb, pos, uri_len, 0);
  pos=pos+uri_len;  /


  if(check_col(pinfo->cinfo, COL_INFO))
    col_append_fstr(pinfo->cinfo, COL_INFO, " %s %s", uri, ver);


  /
  raddr_len = get_nstring(tvb, pos, raddr);
  pos+=2; // skip over size
  if (ajp13_tree)
    proto_tree_add_item(ajp13_tree, hf_ajp13_raddr, tvb, pos, raddr_len, 0);
  pos=pos+raddr_len;  // skip over chars + trailing null

  /
  rhost_len = get_nstring(tvb, pos, rhost);
  pos+=2; // skip over size
  if (ajp13_tree)
    proto_tree_add_item(ajp13_tree, hf_ajp13_rhost, tvb, pos, rhost_len, 0);
  pos=pos+rhost_len;  // skip over chars + trailing null

  /
  srv_len = get_nstring(tvb, pos, srv);
  pos+=2; // skip over size
  if (ajp13_tree)
    proto_tree_add_item(ajp13_tree, hf_ajp13_srv, tvb, pos, srv_len, 0);
  pos=pos+srv_len;  // skip over chars + trailing null

  /
  if (ajp13_tree)
    proto_tree_add_item(ajp13_tree, hf_ajp13_port, tvb, pos, 2, 0);
  pos+=2;

  /
  if (ajp13_tree)
    proto_tree_add_item(ajp13_tree, hf_ajp13_sslp, tvb, pos, 1, 0);
  pos+=1;

  /
  nhdr = tvb_get_ntohs(tvb, pos);
  /
  if (ajp13_tree)
    proto_tree_add_item(ajp13_tree, hf_ajp13_nhdr, tvb, pos, 2, 0);
  pos+=2;
  cd->content_length = 0;
      
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
      hname = val_to_str(hid, req_header_codes, "UNKNOWN");
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
      /
      proto_tree_add_string_format(ajp13_tree, hf_ajp13_hval,
                                   tvb, orig_pos, dp, hname,
                                   "%s: %s", hname, hval);
    }
    if (cl) {
      cl = atoi(hval);
      /
      cd->content_length = cl;
    }
  }
}
