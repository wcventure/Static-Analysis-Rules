static int
dissect_dnp3_al(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree)
{
  guint8        al_ctl, al_seq, al_func, al_class = 0, i;
  guint16       bytes, obj_type;
  gboolean      al_fir, al_fin, al_con;
  guint         data_len = 0, offset = 0;
  proto_item   *ti = NULL, *tc, *t_robj;
  proto_tree   *al_tree = NULL, *field_tree = NULL, *robj_tree = NULL;
  const gchar  *func_code_str;

  data_len = tvb_length(tvb);

  /
  al_ctl = tvb_get_guint8(tvb, offset);
  al_seq = al_ctl & DNP3_AL_SEQ;
  al_fir = al_ctl & DNP3_AL_FIR;
  al_fin = al_ctl & DNP3_AL_FIN;
  al_con = al_ctl & DNP3_AL_CON;
  al_func = tvb_get_guint8(tvb, (offset+1));
  func_code_str = val_to_str(al_func, dnp3_al_func_vals, "Unknown function (0x%02x)");

  if (check_col(pinfo->cinfo, COL_INFO))
    col_append_sep_fstr(pinfo->cinfo, COL_INFO, NULL, "%s", func_code_str);
    col_set_fence(pinfo->cinfo, COL_INFO);

  /
  ti = proto_tree_add_text(tree, tvb, offset, data_len, "Application Layer: (");
  if (al_ctl & DNP3_AL_FIR)  proto_item_append_text(ti, "FIR, ");
  if (al_ctl & DNP3_AL_FIN)  proto_item_append_text(ti, "FIN, ");
  if (al_ctl & DNP3_AL_CON)  proto_item_append_text(ti, "CON, ");
  proto_item_append_text(ti, "Sequence %u, %s)", al_seq, func_code_str);

  /
  al_tree = proto_item_add_subtree(ti, ett_dnp3_al);

  /
  tc = proto_tree_add_uint_format(al_tree, hf_dnp3_al_ctl, tvb, offset, 1, al_ctl,
      "Control: 0x%02x (", al_ctl);
  if (al_ctl & DNP3_AL_FIR)  proto_item_append_text(tc, "FIR, ");
  if (al_ctl & DNP3_AL_FIN)  proto_item_append_text(tc, "FIN, ");
  if (al_ctl & DNP3_AL_CON)  proto_item_append_text(tc, "CON, ");
  proto_item_append_text(tc, "Sequence %u)", al_seq);

  field_tree = proto_item_add_subtree(tc, ett_dnp3_al_ctl);
  proto_tree_add_boolean(field_tree, hf_dnp3_al_fir, tvb, offset, 1, al_ctl);
  proto_tree_add_boolean(field_tree, hf_dnp3_al_fin, tvb, offset, 1, al_ctl);
  proto_tree_add_boolean(field_tree, hf_dnp3_al_con, tvb, offset, 1, al_ctl);
  proto_tree_add_item(field_tree, hf_dnp3_al_seq, tvb, offset, 1, FALSE);
  offset += 1;

  /

  /
  proto_tree_add_uint_format(al_tree, hf_dnp3_al_func, tvb, offset, 1, al_func,
    "Function Code: %s (0x%02x)", func_code_str, al_func);
  offset += 1;

  switch (al_func) {

  case AL_FUNC_READ:     /

  /
  t_robj = proto_tree_add_text(al_tree, tvb, offset, -1, "READ Request Data Objects");
  robj_tree = proto_item_add_subtree(t_robj, ett_dnp3_al_objdet);

  /
  while (offset <= (data_len-2))  {  /
    offset = dnp3_al_process_object(tvb, offset, robj_tree, TRUE, &obj_type);

    /
    switch(obj_type) {
      case AL_OBJ_CLASS0:
      case AL_OBJ_CLASS1:
      case AL_OBJ_CLASS2:
      case AL_OBJ_CLASS3:
        al_class |= (1 << ((obj_type & 0x0f) - 1));
        break;
      default:
        break;
    }
  }

  /
  if (check_col(pinfo->cinfo, COL_INFO) && (al_class > 0)) {
    col_append_sep_fstr(pinfo->cinfo, COL_INFO, NULL, "Class ");
    for (i = 0; i < 4; i++) {
      if (al_class & (1 << i)) {
        col_append_fstr(pinfo->cinfo, COL_INFO, "%u", i);
      }
    }
  }

  break;

  case AL_FUNC_WRITE:     /

  /
  t_robj = proto_tree_add_text(al_tree, tvb, offset, -1, "WRITE Request Data Objects");
  robj_tree = proto_item_add_subtree(t_robj, ett_dnp3_al_objdet);

  /
  while (offset <= (data_len-2))  {  /
    offset = dnp3_al_process_object(tvb, offset, robj_tree, FALSE, &obj_type);
  }

  break;

  case AL_FUNC_SELECT:     /

  /
  t_robj = proto_tree_add_text(al_tree, tvb, offset, -1, "SELECT Request Data Objects");
  robj_tree = proto_item_add_subtree(t_robj, ett_dnp3_al_objdet);

  /
  while (offset <= (data_len-2))  {  /
    offset = dnp3_al_process_object(tvb, offset, robj_tree, FALSE, &obj_type);
  }

  break;

  case AL_FUNC_OPERATE:    /
             /

  /
  t_robj = proto_tree_add_text(al_tree, tvb, offset, -1, "OPERATE Request Data Objects");
  robj_tree = proto_item_add_subtree(t_robj, ett_dnp3_al_objdet);

  /
  while (offset <= (data_len-2))  {  /
    offset = dnp3_al_process_object(tvb, offset, robj_tree, FALSE, &obj_type);
  }

  break;

  case AL_FUNC_DIROP:     /
            /

  /
  t_robj = proto_tree_add_text(al_tree, tvb, offset, -1, "DIRECT OPERATE Request Data Objects");
  robj_tree = proto_item_add_subtree(t_robj, ett_dnp3_al_objdet);

  /
  while (offset <= (data_len-2))  {  /
    offset = dnp3_al_process_object(tvb, offset, robj_tree, FALSE, &obj_type);
  }

  break;

  case AL_FUNC_ENSPMSG:   /

  /
  t_robj = proto_tree_add_text(al_tree, tvb, offset, -1, "Enable Spontaneous Msg's Data Objects");
  robj_tree = proto_item_add_subtree(t_robj, ett_dnp3_al_objdet);

  /
  while (offset <= (data_len-2))  {  /
    offset = dnp3_al_process_object(tvb, offset, robj_tree, FALSE, &obj_type);
  }

  break;

  case AL_FUNC_DELAYMST:  /

  break;

  case AL_FUNC_RESPON:   /
  case AL_FUNC_UNSOLI:   /

  /
  dnp3_al_process_iin(tvb, offset, al_tree);
  offset += 2;

  /
  bytes = tvb_reported_length_remaining(tvb, offset);
  if (bytes > 0)
  {
    /
    t_robj = proto_tree_add_text(al_tree, tvb, offset, -1,"RESPONSE Data Objects");
    robj_tree = proto_item_add_subtree(t_robj, ett_dnp3_al_objdet);

    /
    while (offset <= (data_len-2)) {  /
      offset = dnp3_al_process_object(tvb, offset, robj_tree, FALSE, &obj_type);
    }

    break;
  }

  default:    /

  break;
  }

  return 0;
}
