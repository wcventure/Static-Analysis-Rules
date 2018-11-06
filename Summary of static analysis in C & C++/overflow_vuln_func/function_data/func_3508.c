static void format_line(void *ptr, int level, const char *fmt, va_list vl,
                        char part[3][512], int part_size, int *print_prefix, int type[2])
{
    AVClass* avc = ptr ? *(AVClass **) ptr : NULL;
    part[0][0] = part[1][0] = part[2][0] = 0;
    if(type) type[0] = type[1] = AV_CLASS_CATEGORY_NA + 16;
    if (*print_prefix && avc) {
        if (avc->parent_log_context_offset) {
            AVClass** parent = *(AVClass ***) (((uint8_t *) ptr) +
                                   avc->parent_log_context_offset);
            if (parent && *parent) {
                snprintf(part[0], part_size, "[%s @ %p] ",
                         (*parent)->item_name(parent), parent);
                if(type) type[0] = get_category(((uint8_t *) ptr) + avc->parent_log_context_offset);
            }
        }
        snprintf(part[1], part_size, "[%s @ %p] ",
                 avc->item_name(ptr), ptr);
        if(type) type[1] = get_category(ptr);
    }

    vsnprintf(part[2], part_size, fmt, vl);

    *print_prefix = strlen(part[2]) && part[2][strlen(part[2]) - 1] == '\n';
}
