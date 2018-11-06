void t38_add_address(packet_info *pinfo,
                     address *addr, int port,
                     int other_port,
                     gchar *setup_method, guint32 setup_frame_number)
{
        address null_addr;
        conversation_t* p_conv;
        struct _t38_conversation_info *p_conv_data = NULL;

        /
        if (pinfo->fd->flags.visited)
        {
                return;
        }

        SET_ADDRESS(&null_addr, AT_NONE, 0, NULL);

        /
        p_conv = find_conversation( addr, &null_addr, PT_UDP, port, other_port,
                                NO_ADDR_B | (!other_port ? NO_PORT_B : 0));

        /
        if ( ! p_conv ) {
                p_conv = conversation_new( addr, &null_addr, PT_UDP,
                                           (guint32)port, (guint32)other_port,
                                                                   NO_ADDR2 | (!other_port ? NO_PORT2 : 0));
        }

        /
        conversation_set_dissector(p_conv, t38_udp_handle);

        /
        p_conv_data = conversation_get_proto_data(p_conv, proto_t38);

        /
        if ( ! p_conv_data ) {
                /
                p_conv_data = g_mem_chunk_alloc(t38_conversations);

                conversation_add_proto_data(p_conv, proto_t38, p_conv_data);
        }

        /
        strncpy(p_conv_data->method, setup_method, MAX_T38_SETUP_METHOD_SIZE);
        p_conv_data->method[MAX_T38_SETUP_METHOD_SIZE] = '\0';
        p_conv_data->frame_number = setup_frame_number;
}
