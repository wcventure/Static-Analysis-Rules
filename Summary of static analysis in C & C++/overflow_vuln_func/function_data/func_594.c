static int
sync_pipe_run_command(char** argv, gchar **data, gchar **primary_msg,
                      gchar **secondary_msg, void (*update_cb)(void))
{
    int ret, i;
    GTimeVal start_time;
    GTimeVal end_time;
    float elapsed;
    int logging_enabled;
    
    /
    logging_enabled=( (G_LOG_LEVEL_DEBUG | G_LOG_LEVEL_INFO) & G_LOG_LEVEL_MASK & prefs.console_log_level);
    if(logging_enabled){
        g_get_current_time(&start_time);
        g_log(LOG_DOMAIN_CAPTURE, G_LOG_LEVEL_INFO, "sync_pipe_run_command() starts");
        for(i=0; argv[i] != 0; i++) {
            g_log(LOG_DOMAIN_CAPTURE, G_LOG_LEVEL_DEBUG, "  argv[%d]: %s", i, argv[i]);
        }
    }
    /
    ret=sync_pipe_run_command_actual(argv, data, primary_msg, secondary_msg, update_cb);

    if(logging_enabled){
        g_get_current_time(&end_time);
        elapsed = (float) ((end_time.tv_sec - start_time.tv_sec) +
                           ((end_time.tv_usec - start_time.tv_usec) / 1e6));

        g_log(LOG_DOMAIN_CAPTURE, G_LOG_LEVEL_INFO, "sync_pipe_run_command() ends, taking %.3fs, result=%d", elapsed, ret);

    }
    return ret;
}
