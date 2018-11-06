static void parse_option_number(const char *name, const char *value,
                                uint64_t *ret, Error **errp)
{
    char *postfix;
    uint64_t number;

    number = strtoull(value, &postfix, 0);
    if (*postfix != '\0') {
        error_setg(errp, QERR_INVALID_PARAMETER_VALUE, name, "a number");
        return;
    }
    *ret = number;
}
