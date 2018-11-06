static void *qemu_rdma_data_init(const char *host_port, Error **errp)
{
    RDMAContext *rdma = NULL;
    InetSocketAddress *addr;

    if (host_port) {
        rdma = g_malloc0(sizeof(RDMAContext));
        rdma->current_index = -1;
        rdma->current_chunk = -1;

        addr = inet_parse(host_port, NULL);
        if (addr != NULL) {
            rdma->port = atoi(addr->port);
            rdma->host = g_strdup(addr->host);
        } else {
            ERROR(errp, "bad RDMA migration address '%s'", host_port);
            g_free(rdma);
            rdma = NULL;
        }

        qapi_free_InetSocketAddress(addr);
    }

    return rdma;
}
