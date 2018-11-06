static int commit_one_file(BDRVVVFATState* s,
	int dir_index, uint32_t offset)
{
    direntry_t* direntry = array_get(&(s->directory), dir_index);
    uint32_t c = begin_of_direntry(direntry);
    uint32_t first_cluster = c;
    mapping_t* mapping = find_mapping_for_cluster(s, c);
    uint32_t size = filesize_of_direntry(direntry);
    char* cluster = qemu_malloc(s->cluster_size);
    uint32_t i;
    int fd = 0;

    assert(offset < size);
    assert((offset % s->cluster_size) == 0);

    for (i = s->cluster_size; i < offset; i += s->cluster_size)
	c = modified_fat_get(s, c);

    fd = open(mapping->path, O_RDWR | O_CREAT | O_BINARY, 0666);
    if (fd < 0) {
	fprintf(stderr, "Could not open %s... (%s, %d)\n", mapping->path,
		strerror(errno), errno);
	return fd;
    }
    if (offset > 0)
	if (lseek(fd, offset, SEEK_SET) != offset)
	    return -3;

    while (offset < size) {
	uint32_t c1;
	int rest_size = (size - offset > s->cluster_size ?
		s->cluster_size : size - offset);
	int ret;

	c1 = modified_fat_get(s, c);

	assert((size - offset == 0 && fat_eof(s, c)) ||
		(size > offset && c >=2 && !fat_eof(s, c)));

	ret = vvfat_read(s->bs, cluster2sector(s, c),
	    (uint8_t*)cluster, (rest_size + 0x1ff) / 0x200);

	if (ret < 0)
	    return ret;

	if (write(fd, cluster, rest_size) < 0)
	    return -2;

	offset += rest_size;
	c = c1;
    }

    ftruncate(fd, size);
    close(fd);

    return commit_mappings(s, first_cluster, dir_index);
}
