                        int        ndds_40_hack) {
  const gint original_offset = offset;
  guint32 tk_id;
  guint16 tk_size;
  unsigned int i;
  char    indent_string[40];
  gint    retVal;
  char    type_name[40];

    /

  /
  memset(indent_string, ' ', 40);
  indent_string[indent_level*2] = '\0';

  /
  LONG_ALIGN(offset);
  tk_id = NEXT_guint32(tvb, offset, little_endian);
  offset += 4;

  /
  tk_size = NEXT_guint16(tvb, offset, little_endian);
  offset += 2;

  retVal = tk_size + 6; /

  /
  tk_id &= 0x7fffffff;

  /
  if (indent_level == 0) {
    if (tk_id == RTI_CDR_TK_OCTET) {
      ndds_40_hack = 1;
    }
  }
  if (ndds_40_hack) {
    ++tk_id;
  }

  g_strlcpy(type_name, rtps_util_typecode_id_to_string(tk_id), 40);

    /
  switch(tk_id) {

    /
    case RTI_CDR_TK_UNION: {
        guint32 struct_name_len;
        gint8 * struct_name;
        const char * discriminator_name = "<unknown>";    /
        char *       discriminator_enum_name = NULL;      /
        / /
        guint32 disc_id;    /
        guint16 disc_size;  /
        guint32 disc_offset_begin;
        guint32 num_members;
        guint16 member_length;
        guint32 member_name_len;
        guint8 *member_name = NULL;
        guint8  member_is_pointer;
        guint32 next_offset;
        guint32 field_offset_begin;
        guint32 member_label_count;
        gint32  member_label;
        guint32 discriminator_enum_name_length;
        guint   j;

        /
        /
        LONG_ALIGN(offset);

        /
        struct_name_len = NEXT_guint32(tvb, offset, little_endian);
        offset += 4;
        struct_name = tvb_get_ephemeral_string(tvb, offset, struct_name_len);
        offset += struct_name_len;

        /
        LONG_ALIGN(offset);
        /
        offset += 4;

        /
        /
        disc_id = NEXT_guint32(tvb, offset, little_endian);
        offset += 4;

        disc_size = NEXT_guint16(tvb, offset, little_endian);
        offset += 2;
        disc_offset_begin = offset;
        disc_id &= 0x7fffffff;
        discriminator_name = rtps_util_typecode_id_to_string(disc_id);
        if (disc_id == RTI_CDR_TK_ENUM) {
            /
            LONG_ALIGN(offset);
            discriminator_enum_name_length = NEXT_guint32(tvb, offset, little_endian);
            discriminator_enum_name = tvb_get_ephemeral_string(tvb, offset+4, discriminator_enum_name_length);
        }
        offset = disc_offset_begin + disc_size;
/


        /
        proto_tree_add_text(tree,
                    tvb,
                    original_offset,
                    retVal,
                    "%sunion %s (%s%s%s) {",
                    indent_string,
                    struct_name,
                    discriminator_name,
                    (discriminator_enum_name ? " " : ""),
                    (discriminator_enum_name ? discriminator_enum_name : ""));

        if (seq_max_len != 0) {
          /
          g_snprintf(type_name, 40, "%s", struct_name);
          break;
        }

        /
        LONG_ALIGN(offset);
        num_members = NEXT_guint32(tvb, offset, little_endian);
        offset += 4;

        /
        next_offset = offset;

        for (i = 0; i < num_members; ++i) {
          /
          field_offset_begin = offset = next_offset;

          SHORT_ALIGN(offset);

          /
          member_length = NEXT_guint16(tvb, offset, little_endian);
          offset += 2;
          next_offset = offset + member_length;

          /
          LONG_ALIGN(offset);
          member_name_len = NEXT_guint32(tvb, offset, little_endian);
          offset += 4;

          /
          member_name = tvb_get_ephemeral_string(tvb, offset, member_name_len);
          offset += member_name_len;

          /
          member_is_pointer = tvb_get_guint8(tvb, offset);
          offset++;

          /
          LONG_ALIGN(offset);
          member_label_count = NEXT_guint32(tvb, offset, little_endian);
          offset += 4;

          for (j = 0; j < member_label_count; ++j) {
            /
            LONG_ALIGN(offset);
            member_label = NEXT_guint32(tvb, offset, little_endian);
            offset += 4;

            /
            proto_tree_add_text(tree,
                    tvb,
                    field_offset_begin,
                    retVal,
                    "%s  case %d:",
                    indent_string,
                    member_label);
          }

          offset += rtps_util_add_typecode(
                    tree,
                    tvb,
                    offset,
                    little_endian,
                    indent_level+2,
                    member_is_pointer,
                    0,
                    0,
                    field_offset_begin,
                    member_name,
                    0,
                    NULL,
                    ndds_40_hack);
        }
        /
        g_strlcpy(type_name, "}", 40);
        break;

    } /


    case RTI_CDR_TK_ENUM:
    case RTI_CDR_TK_STRUCT: {
    /
        guint32 struct_name_len;
        gint8 * struct_name;
        guint32 num_members;
        guint16 member_length;
        guint8  member_is_pointer;
        guint16 member_bitfield;
        guint8  member_is_key;
        guint32 member_name_len;
        guint8 *member_name = NULL;
        guint32 next_offset;
        guint32 field_offset_begin;
        guint32 ordinal_number;
        const char * typecode_name;

        /
        LONG_ALIGN(offset);

        /
        struct_name_len = NEXT_guint32(tvb, offset, little_endian);
        offset += 4;

        /
        struct_name = tvb_get_ephemeral_string(tvb, offset, struct_name_len);
        offset += struct_name_len;


        if (tk_id == RTI_CDR_TK_ENUM) {
            typecode_name = "enum";
        } else if (tk_id == RTI_CDR_TK_VALUE_PARARM) {
            /
            /
            guint32 baseTypeCodeLength;
            /
            typecode_name = "<sparse type>";
            SHORT_ALIGN(offset);
            /
            offset += 2;
            LONG_ALIGN(offset);
            /
            offset += 4;
            baseTypeCodeLength = NEXT_guint32(tvb, offset, little_endian);
            offset += 4;
            offset += baseTypeCodeLength;
        } else {
            typecode_name = "struct";
        }

        if (seq_max_len != 0) {
          /
          g_snprintf(type_name, 40, "%s", struct_name);
          break;
        }
        /
        proto_tree_add_text(tree,
                    tvb,
                    original_offset,
                    retVal,
                    "%s%s %s {",
                    indent_string,
                    typecode_name,
                    struct_name);

        /
        LONG_ALIGN(offset);

        /
        num_members = NEXT_guint32(tvb, offset, little_endian);
        offset += 4;

        next_offset = offset;
        for (i = 0; i < num_members; ++i) {
          /
          field_offset_begin = offset = next_offset;

          SHORT_ALIGN(offset);

          /
          member_length = NEXT_guint16(tvb, offset, little_endian);
          offset += 2;
          next_offset = offset + member_length;

          /
          LONG_ALIGN(offset);
          member_name_len = NEXT_guint32(tvb, offset, little_endian);
          offset += 4;

          /
          member_name = tvb_get_ephemeral_string(tvb, offset, member_name_len);
          offset += member_name_len;

          if (tk_id == RTI_CDR_TK_ENUM) {
            /
            LONG_ALIGN(offset);
            ordinal_number = NEXT_guint32(tvb, offset, little_endian);
            offset += 4;

            proto_tree_add_text(tree,
                  tvb,
                  field_offset_begin,
                  (offset-field_offset_begin),
                  "%s  %s = %d;",
                  indent_string,
                  member_name,
                  ordinal_number);
          } else {
            /

            /
            member_is_pointer = tvb_get_guint8(tvb, offset);
            offset++;

            /
            SHORT_ALIGN(offset);
            member_bitfield = NEXT_guint16(tvb, offset, little_endian);
            offset += 2; /

            /
            member_is_key = tvb_get_guint8(tvb, offset);
            offset++;

            offset += rtps_util_add_typecode(
                          tree,
                          tvb,
                          offset,
                          little_endian,
                          indent_level+1,
                          member_is_pointer,
                          member_bitfield,
                          member_is_key,
                          field_offset_begin,
                          member_name,
                          0,
                          NULL,
                          ndds_40_hack);
          }
        }
        /
        g_strlcpy(type_name, "}", 40);
        break;
      }

    case RTI_CDR_TK_WSTRING:
    case RTI_CDR_TK_STRING: {
    /
        guint32 string_length;

        LONG_ALIGN(offset);
        string_length = NEXT_guint32(tvb, offset, little_endian);
        offset += 4;
        g_snprintf(type_name, 40, "%s<%d>",
                (tk_id == RTI_CDR_TK_STRING) ? "string" : "wstring",
                string_length);
        break;
    }

    case RTI_CDR_TK_SEQUENCE: {
    /
        guint32 seq_max_len2;
        LONG_ALIGN(offset);
        seq_max_len2 = NEXT_guint32(tvb, offset, little_endian);
        offset += 4;

        /
        offset += rtps_util_add_typecode(
                          tree,
                          tvb,
                          offset,
                          little_endian,
                          indent_level,
                          is_pointer,
                          bitfield,
                          is_key,
                          offset_begin,
                          name,
                          seq_max_len2,
                          NULL,
                          ndds_40_hack);
        /
        return retVal;
    }

    case RTI_CDR_TK_ARRAY: {
    /
        guint32 size[MAX_ARRAY_DIMENSION]; /
        guint32 dim_max;

        LONG_ALIGN(offset);
        dim_max = NEXT_guint32(tvb, offset, little_endian);
        offset += 4;

        for (i = 0; i < MAX_ARRAY_DIMENSION; ++i) size[i] = 0;
        for (i = 0; i < dim_max; ++i) {
          size[i] = NEXT_guint32(tvb, offset, little_endian);
          offset += 4;
        }

        /
        offset += rtps_util_add_typecode(
                          tree,
                          tvb,
                          offset,
                          little_endian,
                          indent_level,
                          is_pointer,
                          bitfield,
                          is_key,
                          offset_begin,
                          name,
                          0,
                          size,
                          ndds_40_hack);
        /
        return retVal;
    }

    case RTI_CDR_TK_ALIAS: {
    /
        guint32 alias_name_length;
        guint8 *alias_name;

        LONG_ALIGN(offset);
        alias_name_length = NEXT_guint32(tvb, offset, little_endian);
        offset += 4;
        alias_name = tvb_get_ephemeral_string(tvb, offset, alias_name_length);
        offset += alias_name_length;
        g_strlcpy(type_name, alias_name, 40);
        break;
    }


    /
    case RTI_CDR_TK_VALUE_PARARM:
    case RTI_CDR_TK_VALUE: {
        /
        /
        guint32 value_name_len;
        gint8 * value_name;
        const char * type_id_name = "valuetype";
        LONG_ALIGN(offset);

        /
        value_name_len = NEXT_guint32(tvb, offset, little_endian);
        offset += 4;

        /
        value_name = tvb_get_ephemeral_string(tvb, offset, value_name_len);
        offset += value_name_len;

        if (tk_id == RTI_CDR_TK_VALUE_PARARM) {
            type_id_name = "valueparam";
        }
        g_snprintf(type_name, 40, "%s '%s'", type_id_name, value_name);
        break;
    }
  } /

  /
  if (seq_max_len != 0) {
    proto_tree_add_text(tree,
                  tvb,
                  offset_begin,
                  (offset-offset_begin),
                  "%ssequence<%s, %d> %s%s;%s",
                  indent_string,
                  type_name,
                  seq_max_len,
                  is_pointer ? "*" : "",
                  name ? name : "",
                  is_key ? KEY_COMMENT : "");
    return retVal;
  }

  /
  if (arr_dimension != NULL) {
    /
    emem_strbuf_t *dim_str = ep_strbuf_new_label("");
    for (i = 0; i < MAX_ARRAY_DIMENSION; ++i) {
      if (arr_dimension[i] != 0) {
        ep_strbuf_append_printf(dim_str, "[%d]", arr_dimension[i]);
      } else {
        break;
      }
    }
    proto_tree_add_text(tree,
                  tvb,
                  offset_begin,
                  (offset-offset_begin),
                  "%s%s %s%s;%s",
                  indent_string,
                  type_name,
                  name ? name : "",
                  dim_str->str,
                  is_key ? KEY_COMMENT : "");
    return retVal;
  }

  /
  if (bitfield != 0xffff && name != NULL && is_pointer == 0) {
    proto_tree_add_text(tree,
                  tvb,
                  offset_begin,
                  (offset-offset_begin),
                  "%s%s %s:%d;%s",
                  indent_string,
                  type_name,
                  name ? name : "",
                  bitfield,
                  is_key ? KEY_COMMENT : "");
    return retVal;
  }

  /
  proto_tree_add_text(tree,
                  tvb,
                  offset_begin,
                  (offset-offset_begin),
                  "%s%s%s%s%s;%s",
                  indent_string,
                  type_name,
                  name ? " " : "",
                  is_pointer ? "*" : "",
                  name ? name : "",
                  is_key ? KEY_COMMENT : "");
  return retVal;
}
