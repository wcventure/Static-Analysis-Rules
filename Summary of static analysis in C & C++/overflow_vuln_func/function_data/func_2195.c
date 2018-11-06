void *qemu_anon_ram_alloc(size_t size, uint64_t *alignment)
{
    size_t align = QEMU_VMALLOC_ALIGN;
    size_t total = size + align - getpagesize();
    void *ptr = mmap(0, total, PROT_NONE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    size_t offset = QEMU_ALIGN_UP((uintptr_t)ptr, align) - (uintptr_t)ptr;
    void *ptr1;

    if (ptr == MAP_FAILED) {
        return NULL;
    }

    if (alignment) {
        *alignment = align;
    }

    ptr1 = mmap(ptr + offset, size, PROT_READ | PROT_WRITE,
                MAP_FIXED | MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    if (ptr1 == MAP_FAILED) {
        munmap(ptr, total);
        return NULL;
    }

    ptr += offset;
    total -= offset;

    if (offset > 0) {
        munmap(ptr - offset, offset);
    }
    if (total > size) {
        munmap(ptr + size, total - size);
    }

    trace_qemu_anon_ram_alloc(size, ptr);
    return ptr;
}
