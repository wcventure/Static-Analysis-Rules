void qemu_anon_ram_free(void *ptr, size_t size)
{
    trace_qemu_anon_ram_free(ptr, size);
    if (ptr) {
        munmap(ptr, size);
    }
}
