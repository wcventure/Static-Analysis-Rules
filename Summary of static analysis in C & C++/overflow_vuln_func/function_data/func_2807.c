void parse_option_size(const char *name, const char *value,
                       uint64_t *ret, Error **errp)
{
    char *postfix;
    double sizef;

    sizef = strtod(value, &postfix);
    if (sizef < 0 || sizef > UINT64_MAX) {
        error_setg(errp, QERR_INVALID_PARAMETER_VALUE, name,
                   "a non-negative number below 2^64");
        return;
    }
    switch (*postfix) {
    case 'T':
        sizef *= 1024;
        /
    case 'G':
        sizef *= 1024;
        /
    case 'M':
        sizef *= 1024;
        /
    case 'K':
    case 'k':
        sizef *= 1024;
        /
    case 'b':
    case '\0':
        *ret = (uint64_t) sizef;
        break;
    default:
        error_setg(errp, QERR_INVALID_PARAMETER_VALUE, name, "a size");
        error_append_hint(errp, "You may use k, M, G or T suffixes for "
                          "kilobytes, megabytes, gigabytes and terabytes.\n");
        return;
    }
}
