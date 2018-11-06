static gint dissect_dmp_extensions (tvbuff_t *tvb, packet_info *pinfo _U_,
                                    proto_tree *dmp_tree, gint offset)
{
  proto_tree *exts_tree, *ext_tree, *hdr_tree;
  proto_item *exts_item, *en;
  guint8      ext_hdr, ext_length;
  gboolean    more_extensions = TRUE;
  gint        num_ext = 0, boffset = offset;

  exts_item = proto_tree_add_item (dmp_tree, hf_extensions, tvb, offset, -1, ENC_NA);
  exts_tree = proto_item_add_subtree (exts_item, ett_extensions);

  while (more_extensions) {
    /
    ext_hdr = tvb_get_guint8 (tvb, offset);
    more_extensions = (ext_hdr & 0x80);
    ext_length = (ext_hdr & 0x7F) + 1;

    en = proto_tree_add_none_format (exts_tree, hf_extension, tvb, offset, ext_length + 1,
                                     "Extension (#%d)", num_ext + 1);
    ext_tree = proto_item_add_subtree (en, ett_extension);

    en = proto_tree_add_none_format (ext_tree, hf_extension_header, tvb, offset, 1, 
                                     "Extension Length: %u, More %s", ext_length,
                                     (ext_hdr & 0x80) ? "Present" : "Not present");
    hdr_tree = proto_item_add_subtree (en, ett_extension_header);
    proto_tree_add_item (hdr_tree, hf_extension_more, tvb, offset, 1, ENC_BIG_ENDIAN);
    proto_tree_add_item (hdr_tree, hf_extension_length, tvb, offset, 1, ENC_BIG_ENDIAN);
    offset += 1;

    proto_tree_add_item (ext_tree, hf_extension_data, tvb, offset, ext_length, ENC_NA);
    offset += ext_length;
    num_ext++;
  }

  proto_item_append_text (exts_item, " (%d item%s)", num_ext, plurality (num_ext, "", "s"));
  proto_item_set_len (exts_item, offset - boffset);

  return offset;
}
