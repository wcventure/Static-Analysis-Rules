int acpi_table_add(const QemuOpts *opts)
{
    AcpiTableOptions *hdrs = NULL;
    Error *err = NULL;
    char **pathnames = NULL;
    char **cur;

    size_t len, start, allen;
    bool has_header;
    int changed;
    int r;
    struct acpi_table_header hdr;
    char unsigned *table_start;

    {
        OptsVisitor *ov;

        ov = opts_visitor_new(opts);
        visit_type_AcpiTableOptions(opts_get_visitor(ov), &hdrs, NULL, &err);
        opts_visitor_cleanup(ov);
    }

    if (err) {
        goto out;
    }
    if (hdrs->has_file == hdrs->has_data) {
        error_setg(&err, "'-acpitable' requires one of 'data' or 'file'");
        goto out;
    }
    has_header = hdrs->has_file;

    pathnames = g_strsplit(hdrs->has_file ? hdrs->file : hdrs->data, ":", 0);
    if (pathnames == NULL || pathnames[0] == NULL) {
        error_setg(&err, "'-acpitable' requires at least one pathname");
        goto out;
    }

    if (!acpi_tables) {
        allen = sizeof(uint16_t);
        acpi_tables = g_malloc0(allen);
    } else {
        allen = acpi_tables_len;
    }

    start = allen;
    acpi_tables = g_realloc(acpi_tables, start + ACPI_TABLE_HDR_SIZE);
    allen += has_header ? ACPI_TABLE_PFX_SIZE : ACPI_TABLE_HDR_SIZE;

    /

    for (cur = pathnames; *cur; ++cur) {
        int fd = open(*cur, O_RDONLY | O_BINARY);

        if (fd < 0) {
            error_setg(&err, "can't open file %s: %s", *cur, strerror(errno));
            goto out;
        }

        for (;;) {
            char unsigned data[8192];
            r = read(fd, data, sizeof(data));
            if (r == 0) {
                break;
            } else if (r > 0) {
                acpi_tables = g_realloc(acpi_tables, allen + r);
                memcpy(acpi_tables + allen, data, r);
                allen += r;
            } else if (errno != EINTR) {
                error_setg(&err, "can't read file %s: %s",
                           *cur, strerror(errno));
                close(fd);
                goto out;
            }
        }

        close(fd);
    }

    /

    table_start = acpi_tables + start;   /
    changed = 0;

    /
    memcpy(&hdr, has_header ? table_start : dfl_hdr, ACPI_TABLE_HDR_SIZE);

    /
    len = allen - start - ACPI_TABLE_PFX_SIZE;

    hdr._length = cpu_to_le16(len);

    if (hdrs->has_sig) {
        /
        strncpy(hdr.sig, hdrs->sig, sizeof(hdr.sig));
        ++changed;
    }

    /
    if (has_header) {
        unsigned long val;

        /
        val = le32_to_cpu(hdr.length);
        if (val != len) {
            fprintf(stderr,
                "warning: acpitable has wrong length,"
                " header says %lu, actual size %zu bytes\n",
                val, len);
            ++changed;
        }
    }
    /
    hdr.length = cpu_to_le32(len);

    if (hdrs->has_rev) {
        hdr.revision = hdrs->rev;
        ++changed;
    }

    if (hdrs->has_oem_id) {
        /
        strncpy(hdr.oem_id, hdrs->oem_id, sizeof(hdr.oem_id));
        ++changed;
    }

    if (hdrs->has_oem_table_id) {
        /
        strncpy(hdr.oem_table_id, hdrs->oem_table_id,
                sizeof(hdr.oem_table_id));
        ++changed;
    }

    if (hdrs->has_oem_rev) {
        hdr.oem_revision = cpu_to_le32(hdrs->oem_rev);
        ++changed;
    }

    if (hdrs->has_asl_compiler_id) {
        /
        strncpy(hdr.asl_compiler_id, hdrs->asl_compiler_id,
                sizeof(hdr.asl_compiler_id));
        ++changed;
    }

    if (hdrs->has_asl_compiler_rev) {
        hdr.asl_compiler_revision = cpu_to_le32(hdrs->asl_compiler_rev);
        ++changed;
    }

    if (!has_header && !changed) {
        fprintf(stderr, "warning: acpitable: no table headers are specified\n");
    }


    /
    /
    /
    hdr.checksum = 0;    /

    /
    memcpy(table_start, &hdr, sizeof(hdr));

    if (changed || !has_header || 1) {
        ((struct acpi_table_header *)table_start)->checksum =
            acpi_checksum((uint8_t *)table_start + ACPI_TABLE_PFX_SIZE, len);
    }

    /
    (*(uint16_t *)acpi_tables) =
        cpu_to_le32(le32_to_cpu(*(uint16_t *)acpi_tables) + 1);

    acpi_tables_len = allen;

out:
    g_strfreev(pathnames);

    if (hdrs != NULL) {
        QapiDeallocVisitor *dv;

        dv = qapi_dealloc_visitor_new();
        visit_type_AcpiTableOptions(qapi_dealloc_get_visitor(dv), &hdrs, NULL,
                                    NULL);
        qapi_dealloc_visitor_cleanup(dv);
    }

    if (err) {
        fprintf(stderr, "%s\n", error_get_pretty(err));
        error_free(err);
        return -1;
    }
    return 0;
}
