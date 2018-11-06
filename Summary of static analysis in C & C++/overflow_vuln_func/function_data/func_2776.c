static void *file_ram_alloc(RAMBlock *block,
                            ram_addr_t memory,
                            const char *path)
{
    char *filename;
    char *sanitized_name;
    char *c;
    void *area;
    int fd;
    unsigned long hpagesize;

    hpagesize = gethugepagesize(path);
    if (!hpagesize) {
        return NULL;
    }

    if (memory < hpagesize) {
        return NULL;
    }

    if (kvm_enabled() && !kvm_has_sync_mmu()) {
        fprintf(stderr, "host lacks kvm mmu notifiers, -mem-path unsupported\n");
        return NULL;
    }

    /
    sanitized_name = g_strdup(block->mr->name);
    for (c = sanitized_name; *c != '\0'; c++) {
        if (*c == '/')
            *c = '_';
    }

    filename = g_strdup_printf("%s/qemu_back_mem.%s.XXXXXX", path,
                               sanitized_name);
    g_free(sanitized_name);

    fd = mkstemp(filename);
    if (fd < 0) {
        perror("unable to create backing store for hugepages");
        g_free(filename);
        return NULL;
    }
    unlink(filename);
    g_free(filename);

    memory = (memory+hpagesize-1) & ~(hpagesize-1);

    /
    if (ftruncate(fd, memory))
        perror("ftruncate");

    area = mmap(0, memory, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
    if (area == MAP_FAILED) {
        perror("file_ram_alloc: can't mmap RAM pages");
        close(fd);
        return (NULL);
    }

    if (mem_prealloc) {
        int ret, i;
        struct sigaction act, oldact;
        sigset_t set, oldset;

        memset(&act, 0, sizeof(act));
        act.sa_handler = &sigbus_handler;
        act.sa_flags = 0;

        ret = sigaction(SIGBUS, &act, &oldact);
        if (ret) {
            perror("file_ram_alloc: failed to install signal handler");
            exit(1);
        }

        /
        sigemptyset(&set);
        sigaddset(&set, SIGBUS);
        pthread_sigmask(SIG_UNBLOCK, &set, &oldset);

        if (sigsetjmp(sigjump, 1)) {
            fprintf(stderr, "file_ram_alloc: failed to preallocate pages\n");
            exit(1);
        }

        /
        for (i = 0; i < (memory/hpagesize)-1; i++) {
            memset(area + (hpagesize*i), 0, 1);
        }

        ret = sigaction(SIGBUS, &oldact, NULL);
        if (ret) {
            perror("file_ram_alloc: failed to reinstall signal handler");
            exit(1);
        }

        pthread_sigmask(SIG_SETMASK, &oldset, NULL);
    }

    block->fd = fd;
    return area;
}
