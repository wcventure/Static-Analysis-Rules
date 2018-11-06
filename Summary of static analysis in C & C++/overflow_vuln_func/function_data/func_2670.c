static NetSocketState *net_socket_fd_init_dgram(VLANState *vlan, int fd, 
                                          int is_connected)
{
    struct sockaddr_in saddr;
    int newfd;
    socklen_t saddr_len;
    NetSocketState *s;

    /

    if (is_connected) {
	if (getsockname(fd, (struct sockaddr *) &saddr, &saddr_len) == 0) {
	    /
	    if (saddr.sin_addr.s_addr==0) {
		fprintf(stderr, "qemu: error: init_dgram: fd=%d unbound, cannot setup multicast dst addr\n",
			fd);
		return NULL;
	    }
	    /
	    newfd = net_socket_mcast_create(&saddr);
	    if (newfd < 0) {
		/
		close(fd);
		return NULL;
	    }
	    /
	    dup2(newfd, fd);
	    close(newfd);
	
	} else {
	    fprintf(stderr, "qemu: error: init_dgram: fd=%d failed getsockname(): %s\n",
		    fd, strerror(errno));
	    return NULL;
	}
    }

    s = qemu_mallocz(sizeof(NetSocketState));
    if (!s)
        return NULL;
    s->fd = fd;

    s->vc = qemu_new_vlan_client(vlan, net_socket_receive_dgram, s);
    qemu_set_fd_handler(s->fd, net_socket_send_dgram, NULL, s);

    /
    if (is_connected) s->dgram_dst=saddr;

    snprintf(s->vc->info_str, sizeof(s->vc->info_str),
	    "socket: fd=%d (%s mcast=%s:%d)", 
	    fd, is_connected? "cloned" : "",
	    inet_ntoa(saddr.sin_addr), ntohs(saddr.sin_port));
    return s;
}
