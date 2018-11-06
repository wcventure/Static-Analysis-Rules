static void calculate_code_lengths(uint8_t *lengths, uint32_t *counts)
{
    uint32_t nr_nodes, nr_heap, node1, node2;
    int      i, j;
    int32_t  k;

    /
    uint32_t weights[512];
    uint32_t heap[512];
    int32_t  parents[512];

    /
    for (i = 0; i < 256; i++)
        weights[i + 1] = (counts[i] ? counts[i] : 1) << 8;

    nr_nodes = 256;
    nr_heap  = 0;

    heap[0]    = 0;
    weights[0] = 0;
    parents[0] = -2;

    /
    for (i = 1; i <= 256; i++) {
        parents[i] = -1;

        heap[++nr_heap] = i;
        up_heap(nr_heap, heap, weights);
    }

    /
    while (nr_heap > 1) {
        node1   = heap[1];
        heap[1] = heap[nr_heap--];

        down_heap(nr_heap, heap, weights);

        node2   = heap[1];
        heap[1] = heap[nr_heap--];

        down_heap(nr_heap, heap, weights);

        nr_nodes++;

        parents[node1]    = parents[node2] = nr_nodes;
        weights[nr_nodes] = add_weights(weights[node1], weights[node2]);
        parents[nr_nodes] = -1;

        heap[++nr_heap] = nr_nodes;

        up_heap(nr_heap, heap, weights);
    }

    /
    for (i = 1; i <= 256; i++) {
        j = 0;
        k = i;

        while (parents[k] >= 0) {
            k = parents[k];
            j++;
        }

        lengths[i - 1] = j;
    }
}
