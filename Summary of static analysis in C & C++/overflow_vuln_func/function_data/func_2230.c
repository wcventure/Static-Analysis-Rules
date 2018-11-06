static void cpu_ioreq_pio(ioreq_t *req)
{
    int i, sign;

    sign = req->df ? -1 : 1;

    if (req->dir == IOREQ_READ) {
        if (!req->data_is_ptr) {
            req->data = do_inp(req->addr, req->size);
        } else {
            uint32_t tmp;

            for (i = 0; i < req->count; i++) {
                tmp = do_inp(req->addr, req->size);
                cpu_physical_memory_write(
                        req->data + (sign * i * (int64_t)req->size),
                        (uint8_t *) &tmp, req->size);
            }
        }
    } else if (req->dir == IOREQ_WRITE) {
        if (!req->data_is_ptr) {
            do_outp(req->addr, req->size, req->data);
        } else {
            for (i = 0; i < req->count; i++) {
                uint32_t tmp = 0;

                cpu_physical_memory_read(
                        req->data + (sign * i * (int64_t)req->size),
                        (uint8_t*) &tmp, req->size);
                do_outp(req->addr, req->size, tmp);
            }
        }
    }
}
