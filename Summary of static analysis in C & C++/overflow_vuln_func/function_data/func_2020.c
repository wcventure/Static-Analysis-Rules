static int gdb_handle_packet(GDBState *s, const char *line_buf)
{
    CPUState *cpu;
    CPUClass *cc;
    const char *p;
    uint32_t thread;
    int ch, reg_size, type, res;
    char buf[MAX_PACKET_LENGTH];
    uint8_t mem_buf[MAX_PACKET_LENGTH];
    uint8_t *registers;
    target_ulong addr, len;

#ifdef DEBUG_GDB
    printf("command='%s'\n", line_buf);
#endif
    p = line_buf;
    ch = *p++;
    switch(ch) {
    case '?':
        /
        snprintf(buf, sizeof(buf), "T%02xthread:%02x;", GDB_SIGNAL_TRAP,
                 cpu_index(s->c_cpu));
        put_packet(s, buf);
        /
        gdb_breakpoint_remove_all();
        break;
    case 'c':
        if (*p != '\0') {
            addr = strtoull(p, (char **)&p, 16);
            gdb_set_cpu_pc(s, addr);
        }
        s->signal = 0;
        gdb_continue(s);
	return RS_IDLE;
    case 'C':
        s->signal = gdb_signal_to_target (strtoul(p, (char **)&p, 16));
        if (s->signal == -1)
            s->signal = 0;
        gdb_continue(s);
        return RS_IDLE;
    case 'v':
        if (strncmp(p, "Cont", 4) == 0) {
            int res_signal, res_thread;

            p += 4;
            if (*p == '?') {
                put_packet(s, "vCont;c;C;s;S");
                break;
            }
            res = 0;
            res_signal = 0;
            res_thread = 0;
            while (*p) {
                int action, signal;

                if (*p++ != ';') {
                    res = 0;
                    break;
                }
                action = *p++;
                signal = 0;
                if (action == 'C' || action == 'S') {
                    signal = gdb_signal_to_target(strtoul(p, (char **)&p, 16));
                    if (signal == -1) {
                        signal = 0;
                    }
                } else if (action != 'c' && action != 's') {
                    res = 0;
                    break;
                }
                thread = 0;
                if (*p == ':') {
                    thread = strtoull(p+1, (char **)&p, 16);
                }
                action = tolower(action);
                if (res == 0 || (res == 'c' && action == 's')) {
                    res = action;
                    res_signal = signal;
                    res_thread = thread;
                }
            }
            if (res) {
                if (res_thread != -1 && res_thread != 0) {
                    cpu = find_cpu(res_thread);
                    if (cpu == NULL) {
                        put_packet(s, "E22");
                        break;
                    }
                    s->c_cpu = cpu;
                }
                if (res == 's') {
                    cpu_single_step(s->c_cpu, sstep_flags);
                }
                s->signal = res_signal;
                gdb_continue(s);
                return RS_IDLE;
            }
            break;
        } else {
            goto unknown_command;
        }
    case 'k':
        /
        fprintf(stderr, "\nQEMU: Terminated via GDBstub\n");
        exit(0);
    case 'D':
        /
        gdb_breakpoint_remove_all();
        gdb_syscall_mode = GDB_SYS_DISABLED;
        gdb_continue(s);
        put_packet(s, "OK");
        break;
    case 's':
        if (*p != '\0') {
            addr = strtoull(p, (char **)&p, 16);
            gdb_set_cpu_pc(s, addr);
        }
        cpu_single_step(s->c_cpu, sstep_flags);
        gdb_continue(s);
	return RS_IDLE;
    case 'F':
        {
            target_ulong ret;
            target_ulong err;

            ret = strtoull(p, (char **)&p, 16);
            if (*p == ',') {
                p++;
                err = strtoull(p, (char **)&p, 16);
            } else {
                err = 0;
            }
            if (*p == ',')
                p++;
            type = *p;
            if (s->current_syscall_cb) {
                s->current_syscall_cb(s->c_cpu, ret, err);
                s->current_syscall_cb = NULL;
            }
            if (type == 'C') {
                put_packet(s, "T02");
            } else {
                gdb_continue(s);
            }
        }
        break;
    case 'g':
        cpu_synchronize_state(s->g_cpu);
        len = 0;
        for (addr = 0; addr < s->g_cpu->gdb_num_g_regs; addr++) {
            reg_size = gdb_read_register(s->g_cpu, mem_buf + len, addr);
            len += reg_size;
        }
        memtohex(buf, mem_buf, len);
        put_packet(s, buf);
        break;
    case 'G':
        cpu_synchronize_state(s->g_cpu);
        registers = mem_buf;
        len = strlen(p) / 2;
        hextomem((uint8_t *)registers, p, len);
        for (addr = 0; addr < s->g_cpu->gdb_num_g_regs && len > 0; addr++) {
            reg_size = gdb_write_register(s->g_cpu, registers, addr);
            len -= reg_size;
            registers += reg_size;
        }
        put_packet(s, "OK");
        break;
    case 'm':
        addr = strtoull(p, (char **)&p, 16);
        if (*p == ',')
            p++;
        len = strtoull(p, NULL, 16);
        if (target_memory_rw_debug(s->g_cpu, addr, mem_buf, len, false) != 0) {
            put_packet (s, "E14");
        } else {
            memtohex(buf, mem_buf, len);
            put_packet(s, buf);
        }
        break;
    case 'M':
        addr = strtoull(p, (char **)&p, 16);
        if (*p == ',')
            p++;
        len = strtoull(p, (char **)&p, 16);
        if (*p == ':')
            p++;
        hextomem(mem_buf, p, len);
        if (target_memory_rw_debug(s->g_cpu, addr, mem_buf, len,
                                   true) != 0) {
            put_packet(s, "E14");
        } else {
            put_packet(s, "OK");
        }
        break;
    case 'p':
        /
        if (!gdb_has_xml)
            goto unknown_command;
        addr = strtoull(p, (char **)&p, 16);
        reg_size = gdb_read_register(s->g_cpu, mem_buf, addr);
        if (reg_size) {
            memtohex(buf, mem_buf, reg_size);
            put_packet(s, buf);
        } else {
            put_packet(s, "E14");
        }
        break;
    case 'P':
        if (!gdb_has_xml)
            goto unknown_command;
        addr = strtoull(p, (char **)&p, 16);
        if (*p == '=')
            p++;
        reg_size = strlen(p) / 2;
        hextomem(mem_buf, p, reg_size);
        gdb_write_register(s->g_cpu, mem_buf, addr);
        put_packet(s, "OK");
        break;
    case 'Z':
    case 'z':
        type = strtoul(p, (char **)&p, 16);
        if (*p == ',')
            p++;
        addr = strtoull(p, (char **)&p, 16);
        if (*p == ',')
            p++;
        len = strtoull(p, (char **)&p, 16);
        if (ch == 'Z')
            res = gdb_breakpoint_insert(addr, len, type);
        else
            res = gdb_breakpoint_remove(addr, len, type);
        if (res >= 0)
             put_packet(s, "OK");
        else if (res == -ENOSYS)
            put_packet(s, "");
        else
            put_packet(s, "E22");
        break;
    case 'H':
        type = *p++;
        thread = strtoull(p, (char **)&p, 16);
        if (thread == -1 || thread == 0) {
            put_packet(s, "OK");
            break;
        }
        cpu = find_cpu(thread);
        if (cpu == NULL) {
            put_packet(s, "E22");
            break;
        }
        switch (type) {
        case 'c':
            s->c_cpu = cpu;
            put_packet(s, "OK");
            break;
        case 'g':
            s->g_cpu = cpu;
            put_packet(s, "OK");
            break;
        default:
             put_packet(s, "E22");
             break;
        }
        break;
    case 'T':
        thread = strtoull(p, (char **)&p, 16);
        cpu = find_cpu(thread);

        if (cpu != NULL) {
            put_packet(s, "OK");
        } else {
            put_packet(s, "E22");
        }
        break;
    case 'q':
    case 'Q':
        /
        if (!strcmp(p,"qemu.sstepbits")) {
            /
            snprintf(buf, sizeof(buf), "ENABLE=%x,NOIRQ=%x,NOTIMER=%x",
                     SSTEP_ENABLE,
                     SSTEP_NOIRQ,
                     SSTEP_NOTIMER);
            put_packet(s, buf);
            break;
        } else if (is_query_packet(p, "qemu.sstep", '=')) {
            /
            p += 10;
            if (*p != '=') {
                /
                snprintf(buf, sizeof(buf), "0x%x", sstep_flags);
                put_packet(s, buf);
                break;
            }
            p++;
            type = strtoul(p, (char **)&p, 16);
            sstep_flags = type;
            put_packet(s, "OK");
            break;
        } else if (strcmp(p,"C") == 0) {
            /
            put_packet(s, "QC1");
            break;
        } else if (strcmp(p,"fThreadInfo") == 0) {
            s->query_cpu = first_cpu;
            goto report_cpuinfo;
        } else if (strcmp(p,"sThreadInfo") == 0) {
        report_cpuinfo:
            if (s->query_cpu) {
                snprintf(buf, sizeof(buf), "m%x", cpu_index(s->query_cpu));
                put_packet(s, buf);
                s->query_cpu = CPU_NEXT(s->query_cpu);
            } else
                put_packet(s, "l");
            break;
        } else if (strncmp(p,"ThreadExtraInfo,", 16) == 0) {
            thread = strtoull(p+16, (char **)&p, 16);
            cpu = find_cpu(thread);
            if (cpu != NULL) {
                cpu_synchronize_state(cpu);
                len = snprintf((char *)mem_buf, sizeof(mem_buf),
                               "CPU#%d [%s]", cpu->cpu_index,
                               cpu->halted ? "halted " : "running");
                memtohex(buf, mem_buf, len);
                put_packet(s, buf);
            }
            break;
        }
#ifdef CONFIG_USER_ONLY
        else if (strcmp(p, "Offsets") == 0) {
            TaskState *ts = s->c_cpu->opaque;

            snprintf(buf, sizeof(buf),
                     "Text=" TARGET_ABI_FMT_lx ";Data=" TARGET_ABI_FMT_lx
                     ";Bss=" TARGET_ABI_FMT_lx,
                     ts->info->code_offset,
                     ts->info->data_offset,
                     ts->info->data_offset);
            put_packet(s, buf);
            break;
        }
#else /
        else if (strncmp(p, "Rcmd,", 5) == 0) {
            int len = strlen(p + 5);

            if ((len % 2) != 0) {
                put_packet(s, "E01");
                break;
            }
            hextomem(mem_buf, p + 5, len);
            len = len / 2;
            mem_buf[len++] = 0;
            qemu_chr_be_write(s->mon_chr, mem_buf, len);
            put_packet(s, "OK");
            break;
        }
#endif /
        if (is_query_packet(p, "Supported", ':')) {
            snprintf(buf, sizeof(buf), "PacketSize=%x", MAX_PACKET_LENGTH);
            cc = CPU_GET_CLASS(first_cpu);
            if (cc->gdb_core_xml_file != NULL) {
                pstrcat(buf, sizeof(buf), ";qXfer:features:read+");
            }
            put_packet(s, buf);
            break;
        }
        if (strncmp(p, "Xfer:features:read:", 19) == 0) {
            const char *xml;
            target_ulong total_len;

            cc = CPU_GET_CLASS(first_cpu);
            if (cc->gdb_core_xml_file == NULL) {
                goto unknown_command;
            }

            gdb_has_xml = true;
            p += 19;
            xml = get_feature_xml(p, &p, cc);
            if (!xml) {
                snprintf(buf, sizeof(buf), "E00");
                put_packet(s, buf);
                break;
            }

            if (*p == ':')
                p++;
            addr = strtoul(p, (char **)&p, 16);
            if (*p == ',')
                p++;
            len = strtoul(p, (char **)&p, 16);

            total_len = strlen(xml);
            if (addr > total_len) {
                snprintf(buf, sizeof(buf), "E00");
                put_packet(s, buf);
                break;
            }
            if (len > (MAX_PACKET_LENGTH - 5) / 2)
                len = (MAX_PACKET_LENGTH - 5) / 2;
            if (len < total_len - addr) {
                buf[0] = 'm';
                len = memtox(buf + 1, xml + addr, len);
            } else {
                buf[0] = 'l';
                len = memtox(buf + 1, xml + addr, total_len - addr);
            }
            put_packet_binary(s, buf, len + 1);
            break;
        }
        if (is_query_packet(p, "Attached", ':')) {
            put_packet(s, GDB_ATTACHED);
            break;
        }
        /
        goto unknown_command;

    default:
    unknown_command:
        /
        buf[0] = '\0';
        put_packet(s, buf);
        break;
    }
    return RS_IDLE;
}
