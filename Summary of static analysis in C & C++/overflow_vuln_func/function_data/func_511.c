static gboolean
dissect_rtps(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree)
{
  proto_item       *ti;
  proto_tree       *rtps_tree=NULL;
  gint             offset = 0;
  gint             appKind;
  proto_tree       *rtps_submessage_tree;
  guint8           submessageId;
  guint8           flags;
  gboolean         little_endian;
  int              next_submsg;
  int              count_msg_type[11];
  char             buff[200], buff_tmp[30];/

  /

  /
  if (!tvb_bytes_exist(tvb, offset, 4)) return FALSE;
  if (tvb_get_guint8(tvb,offset++) != 'R') return FALSE;
  if (tvb_get_guint8(tvb,offset++) != 'T') return FALSE;
  if (tvb_get_guint8(tvb,offset++) != 'P') return FALSE;
  if (tvb_get_guint8(tvb,offset++) != 'S') return FALSE;

  /
  if (check_col(pinfo->cinfo, COL_PROTOCOL))
   col_set_str(pinfo->cinfo, COL_PROTOCOL, "RTPS");

  if (check_col(pinfo->cinfo, COL_INFO))
   col_clear(pinfo->cinfo, COL_INFO);

  memset(count_msg_type, 0, sizeof(count_msg_type));

  if (tree) {

   /
   ti = proto_tree_add_item(tree, proto_rtps, tvb, 0, -1, FALSE);
   rtps_tree = proto_item_add_subtree(ti, ett_rtps);

   /
   proto_tree_add_text(rtps_tree, tvb, offset, 2,
                       "Protocol  RTPS, version %s",
                       protocol_version_to_string(offset, tvb, buff));
   offset +=2;

   /
   proto_tree_add_text(rtps_tree, tvb, offset, 2,
                       "VendorId: %s",
                       vendor_id_to_string(offset, tvb, buff));
   offset +=2;

   /
   proto_tree_add_text(rtps_tree, tvb, offset, 4,
                       "HostId:   %s",
                       host_id_to_string(offset,tvb,buff));
   offset +=4;

   /
   proto_tree_add_text(rtps_tree, tvb, offset, 4,
                       "App ID:   %s",
                       app_id_to_string(offset, tvb, buff));

  }

  /
  offset=16;

  while (tvb_reported_length_remaining(tvb, offset) > 0) {
    submessageId = tvb_get_guint8(tvb, offset);
    if (submessageId & 0x80) {
      ti = proto_tree_add_text(tree, tvb, offset, -1, "Submessage: %s",
                               val_to_str(submessageId, submessage_id_vals,
                                          "Vendor-specific (0x%02X)"));
    } else {
      ti = proto_tree_add_text(tree, tvb, offset, -1, "Submessage: %s",
                               val_to_str(submessageId, submessage_id_vals,
                                          "Unknown (0x%02X)"));
    }
    rtps_submessage_tree = proto_item_add_subtree(ti, ett_rtps_submessage);
    if (submessageId & 0x80) {
      proto_tree_add_uint_format(rtps_submessage_tree, hf_rtps_submessage_id,
                                 tvb, offset, 1, submessageId,
                                 "Submessage Id: Vendor-specific (0x%02x)",
                                 submessageId);
    } else {
      proto_tree_add_uint(rtps_submessage_tree, hf_rtps_submessage_id,
                          tvb, offset, 1, submessageId);
    }

    flags = tvb_get_guint8(tvb, offset + 1);
    /
    if ((flags & FLAG_E) != 0)      little_endian = TRUE;
      else                          little_endian = FALSE;

    next_submsg  = get_guint16(tvb, offset + 2, little_endian);
    proto_item_set_len(ti, next_submsg);

    switch (submessageId)
    {
      case PAD:
        if (tree)
          dissect_PAD(tvb, offset + 1, flags, next_submsg,
                      rtps_submessage_tree);
        count_msg_type[0]++;
        break;
      case VAR:
        if (tree)
          dissect_VAR(tvb, offset + 1, flags, little_endian, next_submsg,
                      rtps_submessage_tree);
        count_msg_type[1]++;
        break;
      case ISSUE:
        if (tree)
          dissect_ISSUE(tvb, offset + 1, flags, little_endian, next_submsg,
                        rtps_submessage_tree);
        count_msg_type[2]++;
        break;
      case ACK:
        if (tree)
          dissect_ACK(tvb, offset + 1, flags, little_endian, next_submsg,
                      rtps_submessage_tree);
        count_msg_type[3]++;
        break;
      case HEARTBEAT:
        if (tree)
          dissect_HEARTBEAT(tvb, offset + 1, flags, little_endian, next_submsg,
                            rtps_submessage_tree);
        count_msg_type[4]++;
        break;
      case GAP:
        if (tree)
          dissect_GAP(tvb, offset + 1, flags, little_endian, next_submsg,
                      rtps_submessage_tree);
        count_msg_type[5]++;
        break;
      case INFO_TS:
        if (tree)
          dissect_INFO_TS(tvb, offset + 1, flags, little_endian, next_submsg,
                          rtps_submessage_tree);
        count_msg_type[6]++;
        break;
      case INFO_SRC:
        if (tree)
          dissect_INFO_SRC(tvb, offset + 1, flags, little_endian, next_submsg,
                           rtps_submessage_tree);
        count_msg_type[7]++;
        break;
      case INFO_REPLY:
        if (tree)
          dissect_INFO_REPLY(tvb, offset + 1, flags, little_endian, next_submsg,
                             rtps_submessage_tree);
        count_msg_type[8]++;
        break;
      case INFO_DST:
        if (tree)
          dissect_INFO_DST(tvb, offset + 1, flags, next_submsg,
                           rtps_submessage_tree);
        count_msg_type[9]++;
        break;
      default:
        if (tree) {
          proto_tree_add_uint(rtps_submessage_tree, hf_rtps_submessage_flags,
                              tvb, offset + 1, 1, flags);
          proto_tree_add_uint(rtps_submessage_tree, hf_rtps_octets_to_next_header,
                              tvb, offset + 2, 2, next_submsg);
        }
        break;
     }

     /
     offset += next_submsg+4;

  }

  /

  if (check_col(pinfo->cinfo, COL_INFO))
  {
    appKind = tvb_get_guint8(tvb, APP_KIND_BYTE);

    if (appKind == MANAGEDAPPLICATION ) {sprintf(buff,"App: ");}
    if (appKind == MANAGER)             {sprintf(buff,"Man: ");}
    if (appKind == AID_UNKNOWN)         {sprintf(buff,"Unknown:");}

    if (appKind != MANAGEDAPPLICATION  && appKind != MANAGER &&
        appKind != AID_UNKNOWN)         {sprintf(buff,"ERROR in APP type");}

   /
   if (count_msg_type[0]>0) {
       sprintf(buff_tmp,"PAD(%d) ",count_msg_type[0]);
       strcat(buff,buff_tmp);
   }

   if (count_msg_type[1]>0) {
       sprintf(buff_tmp,"VAR(%d) ",count_msg_type[1]);
       strcat(buff,buff_tmp);
   }

   if (count_msg_type[2]>0) {
       sprintf(buff_tmp,"ISSUE(%d) ",count_msg_type[2]);
       strcat(buff,buff_tmp);
   }

   if (count_msg_type[3]>0) {
       sprintf(buff_tmp,"ACK(%d) ",count_msg_type[3]);
       strcat(buff,buff_tmp);
   }

   if (count_msg_type[4]>0) {
       sprintf(buff_tmp,"HEARTBEAT(%d) ",count_msg_type[4]);
       strcat(buff,buff_tmp);
   }

   if (count_msg_type[5]>0) {
       sprintf(buff_tmp,"GAP(%d) ",count_msg_type[5]);
       strcat(buff,buff_tmp);
   }

   if (count_msg_type[6]>0) {
       sprintf(buff_tmp,"INFO_TS(%d) ",count_msg_type[6]);
       strcat(buff,buff_tmp);
   }

   if (count_msg_type[7]>0) {
       sprintf(buff_tmp, "INFO_SRC(%d) ",count_msg_type[7]);
       strcat(buff,buff_tmp);
   }

   if (count_msg_type[8]>0) {
       sprintf(buff_tmp,"INFO_REPLY(%d) ",count_msg_type[8]);
       strcat(buff,buff_tmp);
   }

   if (count_msg_type[9]>0) {
       sprintf(buff_tmp,"INFO_DST(%d) ",count_msg_type[9]);
       strcat(buff,buff_tmp);
   }

   if (count_msg_type[10]>0) {
       sprintf(buff_tmp,"vendor specific(%d) ",count_msg_type[10]);
       strcat(buff,buff_tmp);
   }

   col_add_fstr(pinfo->cinfo, COL_INFO, buff);

  }


  return TRUE;

}  /
