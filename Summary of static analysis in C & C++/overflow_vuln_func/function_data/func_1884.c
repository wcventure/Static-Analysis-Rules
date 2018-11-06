static int init_directories(BDRVVVFATState* s,
	const char* dirname)
{
    bootsector_t* bootsector;
    mapping_t* mapping;
    unsigned int i;
    unsigned int cluster;

    memset(&(s->first_sectors[0]),0,0x40*0x200);

    s->cluster_size=s->sectors_per_cluster*0x200;
    s->cluster_buffer=qemu_malloc(s->cluster_size);

    /
    i = 1+s->sectors_per_cluster*0x200*8/s->fat_type;
    s->sectors_per_fat=(s->sector_count+i)/i; /

    array_init(&(s->mapping),sizeof(mapping_t));
    array_init(&(s->directory),sizeof(direntry_t));

    /
    {
	direntry_t* entry=array_get_next(&(s->directory));
	entry->attributes=0x28; /
	snprintf((char*)entry->name,11,"QEMU VVFAT");
    }

    /
    init_fat(s);

    s->faked_sectors=s->first_sectors_number+s->sectors_per_fat*2;
    s->cluster_count=sector2cluster(s, s->sector_count);

    mapping = array_get_next(&(s->mapping));
    mapping->begin = 0;
    mapping->dir_index = 0;
    mapping->info.dir.parent_mapping_index = -1;
    mapping->first_mapping_index = -1;
    mapping->path = strdup(dirname);
    i = strlen(mapping->path);
    if (i > 0 && mapping->path[i - 1] == '/')
	mapping->path[i - 1] = '\0';
    mapping->mode = MODE_DIRECTORY;
    mapping->read_only = 0;
    s->path = mapping->path;

    for (i = 0, cluster = 0; i < s->mapping.next; i++) {
	/
	/
	int fix_fat = (i != 0);
	mapping = array_get(&(s->mapping), i);

        if (mapping->mode & MODE_DIRECTORY) {
	    mapping->begin = cluster;
	    if(read_directory(s, i)) {
		fprintf(stderr, "Could not read directory %s\n",
			mapping->path);
		return -1;
	    }
	    mapping = array_get(&(s->mapping), i);
	} else {
	    assert(mapping->mode == MODE_UNDEFINED);
	    mapping->mode=MODE_NORMAL;
	    mapping->begin = cluster;
	    if (mapping->end > 0) {
		direntry_t* direntry = array_get(&(s->directory),
			mapping->dir_index);

		mapping->end = cluster + 1 + (mapping->end-1)/s->cluster_size;
		set_begin_of_direntry(direntry, mapping->begin);
	    } else {
		mapping->end = cluster + 1;
		fix_fat = 0;
	    }
	}

	assert(mapping->begin < mapping->end);

	/
	cluster = mapping->end;

	if(cluster > s->cluster_count) {
	    fprintf(stderr,"Directory does not fit in FAT%d (capacity %s)\n",
		    s->fat_type,
		    s->fat_type == 12 ? s->sector_count == 2880 ? "1.44 MB"
								: "2.88 MB"
				      : "504MB");
	    return -EINVAL;
	}

	/
	if (fix_fat) {
	    int j;
	    for(j = mapping->begin; j < mapping->end - 1; j++)
		fat_set(s, j, j+1);
	    fat_set(s, mapping->end - 1, s->max_fat_value);
	}
    }

    mapping = array_get(&(s->mapping), 0);
    s->sectors_of_root_directory = mapping->end * s->sectors_per_cluster;
    s->last_cluster_of_root_directory = mapping->end;

    /
    fat_set(s,0,s->max_fat_value);
    fat_set(s,1,s->max_fat_value);

    s->current_mapping = NULL;

    bootsector=(bootsector_t*)(s->first_sectors+(s->first_sectors_number-1)*0x200);
    bootsector->jump[0]=0xeb;
    bootsector->jump[1]=0x3e;
    bootsector->jump[2]=0x90;
    memcpy(bootsector->name,"QEMU    ",8);
    bootsector->sector_size=cpu_to_le16(0x200);
    bootsector->sectors_per_cluster=s->sectors_per_cluster;
    bootsector->reserved_sectors=cpu_to_le16(1);
    bootsector->number_of_fats=0x2; /
    bootsector->root_entries=cpu_to_le16(s->sectors_of_root_directory*0x10);
    bootsector->total_sectors16=s->sector_count>0xffff?0:cpu_to_le16(s->sector_count);
    bootsector->media_type=(s->fat_type!=12?0xf8:s->sector_count==5760?0xf9:0xf8); /
    s->fat.pointer[0] = bootsector->media_type;
    bootsector->sectors_per_fat=cpu_to_le16(s->sectors_per_fat);
    bootsector->sectors_per_track=cpu_to_le16(s->bs->secs);
    bootsector->number_of_heads=cpu_to_le16(s->bs->heads);
    bootsector->hidden_sectors=cpu_to_le32(s->first_sectors_number==1?0:0x3f);
    bootsector->total_sectors=cpu_to_le32(s->sector_count>0xffff?s->sector_count:0);

    /
    bootsector->u.fat16.drive_number=s->fat_type==12?0:0x80; /
    bootsector->u.fat16.current_head=0;
    bootsector->u.fat16.signature=0x29;
    bootsector->u.fat16.id=cpu_to_le32(0xfabe1afd);

    memcpy(bootsector->u.fat16.volume_label,"QEMU VVFAT ",11);
    memcpy(bootsector->fat_type,(s->fat_type==12?"FAT12   ":s->fat_type==16?"FAT16   ":"FAT32   "),8);
    bootsector->magic[0]=0x55; bootsector->magic[1]=0xaa;

    return 0;
}
