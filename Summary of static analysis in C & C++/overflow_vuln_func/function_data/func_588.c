void
capture_opts_log(const char *log_domain, GLogLevelFlags log_level, capture_options *capture_opts) {
    guint i;

    g_log(log_domain, log_level, "CAPTURE OPTIONS     :");

    for (i = 0; i < capture_opts->ifaces->len; i++) {
        interface_options interface_opts;

        interface_opts = g_array_index(capture_opts->ifaces, interface_options, i);
        g_log(log_domain, log_level, "Interface name[%02d]  : %s", i, interface_opts.name ? interface_opts.name : "(unspecified)");
        g_log(log_domain, log_level, "Interface description[%02d] : %s", i, interface_opts.descr ? interface_opts.descr : "(unspecified)");
        g_log(log_domain, log_level, "Console display name[%02d]: %s", i, interface_opts.console_display_name ? interface_opts.console_display_name : "(unspecified)");
        g_log(log_domain, log_level, "Capture filter[%02d]  : %s", i, interface_opts.cfilter ? interface_opts.cfilter : "(unspecified)");
        g_log(log_domain, log_level, "Snap length[%02d] (%u) : %d", i, interface_opts.has_snaplen, interface_opts.snaplen);
        g_log(log_domain, log_level, "Link Type[%02d]       : %d", i, interface_opts.linktype);
        g_log(log_domain, log_level, "Promiscuous Mode[%02d]: %s", i, interface_opts.promisc_mode?"TRUE":"FALSE");
#if defined(_WIN32) || defined(HAVE_PCAP_CREATE)
        g_log(log_domain, log_level, "Buffer size[%02d]     : %d (MB)", i, interface_opts.buffer_size);
#endif
        g_log(log_domain, log_level, "Monitor Mode[%02d]    : %s", i, interface_opts.monitor_mode?"TRUE":"FALSE");
#ifdef HAVE_PCAP_REMOTE
        g_log(log_domain, log_level, "Capture source[%02d]  : %s", i,
            interface_opts.src_type == CAPTURE_IFLOCAL ? "Local interface" :
            interface_opts.src_type == CAPTURE_IFREMOTE ? "Remote interface" :
            "Unknown");
        if (interface_opts.src_type == CAPTURE_IFREMOTE) {
            g_log(log_domain, log_level, "Remote host[%02d]     : %s", i, interface_opts.remote_host ? interface_opts.remote_host : "(unspecified)");
            g_log(log_domain, log_level, "Remote port[%02d]     : %s", i, interface_opts.remote_port ? interface_opts.remote_port : "(unspecified)");
        }
        g_log(log_domain, log_level, "Authentication[%02d]  : %s", i,
            interface_opts.auth_type == CAPTURE_AUTH_NULL ? "Null" :
            interface_opts.auth_type == CAPTURE_AUTH_PWD ? "By username/password" :
            "Unknown");
        if (interface_opts.auth_type == CAPTURE_AUTH_PWD) {
            g_log(log_domain, log_level, "Auth username[%02d]   : %s", i, interface_opts.auth_username ? interface_opts.auth_username : "(unspecified)");
            g_log(log_domain, log_level, "Auth password[%02d]   : <hidden>", i);
        }
        g_log(log_domain, log_level, "UDP data tfer[%02d]   : %u", i, interface_opts.datatx_udp);
        g_log(log_domain, log_level, "No cap. RPCAP[%02d]   : %u", i, interface_opts.nocap_rpcap);
        g_log(log_domain, log_level, "No cap. local[%02d]   : %u", i, interface_opts.nocap_local);
#endif
#ifdef HAVE_PCAP_SETSAMPLING
        g_log(log_domain, log_level, "Sampling meth.[%02d]  : %d", i, interface_opts.sampling_method);
        g_log(log_domain, log_level, "Sampling param.[%02d] : %d", i, interface_opts.sampling_param);
#endif
    }
    g_log(log_domain, log_level, "Interface name[df]  : %s", capture_opts->default_options.name ? capture_opts->default_options.name : "(unspecified)");
    g_log(log_domain, log_level, "Interface Descr[df] : %s", capture_opts->default_options.descr ? capture_opts->default_options.descr : "(unspecified)");
    g_log(log_domain, log_level, "Capture filter[df]  : %s", capture_opts->default_options.cfilter ? capture_opts->default_options.cfilter : "(unspecified)");
    g_log(log_domain, log_level, "Snap length[df] (%u) : %d", capture_opts->default_options.has_snaplen, capture_opts->default_options.snaplen);
    g_log(log_domain, log_level, "Link Type[df]       : %d", capture_opts->default_options.linktype);
    g_log(log_domain, log_level, "Promiscuous Mode[df]: %s", capture_opts->default_options.promisc_mode?"TRUE":"FALSE");
#if defined(_WIN32) || defined(HAVE_PCAP_CREATE)
    g_log(log_domain, log_level, "Buffer size[df]     : %d (MB)", capture_opts->default_options.buffer_size);
#endif
    g_log(log_domain, log_level, "Monitor Mode[df]    : %s", capture_opts->default_options.monitor_mode?"TRUE":"FALSE");
#ifdef HAVE_PCAP_REMOTE
    g_log(log_domain, log_level, "Capture source[df]  : %s",
        capture_opts->default_options.src_type == CAPTURE_IFLOCAL ? "Local interface" :
        capture_opts->default_options.src_type == CAPTURE_IFREMOTE ? "Remote interface" :
        "Unknown");
    if (capture_opts->default_options.src_type == CAPTURE_IFREMOTE) {
        g_log(log_domain, log_level, "Remote host[df]     : %s", capture_opts->default_options.remote_host ? capture_opts->default_options.remote_host : "(unspecified)");
        g_log(log_domain, log_level, "Remote port[df]     : %s", capture_opts->default_options.remote_port ? capture_opts->default_options.remote_port : "(unspecified)");
    }
    g_log(log_domain, log_level, "Authentication[df]  : %s",
        capture_opts->default_options.auth_type == CAPTURE_AUTH_NULL ? "Null" :
        capture_opts->default_options.auth_type == CAPTURE_AUTH_PWD ? "By username/password" :
        "Unknown");
    if (capture_opts->default_options.auth_type == CAPTURE_AUTH_PWD) {
        g_log(log_domain, log_level, "Auth username[df]   : %s", capture_opts->default_options.auth_username ? capture_opts->default_options.auth_username : "(unspecified)");
        g_log(log_domain, log_level, "Auth password[df]   : <hidden>");
    }
    g_log(log_domain, log_level, "UDP data tfer[df]   : %u", capture_opts->default_options.datatx_udp);
    g_log(log_domain, log_level, "No cap. RPCAP[df]   : %u", capture_opts->default_options.nocap_rpcap);
    g_log(log_domain, log_level, "No cap. local[df]   : %u", capture_opts->default_options.nocap_local);
#endif
#ifdef HAVE_PCAP_SETSAMPLING
    g_log(log_domain, log_level, "Sampling meth. [df] : %d", capture_opts->default_options.sampling_method);
    g_log(log_domain, log_level, "Sampling param.[df] : %d", capture_opts->default_options.sampling_param);
#endif
    g_log(log_domain, log_level, "SavingToFile        : %u", capture_opts->saving_to_file);
    g_log(log_domain, log_level, "SaveFile            : %s", (capture_opts->save_file) ? capture_opts->save_file : "");
    g_log(log_domain, log_level, "GroupReadAccess     : %u", capture_opts->group_read_access);
    g_log(log_domain, log_level, "Fileformat          : %s", (capture_opts->use_pcapng) ? "PCAPNG" : "PCAP");
    g_log(log_domain, log_level, "RealTimeMode        : %u", capture_opts->real_time_mode);
    g_log(log_domain, log_level, "ShowInfo            : %u", capture_opts->show_info);
    g_log(log_domain, log_level, "QuitAfterCap        : %u", capture_opts->quit_after_cap);

    g_log(log_domain, log_level, "MultiFilesOn        : %u", capture_opts->multi_files_on);
    g_log(log_domain, log_level, "FileDuration    (%u) : %u", capture_opts->has_file_duration, capture_opts->file_duration);
    g_log(log_domain, log_level, "RingNumFiles    (%u) : %u", capture_opts->has_ring_num_files, capture_opts->ring_num_files);

    g_log(log_domain, log_level, "AutostopFiles   (%u) : %u", capture_opts->has_autostop_files, capture_opts->autostop_files);
    g_log(log_domain, log_level, "AutostopPackets (%u) : %u", capture_opts->has_autostop_packets, capture_opts->autostop_packets);
    g_log(log_domain, log_level, "AutostopFilesize(%u) : %u (KB)", capture_opts->has_autostop_filesize, capture_opts->autostop_filesize);
    g_log(log_domain, log_level, "AutostopDuration(%u) : %u", capture_opts->has_autostop_duration, capture_opts->autostop_duration);
}
