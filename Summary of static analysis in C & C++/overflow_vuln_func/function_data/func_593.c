gboolean
sync_pipe_start(capture_options *capture_opts, capture_session *cap_session, void (*update_cb)(void))
{
    char ssnap[ARGV_NUMBER_LEN];
    char scount[ARGV_NUMBER_LEN];
    char sfilesize[ARGV_NUMBER_LEN];
    char sfile_duration[ARGV_NUMBER_LEN];
    char sring_num_files[ARGV_NUMBER_LEN];
    char sautostop_files[ARGV_NUMBER_LEN];
    char sautostop_filesize[ARGV_NUMBER_LEN];
    char sautostop_duration[ARGV_NUMBER_LEN];
#ifdef HAVE_PCAP_REMOTE
    char sauth[256];
#endif
#ifdef HAVE_PCAP_SETSAMPLING
    char ssampling[ARGV_NUMBER_LEN];
#endif

#if defined(_WIN32) || defined(HAVE_PCAP_CREATE)
    char buffer_size[ARGV_NUMBER_LEN];
#endif

#ifdef _WIN32
    HANDLE sync_pipe_read;                  /
    HANDLE sync_pipe_write;                 /
    HANDLE signal_pipe;                     /
    GString *args = g_string_sized_new(200);
    gchar *quoted_arg;
    SECURITY_ATTRIBUTES sa;
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    char control_id[ARGV_NUMBER_LEN];
    gchar *signal_pipe_name;
#else
    char errmsg[1024+1];
    int sync_pipe[2];                       /
    enum PIPES { PIPE_READ, PIPE_WRITE };   /
#endif
    int sync_pipe_read_fd;
    int argc;
    char **argv;
    int i;
    guint j;
    interface_options interface_opts;

    if (capture_opts->ifaces->len > 1)
        capture_opts->use_pcapng = TRUE;
    g_log(LOG_DOMAIN_CAPTURE, G_LOG_LEVEL_DEBUG, "sync_pipe_start");
    capture_opts_log(LOG_DOMAIN_CAPTURE, G_LOG_LEVEL_DEBUG, capture_opts);

    cap_session->fork_child = -1;

    argv = init_pipe_args(&argc);
    if (!argv) {
        /
        report_failure("We don't know where to find dumpcap.");
        return FALSE;
    }

    if (capture_opts->ifaces->len > 1)
        argv = sync_pipe_add_arg(argv, &argc, "-t");

    if (capture_opts->use_pcapng)
        argv = sync_pipe_add_arg(argv, &argc, "-n");
    else
        argv = sync_pipe_add_arg(argv, &argc, "-P");

    if (capture_opts->capture_comment) {
        argv = sync_pipe_add_arg(argv, &argc, "--capture-comment");
        argv = sync_pipe_add_arg(argv, &argc, capture_opts->capture_comment);
    }

    if (capture_opts->multi_files_on) {
        if (capture_opts->has_autostop_filesize) {
            argv = sync_pipe_add_arg(argv, &argc, "-b");
            g_snprintf(sfilesize, ARGV_NUMBER_LEN, "filesize:%d",capture_opts->autostop_filesize);
            argv = sync_pipe_add_arg(argv, &argc, sfilesize);
        }

        if (capture_opts->has_file_duration) {
            argv = sync_pipe_add_arg(argv, &argc, "-b");
            g_snprintf(sfile_duration, ARGV_NUMBER_LEN, "duration:%d",capture_opts->file_duration);
            argv = sync_pipe_add_arg(argv, &argc, sfile_duration);
        }

        if (capture_opts->has_ring_num_files) {
            argv = sync_pipe_add_arg(argv, &argc, "-b");
            g_snprintf(sring_num_files, ARGV_NUMBER_LEN, "files:%d",capture_opts->ring_num_files);
            argv = sync_pipe_add_arg(argv, &argc, sring_num_files);
        }

        if (capture_opts->has_autostop_files) {
            argv = sync_pipe_add_arg(argv, &argc, "-a");
            g_snprintf(sautostop_files, ARGV_NUMBER_LEN, "files:%d",capture_opts->autostop_files);
            argv = sync_pipe_add_arg(argv, &argc, sautostop_files);
        }
    } else {
        if (capture_opts->has_autostop_filesize) {
            argv = sync_pipe_add_arg(argv, &argc, "-a");
            g_snprintf(sautostop_filesize, ARGV_NUMBER_LEN, "filesize:%d",capture_opts->autostop_filesize);
            argv = sync_pipe_add_arg(argv, &argc, sautostop_filesize);
        }
    }

    if (capture_opts->has_autostop_packets) {
        argv = sync_pipe_add_arg(argv, &argc, "-c");
        g_snprintf(scount, ARGV_NUMBER_LEN, "%d",capture_opts->autostop_packets);
        argv = sync_pipe_add_arg(argv, &argc, scount);
    }

    if (capture_opts->has_autostop_duration) {
        argv = sync_pipe_add_arg(argv, &argc, "-a");
        g_snprintf(sautostop_duration, ARGV_NUMBER_LEN, "duration:%d",capture_opts->autostop_duration);
        argv = sync_pipe_add_arg(argv, &argc, sautostop_duration);
    }

    if (capture_opts->group_read_access) {
        argv = sync_pipe_add_arg(argv, &argc, "-g");
    }

    for (j = 0; j < capture_opts->ifaces->len; j++) {
        interface_opts = g_array_index(capture_opts->ifaces, interface_options, j);

        argv = sync_pipe_add_arg(argv, &argc, "-i");
        argv = sync_pipe_add_arg(argv, &argc, interface_opts.name);

        if (interface_opts.cfilter != NULL && strlen(interface_opts.cfilter) != 0) {
            argv = sync_pipe_add_arg(argv, &argc, "-f");
            argv = sync_pipe_add_arg(argv, &argc, interface_opts.cfilter);
        }
        if (interface_opts.snaplen != WTAP_MAX_PACKET_SIZE) {
            argv = sync_pipe_add_arg(argv, &argc, "-s");
            g_snprintf(ssnap, ARGV_NUMBER_LEN, "%d", interface_opts.snaplen);
            argv = sync_pipe_add_arg(argv, &argc, ssnap);
        }

        if (interface_opts.linktype != -1) {
            argv = sync_pipe_add_arg(argv, &argc, "-y");
            argv = sync_pipe_add_arg(argv, &argc, linktype_val_to_name(interface_opts.linktype));
        }

        if (!interface_opts.promisc_mode) {
            argv = sync_pipe_add_arg(argv, &argc, "-p");
        }

#if defined(_WIN32) || defined(HAVE_PCAP_CREATE)
        if (interface_opts.buffer_size != DEFAULT_CAPTURE_BUFFER_SIZE) {
            argv = sync_pipe_add_arg(argv, &argc, "-B");
            g_snprintf(buffer_size, ARGV_NUMBER_LEN, "%d", interface_opts.buffer_size);
            argv = sync_pipe_add_arg(argv, &argc, buffer_size);
        }
#endif

#ifdef HAVE_PCAP_CREATE
        if (interface_opts.monitor_mode) {
            argv = sync_pipe_add_arg(argv, &argc, "-I");
        }
#endif

#ifdef HAVE_PCAP_REMOTE
        if (interface_opts.datatx_udp)
            argv = sync_pipe_add_arg(argv, &argc, "-u");

        if (!interface_opts.nocap_rpcap)
            argv = sync_pipe_add_arg(argv, &argc, "-r");

        if (interface_opts.auth_type == CAPTURE_AUTH_PWD) {
            argv = sync_pipe_add_arg(argv, &argc, "-A");
            g_snprintf(sauth, sizeof(sauth), "%s:%s",
                       interface_opts.auth_username,
                       interface_opts.auth_password);
            argv = sync_pipe_add_arg(argv, &argc, sauth);
        }
#endif

#ifdef HAVE_PCAP_SETSAMPLING
        if (interface_opts.sampling_method != CAPTURE_SAMP_NONE) {
            argv = sync_pipe_add_arg(argv, &argc, "-m");
            g_snprintf(ssampling, ARGV_NUMBER_LEN, "%s:%d",
                       interface_opts.sampling_method == CAPTURE_SAMP_BY_COUNT ? "count" :
                       interface_opts.sampling_method == CAPTURE_SAMP_BY_TIMER ? "timer" :
                       "undef",
                       interface_opts.sampling_param);
            argv = sync_pipe_add_arg(argv, &argc, ssampling);
        }
#endif
    }

    /
#ifndef DEBUG_CHILD
    argv = sync_pipe_add_arg(argv, &argc, "-Z");
#ifdef _WIN32
    g_snprintf(control_id, ARGV_NUMBER_LEN, "%d", GetCurrentProcessId());
    argv = sync_pipe_add_arg(argv, &argc, control_id);
#else
    argv = sync_pipe_add_arg(argv, &argc, SIGNAL_PIPE_CTRL_ID_NONE);
#endif
#endif

    if (capture_opts->save_file) {
        argv = sync_pipe_add_arg(argv, &argc, "-w");
        argv = sync_pipe_add_arg(argv, &argc, capture_opts->save_file);
    }
    for (i = 0; i < argc; i++) {
        g_log(LOG_DOMAIN_CAPTURE, G_LOG_LEVEL_DEBUG, "argv[%d]: %s", i, argv[i]);
    }

#ifdef _WIN32
    /
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = NULL;

    /
    /
    if (! CreatePipe(&sync_pipe_read, &sync_pipe_write, &sa, 5120)) {
        /
        report_failure("Couldn't create sync pipe: %s",
                       win32strerror(GetLastError()));
        for (i = 0; i < argc; i++) {
            g_free( (gpointer) argv[i]);
        }
        g_free( (gpointer) argv);
        return FALSE;
    }

    /
    signal_pipe_name = g_strdup_printf(SIGNAL_PIPE_FORMAT, control_id);
    signal_pipe = CreateNamedPipe(utf_8to16(signal_pipe_name),
                                  PIPE_ACCESS_OUTBOUND, PIPE_TYPE_BYTE, 1, 65535, 65535, 0, NULL);
    g_free(signal_pipe_name);

    if (signal_pipe == INVALID_HANDLE_VALUE) {
        /
        report_failure("Couldn't create signal pipe: %s",
                       win32strerror(GetLastError()));
        for (i = 0; i < argc; i++) {
            g_free( (gpointer) argv[i]);
        }
        g_free( (gpointer) argv);
        return FALSE;
    }

    /
    memset(&si, 0, sizeof(si));
    si.cb           = sizeof(si);
#ifdef DEBUG_CHILD
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow  = SW_SHOW;
#else
    si.dwFlags = STARTF_USESTDHANDLES|STARTF_USESHOWWINDOW;
    si.wShowWindow  = SW_HIDE;  /
    si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
    si.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    si.hStdError = sync_pipe_write;
    /
#endif

    /
    /
    /
    for(i=0; argv[i] != 0; i++) {
        if(i != 0) g_string_append_c(args, ' ');    /
        quoted_arg = protect_arg(argv[i]);
        g_string_append(args, quoted_arg);
        g_free(quoted_arg);
    }

    /
    if(!CreateProcess(NULL, utf_8to16(args->str), NULL, NULL, TRUE,
                      CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi)) {
        report_failure("Couldn't run %s in child process: %s",
                       args->str, win32strerror(GetLastError()));
        CloseHandle(sync_pipe_read);
        CloseHandle(sync_pipe_write);
        for (i = 0; i < argc; i++) {
            g_free( (gpointer) argv[i]);
        }
        g_free( (gpointer) argv);
        return FALSE;
    }
    cap_session->fork_child = (int) pi.hProcess;
    g_string_free(args, TRUE);

    /
    /
    sync_pipe_read_fd = _open_osfhandle( (long) sync_pipe_read, _O_BINARY);

    /
    cap_session->signal_pipe_write_fd = _open_osfhandle( (long) signal_pipe, _O_BINARY);

#else /
    if (pipe(sync_pipe) < 0) {
        /
        report_failure("Couldn't create sync pipe: %s", g_strerror(errno));
        for (i = 0; i < argc; i++) {
            g_free( (gpointer) argv[i]);
        }
        g_free(argv);
        return FALSE;
    }

    if ((cap_session->fork_child = fork()) == 0) {
        /
        dup2(sync_pipe[PIPE_WRITE], 2);
        ws_close(sync_pipe[PIPE_READ]);
        execv(argv[0], argv);
        g_snprintf(errmsg, sizeof errmsg, "Couldn't run %s in child process: %s",
                   argv[0], g_strerror(errno));
        sync_pipe_errmsg_to_parent(2, errmsg, "");

        /
        _exit(1);
    }

    if (fetch_dumpcap_pid && cap_session->fork_child > 0)
        fetch_dumpcap_pid(cap_session->fork_child);

    sync_pipe_read_fd = sync_pipe[PIPE_READ];
#endif

    for (i = 0; i < argc; i++) {
        g_free( (gpointer) argv[i]);
    }

    /
    g_free( (gpointer) argv);   /

    /
#ifdef _WIN32
    CloseHandle(sync_pipe_write);
#else
    ws_close(sync_pipe[PIPE_WRITE]);
#endif

    if (cap_session->fork_child == -1) {
        /
        report_failure("Couldn't create child process: %s", g_strerror(errno));
        ws_close(sync_pipe_read_fd);
#ifdef _WIN32
        ws_close(cap_session->signal_pipe_write_fd);
#endif
        return FALSE;
    }

    cap_session->fork_child_status = 0;
    cap_session->capture_opts = capture_opts;

    /
    if (update_cb) update_cb();

    /

    /
    pipe_input_set_handler(sync_pipe_read_fd, (gpointer) cap_session,
                           &cap_session->fork_child, sync_pipe_input_cb);

    return TRUE;
}
