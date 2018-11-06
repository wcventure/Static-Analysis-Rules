static int dissect_jxta_welcome(tvbuff_t * tvb, packet_info * pinfo, proto_tree * tree, address * found_addr, gboolean initiator)
{
    guint offset = 0;
    gint afterwelcome;
    gint first_linelen;
    guint available = tvb_reported_length_remaining(tvb, offset);
    gchar **tokens = NULL;

    if (available < sizeof(JXTA_WELCOME_MSG_SIG)) {
        return (gint) (available - sizeof(JXTA_WELCOME_MSG_SIG));
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

    /

    if (check_col(pinfo->cinfo, COL_PROTOCOL)) {
        col_set_str(pinfo->cinfo, COL_PROTOCOL, "JXTA");
    }

    if (check_col(pinfo->cinfo, COL_INFO)) {
        col_add_str(pinfo->cinfo, COL_INFO, "Welcome");
    }

    {
        gchar *welcomeline = tvb_get_ephemeral_string(tvb, offset, first_linelen);
        gchar **current_token;
        guint token_offset = offset;
        proto_item *jxta_welcome_tree_item = NULL;
        proto_tree *jxta_welcome_tree = NULL;

        tokens = g_strsplit(welcomeline, " ", 255);
        current_token = tokens;

        if (tree) {
            jxta_welcome_tree_item =
                proto_tree_add_none_format(tree, hf_jxta_welcome, tvb, offset, afterwelcome,
                                           "JXTA Connection Welcome Message, %s", welcomeline);
            jxta_welcome_tree = proto_item_add_subtree(jxta_welcome_tree_item, ett_jxta_welcome);
        }

        if (jxta_welcome_tree) {
            proto_item *jxta_welcome_initiator_item =
                proto_tree_add_boolean(jxta_welcome_tree, hf_jxta_welcome_initiator, tvb, 0, 0, initiator);
            PROTO_ITEM_SET_GENERATED(jxta_welcome_initiator_item);
        }

        if (NULL != *current_token) {
            if (jxta_welcome_tree) {
                proto_tree_add_item(jxta_welcome_tree, hf_jxta_welcome_sig, tvb, token_offset, strlen(*current_token), FALSE);
            }

            token_offset += strlen(*current_token) + 1;
            current_token++;
        } else {
            /
            afterwelcome = 0;
            goto Common_Exit;
        }

        if (NULL != *current_token) {
            if (jxta_welcome_tree) {
                proto_tree_add_item(jxta_welcome_tree, hf_jxta_welcome_destAddr, tvb, token_offset, strlen(*current_token),
                                    FALSE);
            }

            token_offset += strlen(*current_token) + 1;
            current_token++;
        } else {
            /
            afterwelcome = 0;
            goto Common_Exit;
        }

        if (NULL != *current_token) {
            if (jxta_welcome_tree) {
                proto_tree_add_item(jxta_welcome_tree, hf_jxta_welcome_pubAddr, tvb, token_offset, strlen(*current_token), FALSE);
            }

            token_offset += strlen(*current_token) + 1;
            current_token++;
        } else {
            /
            afterwelcome = 0;
            goto Common_Exit;
        }

        if (NULL != *current_token) {
            if (jxta_welcome_tree) {
                proto_tree_add_item(jxta_welcome_tree, hf_jxta_welcome_peerid, tvb, token_offset, strlen(*current_token), FALSE);
            }

            if (check_col(pinfo->cinfo, COL_INFO)) {
                col_append_str(pinfo->cinfo, COL_INFO, (initiator ? " -> " : " <- ") );
                col_append_str(pinfo->cinfo, COL_INFO, *current_token);
            }

            if (NULL != found_addr) {
                found_addr->type = AT_URI;
                found_addr->len = strlen(*current_token);
                /
                found_addr->data = g_strdup(*current_token);
            }

            token_offset += strlen(*current_token) + 1;
            current_token++;
        } else {
            /
            afterwelcome = 0;
            goto Common_Exit;
        }

        if (NULL != *current_token) {
            int variable_tokens = 0;
            gchar **variable_token = current_token;
            
            while(NULL != *variable_token) {
                variable_tokens++;
                variable_token++;
            }
        
            if( variable_tokens < 1 ) {
              /
              afterwelcome = 0;
              goto Common_Exit;
            }
            
            if( (2 == variable_tokens) && (0 == strcmp(JXTA_WELCOME_MSG_VERSION_1_1, current_token[variable_tokens -1])) ) {
                  if (jxta_welcome_tree) {
                      proto_tree_add_item(jxta_welcome_tree, hf_jxta_welcome_noProp, tvb, token_offset, strlen(*current_token), FALSE);
                  }

                  token_offset += strlen(*current_token) + 1;
                  current_token++;
                  
                  if (jxta_welcome_tree) {
                      proto_tree_add_item(jxta_welcome_tree, hf_jxta_welcome_version, tvb, token_offset, strlen(*current_token), FALSE);
                  }
            } else {
                /
                int each_variable_token;
                
                for( each_variable_token = 0; each_variable_token < variable_tokens; each_variable_token++ ) {
                  if (jxta_welcome_tree) {
                      jxta_welcome_tree_item = proto_tree_add_item(jxta_welcome_tree, 
                        (each_variable_token < (variable_tokens -1) ? hf_jxta_welcome_variable : hf_jxta_welcome_version),
                        tvb, token_offset, strlen(*current_token), FALSE);
                        
                        proto_item_append_text(jxta_welcome_tree_item, " (UNRECOGNIZED)");
                  }

                  token_offset += strlen(*current_token) + 1;
                  current_token++;
                }
            }
        } else {
            /
            afterwelcome = 0;
            goto Common_Exit;
        }
    }

Common_Exit:
    g_strfreev(tokens);

    col_set_writable(pinfo->cinfo, FALSE);

    return afterwelcome;
}
