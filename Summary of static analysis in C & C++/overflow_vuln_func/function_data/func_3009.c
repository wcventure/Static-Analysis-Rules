static void *file_ram_alloc(RAMBlock *block,
                            ram_addr_t memory,
                            const char *path,
                            Error **errp)
{
    char *filename;
    char *sanitized_name;
    char *c;
    void *area = NULL;
    int fd;
    uint64_t hpagesize;
    Error *local_err = NULL;

    hpagesize = gethugepagesize(path, &local_err);
    if (local_err) {
        error_propagate(errp, local_err);
        goto error;
    }
    block->mr->align = hpagesize;

    if (memory < hpagesize) {
        error_setg(errp, "memory size 0x" RAM_ADDR_FMT " must be equal to "
                   "or larger than huge page size 0x%" PRIx64,
                   memory, hpagesize);
        goto error;
    }

    if (kvm_enabled() && !kvm_has_sync_mmu()) {
        error_setg(errp,
                   "host lacks kvm mmu notifiers, -mem-path unsupported");
        goto error;
    }

    /
    sanitized_name = g_strdup(memory_region_name(block->mr));
    for (c = sanitized_name; *c != '\0'; c++) {
        if (*c == '/')
            *c = '_';
    }

    filename = g_strdup_printf("%s/qemu_back_mem.%s.XXXXXX", path,
                               sanitized_name);
    g_free(sanitized_name);

    fd = mkstemp(filename);
    if (fd < 0) {
        error_setg_errno(errp, errno,
                         "unable to create backing store for hugepages");
        g_free(filename);
        goto error;
    }
    unlink(filename);
    g_free(filename);

    memory = ROUND_UP(memory, hpagesize);

    /
    if (ftruncate(fd, memory)) {
        perror("ftruncate");
    }

    area = mmap(0, memory, PROT_READ | PROT_WRITE,
                (block->flags & RAM_SHARED ? MAP_SHARED : MAP_PRIVATE),
                fd, 0);
    if (area == MAP_FAILED) {
        error_setg_errno(errp, errno,
                         "unable to map backing store for hugepages");
        close(fd);
        goto error;
    }

    if (mem_prealloc) {
        os_mem_prealloc(fd, area, memory);
    }

    block->fd = fd;
    return area;

error:
    if (mem_prealloc) {
        error_report("%s", error_get_pretty(*errp));
        exit(1);
    }
    return NULL;
}
