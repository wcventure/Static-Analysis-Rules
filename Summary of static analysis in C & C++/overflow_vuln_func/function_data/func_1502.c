static int dissect_jxta_welcome(tvbuff_t * tvb, packet_info * pinfo, proto_tree * tree, address *found_addr, gboolean initiator )
{
    int offset = 0;
    int afterwelcome;
    int first_linelen;
    int available = tvb_reported_length_remaining(tvb, offset);

    if (available < (int)sizeof(JXTA_WELCOME_MSG_SIG)) {
        return available - sizeof(JXTA_WELCOME_MSG_SIG);
    }

    if (0 != tvb_memeql(tvb, 0, JXTA_WELCOME_MSG_SIG, sizeof(JXTA_WELCOME_MSG_SIG))) {
        /
        return 0;
    }

    first_linelen = tvb_find_line_end(tvb, offset, -1, &afterwelcome, gDESEGMENT && pinfo->can_desegment);

    if (-1 == first_linelen) {
        if (available > 4096) {
            /
            return 0;
        } else {
            /
            return -1;
        }
    }

    if (check_col(pinfo->cinfo, COL_INFO)) {
        col_set_str(pinfo->cinfo, COL_INFO, "Welcome Message ");
    }

    {
        gchar * welcomeline = tvb_get_string( tvb, offset, first_linelen );
        gchar** tokens = g_strsplit( welcomeline, " ", 6 );
        gchar** current_token = tokens;
        guint token_offset = offset;
        proto_item *jxta_welcome_tree_item = NULL;
        proto_tree *jxta_welcome_tree = NULL;  

        if (tree) {
           jxta_welcome_tree_item = proto_tree_add_item(tree, hf_jxta_welcome, tvb, offset, afterwelcome, FALSE);
           jxta_welcome_tree = proto_item_add_subtree(jxta_welcome_tree_item, ett_jxta_welcome);
           }
        
        if( jxta_welcome_tree ) {
            proto_item *jxta_welcome_initiator_item = proto_tree_add_boolean(jxta_welcome_tree, hf_jxta_welcome_initiator, tvb, 0, 0, initiator);
            PROTO_ITEM_SET_GENERATED(jxta_welcome_initiator_item);
            }

        if( NULL != *current_token ) {
            if( jxta_welcome_tree ) {
                proto_tree_add_item(jxta_welcome_tree, hf_jxta_welcome_sig, tvb, token_offset, strlen( *current_token ), FALSE);
            }

            token_offset += strlen( *current_token ) + 1;
            current_token++;
            }

        if( NULL != *current_token ) {
            if( jxta_welcome_tree ) {
                proto_tree_add_item(jxta_welcome_tree, hf_jxta_welcome_destAddr, tvb, token_offset, strlen( *current_token ), FALSE);
            }

            token_offset += strlen( *current_token ) + 1;
            current_token++;
            }

        if( NULL != *current_token ) {
            if( jxta_welcome_tree ) {
                proto_tree_add_item(jxta_welcome_tree, hf_jxta_welcome_pubAddr, tvb, token_offset, strlen( *current_token ), FALSE);
            }

            token_offset += strlen( *current_token ) + 1;
            current_token++;
            }

        if( NULL != *current_token ) {
            if( jxta_welcome_tree ) {
                proto_tree_add_item(jxta_welcome_tree, hf_jxta_welcome_peerid, tvb, token_offset, strlen( *current_token ), FALSE);
            }

            if (check_col(pinfo->cinfo, COL_INFO)) {
                col_append_str( pinfo->cinfo, COL_INFO, *current_token );
            }

            if( NULL != found_addr ) {
                found_addr->type = AT_STRINGZ;
                found_addr->len = strlen( *current_token ) + 1;
                found_addr->data = g_strdup( *current_token );
            }

            token_offset += strlen( *current_token ) + 1;
            current_token++;
            }

        if( NULL != *current_token ) {
            if( jxta_welcome_tree ) {
                proto_tree_add_item(jxta_welcome_tree, hf_jxta_welcome_noProp, tvb, token_offset, strlen( *current_token ), FALSE);
            }

            token_offset += strlen( *current_token ) + 1;
            current_token++;
            }

        if( NULL != *current_token ) {
            if( jxta_welcome_tree ) {
                proto_tree_add_item(jxta_welcome_tree, hf_jxta_welcome_version, tvb, token_offset, strlen( *current_token ), FALSE);
            }
            token_offset += strlen( *current_token ) + 1;
            current_token++;
            }

        g_free(welcomeline);
        g_strfreev(tokens);
    }

    col_set_writable(pinfo->cinfo, FALSE);

    return afterwelcome;
}
