static multipart_info_t *
get_multipart_info(packet_info *pinfo, const char *str)
{
    const char *start_boundary, *start_protocol = NULL;
    int len_boundary = 0, len_protocol = 0;
    multipart_info_t *m_info = NULL;
    const char *type = pinfo->match_string;
    char *parameters;
    gint dummy;

    if ((type == NULL) || (str == NULL)) {
        /
        return NULL;
    }

    /
    parameters = unfold_and_compact_mime_header(str, &dummy);

    start_boundary = find_parameter(parameters, "boundary=", &len_boundary);

    if(!start_boundary) {
        return NULL;
    }
    if(strncmp(type, "multipart/encrypted", sizeof("multipart/encrypted")-1) == 0) {
        start_protocol = find_parameter(parameters, "protocol=", &len_protocol);
        if(!start_protocol) {
            return NULL;
        }
    }

    /
    m_info = wmem_new(wmem_packet_scope(), multipart_info_t);
    m_info->type = type;
    m_info->boundary = wmem_strndup(wmem_packet_scope(), start_boundary, len_boundary);
    m_info->boundary_length = len_boundary;
    if(start_protocol) {
        m_info->protocol = wmem_strndup(wmem_packet_scope(), start_protocol, len_protocol);
        m_info->protocol_length = len_protocol;
    } else {
        m_info->protocol = NULL;
        m_info->protocol_length = -1;
    }
    m_info->orig_content_type = NULL;
    m_info->orig_parameters = NULL;

    return m_info;
}
