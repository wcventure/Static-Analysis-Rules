static int check_directory_consistency(BDRVVVFATState *s,
	int cluster_num, const char* path)
{
    int ret = 0;
    unsigned char* cluster = g_malloc(s->cluster_size);
    direntry_t* direntries = (direntry_t*)cluster;
    mapping_t* mapping = find_mapping_for_cluster(s, cluster_num);

    long_file_name lfn;
    int path_len = strlen(path);
    char path2[PATH_MAX];

    assert(path_len < PATH_MAX); /
    pstrcpy(path2, sizeof(path2), path);
    path2[path_len] = '/';
    path2[path_len + 1] = '\0';

    if (mapping) {
	const char* basename = get_basename(mapping->path);
	const char* basename2 = get_basename(path);

	assert(mapping->mode & MODE_DIRECTORY);

	assert(mapping->mode & MODE_DELETED);
	mapping->mode &= ~MODE_DELETED;

	if (strcmp(basename, basename2))
	    schedule_rename(s, cluster_num, g_strdup(path));
    } else
	/
	schedule_mkdir(s, cluster_num, g_strdup(path));

    lfn_init(&lfn);
    do {
	int i;
	int subret = 0;

	ret++;

	if (s->used_clusters[cluster_num] & USED_ANY) {
	    fprintf(stderr, "cluster %d used more than once\n", (int)cluster_num);
	    return 0;
	}
	s->used_clusters[cluster_num] = USED_DIRECTORY;

DLOG(fprintf(stderr, "read cluster %d (sector %d)\n", (int)cluster_num, (int)cluster2sector(s, cluster_num)));
	subret = vvfat_read(s->bs, cluster2sector(s, cluster_num), cluster,
		s->sectors_per_cluster);
	if (subret) {
	    fprintf(stderr, "Error fetching direntries\n");
	fail:
            g_free(cluster);
	    return 0;
	}

	for (i = 0; i < 0x10 * s->sectors_per_cluster; i++) {
	    int cluster_count = 0;

DLOG(fprintf(stderr, "check direntry %d:\n", i); print_direntry(direntries + i));
	    if (is_volume_label(direntries + i) || is_dot(direntries + i) ||
		    is_free(direntries + i))
		continue;

	    subret = parse_long_name(&lfn, direntries + i);
	    if (subret < 0) {
		fprintf(stderr, "Error in long name\n");
		goto fail;
	    }
	    if (subret == 0 || is_free(direntries + i))
		continue;

	    if (fat_chksum(direntries+i) != lfn.checksum) {
		subret = parse_short_name(s, &lfn, direntries + i);
		if (subret < 0) {
		    fprintf(stderr, "Error in short name (%d)\n", subret);
		    goto fail;
		}
		if (subret > 0 || !strcmp((char*)lfn.name, ".")
			|| !strcmp((char*)lfn.name, ".."))
		    continue;
	    }
	    lfn.checksum = 0x100; /

	    if (path_len + 1 + lfn.len >= PATH_MAX) {
		fprintf(stderr, "Name too long: %s/%s\n", path, lfn.name);
		goto fail;
	    }
            pstrcpy(path2 + path_len + 1, sizeof(path2) - path_len - 1,
                    (char*)lfn.name);

	    if (is_directory(direntries + i)) {
		if (begin_of_direntry(direntries + i) == 0) {
		    DLOG(fprintf(stderr, "invalid begin for directory: %s\n", path2); print_direntry(direntries + i));
		    goto fail;
		}
		cluster_count = check_directory_consistency(s,
			begin_of_direntry(direntries + i), path2);
		if (cluster_count == 0) {
		    DLOG(fprintf(stderr, "problem in directory %s:\n", path2); print_direntry(direntries + i));
		    goto fail;
		}
	    } else if (is_file(direntries + i)) {
		/
		cluster_count = get_cluster_count_for_direntry(s, direntries + i, path2);
		if (cluster_count !=
			(le32_to_cpu(direntries[i].size) + s->cluster_size
			 - 1) / s->cluster_size) {
		    DLOG(fprintf(stderr, "Cluster count mismatch\n"));
		    goto fail;
		}
	    } else
                abort(); /

	    ret += cluster_count;
	}

	cluster_num = modified_fat_get(s, cluster_num);
    } while(!fat_eof(s, cluster_num));

    g_free(cluster);
    return ret;
}
