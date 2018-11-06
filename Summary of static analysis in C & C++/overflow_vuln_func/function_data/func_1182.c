static gint dissect_dmp_envelope (tvbuff_t *tvb, packet_info *pinfo,
                                  proto_tree *dmp_tree, gint offset)
{
  proto_tree *envelope_tree = NULL;
  proto_tree *field_tree = NULL;
  proto_item *en = NULL, *tf = NULL, *vf = NULL;
  proto_item *hidden_item;
  guint8      envelope, time_diff;
  guint16     subm_time, no_rec, value16;
  gint32      secs = 0;
  gchar      *env_flags = NULL;
  guint       prev_rec_no = 0;
  gint        boffset = offset, i;
  gboolean    using_short_id = FALSE;

  en = proto_tree_add_item (dmp_tree, hf_envelope, tvb, offset, 10, ENC_NA);
  envelope_tree = proto_item_add_subtree (en, ett_envelope);

  envelope = tvb_get_guint8 (tvb, offset);
  dmp.prot_id = (envelope & 0xF8) >> 3;
  dmp.version = (envelope & 0x07) + 1;

  /
  tf = proto_tree_add_uint_format (envelope_tree, hf_envelope_version,
                                   tvb, offset, 1, dmp.version,
                                   "Protocol Version: %d", dmp.version);

  field_tree = proto_item_add_subtree (tf, ett_envelope_version);
  vf = proto_tree_add_item (field_tree, hf_envelope_protocol_id, tvb, offset, 1, ENC_BIG_ENDIAN);
  if (dmp.prot_id == PROT_NAT) {
    proto_item_append_text (vf, " (national version of DMP)");
    proto_item_append_text (tf, " (national)");
  } else if (dmp.prot_id == PROT_DMP) {
    proto_item_append_text (vf, " (correct)");
  } else {
    proto_item_append_text (vf, " (incorrect, should be 0x1d)");
  }
  vf = proto_tree_add_item (field_tree, hf_envelope_version_value, tvb, offset, 1, ENC_BIG_ENDIAN);
  offset += 1;

  if (dmp.version > DMP_VERSION_2) {
    /
    proto_item_append_text (vf, " (unsupported)");
    proto_item_append_text (tf, " (unsupported)");
    expert_add_info_format (pinfo, vf, PI_UNDECODED, PI_ERROR,
                            "Unsupported DMP Version: %d", dmp.version);
    return offset;
  }

  envelope = tvb_get_guint8 (tvb, offset);
  dmp.addr_enc = ((envelope & 0x10) >> 4);
  dmp.checksum = ((envelope & 0x08) >> 3);
  dmp.msg_type = (envelope & 0x07);

  if (dmp.msg_type != ACK) {
    /
    tf = proto_tree_add_uint_format (envelope_tree, hf_envelope_hop_count,
                                     tvb, offset, 1, envelope,
                                     "Hop Count: %d", (envelope & 0xE0) >> 5);
    field_tree = proto_item_add_subtree (tf, ett_envelope_hop_count);
    proto_tree_add_item (field_tree, hf_envelope_hop_count_value, tvb, offset, 1, ENC_BIG_ENDIAN);
  } else {
    if (dmp.version >= DMP_VERSION_2) {
      /
      dmp.extensions = (envelope & 0x80);
      tf = proto_tree_add_boolean_format (envelope_tree, hf_envelope_extensions,
                                          tvb, offset, 1, envelope,
                                          "Extensions: %s",
                                          (envelope & 0x80) ? "Present" : "Absent");
      field_tree = proto_item_add_subtree (tf, ett_envelope_extensions);
      proto_tree_add_item (field_tree, hf_envelope_extensions, tvb, offset, 1, ENC_BIG_ENDIAN);
    }

    /
    dmp.ack_rec_present = (envelope & 0x20);
    tf = proto_tree_add_boolean_format (envelope_tree,hf_envelope_rec_present,
                                        tvb, offset, 1, envelope,
                                        "Recipient Present: %s",
                                        (envelope & 0x20) ? "Present" : "Absent");
    field_tree = proto_item_add_subtree (tf, ett_envelope_rec_present);
    proto_tree_add_item (field_tree, hf_envelope_rec_present, tvb, offset, 1, ENC_BIG_ENDIAN);
  }

  /
  tf = proto_tree_add_boolean_format (envelope_tree, hf_envelope_addr_enc,
                                      tvb, offset, 1, envelope,
                                      "Address Encoding: %s",
                                      (envelope & 0x10) ?
                                      addr_enc.true_string :
                                      addr_enc.false_string);
  field_tree = proto_item_add_subtree (tf, ett_envelope_addr_enc);
  proto_tree_add_item (field_tree, hf_envelope_addr_enc, tvb, offset, 1, ENC_BIG_ENDIAN);

  /
  tf = proto_tree_add_boolean_format (envelope_tree, hf_envelope_checksum,
                                      tvb, offset, 1, envelope,
                                      "Checksum: %s",
                                      (envelope & 0x08) ? "Used" : "Not used");
  field_tree = proto_item_add_subtree (tf, ett_envelope_checksum);
  proto_tree_add_item (field_tree, hf_envelope_checksum, tvb, offset, 1, ENC_BIG_ENDIAN);

  /
  tf = proto_tree_add_uint_format (envelope_tree, hf_envelope_type,
                                   tvb, offset, 1, envelope,
                                   "Content Type: %s (%d)",
                                   val_to_str (envelope & 0x07,
                                               type_vals, "Unknown"),
                                   envelope & 0x07);
  field_tree = proto_item_add_subtree (tf, ett_envelope_cont_type);
  proto_tree_add_item (field_tree, hf_envelope_type, tvb, offset, 1, ENC_BIG_ENDIAN);

  proto_item_append_text (en, ", Checksum %s", (envelope >> 3) & 0x01 ? "Used" : "Not used");
  offset += 1;

  if (dmp.msg_type >= ACK) {
    proto_item_set_len (en, offset - boffset);
    return offset;
  }

  if (dmp.version >= DMP_VERSION_2) {
    envelope = tvb_get_guint8 (tvb, offset);
    /
    tf = proto_tree_add_boolean_format (envelope_tree, hf_envelope_extensions,
                                        tvb, offset, 1, envelope,
                                        "Extensions: %s",
                                        (envelope & 0x80) ? "Present" : "Absent");
    field_tree = proto_item_add_subtree (tf, ett_envelope_extensions);
    proto_tree_add_item (field_tree, hf_envelope_extensions, tvb, offset, 1, ENC_BIG_ENDIAN);
    dmp.extensions = (envelope & 0x80);

    /
    dmp.msg_id_type = (envelope & 0x60) >> 5;
    tf = proto_tree_add_uint_format (envelope_tree, hf_envelope_msg_id_type,
                                     tvb, offset, 1, envelope,
                                     "Message Identifier Type: %s (%d)", 
                                     val_to_str (dmp.msg_id_type, msg_id_type_vals, "Unknown"),
                                     dmp.msg_id_type);
    field_tree = proto_item_add_subtree (tf, ett_envelope_msg_id_type);
    proto_tree_add_item (field_tree, hf_envelope_msg_id_type, tvb, offset, 1, ENC_BIG_ENDIAN);

    if (dmp.msg_id_type == X400_MSG_ID || dmp.msg_id_type == NAT_MSG_ID) {
      /
      dmp.mts_id_length = (envelope & 0x1F);
      dmp.mts_id_item = proto_tree_add_uint_format (envelope_tree, hf_envelope_mts_id_length,
                                                    tvb, offset, 1, envelope,
                                                    "MTS Identifier Length: %u", 
                                                    dmp.mts_id_length);
      field_tree = proto_item_add_subtree (dmp.mts_id_item, ett_envelope_mts_id_length);
      proto_tree_add_item (field_tree, hf_envelope_mts_id_length, tvb, offset, 1, ENC_BIG_ENDIAN);
      offset += 1;
    } else {
      proto_tree_add_item (field_tree, hf_envelope_msg_id_length, tvb, offset, 1, ENC_BIG_ENDIAN);
      if (envelope & 0x10) {
        /
        using_short_id = TRUE;
      } else {
        tf = proto_tree_add_item (field_tree, hf_reserved_0x0F, tvb, offset, 1, ENC_BIG_ENDIAN);
        if (envelope & 0x0F) {
          expert_add_info_format (pinfo, tf, PI_UNDECODED, PI_WARN, "Reserved value");
        }
        offset += 1;
      }
    }
  }

  /
  dmp.msg_id = tvb_get_ntohs (tvb, offset);
  if (using_short_id) {
    dmp.msg_id &= 0x0FFF;
  }
  tf = proto_tree_add_uint (envelope_tree, hf_envelope_msg_id, tvb, offset, 2, dmp.msg_id);
  hidden_item = proto_tree_add_uint (envelope_tree, hf_dmp_id, tvb, offset, 2, dmp.msg_id);
  if (using_short_id) {
    field_tree = proto_item_add_subtree (tf, ett_envelope_msg_id);
    proto_tree_add_item (field_tree, hf_envelope_msg_id_12bit, tvb, offset, 2, ENC_BIG_ENDIAN);
  } else if (dmp.version >= DMP_VERSION_2 && dmp.msg_id_type == ONLY_DMP_ID && dmp.msg_id < 4096) {
    expert_add_info_format (pinfo, tf, PI_PROTOCOL, PI_NOTE, "Id < 4096 - should use ShortId");
  }
  PROTO_ITEM_SET_HIDDEN (hidden_item);
  offset += 2;

  if (dmp.version >= DMP_VERSION_2) {
    if ((dmp.msg_type != REPORT) && (dmp.msg_id_type == X400_MSG_ID || dmp.msg_id_type == NAT_MSG_ID)) {
      offset = dissect_mts_identifier (tvb, pinfo, envelope_tree, offset, FALSE);
    }
  }

  /
  subm_time = tvb_get_ntohs (tvb, offset);
  dmp.subm_time = dmp_dec_subm_time ((guint16)(subm_time & 0x7FFF),
                                     (gint32) pinfo->fd->abs_ts.secs);
  tf = proto_tree_add_uint_format (envelope_tree, hf_envelope_subm_time, tvb,
                                   offset, 2, subm_time,
                                   "Submission time: %s",
                                   (subm_time & 0x7FFF) >= 0x7FF8 ?
                                   "Reserved" :
                                   abs_time_secs_to_str (dmp.subm_time, ABSOLUTE_TIME_LOCAL, TRUE));
  field_tree = proto_item_add_subtree (tf, ett_envelope_subm_time);
  proto_tree_add_item (field_tree, hf_envelope_time_diff_present, tvb, offset, 2, ENC_BIG_ENDIAN);
  proto_tree_add_item (field_tree, hf_envelope_subm_time_value, tvb, offset, 2, ENC_BIG_ENDIAN);
  offset += 2;

  if (subm_time & 0x8000) {
    /
    time_diff = tvb_get_guint8 (tvb, offset);
    tf = proto_tree_add_uint_format (envelope_tree, hf_envelope_time_diff,
                                     tvb, offset, 1, time_diff,
                                     "Time Difference: ");
    field_tree = proto_item_add_subtree (tf, ett_envelope_time_diff);
    proto_tree_add_item (field_tree, hf_envelope_time_diff_value, tvb, offset, 1, ENC_BIG_ENDIAN);
    secs = dmp_dec_time_diff (time_diff);
    if (secs == DMP_TIME_RESERVED) {
      proto_item_append_text (tf, "Reserved (0x%2.2x)", time_diff);
    } else {
      proto_item_append_text (tf, "%s", time_secs_to_str (secs));
    }
    offset += 1;
  }

  /
  envelope = tvb_get_guint8 (tvb, offset);
  tf = proto_tree_add_uint_format (envelope_tree, hf_envelope_flags,
                                   tvb, offset, 1, envelope,
                                   "Envelope Flags");

  field_tree = proto_item_add_subtree (tf, ett_envelope_flags);
  proto_tree_add_item (field_tree, hf_envelope_content_id_discarded, tvb, offset, 1, ENC_BIG_ENDIAN);
  proto_tree_add_item (field_tree, hf_envelope_recip_reassign_prohib, tvb, offset, 1, ENC_BIG_ENDIAN);
  proto_tree_add_item (field_tree, hf_envelope_dl_expansion_prohib, tvb, offset, 1, ENC_BIG_ENDIAN);

  if (envelope & 0xE0) {
    env_flags = ep_strdup_printf ("%s%s%s",
                                  (envelope & 0x80) ? ", ContId discarded" : "",
                                  (envelope & 0x40) ? ", Reass prohibited" : "",
                                  (envelope & 0x20) ? ", DLE prohibited"   : "");
    proto_item_append_text (tf, ":%s", &env_flags[1]);
  } else {
    proto_item_append_text (tf, " (none)");
  }

  /
  no_rec = (envelope & 0x1F);
  tf = proto_tree_add_uint_format (envelope_tree, hf_envelope_recipients,
                                   tvb, offset, 1, envelope,
                                   "Recipient Count: %d", no_rec);

  field_tree = proto_item_add_subtree (tf, ett_envelope_recipients);
  proto_tree_add_item (field_tree, hf_envelope_recipients, tvb, offset, 1, ENC_BIG_ENDIAN);
  offset += 1;

  if (no_rec == 0) {
    /
    value16 = tvb_get_ntohs (tvb, offset);
    no_rec = value16 & 0x7FFF;
    tf = proto_tree_add_uint_format (envelope_tree,hf_envelope_ext_recipients,
                                     tvb, offset, 2, value16,
                                     "Extended Recipient Count: %d%s", no_rec,
                                     (no_rec < 32 ?
                                      " (incorrect, reserved value)" : ""));

    field_tree = proto_item_add_subtree (tf, ett_envelope_ext_recipients);
    en = proto_tree_add_item (field_tree, hf_reserved_0x8000, tvb, offset, 2, ENC_BIG_ENDIAN);
    if (value16 & 0x8000) {
      expert_add_info_format (pinfo, en, PI_UNDECODED, PI_WARN,
                              "Reserved value");
    }
    proto_tree_add_item (field_tree, hf_envelope_ext_recipients, tvb, offset, 2, ENC_BIG_ENDIAN);
    offset += 2;
  }

  if (dmp.msg_type != REPORT) {
    /
    offset = dissect_dmp_originator (tvb, pinfo, envelope_tree, offset);
  }

  for (i = 1; i <= no_rec; i++) {
    /
    offset = dissect_dmp_address (tvb, pinfo, envelope_tree, offset,
                                  &prev_rec_no, FALSE);
  }

  if (dmp.version >= DMP_VERSION_2) {
    if ((dmp.msg_id_type == X400_MSG_ID || dmp.msg_id_type == NAT_MSG_ID) &&
        dmp.notif_req && (dmp.msg_type == STANAG || dmp.msg_type == IPM))
    {
      offset = dissect_ipm_identifier (tvb, pinfo, envelope_tree, offset, FALSE);
    }
  }

  proto_item_set_len (en, offset - boffset);

  return offset;
}
