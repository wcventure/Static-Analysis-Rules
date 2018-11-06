int
capture(void)
{
  GtkWidget  *cap_w, *main_vb, *count_lb, *sctp_lb, *tcp_lb, *udp_lb, *icmp_lb,
             *ospf_lb, *gre_lb, *netbios_lb, *ipx_lb, *vines_lb, *other_lb, *stop_bt;
  pcap_t     *pch;
  int         pcap_encap;
  int         snaplen;
  gchar       err_str[PCAP_ERRBUF_SIZE], label_str[32];
  loop_data   ld;
  bpf_u_int32 netnum, netmask;
  time_t      upd_time, cur_time;
  int         err, inpkts;
  char        errmsg[4096+1];
#ifndef _WIN32
  static const char ppamsg[] = "can't find PPA for ";
  char       *libpcap_warn;
#endif
  fd_set      set1;
  struct timeval timeout;
#ifdef linux
  int         pcap_fd = 0;
#endif
#ifdef _WIN32 
  WORD wVersionRequested; 
  WSADATA wsaData; 
#endif
#ifndef _WIN32
  int         pipe_fd = -1;
  struct pcap_hdr hdr;
#endif

  /
#ifdef _WIN32 
  wVersionRequested = MAKEWORD( 1, 1 ); 
  err = WSAStartup( wVersionRequested, &wsaData ); 
  if (err!=0) { 
    snprintf(errmsg, sizeof errmsg, 
      "Couldn't initialize Windows Sockets."); 
	pch=NULL; 
    goto error; 
  } 
#endif 

  ld.go             = TRUE;
  ld.counts.total   = 0;
  ld.max            = cfile.count;
  ld.linktype       = WTAP_ENCAP_UNKNOWN;
  ld.from_pipe      = FALSE;
  ld.sync_packets   = 0;
  ld.counts.sctp    = 0;
  ld.counts.tcp     = 0;
  ld.counts.udp     = 0;
  ld.counts.icmp    = 0;
  ld.counts.ospf    = 0;
  ld.counts.gre     = 0;
  ld.counts.ipx     = 0;
  ld.counts.netbios = 0;
  ld.counts.vines   = 0;
  ld.counts.other   = 0;
  ld.pdh            = NULL;

  /
  pch = pcap_open_live(cfile.iface, cfile.snap, 1, CAP_READ_TIMEOUT, err_str);

  if (pch == NULL) {
#ifdef _WIN32
    /
    if (!capture_child) {
      while (gtk_events_pending()) gtk_main_iteration();
    }

    /
    snprintf(errmsg, sizeof errmsg,
	"The capture session could not be initiated (%s).\n"
	"Please check that you have the proper interface specified.\n"
	"\n"
	"Note that the driver Ethereal uses for packet capture on Windows\n"
	"doesn't support capturing on Token Ring interfaces, and doesn't\n"
	"support capturing on PPP/WAN interfaces in Windows NT/2000.\n",
	err_str);
    goto error;
#else
    /
    pipe_fd = pipe_open_live(cfile.iface, &hdr, &ld, err_str);

    if (pipe_fd == -1) {
      /
      if (!capture_child) {
	while (gtk_events_pending()) gtk_main_iteration();
      }

      /
      if (strncmp(err_str, ppamsg, sizeof ppamsg - 1) == 0)
	libpcap_warn =
	  "\n\n"
	  "You are running Ethereal with a version of the libpcap library\n"
	  "that doesn't handle HP-UX network devices well; this means that\n"
	  "Ethereal may not be able to capture packets.\n"
	  "\n"
	  "To fix this, you will need to download the source to Ethereal\n"
	  "from ethereal.zing.org if you have not already done so, read\n"
	  "the instructions in the \"README.hpux\" file in the source\n"
	  "distribution, download the source to libpcap if you have not\n"
	  "already done so, patch libpcap as per the instructions, rebuild\n"
	  "and install libpcap, and then build Ethereal (if you have already\n"
	  "built Ethereal from source, do a \"make distclean\" and re-run\n"
	  "configure before building).";
      else
	libpcap_warn = "";
      snprintf(errmsg, sizeof errmsg,
	  "The capture session could not be initiated (%s).\n"
	  "Please check to make sure you have sufficient permissions, and that\n"
	  "you have the proper interface or pipe specified.%s", err_str,
	  libpcap_warn);
      goto error;
    }
#endif
  }

  /
  if (cfile.cfilter && !ld.from_pipe) {
    /
    if (pcap_lookupnet (cfile.iface, &netnum, &netmask, err_str) < 0) {
      snprintf(errmsg, sizeof errmsg,
        "Can't use filter:  Couldn't obtain netmask info (%s).", err_str);
      goto error;
    }
    if (pcap_compile(pch, &cfile.fcode, cfile.cfilter, 1, netmask) < 0) {
      snprintf(errmsg, sizeof errmsg, "Unable to parse filter string (%s).",
	pcap_geterr(pch));
      goto error;
    }
    if (pcap_setfilter(pch, &cfile.fcode) < 0) {
      snprintf(errmsg, sizeof errmsg, "Can't install filter (%s).",
	pcap_geterr(pch));
      goto error;
    }
  }

  /
#ifndef _WIN32
  if (ld.from_pipe) {
    pcap_encap = hdr.network;
    snaplen = hdr.snaplen;
  } else
#endif
  {
    pcap_encap = pcap_datalink(pch);
    snaplen = pcap_snapshot(pch);
  }
  ld.linktype = wtap_pcap_encap_to_wtap_encap(pcap_encap);
  if (ld.linktype == WTAP_ENCAP_UNKNOWN) {
    strcpy(errmsg, "The network you're capturing from is of a type"
             " that Ethereal doesn't support.");
    goto error;
  }
  ld.pdh = wtap_dump_fdopen(cfile.save_file_fd, WTAP_FILE_PCAP,
      ld.linktype, snaplen, &err);

  if (ld.pdh == NULL) {
    /
    switch (err) {

    case WTAP_ERR_CANT_OPEN:
      strcpy(errmsg, "The file to which the capture would be saved"
               " couldn't be created for some unknown reason.");
      break;

    case WTAP_ERR_SHORT_WRITE:
      strcpy(errmsg, "A full header couldn't be written to the file"
               " to which the capture would be saved.");
      break;

    default:
      if (err < 0) {
        sprintf(errmsg, "The file to which the capture would be"
                     " saved (\"%s\") could not be opened: Error %d.",
 			cfile.save_file, err);
      } else {
        sprintf(errmsg, "The file to which the capture would be"
                     " saved (\"%s\") could not be opened: %s.",
 			cfile.save_file, strerror(err));
      }
      break;
    }
    goto error;
  }

  /

  if (capture_child) {
    /
    fflush(wtap_dump_file(ld.pdh));
    write(1, "0;", 2);
  }

  cap_w = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(cap_w), "Ethereal: Capture");
  gtk_window_set_modal(GTK_WINDOW(cap_w), TRUE);

  /
  main_vb = gtk_vbox_new(FALSE, 1);
  gtk_container_border_width(GTK_CONTAINER(main_vb), 5);
  gtk_container_add(GTK_CONTAINER(cap_w), main_vb);
  gtk_widget_show(main_vb);

  count_lb = gtk_label_new("Count: 0");
  gtk_box_pack_start(GTK_BOX(main_vb), count_lb, FALSE, FALSE, 3);
  gtk_widget_show(count_lb);

  sctp_lb = gtk_label_new("SCTP: 0 (0.0%)");
  gtk_box_pack_start(GTK_BOX(main_vb), sctp_lb, FALSE, FALSE, 3);
  gtk_widget_show(sctp_lb);

  tcp_lb = gtk_label_new("TCP: 0 (0.0%)");
  gtk_box_pack_start(GTK_BOX(main_vb), tcp_lb, FALSE, FALSE, 3);
  gtk_widget_show(tcp_lb);

  udp_lb = gtk_label_new("UDP: 0 (0.0%)");
  gtk_box_pack_start(GTK_BOX(main_vb), udp_lb, FALSE, FALSE, 3);
  gtk_widget_show(udp_lb);

  icmp_lb = gtk_label_new("ICMP: 0 (0.0%)");
  gtk_box_pack_start(GTK_BOX(main_vb), icmp_lb, FALSE, FALSE, 3);
  gtk_widget_show(icmp_lb);

  ospf_lb = gtk_label_new("OSPF: 0 (0.0%)");
  gtk_box_pack_start(GTK_BOX(main_vb), ospf_lb, FALSE, FALSE, 3);
  gtk_widget_show(ospf_lb);

  gre_lb = gtk_label_new("GRE: 0 (0.0%)");
  gtk_box_pack_start(GTK_BOX(main_vb), gre_lb, FALSE, FALSE, 3);
  gtk_widget_show(gre_lb);

  netbios_lb = gtk_label_new("NetBIOS: 0 (0.0%)");
  gtk_box_pack_start(GTK_BOX(main_vb), netbios_lb, FALSE, FALSE, 3);
  gtk_widget_show(netbios_lb);

  ipx_lb = gtk_label_new("IPX: 0 (0.0%)");
  gtk_box_pack_start(GTK_BOX(main_vb), ipx_lb, FALSE, FALSE, 3);
  gtk_widget_show(ipx_lb);

  vines_lb = gtk_label_new("VINES: 0 (0.0%)");
  gtk_box_pack_start(GTK_BOX(main_vb), vines_lb, FALSE, FALSE, 3);
  gtk_widget_show(vines_lb);

  other_lb = gtk_label_new("Other: 0 (0.0%)");
  gtk_box_pack_start(GTK_BOX(main_vb), other_lb, FALSE, FALSE, 3);
  gtk_widget_show(other_lb);

  /
  stop_bt = gtk_button_new_with_label ("Stop");
  gtk_signal_connect(GTK_OBJECT(stop_bt), "clicked",
    GTK_SIGNAL_FUNC(capture_stop_cb), (gpointer) &ld);
  gtk_signal_connect(GTK_OBJECT(cap_w), "delete_event",
	GTK_SIGNAL_FUNC(capture_delete_cb), (gpointer) &ld);
  gtk_box_pack_end(GTK_BOX(main_vb), stop_bt, FALSE, FALSE, 3);
  GTK_WIDGET_SET_FLAGS(stop_bt, GTK_CAN_DEFAULT);
  gtk_widget_grab_default(stop_bt);
  GTK_WIDGET_SET_FLAGS(stop_bt, GTK_CAN_DEFAULT);
  gtk_widget_grab_default(stop_bt);
  gtk_widget_show(stop_bt);

  gtk_widget_show(cap_w);

  upd_time = time(NULL);
#ifdef linux
  if (!ld.from_pipe) pcap_fd = pcap_fileno(pch);
#endif
  while (ld.go) {
    while (gtk_events_pending()) gtk_main_iteration();

#ifndef _WIN32
    if (ld.from_pipe) {
      FD_ZERO(&set1);
      FD_SET(pipe_fd, &set1);
      timeout.tv_sec = 0;
      timeout.tv_usec = CAP_READ_TIMEOUT*1000;
      if (select(pipe_fd+1, &set1, NULL, NULL, &timeout) != 0) {
	/
	inpkts = pipe_dispatch(pipe_fd, &ld, &hdr);
      } else
	inpkts = 0;
    }
    else
#endif
    {
#ifdef linux
      /
      FD_ZERO(&set1);
      FD_SET(pcap_fd, &set1);
      timeout.tv_sec = 0;
      timeout.tv_usec = CAP_READ_TIMEOUT*1000;
      if (select(pcap_fd+1, &set1, NULL, NULL, &timeout) != 0) {
	/
	inpkts = pcap_dispatch(pch, 1, capture_pcap_cb, (u_char *) &ld);
      } else
	inpkts = 0;
#else
      inpkts = pcap_dispatch(pch, 1, capture_pcap_cb, (u_char *) &ld);
#endif
    }
    if (inpkts > 0)
      ld.sync_packets += inpkts;
    /
    cur_time = time(NULL);
    if (cur_time > upd_time) {
      upd_time = cur_time;

      sprintf(label_str, "Count: %d", ld.counts.total);
      gtk_label_set(GTK_LABEL(count_lb), label_str);

      sprintf(label_str, "SCTP: %d (%.1f%%)", ld.counts.sctp,
                pct(ld.counts.sctp, ld.counts.total));
      gtk_label_set(GTK_LABEL(sctp_lb), label_str);

      sprintf(label_str, "TCP: %d (%.1f%%)", ld.counts.tcp,
		pct(ld.counts.tcp, ld.counts.total));
      gtk_label_set(GTK_LABEL(tcp_lb), label_str);

      sprintf(label_str, "UDP: %d (%.1f%%)", ld.counts.udp,
		pct(ld.counts.udp, ld.counts.total));
      gtk_label_set(GTK_LABEL(udp_lb), label_str);

      sprintf(label_str, "ICMP: %d (%.1f%%)", ld.counts.icmp,
		pct(ld.counts.icmp, ld.counts.total));
      gtk_label_set(GTK_LABEL(icmp_lb), label_str);

      sprintf(label_str, "OSPF: %d (%.1f%%)", ld.counts.ospf,
		pct(ld.counts.ospf, ld.counts.total));
      gtk_label_set(GTK_LABEL(ospf_lb), label_str);

      sprintf(label_str, "GRE: %d (%.1f%%)", ld.counts.gre,
		pct(ld.counts.gre, ld.counts.total));
      gtk_label_set(GTK_LABEL(gre_lb), label_str);

      sprintf(label_str, "NetBIOS: %d (%.1f%%)", ld.counts.netbios,
		pct(ld.counts.netbios, ld.counts.total));
      gtk_label_set(GTK_LABEL(netbios_lb), label_str);

      sprintf(label_str, "IPX: %d (%.1f%%)", ld.counts.ipx,
		pct(ld.counts.ipx, ld.counts.total));
      gtk_label_set(GTK_LABEL(ipx_lb), label_str);

      sprintf(label_str, "VINES: %d (%.1f%%)", ld.counts.vines,
		pct(ld.counts.vines, ld.counts.total));
      gtk_label_set(GTK_LABEL(vines_lb), label_str);

      sprintf(label_str, "Other: %d (%.1f%%)", ld.counts.other,
		pct(ld.counts.other, ld.counts.total));
      gtk_label_set(GTK_LABEL(other_lb), label_str);

      /
      fflush(wtap_dump_file(ld.pdh));
      if (capture_child && ld.sync_packets) {
	/
	char tmp[20];
	sprintf(tmp, "%d*", ld.sync_packets);
	write(1, tmp, strlen(tmp));
	ld.sync_packets = 0;
      }
    }
  }
    
  if (!wtap_dump_close(ld.pdh, &err)) {
    /
    switch (err) {

    case WTAP_ERR_CANT_CLOSE:
      simple_dialog(ESD_TYPE_WARN, NULL,
        	"The file to which the capture was being saved"
		" couldn't be closed for some unknown reason.");
      break;

    case WTAP_ERR_SHORT_WRITE:
      simple_dialog(ESD_TYPE_WARN, NULL,
		"Not all the data could be written to the file"
		" to which the capture was being saved.");
      break;

    default:
      simple_dialog(ESD_TYPE_WARN, NULL,
		"The file to which the capture was being"
		" saved (\"%s\") could not be closed: %s.",
		cfile.save_file, wtap_strerror(err));
      break;
    }
  }
#ifndef _WIN32
  if (ld.from_pipe)
    close(pipe_fd);
  else
#endif
    pcap_close(pch);

  gtk_grab_remove(GTK_WIDGET(cap_w));
  gtk_widget_destroy(GTK_WIDGET(cap_w));

  return TRUE;

error:
  /
  close(cfile.save_file_fd);

  /
  unlink(cfile.save_file); /
  g_free(cfile.save_file);
  cfile.save_file = NULL;
  if (capture_child) {
    /
    send_errmsg_to_parent(errmsg);
  } else {
    /
    simple_dialog(ESD_TYPE_CRIT, NULL, errmsg);
  }
  if (pch != NULL && !ld.from_pipe)
    pcap_close(pch);

  return FALSE;
}
