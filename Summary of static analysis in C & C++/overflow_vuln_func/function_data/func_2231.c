static void cpu_ioreq_move(ioreq_t *req)
{
    int i, sign;

    sign = req->df ? -1 : 1;

    if (!req->data_is_ptr) {
        if (req->dir == IOREQ_READ) {
            for (i = 0; i < req->count; i++) {
                cpu_physical_memory_read(
                        req->addr + (sign * i * (int64_t)req->size),
                        (uint8_t *) &req->data, req->size);
            }
        } else if (req->dir == IOREQ_WRITE) {
            for (i = 0; i < req->count; i++) {
                cpu_physical_memory_write(
                        req->addr + (sign * i * (int64_t)req->size),
                        (uint8_t *) &req->data, req->size);
            }
        }
    } else {
        uint64_t tmp;

        if (req->dir == IOREQ_READ) {
            for (i = 0; i < req->count; i++) {
                cpu_physical_memory_read(
                        req->addr + (sign * i * (int64_t)req->size),
                        (uint8_t*) &tmp, req->size);
                cpu_physical_memory_write(
                        req->data + (sign * i * (int64_t)req->size),
                        (uint8_t*) &tmp, req->size);
            }
        } else if (req->dir == IOREQ_WRITE) {
            for (i = 0; i < req->count; i++) {
                cpu_physical_memory_read(
                        req->data + (sign * i * (int64_t)req->size),
                        (uint8_t*) &tmp, req->size);
                cpu_physical_memory_write(
                        req->addr + (sign * i * (int64_t)req->size),
                        (uint8_t*) &tmp, req->size);
            }
        }
    }
}
