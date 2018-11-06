static void
get_amqp_1_0_value_formatter(tvbuff_t *tvb,
                             packet_info *pinfo,
                             guint8 code,
                             int offset,
                             int hf_amqp_type,
                             const char *name,
                             guint32 hf_amqp_subtype_count,
                             const int **hf_amqp_subtypes,
                             guint *length_size,
                             proto_item *item)
{
    struct amqp1_typeinfo* element_type;
    const char *value = NULL;

    element_type = decode_fixed_type(code);
    if (element_type)
    {
        struct amqp_synonym_types_t *synonyms;
        int shift_view = 0;

        /
        if (proto_registrar_get_ftype(hf_amqp_type) == FT_NONE)
        {
            for (synonyms = amqp_synonym_types; synonyms->hf_none != NULL; synonyms++)
            {
                if (*(synonyms->hf_none) == hf_amqp_type)
                {
                    if (IS_FT_UINT(element_type->ftype) && synonyms->hf_uint != NULL)
                        hf_amqp_type = *(synonyms->hf_uint);
                    else if (IS_FT_STRING(element_type->ftype) && synonyms->hf_str != NULL)
                        hf_amqp_type = *(synonyms->hf_str);
                    else if (element_type->ftype == FT_BYTES && synonyms->hf_bin != NULL)
                        hf_amqp_type = *(synonyms->hf_bin);
                    else if (element_type->ftype == FT_GUID && synonyms->hf_guid != NULL)
                        hf_amqp_type = *(synonyms->hf_guid);
                    break;
                }
            }
        }

        if (proto_registrar_get_ftype(hf_amqp_type) != FT_NONE)
        {
            /
            *length_size = element_type->dissector(tvb, pinfo,
                                                   offset,
                                                   element_type->known_size,
                                                   item, hf_amqp_type);
        }
        else if(code == AMQP_1_0_TYPE_NULL)
        {
            /
            if(hf_amqp_type == hf_amqp_1_0_list)
            {
                proto_tree_add_none_format(item, hf_amqp_type,
                                           tvb,
                                           offset-1,
                                           1,
                                           "%s: (null)",
                                           name ? name : proto_registrar_get_name(hf_amqp_type));
            }
        }
        else
        {
            /
            *length_size = element_type->formatter(tvb, offset, element_type->known_size, &value);

            if (code/16 > 0x9) /
               /
               shift_view = element_type->known_size;
            else if(*length_size == 0)
                /
                shift_view = -1;

            proto_tree_add_none_format(item, hf_amqp_type,
                                       tvb,
                                       offset+shift_view,
                                       (*length_size)-shift_view,
                                       "%s (%s): %s",
                                       name ? name : proto_registrar_get_name(hf_amqp_type),
                                       element_type->amqp_typename, value);
        }
    }
    else { /
        switch (code) {
            case AMQP_1_0_TYPE_LIST0:
            case AMQP_1_0_TYPE_LIST8:
            case AMQP_1_0_TYPE_LIST32:
                *length_size = dissect_amqp_1_0_list(tvb,
                                                     pinfo,
                                                     offset-1, /
                                                     item,
                                                     hf_amqp_type,
                                                     hf_amqp_subtype_count,
                                                     hf_amqp_subtypes, name)-1; /
                break;
            case AMQP_1_0_TYPE_MAP8:
            case AMQP_1_0_TYPE_MAP32:
                 /
                *length_size = dissect_amqp_1_0_map(tvb, pinfo, offset-1, item, hf_amqp_type, name)-1;
                break;
            case AMQP_1_0_TYPE_ARRAY8:
            case AMQP_1_0_TYPE_ARRAY32:
                *length_size = dissect_amqp_1_0_array(tvb,
                                                      pinfo,
                                                      offset-1, /
                                                      item,
                                                      hf_amqp_type,
                                                      hf_amqp_subtype_count,
                                                      hf_amqp_subtypes, name)-1; /
                break;
            default:
                expert_add_info_format(pinfo,
                                       item,
                                       &ei_amqp_unknown_amqp_type,
                                       "Unknown AMQP type %d (0x%x) of field \"%s\"",
                                       code, code,
                                       name ? name : proto_registrar_get_name(hf_amqp_type));
                *length_size = tvb_reported_length_remaining(tvb, offset); /
                break;
        }
    }
}
