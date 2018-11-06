static void
dissect_meta_record_tags(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree) {
  proto_item *pi            = NULL;
  proto_item *tag_pi        = NULL;
  proto_item *tag_tree;
  proto_item *section_pi    = NULL;
  proto_item *section_tree  = tree;
  proto_item *sectionlen_pi = NULL;

  guint16                sectiontype  = ERF_META_SECTION_NONE;
  guint16                tagtype      = 0;
  guint16                taglength    = 0;
  const gchar           *tagvalstring = NULL;
  erf_meta_tag_info_t   *tag_info;

  /
  erf_meta_hf_template_t tag_template_unknown = { 0, { "Unknown", "unknown",
    FT_BYTES, BASE_NONE, NULL, 0x0, NULL, HFILL } };
  erf_meta_tag_info_t    tag_info_local       = { 0, 0, &tag_template_unknown,
    ett_erf_meta_tag, hf_erf_meta_tag_unknown, NULL };

  int     offset        = 0;
  int     sectionoffset = 0;
  guint16 sectionid     = 0;
  guint16 sectionlen    = 0;

  /
  col_set_str(pinfo->cinfo, COL_INFO, "MetaERF Record");

  /
  while (tvb_captured_length_remaining(tvb, offset) > 4) {
    tagtype = tvb_get_ntohs(tvb, offset);
    taglength = tvb_get_ntohs(tvb, offset + 2);
    tag_tree = NULL;

    if (ERF_META_IS_SECTION(tagtype))
      sectiontype = tagtype;

    /
    tag_info_local.code = tagtype;
    tag_info_local.section = sectiontype;
    tag_info = (erf_meta_tag_info_t*) wmem_map_lookup(erf_meta_index.tag_table, GUINT_TO_POINTER(ERF_TAG_INFO_KEY(&tag_info_local)));

    /
    if (tag_info == NULL)
      tag_info = &tag_info_local;

    /
    if (ERF_META_IS_SECTION(tagtype)) { /
      if (section_pi) {
        /
        proto_item_set_len(section_pi, offset - sectionoffset);
        if (sectionlen_pi) {
          check_section_length(pinfo, sectionlen_pi, offset, sectionoffset, sectionlen);
        }
      }

      sectionoffset = offset;
      if (tag_info->tag_template == &tag_template_unknown) {
        /
        sectiontype = ERF_META_SECTION_UNKNOWN;
        tag_info = erf_meta_index.unknown_section_info;
      }
      DISSECTOR_ASSERT(tag_info->extra);

      tagvalstring = val_to_str(tagtype, VALS(wmem_array_get_raw(erf_meta_index.vs_list)), "Unknown Section (0x%x)");
      section_tree = proto_tree_add_subtree_format(tree, tvb, offset, 0, tag_info->extra->ett_value, &section_pi, "MetaERF %s", tagvalstring);
      tag_tree = proto_tree_add_subtree_format(section_tree, tvb, offset, taglength + 4, tag_info->ett, &tag_pi, "%s Header", tagvalstring);

      if (taglength > 0) {
        sectionid = tvb_get_ntohs(tvb, offset + 4);
        sectionlen = tvb_get_ntohs(tvb, offset + 6);

        /
        proto_tree_add_uint(tag_tree, tag_info->hf_value, tvb, offset + 4, 2, sectionid);
        if (sectionid != 0)
          proto_item_append_text(section_pi, " %u", sectionid);

        /
        sectionlen_pi = proto_tree_add_uint(tag_tree, tag_info->extra->hf_values[0], tvb, offset + 6, 2, sectionlen);

        /
        if (taglength > 4) {
          proto_tree_add_item(tag_tree, tag_info->extra->hf_values[1], tvb, offset + 8, taglength - 4, ENC_NA);
        }
      }
    } else { /
      enum ftenum tag_ft;
      char        pi_label[ITEM_LABEL_LENGTH+1];
      gboolean    dissected = TRUE;
      guint32     value32;
      guint64     value64;
      gchar      *tmp = NULL;

      tag_ft = tag_info->tag_template->hfinfo.type;
      pi_label[0] = '\0';

      /
      if (offset == 0) {
        section_tree = proto_tree_add_subtree(tree, tvb, offset, 0, ett_erf_meta, &section_pi, "MetaERF No Section");
      }

      /
      /
      switch (tagtype) {
      /
      case ERF_META_TAG_if_speed:
      case ERF_META_TAG_if_tx_speed:
        value64 = tvb_get_ntoh64(tvb, offset + 4);
        tmp = format_size((gint64) value64, (format_size_flags_e)(format_size_unit_bits_s|format_size_prefix_si));
        tag_pi = proto_tree_add_uint64_format_value(section_tree, tag_info->hf_value, tvb, offset + 4, taglength, value64, "%s (%" G_GINT64_MODIFIER "u bps)", tmp, value64);
        g_free(tmp);
        break;

      case ERF_META_TAG_if_rx_power:
      case ERF_META_TAG_if_tx_power:
        value32 = tvb_get_ntohl(tvb, offset + 4);
        tag_pi = proto_tree_add_int_format_value(section_tree, tag_info->hf_value, tvb, offset + 4, taglength, (gint32) value32, "%.2fdBm", (float)((gint32) value32)/100.0);
        break;

      case ERF_META_TAG_loc_lat:
      case ERF_META_TAG_loc_long:
        value32 = tvb_get_ntohl(tvb, offset + 4);
        tag_pi = proto_tree_add_int_format_value(section_tree, tag_info->hf_value, tvb, offset + 4, taglength, (gint32) value32, "%.2f", (double)((gint32) value32)*1000000.0);
        break;

      case ERF_META_TAG_mask_cidr:
        value32 = tvb_get_ntohl(tvb, offset + 4);
        tag_pi = proto_tree_add_uint_format_value(section_tree, tag_info->hf_value, tvb, offset + 4, taglength, value32, "/%u", value32);
        break;

      case ERF_META_TAG_mem:
        value64 = tvb_get_ntoh64(tvb, offset + 4);
        tmp = format_size((gint64) value64, (format_size_flags_e)(format_size_unit_bytes|format_size_prefix_iec));
        tag_pi = proto_tree_add_uint64_format_value(section_tree, tag_info->hf_value, tvb, offset + 4, taglength, value64, "%s (%" G_GINT64_MODIFIER"u bytes)", tmp, value64);
        g_free(tmp);
        break;

      case ERF_META_TAG_parent_section:
        DISSECTOR_ASSERT(tag_info->extra);
        value32 = tvb_get_ntohs(tvb, offset + 4);
        /
        tag_tree = proto_tree_add_subtree_format(section_tree, tvb, offset + 4, taglength, tag_info->ett, &tag_pi, "%s: %s %u", tag_info->tag_template->hfinfo.name,
            val_to_str(value32, VALS(wmem_array_get_raw(erf_meta_index.vs_list)), "Unknown Section (%u)"), tvb_get_ntohs(tvb, offset + 4 + 2));

        proto_tree_add_uint_format_value(tag_tree, tag_info->hf_value, tvb, offset + 4, MIN(2, taglength), value32, "%s (%u)",
            val_to_str(value32, VALS(wmem_array_get_raw(erf_meta_index.vs_abbrev_list)), "Unknown"), value32);
        proto_tree_add_item(tag_tree, tag_info->extra->hf_values[0], tvb, offset + 6, MIN(2, taglength - 2), ENC_BIG_ENDIAN);
        break;

      case ERF_META_TAG_reset:
        tag_pi = proto_tree_add_item(section_tree, tag_info->hf_value, tvb, offset + 4, taglength, ENC_NA);
        expert_add_info(pinfo, tag_pi, &ei_erf_meta_reset);
        break;

      case ERF_META_TAG_if_link_status:
        DISSECTOR_ASSERT(tag_info->extra);
        {
          const int *link_status_flags[] = {
            &tag_info->extra->hf_values[0],
            NULL
          };

          tag_pi = proto_tree_add_bitmask(section_tree, tvb, offset + 4, tag_info->hf_value, tag_info->ett, link_status_flags, ENC_BIG_ENDIAN);
          tag_tree = proto_item_get_subtree(tag_pi);
        }
        break;

      case ERF_META_TAG_tunneling_mode:
        DISSECTOR_ASSERT(tag_info->extra);
        {
          const int* tunneling_mode_flags[array_length(erf_tunneling_modes)+1];
          int i;

          /
          for (i = 0; i < (int) array_length(erf_tunneling_modes); i++) {
            tunneling_mode_flags[i] = &tag_info->extra->hf_values[i];
          }
          tunneling_mode_flags[array_length(erf_tunneling_modes)] = NULL;

          tag_pi = proto_tree_add_bitmask(section_tree, tvb, offset + 4, tag_info->hf_value, tag_info->ett, tunneling_mode_flags, ENC_BIG_ENDIAN);
        }
        break;

      case ERF_META_TAG_ns_dns_ipv4:
      case ERF_META_TAG_ns_dns_ipv6:
      case ERF_META_TAG_ns_host_ipv4:
      case ERF_META_TAG_ns_host_ipv6:
      case ERF_META_TAG_ns_host_mac:
      case ERF_META_TAG_ns_host_eui:
      case ERF_META_TAG_ns_host_wwn:
      case ERF_META_TAG_ns_host_ib_gid:
      case ERF_META_TAG_ns_host_ib_lid:
      case ERF_META_TAG_ns_host_fc_id:
      {
        int addr_len = ftype_length(tag_ft);

        DISSECTOR_ASSERT(tag_info->extra);

        tag_tree = proto_tree_add_subtree(section_tree, tvb, offset + 4, taglength, tag_info->ett, &tag_pi, tag_info->tag_template->hfinfo.name);
        /
        pi = proto_tree_add_item(tag_tree, tag_info->hf_value, tvb, offset + 4, MIN(addr_len, taglength), IS_FT_INT(tag_ft) || IS_FT_UINT(tag_ft) ? ENC_BIG_ENDIAN : ENC_NA);
        /
        proto_tree_add_item(tag_tree, tag_info->extra->hf_values[0], tvb, offset + 4 + addr_len, taglength - addr_len, ENC_UTF_8);
        if (pi) {
          proto_item_fill_label(PITEM_FINFO(pi), pi_label);
          /
          proto_item_append_text(tag_pi, "%s %s", pi_label /,
              tvb_get_stringzpad(wmem_packet_scope(), tvb, offset + 4 + addr_len, taglength - addr_len, ENC_UTF_8));
        }

        break;
      }

      default:
        dissected = FALSE;
        break;
      }

      /
      if (!dissected) {
        if (IS_FT_INT(tag_ft) || IS_FT_UINT(tag_ft)) {
          tag_pi = proto_tree_add_item(section_tree, tag_info->hf_value, tvb, offset + 4, taglength, ENC_BIG_ENDIAN);
        } else if (IS_FT_STRING(tag_ft)) {
          tag_pi = proto_tree_add_item(section_tree, tag_info->hf_value, tvb, offset + 4, taglength, ENC_UTF_8);
        } else if (IS_FT_TIME(tag_ft)) {
          /
          /
          nstime_t t;
          guint64 ts;

          ts = tvb_get_letoh64(tvb, offset + 4);

          t.secs = (long) (ts >> 32);
          ts  = ((ts & 0xffffffff) * 1000 * 1000 * 1000);
          ts += (ts & 0x80000000) << 1; /
          t.nsecs = ((int) (ts >> 32));
          if (t.nsecs >= 1000000000) {
            t.nsecs -= 1000000000;
            t.secs += 1;
          }

          tag_pi = proto_tree_add_time(section_tree, tag_info->hf_value, tvb, offset + 4, taglength, &t);
        } else {
          tag_pi = proto_tree_add_item(section_tree, tag_info->hf_value, tvb, offset + 4, taglength, ENC_NA);
        }
      }
    }

    /
    if (!tag_tree)
      tag_tree = proto_item_add_subtree(tag_pi, tag_info->ett);

    /
    /
    proto_tree_add_uint_format_value(tag_tree, hf_erf_meta_tag_type, tvb, offset, 2, tagtype, "%s (%u)", val_to_str(tagtype, VALS(wmem_array_get_raw(erf_meta_index.vs_abbrev_list)), "Unknown"), tagtype);
    proto_tree_add_uint(tag_tree, hf_erf_meta_tag_len, tvb, offset + 2, 2, taglength);

    offset += ((taglength + 4) + 0x3) & ~0x3;
  }

  /
  proto_item_set_len(section_pi, offset - sectionoffset);
  check_section_length(pinfo, sectionlen_pi, offset, sectionoffset, sectionlen);
}
