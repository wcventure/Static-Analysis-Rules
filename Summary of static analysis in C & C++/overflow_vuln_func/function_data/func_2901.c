int bdrv_open(BlockDriverState *bs, const char *filename, int snapshot)
{
    int fd;
    int64_t size;
    struct cow_header_v2 cow_header;
#ifndef _WIN32
    char template[] = "/tmp/vl.XXXXXX";
    int cow_fd;
    struct stat st;
#endif

    bs->read_only = 0;
    bs->fd = -1;
    bs->cow_fd = -1;
    bs->cow_bitmap = NULL;
    strcpy(bs->filename, filename);

    /
#ifdef _WIN32
    fd = open(filename, O_RDWR | O_BINARY);
#else
    fd = open(filename, O_RDWR | O_LARGEFILE);
#endif
    if (fd < 0) {
        /
#ifdef _WIN32
        fd = open(filename, O_RDONLY | O_BINARY);
#else
        fd = open(filename, O_RDONLY | O_LARGEFILE);
#endif
        if (fd < 0) {
            perror(filename);
            goto fail;
        }
        if (!snapshot)
            bs->read_only = 1;
    }
    bs->fd = fd;

    /
    if (read(fd, &cow_header, sizeof(cow_header)) != sizeof(cow_header)) {
        fprintf(stderr, "%s: could not read header\n", filename);
        goto fail;
    }
#ifndef _WIN32
    if (be32_to_cpu(cow_header.magic) == COW_MAGIC &&
        be32_to_cpu(cow_header.version) == COW_VERSION) {
        /
        size = cow_header.size;
#ifndef WORDS_BIGENDIAN
        size = bswap64(size);
#endif    
        bs->total_sectors = size / 512;

        bs->cow_fd = fd;
        bs->fd = -1;
        if (cow_header.backing_file[0] != '\0') {
            if (stat(cow_header.backing_file, &st) != 0) {
                fprintf(stderr, "%s: could not find original disk image '%s'\n", filename, cow_header.backing_file);
                goto fail;
            }
            if (st.st_mtime != be32_to_cpu(cow_header.mtime)) {
                fprintf(stderr, "%s: original raw disk image '%s' does not match saved timestamp\n", filename, cow_header.backing_file);
                goto fail;
            }
            fd = open(cow_header.backing_file, O_RDONLY | O_LARGEFILE);
            if (fd < 0)
                goto fail;
            bs->fd = fd;
        }
        /
        bs->cow_bitmap_size = ((bs->total_sectors + 7) >> 3) + sizeof(cow_header);
        bs->cow_bitmap_addr = mmap(get_mmap_addr(bs->cow_bitmap_size), 
                                   bs->cow_bitmap_size, 
                                   PROT_READ | PROT_WRITE,
                                   MAP_SHARED, bs->cow_fd, 0);
        if (bs->cow_bitmap_addr == MAP_FAILED)
            goto fail;
        bs->cow_bitmap = bs->cow_bitmap_addr + sizeof(cow_header);
        bs->cow_sectors_offset = (bs->cow_bitmap_size + 511) & ~511;
        snapshot = 0;
    } else 
#endif
    {
        /
        size = lseek64(fd, 0, SEEK_END);
        bs->total_sectors = size / 512;
        bs->fd = fd;
    }

#ifndef _WIN32
    if (snapshot) {
        /
        cow_fd = mkstemp64(template);
        if (cow_fd < 0)
            goto fail;
        bs->cow_fd = cow_fd;
	unlink(template);
        
        /
        bs->cow_bitmap_size = (bs->total_sectors + 7) >> 3;
        bs->cow_bitmap_addr = mmap(get_mmap_addr(bs->cow_bitmap_size), 
                                   bs->cow_bitmap_size, 
                                   PROT_READ | PROT_WRITE,
                                   MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        if (bs->cow_bitmap_addr == MAP_FAILED)
            goto fail;
        bs->cow_bitmap = bs->cow_bitmap_addr;
        bs->cow_sectors_offset = 0;
    }
#endif
    
    bs->inserted = 1;

    /
    if (bs->change_cb)
        bs->change_cb(bs->change_opaque);

    return 0;
 fail:
    bdrv_close(bs);
    return -1;
}
