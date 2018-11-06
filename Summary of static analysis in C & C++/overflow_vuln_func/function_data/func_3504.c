static void down_heap(uint32_t nr_heap, uint32_t *heap, uint32_t *weights)
{
    uint32_t val = 1;
    uint32_t val2;
    uint32_t initial_val = heap[val];

    while (1) {
        val2 = val << 1;

        if (val2 > nr_heap)
            break;

        if (val2 < nr_heap && weights[heap[val2 + 1]] < weights[heap[val2]])
            val2++;

        if (weights[initial_val] < weights[heap[val2]])
            break;

        heap[val] = heap[val2];

        val = val2;
    }

    heap[val] = initial_val;
}
