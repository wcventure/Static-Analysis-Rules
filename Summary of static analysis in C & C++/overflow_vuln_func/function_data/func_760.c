static int
dnp3_al_process_object(tvbuff_t *tvb, int offset, proto_tree *robj_tree, gboolean header_only, guint16 *al_objtype)
{

  guint8        al_2bit, al_objq, al_objq_index, al_objq_code, al_ptflags, al_ctlobj_code,
                al_ctlobj_code_c, al_ctlobj_code_m, al_ctlobj_code_tc, al_ctlobj_count, al_bi_val, bitindex=0;
  guint16       al_obj, al_val16=0, al_ctlobj_stat, al_relms;
  guint32       item_num, al_val32, num_items=0, al_ptaddr=0, al_ctlobj_on, al_ctlobj_off;
  nstime_t      al_cto, al_reltime, al_abstime;
  gboolean      al_bit;
  guint         data_pos;
  gfloat        al_valflt;
  gdouble       al_valdbl;
  int           orig_offset, rangebytes=0, indexbytes=0;
  proto_item    *object_item = NULL, *point_item = NULL, *qualifier_item = NULL, *range_item = NULL;
  proto_tree    *object_tree = NULL, *point_tree, *qualifier_tree, *range_tree;
  const gchar   *ctl_code_str, *ctl_misc_str, *ctl_tc_str, *ctl_status_str;

  orig_offset = offset;

  /
  *al_objtype =
  al_obj = tvb_get_ntohs(tvb, offset);

  /
  object_item = proto_tree_add_uint_format(robj_tree, hf_dnp3_al_obj, tvb, offset, 2, al_obj,
     "Object(s): %s (0x%04x)", val_to_str(al_obj, dnp3_al_obj_vals, "Unknown Object - Abort Decoding..."), al_obj);
  object_tree = proto_item_add_subtree(object_item, ett_dnp3_al_obj);

  offset += 2;

  /
  al_objq = tvb_get_guint8(tvb, offset);
  al_objq_index = al_objq & AL_OBJQ_INDEX;
  al_objq_index = al_objq_index >> 4;
  al_objq_code = al_objq & AL_OBJQ_CODE;

  qualifier_item = proto_tree_add_text(object_tree, tvb, offset, 1, "Qualifier Field, Prefix: %s, Code: %s",
    val_to_str(al_objq_index, dnp3_al_objq_index_vals, "Unknown Index Type"),
    val_to_str(al_objq_code, dnp3_al_objq_code_vals, "Unknown Code Type"));
  qualifier_tree = proto_item_add_subtree(qualifier_item, ett_dnp3_al_obj_qualifier);
  proto_tree_add_item(qualifier_tree, hf_dnp3_al_objq_index, tvb, offset, 1, FALSE);
  proto_tree_add_item(qualifier_tree, hf_dnp3_al_objq_code, tvb, offset, 1, FALSE);

  offset += 1;

  /
  range_item = proto_tree_add_text(object_tree, tvb, offset, 0, "Number of Items: ");
  range_tree = proto_item_add_subtree(range_item, ett_dnp3_al_obj_range);

  switch (al_objq_code)
  {
    case AL_OBJQL_CODE_SSI8:           /
      num_items = ( tvb_get_guint8(tvb, offset+1) - tvb_get_guint8(tvb, offset) + 1);
      PROTO_ITEM_SET_GENERATED(range_item);
      al_ptaddr = tvb_get_guint8(tvb, offset);
      proto_tree_add_item(range_tree, hf_dnp3_al_range_start8, tvb, offset, 1, TRUE);
      proto_tree_add_item(range_tree, hf_dnp3_al_range_stop8, tvb, offset + 1, 1, TRUE);
      rangebytes = 2;
      break;
    case AL_OBJQL_CODE_SSI16:          /
      num_items = ( tvb_get_letohs(tvb, offset+2) - tvb_get_letohs(tvb, (offset)) + 1);
      PROTO_ITEM_SET_GENERATED(range_item);
      al_ptaddr = tvb_get_letohs(tvb, offset);
      proto_tree_add_item(range_tree, hf_dnp3_al_range_start16, tvb, offset, 2, TRUE);
      proto_tree_add_item(range_tree, hf_dnp3_al_range_stop16, tvb, offset + 2, 2, TRUE);
      rangebytes = 4;
      break;
    case AL_OBJQL_CODE_SSI32:          /
      num_items = ( tvb_get_letohl(tvb, offset+4) - tvb_get_letohl(tvb, offset) + 1);
      PROTO_ITEM_SET_GENERATED(range_item);
      al_ptaddr = tvb_get_letohl(tvb, offset);
      proto_tree_add_item(range_tree, hf_dnp3_al_range_start32, tvb, offset, 4, TRUE);
      proto_tree_add_item(range_tree, hf_dnp3_al_range_stop32, tvb, offset + 4, 2, TRUE);
      rangebytes = 8;
      break;
    case AL_OBJQL_CODE_AA8:            /
      num_items = 1;
      PROTO_ITEM_SET_GENERATED(range_item);
      al_ptaddr = tvb_get_guint8(tvb, offset);
      proto_tree_add_item(range_tree, hf_dnp3_al_range_abs8, tvb, offset, 1, TRUE);
      rangebytes = 1;
      break;
    case AL_OBJQL_CODE_AA16:           /
      num_items = 1;
      PROTO_ITEM_SET_GENERATED(range_item);
      al_ptaddr = tvb_get_letohs(tvb, offset);
      proto_tree_add_item(range_tree, hf_dnp3_al_range_abs16, tvb, offset, 2, TRUE);
      rangebytes = 2;
      break;
    case AL_OBJQL_CODE_AA32:           /
      num_items = 1;
      PROTO_ITEM_SET_GENERATED(range_item);
      al_ptaddr = tvb_get_letohl(tvb, offset);
      proto_tree_add_item(range_tree, hf_dnp3_al_range_abs32, tvb, offset, 4, TRUE);
      rangebytes = 4;
      break;
    case AL_OBJQL_CODE_SF8:            /
      num_items = tvb_get_guint8(tvb, offset);
      proto_tree_add_item(range_tree, hf_dnp3_al_range_quant8, tvb, offset, 1, TRUE);
      rangebytes = 1;
      proto_item_set_len(range_item, rangebytes);
      break;
    case AL_OBJQL_CODE_SF16:           /
      num_items = tvb_get_letohs(tvb, offset);
      proto_tree_add_item(range_tree, hf_dnp3_al_range_quant16, tvb, offset, 2, TRUE);
      rangebytes = 2;
      proto_item_set_len(range_item, rangebytes);
      break;
    case AL_OBJQL_CODE_SF32:           /
      num_items = tvb_get_letohl(tvb, offset);
      proto_tree_add_item(range_tree, hf_dnp3_al_range_quant32, tvb, offset, 4, TRUE);
      rangebytes = 4;
      proto_item_set_len(range_item, rangebytes);
      break;
  }
  if (num_items)
    proto_item_append_text(object_item, ", %u point%s", num_items, plurality(num_items, "", "s"));
  proto_item_append_text(range_item, "%u", num_items);

  offset += rangebytes;

  bitindex = 0; /

  /
  if (!header_only || al_objq_index > 0) {
    for (item_num = 0; item_num < num_items; item_num++)
    {
      /
      point_item = proto_tree_add_text(object_tree, tvb, offset, -1, "Point Number");
      point_tree = proto_item_add_subtree(point_item, ett_dnp3_al_obj_point);

      data_pos = offset;
      indexbytes = dnp3_al_obj_procindex(tvb, offset, al_objq_index, &al_ptaddr, point_tree);
      proto_item_append_text(point_item, " %u", al_ptaddr);
      data_pos += indexbytes;

      if (!header_only) {
        switch (al_obj)
        {

          case AL_OBJ_BI_ALL:      /
          case AL_OBJ_BIC_ALL:     /
          case AL_OBJ_2BI_ALL:     /
          case AL_OBJ_CTR_ALL:     /
          case AL_OBJ_CTRC_ALL:    /
          case AL_OBJ_AI_ALL:      /
          case AL_OBJ_AIC_ALL:     /

            offset = data_pos;
            break;

          case AL_OBJ_BI_1BIT:    /
          case AL_OBJ_BO:         /

            /
            if (bitindex > 7)
            {
              bitindex = 0;
              offset += (indexbytes + 1);
            }

            /
            al_bi_val = tvb_get_guint8(tvb, offset);
            al_bit = (al_bi_val & (1 << bitindex)) > 0;

            proto_item_append_text(point_item, ", Value: %u", al_bit);
            proto_tree_add_boolean(point_tree, hf_dnp3_al_bit, tvb, offset, 1, al_bit);
            proto_item_set_len(point_item, indexbytes + 1);

            /
            if (item_num == (num_items-1))
            {
              offset += (indexbytes + 1);
            }

            break;

          case AL_OBJ_2BI_NF:    /

            if (bitindex > 3)
            {
              bitindex = 0;
              offset += (indexbytes + 1);
            }

            /
            al_bi_val = tvb_get_guint8(tvb, offset);
            al_2bit = ((al_bi_val >> (bitindex << 1)) & 3);

            proto_item_append_text(point_item, ", Value: %u", al_2bit);
            proto_tree_add_uint(point_tree, hf_dnp3_al_2bit, tvb, offset, 1, al_2bit);
            proto_item_set_len(point_item, indexbytes + 1);

            /
            if (item_num == (num_items-1))
            {
              offset += (indexbytes + 1);
            }

            break;


          case AL_OBJ_BI_STAT:    /
          case AL_OBJ_BIC_NOTIME: /
          case AL_OBJ_BO_STAT:    /

            /
            al_ptflags = tvb_get_guint8(tvb, data_pos);

            switch (al_obj) {
              case AL_OBJ_BI_STAT:
              case AL_OBJ_BIC_NOTIME:
                dnp3_al_obj_quality(tvb, data_pos, al_ptflags, point_tree, point_item, BIN_IN);
                break;
              case AL_OBJ_BO_STAT:
                dnp3_al_obj_quality(tvb, data_pos, al_ptflags, point_tree, point_item, BIN_OUT);
                break;
            }
            data_pos += 1;

            al_bit = (al_ptflags & AL_OBJ_BI_FLAG7) > 0;
            proto_item_append_text(point_item, ", Value: %u", al_bit);

            proto_item_set_len(point_item, data_pos - offset);

            offset = data_pos;
            break;

          case AL_OBJ_2BI_STAT:    /
          case AL_OBJ_2BIC_NOTIME: /

            /
            al_ptflags = tvb_get_guint8(tvb, data_pos);
            dnp3_al_obj_quality(tvb, data_pos, al_ptflags, point_tree, point_item, BIN_IN);
            data_pos += 1;

            al_2bit = (al_ptflags >> 6) & 3;
            proto_item_append_text(point_item, ", Value: %u", al_2bit);
            proto_item_set_len(point_item, data_pos - offset);

            offset = data_pos;
            break;

          case AL_OBJ_BIC_TIME:   /

            /
            al_ptflags = tvb_get_guint8(tvb, data_pos);
            dnp3_al_obj_quality(tvb, data_pos, al_ptflags, point_tree, point_item, BIN_IN);
            data_pos += 1;


            /
            dnp3_al_get_timestamp(&al_abstime, tvb, data_pos);
            proto_tree_add_time(point_tree, hf_dnp3_al_timestamp, tvb, data_pos, 6, &al_abstime);
            data_pos += 6;

            al_bit = (al_ptflags & AL_OBJ_BI_FLAG7) >> 7; /
            proto_item_append_text(point_item, ", Value: %u, Timestamp: %s", al_bit, abs_time_to_str(&al_abstime));
            proto_item_set_len(point_item, data_pos - offset);

            offset = data_pos;
            break;

          case AL_OBJ_2BIC_TIME:   /

            /
            al_ptflags = tvb_get_guint8(tvb, data_pos);
            dnp3_al_obj_quality(tvb, (offset+indexbytes), al_ptflags, point_tree, point_item, BIN_IN);
            data_pos += 1;


            /
            dnp3_al_get_timestamp(&al_abstime, tvb, data_pos);
            proto_tree_add_time(point_tree, hf_dnp3_al_timestamp, tvb, data_pos, 6, &al_abstime);
            data_pos += 6;

            al_2bit = (al_ptflags >> 6) & 3; /
            proto_item_append_text(point_item, ", Value: %u, Timestamp: %s", al_2bit, abs_time_to_str(&al_abstime));
            proto_item_set_len(point_item, data_pos - offset);

            offset = data_pos;
            break;

          case AL_OBJ_BIC_RTIME:   /

            /
            al_ptflags = tvb_get_guint8(tvb, data_pos);
            dnp3_al_obj_quality(tvb, data_pos, al_ptflags, point_tree, point_item, BIN_IN);
            data_pos += 1;

            /
            al_relms = tvb_get_letohs(tvb, data_pos);
            al_reltime.secs = al_relms / 1000;
            al_reltime.nsecs = (al_relms % 1000) * 1000;
            /
            nstime_sum(&al_abstime, &al_cto, &al_reltime);
            proto_tree_add_time(point_tree, hf_dnp3_al_rel_timestamp, tvb, data_pos, 2, &al_reltime);
            data_pos += 2;

            al_bit = (al_ptflags & AL_OBJ_BI_FLAG7) >> 7; /
            proto_item_append_text(point_item, ", Value: %u, Timestamp: %s", al_bit, abs_time_to_str(&al_abstime));
            proto_item_set_len(point_item, data_pos - offset);

            offset = data_pos;
            break;

          case AL_OBJ_CTLOP_BLK:  /

            al_ctlobj_code = tvb_get_guint8(tvb, data_pos);
            data_pos += 1;

            /
            al_ctlobj_code_c = al_ctlobj_code & AL_OBJCTLC_CODE;
            ctl_code_str = val_to_str(al_ctlobj_code_c, dnp3_al_ctlc_code_vals, "Ctrl Code Invalid (0x%02x)");

            /
            al_ctlobj_code_m = al_ctlobj_code & AL_OBJCTLC_MISC;
            ctl_misc_str = val_to_str(al_ctlobj_code_m, dnp3_al_ctlc_misc_vals, "");

            /
            al_ctlobj_code_tc = al_ctlobj_code & AL_OBJCTLC_TC;
            ctl_tc_str = val_to_str(al_ctlobj_code_tc, dnp3_al_ctlc_tc_vals, "");

            /
            al_ctlobj_count = tvb_get_guint8(tvb, data_pos);
            data_pos += 1;

            /
            al_ctlobj_on = tvb_get_letohl(tvb, data_pos);
            data_pos += 4;

            /
            al_ctlobj_off = tvb_get_letohl(tvb, data_pos);
            data_pos += 4;

            al_ctlobj_stat = tvb_get_guint8(tvb, data_pos);
            proto_tree_add_item(point_item, hf_dnp3_al_ctrlstatus, tvb, data_pos, 1, TRUE);
            ctl_status_str = val_to_str(al_ctlobj_stat, dnp3_al_ctl_status_vals, "Invalid Status (0x%02x)");
            data_pos += 1;

            proto_item_append_text(point_item, ", Control Code: [%s,%s,%s (0x%02x)]",
                 ctl_code_str, ctl_misc_str, ctl_tc_str, al_ctlobj_code);

            proto_tree_add_text(point_tree, tvb, data_pos - 11, 11,
               "  [Count: %u] [On-Time: %u] [Off-Time: %u] [Status: %s (0x%02x)]",
                   al_ctlobj_count, al_ctlobj_on, al_ctlobj_off, ctl_status_str, al_ctlobj_stat);

            proto_item_set_len(point_item, data_pos - offset);

            offset = data_pos;
            break;

          case AL_OBJ_AO_32OPB:   /
          case AL_OBJ_AO_16OPB:   /
          case AL_OBJ_AO_FLTOPB:  /
          case AL_OBJ_AO_DBLOPB:  /

            switch (al_obj)
            {
              case AL_OBJ_AO_32OPB:
                al_val32 = tvb_get_letohl(tvb, data_pos);
                proto_item_append_text(point_item, ", Value: %u", al_val32);
                proto_tree_add_item(point_tree, hf_dnp3_al_anaout32, tvb, data_pos, 4, TRUE);
                data_pos += 4;
                break;
              case AL_OBJ_AO_16OPB:
                al_val32 = tvb_get_letohs(tvb, data_pos);
                proto_item_append_text(point_item, ", Value: %u", al_val32);
                proto_tree_add_item(point_tree, hf_dnp3_al_anaout16, tvb, data_pos, 2, TRUE);
                data_pos += 2;
                break;
              case AL_OBJ_AO_FLTOPB:
                al_valflt = tvb_get_letohieee_float(tvb, data_pos);
                proto_item_append_text(point_item, ", Value: %g", al_valflt);
                proto_tree_add_item(point_tree, hf_dnp3_al_anaoutflt, tvb, data_pos, 4, TRUE);
                data_pos += 4;
                break;
              case AL_OBJ_AO_DBLOPB:
                al_valdbl = tvb_get_letohieee_double(tvb, data_pos);
                proto_item_append_text(point_item, ", Value: %lg", al_valdbl);
                proto_tree_add_item(point_tree, hf_dnp3_al_anaoutdbl, tvb, data_pos, 8, TRUE);
                data_pos += 8;
                break;
            }

            /
            al_ctlobj_stat = tvb_get_guint8(tvb, data_pos);
            ctl_status_str = val_to_str(al_ctlobj_stat, dnp3_al_ctl_status_vals, "Invalid Status (0x%02x)");
            proto_item_append_text(point_item, " [Status: %s (0x%02x)]", ctl_status_str, al_ctlobj_stat);
            proto_tree_add_item(point_tree, hf_dnp3_al_ctrlstatus, tvb, data_pos, 1, TRUE);
            data_pos += 1;

            proto_item_set_len(point_item, data_pos - offset);

            offset = data_pos;
            break;

          case AL_OBJ_CTR_32:     /
          case AL_OBJ_CTR_16:     /
          case AL_OBJ_DCTR_32:    /
          case AL_OBJ_DCTR_16:    /
          case AL_OBJ_CTR_32NF:   /
          case AL_OBJ_CTR_16NF:   /
          case AL_OBJ_DCTR_32NF:  /
          case AL_OBJ_DCTR_16NF:  /
          case AL_OBJ_FCTR_32:    /
          case AL_OBJ_FCTR_16:    /
          case AL_OBJ_FDCTR_32:	  /
          case AL_OBJ_FDCTR_16:   /
          case AL_OBJ_FCTR_32T:   /
          case AL_OBJ_FCTR_16T:   /
          case AL_OBJ_FDCTR_32T:  /
          case AL_OBJ_FDCTR_16T:  /
          case AL_OBJ_FCTR_32NF:  /
          case AL_OBJ_FCTR_16NF:  /
          case AL_OBJ_FDCTR_32NF: /
          case AL_OBJ_FDCTR_16NF: /
          case AL_OBJ_CTRC_32:    /
          case AL_OBJ_CTRC_16:    /
          case AL_OBJ_DCTRC_32:   /
          case AL_OBJ_DCTRC_16:   /
          case AL_OBJ_CTRC_32T:   /
          case AL_OBJ_CTRC_16T:   /
          case AL_OBJ_DCTRC_32T:  /
          case AL_OBJ_DCTRC_16T:  /
          case AL_OBJ_FCTRC_32:   /
          case AL_OBJ_FCTRC_16:   /
          case AL_OBJ_FDCTRC_32:  /
          case AL_OBJ_FDCTRC_16:  /
          case AL_OBJ_FCTRC_32T:  /
          case AL_OBJ_FCTRC_16T:  /
          case AL_OBJ_FDCTRC_32T: /
          case AL_OBJ_FDCTRC_16T: /

            /
            switch (al_obj)
            {
              case AL_OBJ_CTR_32NF:
              case AL_OBJ_CTR_16NF:
              case AL_OBJ_DCTR_32NF:
              case AL_OBJ_DCTR_16NF:
              case AL_OBJ_FCTR_32NF:
              case AL_OBJ_FCTR_16NF:
              case AL_OBJ_FDCTR_32NF:
              case AL_OBJ_FDCTR_16NF:
                break;

              default:
                al_ptflags = tvb_get_guint8(tvb, data_pos);
                dnp3_al_obj_quality(tvb, data_pos, al_ptflags, point_tree, point_item, COUNTER);
                data_pos += 1;
                break;
            }

            /
            switch (al_obj)
            {
              case AL_OBJ_CTR_32:
              case AL_OBJ_DCTR_32:
              case AL_OBJ_CTR_32NF:
              case AL_OBJ_DCTR_32NF:
              case AL_OBJ_FCTR_32:
              case AL_OBJ_FDCTR_32:
              case AL_OBJ_FCTR_32T:
              case AL_OBJ_FDCTR_32T:
              case AL_OBJ_FCTR_32NF:
              case AL_OBJ_FDCTR_32NF:
              case AL_OBJ_CTRC_32:
              case AL_OBJ_DCTRC_32:
              case AL_OBJ_CTRC_32T:
              case AL_OBJ_DCTRC_32T:
              case AL_OBJ_FCTRC_32:
              case AL_OBJ_FDCTRC_32:
              case AL_OBJ_FCTRC_32T:
              case AL_OBJ_FDCTRC_32T:

                al_val32 = tvb_get_letohl(tvb, data_pos);
                proto_item_append_text(point_item, ", Count: %u", al_val32);
                proto_tree_add_item(point_tree, hf_dnp3_al_cnt32, tvb, data_pos, 4, TRUE);
                data_pos += 4;
                break;

              case AL_OBJ_CTR_16:
              case AL_OBJ_DCTR_16:
              case AL_OBJ_CTR_16NF:
              case AL_OBJ_DCTR_16NF:
              case AL_OBJ_FCTR_16:
              case AL_OBJ_FDCTR_16:
              case AL_OBJ_FCTR_16T:
              case AL_OBJ_FDCTR_16T:
              case AL_OBJ_FCTR_16NF:
              case AL_OBJ_FDCTR_16NF:
              case AL_OBJ_CTRC_16:
              case AL_OBJ_DCTRC_16:
              case AL_OBJ_CTRC_16T:
              case AL_OBJ_DCTRC_16T:
              case AL_OBJ_FCTRC_16:
              case AL_OBJ_FDCTRC_16:
              case AL_OBJ_FCTRC_16T:
              case AL_OBJ_FDCTRC_16T:

                al_val16 = tvb_get_letohs(tvb, data_pos);
                proto_item_append_text(point_item, ", Count: %u", al_val16);
                proto_tree_add_item(point_tree, hf_dnp3_al_cnt16, tvb, data_pos, 2, TRUE);
                data_pos += 2;
                break;
            }

            /
            switch (al_obj)
            {
              case AL_OBJ_FCTR_32T:
              case AL_OBJ_FCTR_16T:
              case AL_OBJ_FDCTR_32T:
              case AL_OBJ_FDCTR_16T:
              case AL_OBJ_CTRC_32T:
              case AL_OBJ_CTRC_16T:
              case AL_OBJ_DCTRC_32T:
              case AL_OBJ_DCTRC_16T:
              case AL_OBJ_FCTRC_32T:
              case AL_OBJ_FCTRC_16T:
              case AL_OBJ_FDCTRC_32T:
              case AL_OBJ_FDCTRC_16T:
                dnp3_al_get_timestamp(&al_abstime, tvb, data_pos);
                proto_item_append_text(point_item, ", Timestamp: %s", abs_time_to_str(&al_abstime));
                proto_tree_add_time(point_tree, hf_dnp3_al_timestamp, tvb, data_pos, 6, &al_abstime);
                data_pos += 6;
                break;
            }

            proto_item_set_len(point_item, data_pos - offset);
            offset = data_pos;

            break;

          case AL_OBJ_AI_32:        /
          case AL_OBJ_AI_16:        /
          case AL_OBJ_AI_32NF:      /
          case AL_OBJ_AI_16NF:      /
          case AL_OBJ_AI_FLT:       /
          case AL_OBJ_AI_DBL:       /
          case AL_OBJ_AIF_FLT:      /
          case AL_OBJ_AIF_DBL:      /
          case AL_OBJ_AIC_32NT:     /
          case AL_OBJ_AIC_16NT:     /
          case AL_OBJ_AIC_32T:      /
          case AL_OBJ_AIC_16T:      /
          case AL_OBJ_AIC_FLTNT:    /
          case AL_OBJ_AIC_DBLNT:    /
          case AL_OBJ_AIC_FLTT:     /
          case AL_OBJ_AIC_DBLT:     /
          case AL_OBJ_AIFC_FLTNT:   /
          case AL_OBJ_AIFC_DBLNT:   /
          case AL_OBJ_AIFC_FLTT:    /
          case AL_OBJ_AIFC_DBLT:    /

            /
            switch (al_obj)
            {
              case AL_OBJ_AI_32NF:
              case AL_OBJ_AI_16NF:
                break;

              default:
                al_ptflags = tvb_get_guint8(tvb, data_pos);
                dnp3_al_obj_quality(tvb, data_pos, al_ptflags, point_tree, point_item, ANA_IN);
                data_pos += 1;
                break;
            }

            switch (al_obj)
            {
              case AL_OBJ_AI_32:
              case AL_OBJ_AI_32NF:
              case AL_OBJ_AIC_32NT:
              case AL_OBJ_AIC_32T:

                al_val32 = tvb_get_letohl(tvb, data_pos);
                proto_item_append_text(point_item, ", Value: %u", al_val32);
                proto_tree_add_item(point_tree, hf_dnp3_al_ana32, tvb, data_pos, 4, TRUE);
                data_pos += 4;
                break;

              case AL_OBJ_AI_16:
              case AL_OBJ_AI_16NF:
              case AL_OBJ_AIC_16NT:
              case AL_OBJ_AIC_16T:

                al_val16 = tvb_get_letohs(tvb, data_pos);
                proto_item_append_text(point_item, ", Value: %u", al_val16);
                proto_tree_add_item(point_tree, hf_dnp3_al_ana16, tvb, data_pos, 2, TRUE);
                data_pos += 2;
                break;

              case AL_OBJ_AI_FLT:
              case AL_OBJ_AIF_FLT:
              case AL_OBJ_AIC_FLTNT:
              case AL_OBJ_AIC_FLTT:
              case AL_OBJ_AIFC_FLTNT:
              case AL_OBJ_AIFC_FLTT:

                al_valflt = tvb_get_letohieee_float(tvb, data_pos);
                proto_item_append_text(point_item, ", Value: %g", al_valflt);
                proto_tree_add_item(point_tree, hf_dnp3_al_anaflt, tvb, data_pos, 4, TRUE);
                data_pos += 4;
                break;

              case AL_OBJ_AI_DBL:
              case AL_OBJ_AIF_DBL:
              case AL_OBJ_AIC_DBLNT:
              case AL_OBJ_AIC_DBLT:
              case AL_OBJ_AIFC_DBLNT:
              case AL_OBJ_AIFC_DBLT:

                al_valdbl = tvb_get_letohieee_double(tvb, data_pos);
                proto_item_append_text(point_item, ", Value: %lg", al_valdbl);
                proto_tree_add_item(point_tree, hf_dnp3_al_anadbl, tvb, data_pos, 8, TRUE);
                data_pos += 8;
                break;
            }

            /
            switch (al_obj)
            {
              case AL_OBJ_AIC_32T:
              case AL_OBJ_AIC_16T:
              case AL_OBJ_AIC_FLTT:
              case AL_OBJ_AIC_DBLT:
              case AL_OBJ_AIFC_FLTT:
              case AL_OBJ_AIFC_DBLT:
                dnp3_al_get_timestamp(&al_abstime, tvb, data_pos);
                proto_item_append_text(point_item, ", Timestamp: %s", abs_time_to_str(&al_abstime));
                proto_tree_add_time(point_tree, hf_dnp3_al_timestamp, tvb, data_pos, 6, &al_abstime);
                data_pos += 6;
                break;
            }

            proto_item_set_len(point_item, data_pos - offset);

            offset = data_pos;
            break;

          case AL_OBJ_AO_32:     /
          case AL_OBJ_AO_16:     /
          case AL_OBJ_AO_FLT:    /
          case AL_OBJ_AO_DBL:    /

            /
            al_ptflags = tvb_get_guint8(tvb, data_pos);
            dnp3_al_obj_quality(tvb, data_pos, al_ptflags, point_tree, point_item, ANA_OUT);
            data_pos += 1;

            switch (al_obj)
            {
              case AL_OBJ_AO_32:     /

                al_val32 = tvb_get_letohl(tvb, data_pos);
                proto_item_append_text(point_item, ", Value: %u", al_val32);
                proto_tree_add_item(point_tree, hf_dnp3_al_anaout32, tvb, data_pos, 4, TRUE);
                data_pos += 4;
                break;

              case AL_OBJ_AO_16:     /

                al_val16 = tvb_get_letohs(tvb, data_pos);
                proto_item_append_text(point_item, ", Value: %u", al_val16);
                proto_tree_add_item(point_tree, hf_dnp3_al_anaout16, tvb, data_pos, 2, TRUE);
                data_pos += 2;
                break;

              case AL_OBJ_AO_FLT:     /

                al_valflt = tvb_get_letohieee_float(tvb, data_pos);
                proto_item_append_text(point_item, ", Value: %g", al_valflt);
                proto_tree_add_item(point_tree, hf_dnp3_al_anaoutflt, tvb, data_pos, 4, TRUE);
                data_pos += 4;
                break;

              case AL_OBJ_AO_DBL:     /

                al_valdbl = tvb_get_letohieee_double(tvb, data_pos);
                proto_item_append_text(point_item, ", Value: %lg", al_valdbl);
                proto_tree_add_item(point_tree, hf_dnp3_al_anaoutdbl, tvb, data_pos, 8, TRUE);
                data_pos += 8;
                break;
            }

            proto_item_set_len(point_item, data_pos - offset);
            offset = data_pos;

            break;

          case AL_OBJ_TD:    /
          case AL_OBJ_TDCTO: /

            dnp3_al_get_timestamp(&al_cto, tvb, data_pos);
            proto_tree_add_time(object_tree, hf_dnp3_al_timestamp, tvb, data_pos, 6, &al_cto);
            data_pos += 6;
            proto_item_set_len(point_item, data_pos - offset);

            offset = data_pos;
            break;

          case AL_OBJ_TDELAYF: /

            al_val16 = tvb_get_letohs(tvb, data_pos);
            proto_tree_add_text(object_tree, tvb, data_pos, 2, "Time Delay: %u ms", al_val16);
            data_pos += 2;
            proto_item_set_len(point_item, data_pos - offset);

            offset = data_pos;
            break;

          case AL_OBJ_CLASS0:  /
          case AL_OBJ_CLASS1:
          case AL_OBJ_CLASS2:
          case AL_OBJ_CLASS3:

            /
            offset = data_pos;
            break;

          case AL_OBJ_IIN:     /

            /
            proto_tree_add_text(object_tree, tvb, data_pos, 1, "Value: %u", tvb_get_guint8(tvb, data_pos));
            data_pos += 1;
            proto_item_set_len(point_item, data_pos - offset);

            offset = data_pos;
            break;

          default:             /

            proto_tree_add_text(object_tree, tvb, offset, tvb_reported_length_remaining(tvb, offset),
              "Unknown Data Chunk, %u Bytes", tvb_reported_length_remaining(tvb, offset));
            offset = tvb_length(tvb); /
            break;
        }
        /
        bitindex++;

        /
        al_ptaddr++;
      }
    }
  }
  proto_item_set_len(object_item, offset - orig_offset);

  return offset;
}
