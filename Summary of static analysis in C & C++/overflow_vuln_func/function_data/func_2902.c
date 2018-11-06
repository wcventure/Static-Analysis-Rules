static int load_elf_binary(struct linux_binprm * bprm, struct target_pt_regs * regs,
                           struct image_info * info)
{
    struct elfhdr elf_ex;
    struct elfhdr interp_elf_ex;
    struct exec interp_ex;
    int interpreter_fd = -1; /
    unsigned long load_addr, load_bias;
    int load_addr_set = 0;
    unsigned int interpreter_type = INTERPRETER_NONE;
    unsigned char ibcs2_interpreter;
    int i;
    unsigned long mapped_addr;
    struct elf_phdr * elf_ppnt;
    struct elf_phdr *elf_phdata;
    unsigned long elf_bss, k, elf_brk;
    int retval;
    char * elf_interpreter;
    unsigned long elf_entry, interp_load_addr = 0;
    int status;
    unsigned long start_code, end_code, end_data;
    unsigned long elf_stack;
    char passed_fileno[6];

    ibcs2_interpreter = 0;
    status = 0;
    load_addr = 0;
    load_bias = 0;
    elf_ex = *((struct elfhdr *) bprm->buf);          /
#ifdef BSWAP_NEEDED
    bswap_ehdr(&elf_ex);
#endif

    if (elf_ex.e_ident[0] != 0x7f ||
	strncmp(&elf_ex.e_ident[1], "ELF",3) != 0) {
	    return  -ENOEXEC;
    }

    /
    if ((elf_ex.e_type != ET_EXEC && elf_ex.e_type != ET_DYN) ||
       				(! elf_check_arch(elf_ex.e_machine))) {
	    return -ENOEXEC;
    }

    /
    elf_phdata = (struct elf_phdr *)malloc(elf_ex.e_phentsize*elf_ex.e_phnum);
    if (elf_phdata == NULL) {
	return -ENOMEM;
    }

    retval = lseek(bprm->fd, elf_ex.e_phoff, SEEK_SET);
    if(retval > 0) {
	retval = read(bprm->fd, (char *) elf_phdata, 
				elf_ex.e_phentsize * elf_ex.e_phnum);
    }

    if (retval < 0) {
	perror("load_elf_binary");
	exit(-1);
	free (elf_phdata);
	return -errno;
    }

#ifdef BSWAP_NEEDED
    elf_ppnt = elf_phdata;
    for (i=0; i<elf_ex.e_phnum; i++, elf_ppnt++) {
        bswap_phdr(elf_ppnt);
    }
#endif
    elf_ppnt = elf_phdata;

    elf_bss = 0;
    elf_brk = 0;


    elf_stack = ~0UL;
    elf_interpreter = NULL;
    start_code = ~0UL;
    end_code = 0;
    end_data = 0;

    for(i=0;i < elf_ex.e_phnum; i++) {
	if (elf_ppnt->p_type == PT_INTERP) {
	    if ( elf_interpreter != NULL )
	    {
		free (elf_phdata);
		free(elf_interpreter);
		close(bprm->fd);
		return -EINVAL;
	    }

	    /

	    elf_interpreter = (char *)malloc(elf_ppnt->p_filesz);

	    if (elf_interpreter == NULL) {
		free (elf_phdata);
		close(bprm->fd);
		return -ENOMEM;
	    }

	    retval = lseek(bprm->fd, elf_ppnt->p_offset, SEEK_SET);
	    if(retval >= 0) {
		retval = read(bprm->fd, elf_interpreter, elf_ppnt->p_filesz);
	    }
	    if(retval < 0) {
	 	perror("load_elf_binary2");
		exit(-1);
	    }	

	    /

	    /

	    if (strcmp(elf_interpreter,"/usr/lib/libc.so.1") == 0 ||
		strcmp(elf_interpreter,"/usr/lib/ld.so.1") == 0) {
	      ibcs2_interpreter = 1;
	    }

#if 0
	    printf("Using ELF interpreter %s\n", elf_interpreter);
#endif
	    if (retval >= 0) {
		retval = open(path(elf_interpreter), O_RDONLY);
		if(retval >= 0) {
		    interpreter_fd = retval;
		}
		else {
		    perror(elf_interpreter);
		    exit(-1);
		    /
		}
	    }

	    if (retval >= 0) {
		retval = lseek(interpreter_fd, 0, SEEK_SET);
		if(retval >= 0) {
		    retval = read(interpreter_fd,bprm->buf,128);
		}
	    }
	    if (retval >= 0) {
		interp_ex = *((struct exec *) bprm->buf); /
		interp_elf_ex=*((struct elfhdr *) bprm->buf); /
	    }
	    if (retval < 0) {
		perror("load_elf_binary3");
		exit(-1);
		free (elf_phdata);
		free(elf_interpreter);
		close(bprm->fd);
		return retval;
	    }
	}
	elf_ppnt++;
    }

    /
    if (elf_interpreter){
	interpreter_type = INTERPRETER_ELF | INTERPRETER_AOUT;

	/
	if ((N_MAGIC(interp_ex) != OMAGIC) && (N_MAGIC(interp_ex) != ZMAGIC) &&
	    	(N_MAGIC(interp_ex) != QMAGIC)) {
	  interpreter_type = INTERPRETER_ELF;
	}

	if (interp_elf_ex.e_ident[0] != 0x7f ||
	    	strncmp(&interp_elf_ex.e_ident[1], "ELF",3) != 0) {
	    interpreter_type &= ~INTERPRETER_ELF;
	}

	if (!interpreter_type) {
	    free(elf_interpreter);
	    free(elf_phdata);
	    close(bprm->fd);
	    return -ELIBBAD;
	}
    }

    /

    if (!bprm->sh_bang) {
	char * passed_p;

	if (interpreter_type == INTERPRETER_AOUT) {
	    sprintf(passed_fileno, "%d", bprm->fd);
	    passed_p = passed_fileno;

	    if (elf_interpreter) {
		bprm->p = copy_strings(1,&passed_p,bprm->page,bprm->p);
		bprm->argc++;
	    }
	}
	if (!bprm->p) {
	    if (elf_interpreter) {
	        free(elf_interpreter);
	    }
	    free (elf_phdata);
	    close(bprm->fd);
	    return -E2BIG;
	}
    }

    /
    info->end_data = 0;
    info->end_code = 0;
    info->start_mmap = (unsigned long)ELF_START_MMAP;
    info->mmap = 0;
    elf_entry = (unsigned long) elf_ex.e_entry;

    /
    info->rss = 0;
    bprm->p = setup_arg_pages(bprm->p, bprm, info);
    info->start_stack = bprm->p;

    /

    for(i = 0, elf_ppnt = elf_phdata; i < elf_ex.e_phnum; i++, elf_ppnt++) {
        int elf_prot = 0;
        int elf_flags = 0;
        unsigned long error;
        
	if (elf_ppnt->p_type != PT_LOAD)
            continue;
        
        if (elf_ppnt->p_flags & PF_R) elf_prot |= PROT_READ;
        if (elf_ppnt->p_flags & PF_W) elf_prot |= PROT_WRITE;
        if (elf_ppnt->p_flags & PF_X) elf_prot |= PROT_EXEC;
        elf_flags = MAP_PRIVATE | MAP_DENYWRITE;
        if (elf_ex.e_type == ET_EXEC || load_addr_set) {
            elf_flags |= MAP_FIXED;
        } else if (elf_ex.e_type == ET_DYN) {
            /
            /
            error = target_mmap(0, ET_DYN_MAP_SIZE,
                                PROT_NONE, MAP_PRIVATE | MAP_ANON, 
                                -1, 0);
            if (error == -1) {
                perror("mmap");
                exit(-1);
            }
            load_bias = TARGET_ELF_PAGESTART(error - elf_ppnt->p_vaddr);
        }
        
        error = target_mmap(TARGET_ELF_PAGESTART(load_bias + elf_ppnt->p_vaddr),
                            (elf_ppnt->p_filesz +
                             TARGET_ELF_PAGEOFFSET(elf_ppnt->p_vaddr)),
                            elf_prot,
                            (MAP_FIXED | MAP_PRIVATE | MAP_DENYWRITE),
                            bprm->fd,
                            (elf_ppnt->p_offset - 
                             TARGET_ELF_PAGEOFFSET(elf_ppnt->p_vaddr)));
        if (error == -1) {
            perror("mmap");
            exit(-1);
        }

#ifdef LOW_ELF_STACK
        if (TARGET_ELF_PAGESTART(elf_ppnt->p_vaddr) < elf_stack)
            elf_stack = TARGET_ELF_PAGESTART(elf_ppnt->p_vaddr);
#endif
        
        if (!load_addr_set) {
            load_addr_set = 1;
            load_addr = elf_ppnt->p_vaddr - elf_ppnt->p_offset;
            if (elf_ex.e_type == ET_DYN) {
                load_bias += error -
                    TARGET_ELF_PAGESTART(load_bias + elf_ppnt->p_vaddr);
                load_addr += load_bias;
            }
        }
        k = elf_ppnt->p_vaddr;
        if (k < start_code) 
            start_code = k;
        k = elf_ppnt->p_vaddr + elf_ppnt->p_filesz;
        if (k > elf_bss) 
            elf_bss = k;
        if ((elf_ppnt->p_flags & PF_X) && end_code <  k)
            end_code = k;
        if (end_data < k) 
            end_data = k;
        k = elf_ppnt->p_vaddr + elf_ppnt->p_memsz;
        if (k > elf_brk) elf_brk = k;
    }

    elf_entry += load_bias;
    elf_bss += load_bias;
    elf_brk += load_bias;
    start_code += load_bias;
    end_code += load_bias;
    //    start_data += load_bias;
    end_data += load_bias;

    if (elf_interpreter) {
	if (interpreter_type & 1) {
	    elf_entry = load_aout_interp(&interp_ex, interpreter_fd);
	}
	else if (interpreter_type & 2) {
	    elf_entry = load_elf_interp(&interp_elf_ex, interpreter_fd,
					    &interp_load_addr);
	}

	close(interpreter_fd);
	free(elf_interpreter);

	if (elf_entry == ~0UL) {
	    printf("Unable to load interpreter\n");
	    free(elf_phdata);
	    exit(-1);
	    return 0;
	}
    }

    free(elf_phdata);

    if (loglevel)
	load_symbols(&elf_ex, bprm->fd);

    if (interpreter_type != INTERPRETER_AOUT) close(bprm->fd);
    info->personality = (ibcs2_interpreter ? PER_SVR4 : PER_LINUX);

#ifdef LOW_ELF_STACK
    info->start_stack = bprm->p = elf_stack - 4;
#endif
    bprm->p = (unsigned long)
      create_elf_tables((char *)bprm->p,
		    bprm->argc,
		    bprm->envc,
                    &elf_ex,
                    load_addr, load_bias,
		    interp_load_addr,
		    (interpreter_type == INTERPRETER_AOUT ? 0 : 1),
		    info);
    if (interpreter_type == INTERPRETER_AOUT)
      info->arg_start += strlen(passed_fileno) + 1;
    info->start_brk = info->brk = elf_brk;
    info->end_code = end_code;
    info->start_code = start_code;
    info->end_data = end_data;
    info->start_stack = bprm->p;

    /
    set_brk(elf_bss, elf_brk);

    padzero(elf_bss);

#if 0
    printf("(start_brk) %x\n" , info->start_brk);
    printf("(end_code) %x\n" , info->end_code);
    printf("(start_code) %x\n" , info->start_code);
    printf("(end_data) %x\n" , info->end_data);
    printf("(start_stack) %x\n" , info->start_stack);
    printf("(brk) %x\n" , info->brk);
#endif

    if ( info->personality == PER_SVR4 )
    {
	    /
	    mapped_addr = target_mmap(0, host_page_size, PROT_READ | PROT_EXEC,
                                      MAP_FIXED | MAP_PRIVATE, -1, 0);
    }

#ifdef ELF_PLAT_INIT
    /
    ELF_PLAT_INIT(regs);
#endif


    info->entry = elf_entry;

    return 0;
}
