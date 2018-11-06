static gint dissect_dmp_originator (tvbuff_t *tvb, packet_info *pinfo,
                                    proto_tree *envelope_tree, gint offset)
{
  proto_tree *field_tree = NULL, *rec_tree = NULL;
  proto_item *en = NULL, *tf = NULL;
  guint8      value, dmp_addr_form;
  gint        boffset = offset;
  gboolean    p2_addr = FALSE;

  tf = proto_tree_add_item (envelope_tree, hf_addr_originator, tvb, offset, -1, ENC_NA);
  field_tree = proto_item_add_subtree (tf, ett_address);

  if (dmp.addr_enc == DIRECT_ADDR) {
    offset = dissect_dmp_direct_addr (tvb, pinfo, field_tree, tf,
                                      offset, -1, -1, ORIGINATOR);
  } else {
    value = tvb_get_guint8 (tvb, offset);
    dmp_addr_form = (value & 0xE0) >> 5;

    if (dmp.version == DMP_VERSION_1 && !(dmp.prot_id == PROT_NAT && dmp_nat_decode == NAT_DECODE_THALES)) {
      en = proto_tree_add_uint_format (field_tree, hf_addr_ext_form_orig_v1, tvb,
                                       offset, 1, value,
                                       "Address Form: %s",
                                       val_to_str (dmp_addr_form,
                                                   addr_form_orig_v1, "Reserved"));
      rec_tree = proto_item_add_subtree (en, ett_address_ext_form);
      proto_tree_add_item (rec_tree, hf_addr_ext_form_orig_v1, tvb, offset, 1, ENC_BIG_ENDIAN);
    } else {
      en = proto_tree_add_uint_format (field_tree, hf_addr_ext_form_orig, tvb,
                                       offset, 1, value,
                                       "Address Form: %s",
                                       val_to_str (dmp_addr_form,
                                                   addr_form_orig, "Reserved"));
      rec_tree = proto_item_add_subtree (en, ett_address_ext_form);
      proto_tree_add_item (rec_tree, hf_addr_ext_form_orig, tvb, offset, 1, ENC_BIG_ENDIAN);
    }

    en = proto_tree_add_item (rec_tree, hf_reserved_0x1F, tvb, offset, 1, ENC_BIG_ENDIAN);
    if (value & 0x1F) {
      expert_add_info_format (pinfo, en, PI_UNDECODED, PI_WARN,
                              "Reserved value");
    }
    offset += 1;

    if (dmp.version == DMP_VERSION_1 && !(dmp.prot_id == PROT_NAT && dmp_nat_decode == NAT_DECODE_THALES)) {
      switch (dmp_addr_form) {
      
      case P1_DIRECT:
        offset = dissect_dmp_direct_addr (tvb, pinfo, field_tree,
                                          tf, offset, -1, -1,
                                          ORIGINATOR);
        break;
      
      case P1_EXTENDED:
        offset = dissect_dmp_ext_addr (tvb, pinfo, field_tree, tf, offset, -1,
                                       -1, ORIGINATOR);
        break;
      
      default:
        proto_item_append_text (tf, " (invalid address form)");
        break;
      
      }
    } else {
      switch (dmp_addr_form) {
      
      case P1_DIRECT:
      case P1_P2_DIRECT:
      case P1_DIRECT_P2_EXTENDED:
        offset = dissect_dmp_direct_addr (tvb, pinfo, field_tree,
                                          tf, offset, -1, -1,
                                          ORIGINATOR);
        break;
      
      case P1_EXTENDED:
      case P1_EXTENDED_P2_DIRECT:
      case P1_P2_EXTENDED:
        offset = dissect_dmp_ext_addr (tvb, pinfo, field_tree, tf, offset, -1,
                                       -1, ORIGINATOR);
        break;
      
      default:
        proto_item_append_text (tf, " (invalid address form)");
        break;
      
      }
    
      switch (dmp_addr_form) {
      
      case P1_P2_DIRECT:
      case P1_EXTENDED_P2_DIRECT:
        offset = dissect_dmp_direct_addr (tvb, pinfo, field_tree,
                                          tf, offset, -1, -1,
                                          ORIG_P2_ADDRESS);
        p2_addr = TRUE;
        break;
      
      case P1_DIRECT_P2_EXTENDED:
      case P1_P2_EXTENDED:
        offset = dissect_dmp_ext_addr (tvb, pinfo, field_tree, tf, offset, -1,
                                       -1, ORIG_P2_ADDRESS);
        p2_addr = TRUE;
        break;
      
      }
    }

    if (p2_addr) {
      if (dmp.msg_type == NOTIF) {
        en = proto_tree_add_boolean (field_tree, hf_addr_int_rec, tvb,
                                     offset, 0, TRUE);
      } else {
        en = proto_tree_add_boolean (field_tree, hf_addr_dl_expanded, tvb,
                                     offset, 0, TRUE);
      }
      PROTO_ITEM_SET_GENERATED (en);
    }
  }
  proto_item_set_len (tf, offset - boffset);

  return offset;
}
