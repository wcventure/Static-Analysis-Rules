static int img_create(int argc, char **argv)
{
    int c;
    uint64_t img_size = -1;
    const char *fmt = "raw";
    const char *base_fmt = NULL;
    const char *filename;
    const char *base_filename = NULL;
    char *options = NULL;
    Error *local_err = NULL;

    for(;;) {
        c = getopt(argc, argv, "F:b:f:he6o:");
        if (c == -1) {
            break;
        }
        switch(c) {
        case '?':
        case 'h':
            help();
            break;
        case 'F':
            base_fmt = optarg;
            break;
        case 'b':
            base_filename = optarg;
            break;
        case 'f':
            fmt = optarg;
            break;
        case 'e':
            error_report("option -e is deprecated, please use \'-o "
                  "encryption\' instead!");
            return 1;
        case '6':
            error_report("option -6 is deprecated, please use \'-o "
                  "compat6\' instead!");
            return 1;
        case 'o':
            options = optarg;
            break;
        }
    }

    /
    if (optind >= argc) {
        help();
    }
    filename = argv[optind++];

    /
    if (optind < argc) {
        int64_t sval;
        char *end;
        sval = strtosz_suffix(argv[optind++], &end, STRTOSZ_DEFSUFFIX_B);
        if (sval < 0 || *end) {
            error_report("Invalid image size specified! You may use k, M, G or "
                  "T suffixes for ");
            error_report("kilobytes, megabytes, gigabytes and terabytes.");
            return 1;
        }
        img_size = (uint64_t)sval;
    }

    if (options && is_help_option(options)) {
        return print_block_option_help(filename, fmt);
    }

    bdrv_img_create(filename, fmt, base_filename, base_fmt,
                    options, img_size, BDRV_O_FLAGS, &local_err);
    if (error_is_set(&local_err)) {
        error_report("%s", error_get_pretty(local_err));
        error_free(local_err);
        return 1;
    }

    return 0;
}
