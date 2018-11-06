static int
ivshmem_server_handle_new_conn(IvshmemServer *server)
{
    IvshmemServerPeer *peer, *other_peer;
    struct sockaddr_un unaddr;
    socklen_t unaddr_len;
    int newfd;
    unsigned i;

    /
    unaddr_len = sizeof(unaddr);
    newfd = qemu_accept(server->sock_fd,
                        (struct sockaddr *)&unaddr, &unaddr_len);

    if (newfd < 0) {
        IVSHMEM_SERVER_DEBUG(server, "cannot accept() %s\n", strerror(errno));
        return -1;
    }

    qemu_set_nonblock(newfd);
    IVSHMEM_SERVER_DEBUG(server, "accept()=%d\n", newfd);

    /
    peer = g_malloc0(sizeof(*peer));
    peer->sock_fd = newfd;

    /
    /
    for (i = 0; i < G_MAXUINT16; i++) {
        if (ivshmem_server_search_peer(server, server->cur_id) == NULL) {
            break;
        }
        server->cur_id++;
    }
    if (i == G_MAXUINT16) {
        IVSHMEM_SERVER_DEBUG(server, "cannot allocate new client id\n");
        goto fail;
    }
    peer->id = server->cur_id++;

    /
    peer->vectors_count = server->n_vectors;
    for (i = 0; i < peer->vectors_count; i++) {
        if (event_notifier_init(&peer->vectors[i], FALSE) < 0) {
            IVSHMEM_SERVER_DEBUG(server, "cannot create eventfd\n");
            goto fail;
        }
    }

    /
    if (ivshmem_server_send_initial_info(server, peer) < 0) {
        IVSHMEM_SERVER_DEBUG(server, "cannot send initial info\n");
        goto fail;
    }

    /
    QTAILQ_FOREACH(other_peer, &server->peer_list, next) {
        for (i = 0; i < peer->vectors_count; i++) {
            ivshmem_server_send_one_msg(other_peer->sock_fd, peer->id,
                                        peer->vectors[i].wfd);
        }
    }

    /
    QTAILQ_FOREACH(other_peer, &server->peer_list, next) {
        for (i = 0; i < peer->vectors_count; i++) {
            ivshmem_server_send_one_msg(peer->sock_fd, other_peer->id,
                                        other_peer->vectors[i].wfd);
        }
    }

    /
    for (i = 0; i < peer->vectors_count; i++) {
        ivshmem_server_send_one_msg(peer->sock_fd, peer->id,
                                    event_notifier_get_fd(&peer->vectors[i]));
    }

    QTAILQ_INSERT_TAIL(&server->peer_list, peer, next);
    IVSHMEM_SERVER_DEBUG(server, "new peer id = %" PRId64 "\n",
                         peer->id);
    return 0;

fail:
    while (i--) {
        event_notifier_cleanup(&peer->vectors[i]);
    }
    close(newfd);
    g_free(peer);
    return -1;
}
