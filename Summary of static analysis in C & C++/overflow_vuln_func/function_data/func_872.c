#if 0
static void dissect_syslog(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree)
{
  gint pri = -1, lev, fac;
  gint msg_off = 0, msg_len;
  gint ellipsis_len = (COL_INFO_LEN - strlen(ELLIPSIS)) - 1;
  proto_item *ti;
  proto_tree *syslog_tree;
  gchar msg_str[COL_INFO_LEN];
#else
static void dissect_syslog(const u_char *pd, int o, frame_data *fd, proto_tree *tree)
{
  packet_info	*pinfo = &pi;
  tvbuff_t	*tvb = tvb_create_from_top(o);
  gint pri = -1, lev = -1, fac = -1;
  gint msg_off = 0, msg_len;
  gint ellipsis_len = (COL_INFO_LEN - strlen(ELLIPSIS)) - 1;
  proto_item *ti;
  proto_tree *syslog_tree;
  gchar msg_str[COL_INFO_LEN];

#endif

  pinfo->current_proto = "Syslog";
  msg_len = tvb_length(tvb);
  
  if (tvb_get_guint8(tvb, 0) == '<' && isdigit(tvb_get_guint8(tvb, 1))) {
    msg_off++;
    pri = 0;
    while (isdigit(tvb_get_guint8(tvb, msg_off)) && msg_off <= MAX_DIGITS &&
           tvb_length_remaining(tvb, msg_off)) {
      pri = pri * 10 + (tvb_get_guint8(tvb, msg_off) - '0');
      msg_off++;
    }
    if (tvb_get_guint8(tvb, msg_off) == '>')
      msg_off++;
    fac = (pri & FACILITY_MASK) >> 3;
    lev = pri & PRIORITY_MASK;
  }

  /
  msg_len = tvb_length_remaining(tvb, msg_off);
  if (msg_len > ellipsis_len) {
    tvb_memcpy(tvb, msg_str, msg_off, ellipsis_len);
    strcpy (msg_str + ellipsis_len, ELLIPSIS);
    msg_str[COL_INFO_LEN] = '\0';
  } else {
    tvb_memcpy(tvb, msg_str, msg_off, msg_len);
    msg_str[msg_len] = '\0';
  }
    
  if (check_col(pinfo->fd, COL_PROTOCOL)) 
    col_add_str(pinfo->fd, COL_PROTOCOL, "Syslog");
    
  if (check_col(pinfo->fd, COL_INFO)) {
    if (pri >= 0) {
      col_add_fstr(pinfo->fd, COL_INFO, "%s.%s: %s", 
        val_to_str(fac, short_fac, "UNKNOWN"),
        val_to_str(lev, short_lev, "UNKNOWN"), msg_str);
    } else {
      col_add_fstr(pinfo->fd, COL_INFO, "%s", msg_str);
    }
  }
  
  if (tree) {
    if (pri >= 0) {
      ti = proto_tree_add_protocol_format(tree, proto_syslog, tvb, 0,
        tvb_length(tvb), "Syslog message: %s.%s: %s",
        val_to_str(fac, short_fac, "UNKNOWN"),
        val_to_str(lev, short_lev, "UNKNOWN"), msg_str);
    } else {
      ti = proto_tree_add_protocol_format(tree, proto_syslog, tvb, 0,
        tvb_length(tvb), "Syslog message: (unknown): %s", msg_str);
    }
    syslog_tree = proto_item_add_subtree(ti, ett_syslog);
    if (pri >= 0) {
      ti = proto_tree_add_uint(syslog_tree, hf_syslog_facility, tvb, 0,
        msg_off, pri);
      ti = proto_tree_add_uint(syslog_tree, hf_syslog_level, tvb, 0,
        msg_off, pri);
    }
    proto_tree_add_uint_format(syslog_tree, hf_syslog_msg_len, tvb, msg_off,
      msg_len, msg_len, "Message (%d byte%s)", msg_len, plurality(msg_len, "", "s"));
  }
  return;
}
