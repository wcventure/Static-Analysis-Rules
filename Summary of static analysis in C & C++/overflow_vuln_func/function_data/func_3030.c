static int cpu_x86_find_by_name(x86_def_t *x86_cpu_def, const char *cpu_model)
{
    unsigned int i;
    x86_def_t *def;

    char *s = g_strdup(cpu_model);
    char *featurestr, *name = strtok(s, ",");
    /
    uint32_t plus_features = 0, plus_ext_features = 0;
    uint32_t plus_ext2_features = 0, plus_ext3_features = 0;
    uint32_t plus_kvm_features = 0, plus_svm_features = 0;
    /
    uint32_t minus_features = 0, minus_ext_features = 0;
    uint32_t minus_ext2_features = 0, minus_ext3_features = 0;
    uint32_t minus_kvm_features = 0, minus_svm_features = 0;
    uint32_t numvalue;

    for (def = x86_defs; def; def = def->next)
        if (name && !strcmp(name, def->name))
            break;
    if (kvm_enabled() && name && strcmp(name, "host") == 0) {
        cpu_x86_fill_host(x86_cpu_def);
    } else if (!def) {
        goto error;
    } else {
        memcpy(x86_cpu_def, def, sizeof(*def));
    }

    plus_kvm_features = ~0; /

    add_flagname_to_bitmaps("hypervisor", &plus_features,
        &plus_ext_features, &plus_ext2_features, &plus_ext3_features,
        &plus_kvm_features, &plus_svm_features);

    featurestr = strtok(NULL, ",");

    while (featurestr) {
        char *val;
        if (featurestr[0] == '+') {
            add_flagname_to_bitmaps(featurestr + 1, &plus_features,
                            &plus_ext_features, &plus_ext2_features,
                            &plus_ext3_features, &plus_kvm_features,
                            &plus_svm_features);
        } else if (featurestr[0] == '-') {
            add_flagname_to_bitmaps(featurestr + 1, &minus_features,
                            &minus_ext_features, &minus_ext2_features,
                            &minus_ext3_features, &minus_kvm_features,
                            &minus_svm_features);
        } else if ((val = strchr(featurestr, '='))) {
            *val = 0; val++;
            if (!strcmp(featurestr, "family")) {
                char *err;
                numvalue = strtoul(val, &err, 0);
                if (!*val || *err) {
                    fprintf(stderr, "bad numerical value %s\n", val);
                    goto error;
                }
                x86_cpu_def->family = numvalue;
            } else if (!strcmp(featurestr, "model")) {
                char *err;
                numvalue = strtoul(val, &err, 0);
                if (!*val || *err || numvalue > 0xff) {
                    fprintf(stderr, "bad numerical value %s\n", val);
                    goto error;
                }
                x86_cpu_def->model = numvalue;
            } else if (!strcmp(featurestr, "stepping")) {
                char *err;
                numvalue = strtoul(val, &err, 0);
                if (!*val || *err || numvalue > 0xf) {
                    fprintf(stderr, "bad numerical value %s\n", val);
                    goto error;
                }
                x86_cpu_def->stepping = numvalue ;
            } else if (!strcmp(featurestr, "level")) {
                char *err;
                numvalue = strtoul(val, &err, 0);
                if (!*val || *err) {
                    fprintf(stderr, "bad numerical value %s\n", val);
                    goto error;
                }
                x86_cpu_def->level = numvalue;
            } else if (!strcmp(featurestr, "xlevel")) {
                char *err;
                numvalue = strtoul(val, &err, 0);
                if (!*val || *err) {
                    fprintf(stderr, "bad numerical value %s\n", val);
                    goto error;
                }
                if (numvalue < 0x80000000) {
                    numvalue += 0x80000000;
                }
                x86_cpu_def->xlevel = numvalue;
            } else if (!strcmp(featurestr, "vendor")) {
                if (strlen(val) != 12) {
                    fprintf(stderr, "vendor string must be 12 chars long\n");
                    goto error;
                }
                x86_cpu_def->vendor1 = 0;
                x86_cpu_def->vendor2 = 0;
                x86_cpu_def->vendor3 = 0;
                for(i = 0; i < 4; i++) {
                    x86_cpu_def->vendor1 |= ((uint8_t)val[i    ]) << (8 * i);
                    x86_cpu_def->vendor2 |= ((uint8_t)val[i + 4]) << (8 * i);
                    x86_cpu_def->vendor3 |= ((uint8_t)val[i + 8]) << (8 * i);
                }
                x86_cpu_def->vendor_override = 1;
            } else if (!strcmp(featurestr, "model_id")) {
                pstrcpy(x86_cpu_def->model_id, sizeof(x86_cpu_def->model_id),
                        val);
            } else if (!strcmp(featurestr, "tsc_freq")) {
                int64_t tsc_freq;
                char *err;

                tsc_freq = strtosz_suffix_unit(val, &err,
                                               STRTOSZ_DEFSUFFIX_B, 1000);
                if (!*val || *err) {
                    fprintf(stderr, "bad numerical value %s\n", val);
                    goto error;
                }
                x86_cpu_def->tsc_khz = tsc_freq / 1000;
            } else {
                fprintf(stderr, "unrecognized feature %s\n", featurestr);
                goto error;
            }
        } else if (!strcmp(featurestr, "check")) {
            check_cpuid = 1;
        } else if (!strcmp(featurestr, "enforce")) {
            check_cpuid = enforce_cpuid = 1;
        } else {
            fprintf(stderr, "feature string `%s' not in format (+feature|-feature|feature=xyz)\n", featurestr);
            goto error;
        }
        featurestr = strtok(NULL, ",");
    }
    x86_cpu_def->features |= plus_features;
    x86_cpu_def->ext_features |= plus_ext_features;
    x86_cpu_def->ext2_features |= plus_ext2_features;
    x86_cpu_def->ext3_features |= plus_ext3_features;
    x86_cpu_def->kvm_features |= plus_kvm_features;
    x86_cpu_def->svm_features |= plus_svm_features;
    x86_cpu_def->features &= ~minus_features;
    x86_cpu_def->ext_features &= ~minus_ext_features;
    x86_cpu_def->ext2_features &= ~minus_ext2_features;
    x86_cpu_def->ext3_features &= ~minus_ext3_features;
    x86_cpu_def->kvm_features &= ~minus_kvm_features;
    x86_cpu_def->svm_features &= ~minus_svm_features;
    if (check_cpuid) {
        if (check_features_against_host(x86_cpu_def) && enforce_cpuid)
            goto error;
    }
    g_free(s);
    return 0;

error:
    g_free(s);
    return -1;
}
