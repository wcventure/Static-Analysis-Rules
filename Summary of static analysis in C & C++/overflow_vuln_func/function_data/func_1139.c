static gint
process_body_part(proto_tree *tree, tvbuff_t *tvb, multipart_info_t *m_info,
        packet_info *pinfo, gint start, gint idx,
        gboolean *last_boundary)
{
    proto_tree *subtree;
    proto_item *ti;
    gint offset = start, next_offset = 0;
    char *parameters = NULL;
    gint body_start, boundary_start, boundary_line_len;

    gchar *content_type_str = NULL;
    gchar *content_encoding_str = NULL;
    char *filename = NULL;
    char *mimetypename = NULL;
    int  len = 0;
    gboolean last_field = FALSE;
    gboolean is_raw_data = FALSE;

    const guint8 *boundary = (guint8 *)m_info->boundary;
    gint boundary_len = m_info->boundary_length;

    ti = proto_tree_add_item(tree, hf_multipart_part, tvb, start, 0, ENC_ASCII|ENC_NA);
    subtree = proto_item_add_subtree(ti, ett_multipart_body);

    /
    boundary_start = find_next_boundary(tvb, offset, boundary, boundary_len,
            &boundary_line_len, last_boundary);

    if (boundary_start <= 0) {
        return -1;
    }

    /

    while (!last_field)
    {
        gint colon_offset;
        char *hdr_str;
        char *header_str;

        /
        next_offset = imf_find_field_end(tvb, offset, tvb_reported_length_remaining(tvb, offset)+offset, &last_field);
        /
        /
        /
        if (last_field && (next_offset+2) <= boundary_start) {
            /
            next_offset += 2;
        } else if((next_offset-2) == boundary_start) {
            /
            last_field = TRUE;
            next_offset -= 2;
        } else if (next_offset > boundary_start) {
            /
            next_offset = boundary_start;
        }

        hdr_str = tvb_get_string_enc(wmem_packet_scope(), tvb, offset, next_offset - offset, ENC_ASCII);

        header_str = unfold_and_compact_mime_header(hdr_str, &colon_offset);
        if (colon_offset <= 0) {
            /
            next_offset = offset;
            break;
        } else {
            gint hf_index;

            /
            header_str[colon_offset] = '\0';
            hf_index = is_known_multipart_header(header_str, colon_offset);

            if (hf_index == -1) {
                if(isprint_string(hdr_str)) {
                    proto_tree_add_format_text(subtree, tvb, offset, next_offset - offset);
                } else {
                    /
                    next_offset = offset;
                    break;
                }
            } else {
                char *value_str = header_str + colon_offset + 1;

                proto_tree_add_string_format(subtree,
                      hf_header_array[hf_index], tvb,
                      offset, next_offset - offset,
                      (const char *)value_str, "%s",
                      tvb_format_text(tvb, offset, next_offset - offset));

                switch (hf_index) {
                    case POS_ORIGINALCONTENT:
                        {
                            gint semicolon_offset;
                            /
                            char* type_str = find_parameter(value_str, "type=", NULL);
                            if(type_str != NULL) {
                                value_str = type_str;
                            }

                            semicolon_offset = index_of_char(
                                    value_str, ';');

                            if (semicolon_offset > 0) {
                                value_str[semicolon_offset] = '\0';
                                m_info->orig_parameters = wmem_strdup(wmem_packet_scope(),
                                                            value_str + semicolon_offset + 1);
                            }

                            m_info->orig_content_type = wmem_ascii_strdown(wmem_packet_scope(), value_str, -1);
                        }
                        break;
                    case POS_CONTENT_TYPE:
                        {
                            /
                            gint semicolon_offset = index_of_char(
                                    value_str, ';');

                            if (semicolon_offset > 0) {
                                value_str[semicolon_offset] = '\0';
                                parameters = wmem_strdup(wmem_packet_scope(), value_str + semicolon_offset + 1);
                            } else {
                                parameters = NULL;
                            }

                            content_type_str = wmem_ascii_strdown(wmem_packet_scope(), value_str, -1);

                            /
                            proto_item_append_text(ti, " (%s)", content_type_str);

                            /
                            if((mimetypename = find_parameter(parameters, "name=", &len)) != NULL) {
                              mimetypename = g_strndup(mimetypename, len);
                            }

                            if(strncmp(content_type_str, "application/octet-stream",
                                    sizeof("application/octet-stream")-1) == 0) {
                                is_raw_data = TRUE;
                            }

                            /
                            if(m_info->protocol && idx == 0
                                && (is_raw_data || g_ascii_strncasecmp(content_type_str, m_info->protocol,
                                                        strlen(m_info->protocol)) != 0))
                            {
                                return -1;
                            }
                        }
                        break;
                        case POS_CONTENT_TRANSFER_ENCODING:
                        {
                            /
                            gint cr_offset = index_of_char(value_str, '\r');

                            if (cr_offset > 0) {
                                value_str[cr_offset] = '\0';
                            }

                            content_encoding_str = wmem_ascii_strdown(wmem_packet_scope(), value_str, -1);
                        }
                        break;
                        case POS_CONTENT_DISPOSITION:
                            {
                            /
                            if((filename = find_parameter(value_str, "filename=", &len)) != NULL) {
                                filename = g_strndup(filename, len);
                            }
                        }
                        break;
                    default:
                        break;
                }
            }
        }
        offset = next_offset;
    }

    body_start = next_offset;

    /

    {
        gint body_len = boundary_start - body_start;
        tvbuff_t *tmp_tvb = tvb_new_subset_length(tvb, body_start, body_len);
        /
        /
        /
        if(m_info->protocol && idx == 1 && is_raw_data)
        {
            gssapi_encrypt_info_t  encrypt;

            memset(&encrypt, 0, sizeof(encrypt));
            encrypt.decrypt_gssapi_tvb=DECRYPT_GSSAPI_NORMAL;

            dissect_kerberos_encrypted_message(tmp_tvb, pinfo, subtree, &encrypt);

            if(encrypt.gssapi_decrypted_tvb){
                    tmp_tvb = encrypt.gssapi_decrypted_tvb;
                    is_raw_data = FALSE;
                    content_type_str = m_info->orig_content_type;
                    parameters = m_info->orig_parameters;
            } else if(encrypt.gssapi_encrypted_tvb) {
                    tmp_tvb = encrypt.gssapi_encrypted_tvb;
                    proto_tree_add_expert(tree, pinfo, &ei_multipart_decryption_not_possible, tmp_tvb, 0, -1);
            }
        }

        if (!is_raw_data &&
            content_type_str) {

            /
            gboolean dissected;

            /

            if(content_encoding_str && remove_base64_encoding) {

                if(!g_ascii_strncasecmp(content_encoding_str, "base64", 6))
                    tmp_tvb = base64_decode(pinfo, tmp_tvb, filename ? filename : (mimetypename ? mimetypename : content_type_str));

            }

            /
            dissected = dissector_try_string(multipart_media_subdissector_table,
                        content_type_str, tmp_tvb, pinfo, subtree, parameters);
            if (! dissected) {
                /
                dissected = dissector_try_string(media_type_dissector_table,
                        content_type_str, tmp_tvb, pinfo, subtree, parameters);
            }
            if (! dissected) {
                const char *save_match_string = pinfo->match_string;
                pinfo->match_string = content_type_str;
                call_dissector_with_data(media_handle, tmp_tvb, pinfo, subtree, parameters);
                pinfo->match_string = save_match_string;
            }
            parameters = NULL; /
        } else {
            call_data_dissector(tmp_tvb, pinfo, subtree);
        }
        proto_item_set_len(ti, boundary_start - start);
        if (*last_boundary == TRUE) {
           proto_tree_add_item(tree, hf_multipart_last_boundary, tvb, boundary_start, boundary_line_len, ENC_NA|ENC_ASCII);
        } else {
           proto_tree_add_item(tree, hf_multipart_boundary, tvb, boundary_start, boundary_line_len, ENC_NA|ENC_ASCII);
        }

        g_free(filename);
        g_free(mimetypename);

        return boundary_start + boundary_line_len;
    }
}
