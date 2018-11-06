static void create_dummy_signal_pipe() {
    gchar *dummy_signal_pipe_name;
    
    if (dummy_signal_pipe != NULL) return;
    
    if (!dummy_control_id) {
	dummy_control_id = g_strdup_printf("%d.dummy", GetCurrentProcessId());
    }
    
    /
    dummy_signal_pipe_name = g_strdup_printf(SIGNAL_PIPE_FORMAT, dummy_control_id);
    dummy_signal_pipe = CreateNamedPipe(utf_8to16(dummy_signal_pipe_name),
                                  PIPE_ACCESS_OUTBOUND, PIPE_TYPE_BYTE, 1, 65535, 65535, 0, NULL);
    g_free(dummy_signal_pipe_name);
}
