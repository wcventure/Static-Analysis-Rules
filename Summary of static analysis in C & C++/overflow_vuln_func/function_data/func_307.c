void
ssl_session_init(SslDecryptSession* ssl_session)
{
    ssl_debug_printf("ssl_session_init: initializing ptr %p size %" G_GSIZE_MODIFIER "u\n",
                     (void *)ssl_session, sizeof(SslDecryptSession));

    ssl_session->master_secret.data = ssl_session->_master_secret;
    ssl_session->session_id.data = ssl_session->_session_id;
    ssl_session->client_random.data = ssl_session->_client_random;
    ssl_session->server_random.data = ssl_session->_server_random;
    ssl_session->session_ticket.data = ssl_session->_session_ticket;
    ssl_session->session_ticket.data_len = 0;
    ssl_session->master_secret.data_len = 48;
    ssl_session->server_data_for_iv.data_len = 0;
    ssl_session->server_data_for_iv.data = ssl_session->_server_data_for_iv;
    ssl_session->client_data_for_iv.data_len = 0;
    ssl_session->client_data_for_iv.data = ssl_session->_client_data_for_iv;
    ssl_session->app_data_segment.data=NULL;
    ssl_session->app_data_segment.data_len=0;
    SET_ADDRESS(&ssl_session->srv_addr, AT_NONE, 0, NULL);
    ssl_session->srv_ptype = PT_NONE;
    ssl_session->srv_port = 0;
}
