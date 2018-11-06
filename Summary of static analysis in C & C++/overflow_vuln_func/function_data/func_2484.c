static int read_directory(BDRVVVFATState* s, int mapping_index)
{
    mapping_t* mapping = array_get(&(s->mapping), mapping_index);
    direntry_t* direntry;
    const char* dirname = mapping->path;
    int first_cluster = mapping->begin;
    int parent_index = mapping->info.dir.parent_mapping_index;
    mapping_t* parent_mapping = (mapping_t*)
        (parent_index >= 0 ? array_get(&(s->mapping), parent_index) : NULL);
    int first_cluster_of_parent = parent_mapping ? parent_mapping->begin : -1;

    DIR* dir=opendir(dirname);
    struct dirent* entry;
    int i;

    assert(mapping->mode & MODE_DIRECTORY);

    if(!dir) {
        mapping->end = mapping->begin;
        return -1;
    }

    i = mapping->info.dir.first_dir_index =
            first_cluster == 0 ? 0 : s->directory.next;

    if (first_cluster != 0) {
        /
        (void)create_short_and_long_name(s, i, ".", 1);
        (void)create_short_and_long_name(s, i, "..", 1);
    }

    /
    while((entry=readdir(dir))) {
        unsigned int length=strlen(dirname)+2+strlen(entry->d_name);
        char* buffer;
        direntry_t* direntry;
        struct stat st;
        int is_dot=!strcmp(entry->d_name,".");
        int is_dotdot=!strcmp(entry->d_name,"..");

        if(first_cluster == 0 && (is_dotdot || is_dot))
            continue;

        buffer = g_malloc(length);
        snprintf(buffer,length,"%s/%s",dirname,entry->d_name);

        if(stat(buffer,&st)<0) {
            g_free(buffer);
            continue;
        }

        /
        if (!is_dot && !is_dotdot) {
            direntry = create_short_and_long_name(s, i, entry->d_name, 0);
        } else {
            direntry = array_get(&(s->directory), is_dot ? i : i + 1);
        }
        direntry->attributes=(S_ISDIR(st.st_mode)?0x10:0x20);
        direntry->reserved[0]=direntry->reserved[1]=0;
        direntry->ctime=fat_datetime(st.st_ctime,1);
        direntry->cdate=fat_datetime(st.st_ctime,0);
        direntry->adate=fat_datetime(st.st_atime,0);
        direntry->begin_hi=0;
        direntry->mtime=fat_datetime(st.st_mtime,1);
        direntry->mdate=fat_datetime(st.st_mtime,0);
        if(is_dotdot)
            set_begin_of_direntry(direntry, first_cluster_of_parent);
        else if(is_dot)
            set_begin_of_direntry(direntry, first_cluster);
        else
            direntry->begin=0; /
        if (st.st_size > 0x7fffffff) {
            fprintf(stderr, "File %s is larger than 2GB\n", buffer);
            g_free(buffer);
            closedir(dir);
            return -2;
        }
        direntry->size=cpu_to_le32(S_ISDIR(st.st_mode)?0:st.st_size);

        /
        if(!is_dot && !is_dotdot && (S_ISDIR(st.st_mode) || st.st_size)) {
            s->current_mapping = array_get_next(&(s->mapping));
            s->current_mapping->begin=0;
            s->current_mapping->end=st.st_size;
            /
            s->current_mapping->dir_index=s->directory.next-1;
            s->current_mapping->first_mapping_index = -1;
            if (S_ISDIR(st.st_mode)) {
                s->current_mapping->mode = MODE_DIRECTORY;
                s->current_mapping->info.dir.parent_mapping_index =
                    mapping_index;
            } else {
                s->current_mapping->mode = MODE_UNDEFINED;
                s->current_mapping->info.file.offset = 0;
            }
            s->current_mapping->path=buffer;
            s->current_mapping->read_only =
                (st.st_mode & (S_IWUSR | S_IWGRP | S_IWOTH)) == 0;
        } else {
            g_free(buffer);
        }
    }
    closedir(dir);

    /
    while(s->directory.next%(0x10*s->sectors_per_cluster)) {
        direntry_t* direntry=array_get_next(&(s->directory));
        memset(direntry,0,sizeof(direntry_t));
    }

/
#define ROOT_ENTRIES (0x02 * 0x10 * s->sectors_per_cluster)
    if (mapping_index == 0 && s->directory.next < ROOT_ENTRIES) {
        /
        int cur = s->directory.next;
        array_ensure_allocated(&(s->directory), ROOT_ENTRIES - 1);
        s->directory.next = ROOT_ENTRIES;
        memset(array_get(&(s->directory), cur), 0,
                (ROOT_ENTRIES - cur) * sizeof(direntry_t));
    }

    /
    mapping = array_get(&(s->mapping), mapping_index);
    first_cluster += (s->directory.next - mapping->info.dir.first_dir_index)
        * 0x20 / s->cluster_size;
    mapping->end = first_cluster;

    direntry = array_get(&(s->directory), mapping->dir_index);
    set_begin_of_direntry(direntry, mapping->begin);

    return 0;
}
