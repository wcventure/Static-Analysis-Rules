void
capture_opts_init(capture_options *capture_opts)
{
  capture_opts->ifaces                          = g_array_new(FALSE, FALSE, sizeof(interface_options));
  capture_opts->all_ifaces                      = g_array_new(FALSE, FALSE, sizeof(interface_t));
  capture_opts->num_selected                    = 0;
  capture_opts->default_options.name            = NULL;
  capture_opts->default_options.descr           = NULL;
  capture_opts->default_options.cfilter         = NULL;
  capture_opts->default_options.has_snaplen     = FALSE;
  capture_opts->default_options.snaplen         = WTAP_MAX_PACKET_SIZE;
  capture_opts->default_options.linktype        = -1;
  capture_opts->default_options.promisc_mode    = TRUE;
#if defined(_WIN32) || defined(HAVE_PCAP_CREATE)
  capture_opts->default_options.buffer_size     = DEFAULT_CAPTURE_BUFFER_SIZE;
#endif
  capture_opts->default_options.monitor_mode    = FALSE;
#ifdef HAVE_PCAP_REMOTE
  capture_opts->default_options.src_type        = CAPTURE_IFLOCAL;
  capture_opts->default_options.remote_host     = NULL;
  capture_opts->default_options.remote_port     = NULL;
  capture_opts->default_options.auth_type       = CAPTURE_AUTH_NULL;
  capture_opts->default_options.auth_username   = NULL;
  capture_opts->default_options.auth_password   = NULL;
  capture_opts->default_options.datatx_udp      = FALSE;
  capture_opts->default_options.nocap_rpcap     = TRUE;
  capture_opts->default_options.nocap_local     = FALSE;
#endif
#ifdef HAVE_PCAP_SETSAMPLING
  capture_opts->default_options.sampling_method = CAPTURE_SAMP_NONE;
  capture_opts->default_options.sampling_param  = 0;
#endif
  capture_opts->saving_to_file                  = FALSE;
  capture_opts->save_file                       = NULL;
  capture_opts->group_read_access               = FALSE;
#ifdef PCAP_NG_DEFAULT
  capture_opts->use_pcapng                      = TRUE;             /
#else
  capture_opts->use_pcapng                      = FALSE;            /
#endif
  capture_opts->real_time_mode                  = TRUE;
  capture_opts->show_info                       = TRUE;
  capture_opts->quit_after_cap                  = getenv("WIRESHARK_QUIT_AFTER_CAPTURE") ? TRUE : FALSE;
  capture_opts->restart                         = FALSE;

  capture_opts->multi_files_on                  = FALSE;
  capture_opts->has_file_duration               = FALSE;
  capture_opts->file_duration                   = 60;               /
  capture_opts->has_ring_num_files              = FALSE;
  capture_opts->ring_num_files                  = RINGBUFFER_MIN_NUM_FILES;

  capture_opts->has_autostop_files              = FALSE;
  capture_opts->autostop_files                  = 1;
  capture_opts->has_autostop_packets            = FALSE;
  capture_opts->autostop_packets                = 0;
  capture_opts->has_autostop_filesize           = FALSE;
  capture_opts->autostop_filesize               = 1024;             /
  capture_opts->has_autostop_duration           = FALSE;
  capture_opts->autostop_duration               = 60;               /
  capture_opts->capture_comment                 = NULL;

  capture_opts->output_to_pipe                  = FALSE;
  capture_opts->capture_child                   = FALSE;
}
