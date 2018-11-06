static void up_heap(uint32_t val, uint32_t *heap, uint32_t *weights)
{
    uint32_t initial_val = heap[val];

    while (weights[initial_val] < weights[heap[val >> 1]]) {
        heap[val] = heap[val >> 1];
        val     >>= 1;
    }

    heap[val] = initial_val;
}
