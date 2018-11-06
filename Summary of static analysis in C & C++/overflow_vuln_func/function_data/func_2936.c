static void increase_dynamic_storage(IVShmemState *s, int new_min_size) {

    int j, old_nb_alloc;

    old_nb_alloc = s->nb_peers;

    while (new_min_size >= s->nb_peers)
        s->nb_peers = s->nb_peers * 2;

    IVSHMEM_DPRINTF("bumping storage to %d guests\n", s->nb_peers);
    s->peers = g_realloc(s->peers, s->nb_peers * sizeof(Peer));

    /
    for (j = old_nb_alloc; j < s->nb_peers; j++) {
        s->peers[j].eventfds = NULL;
        s->peers[j].nb_eventfds = 0;
    }
}
