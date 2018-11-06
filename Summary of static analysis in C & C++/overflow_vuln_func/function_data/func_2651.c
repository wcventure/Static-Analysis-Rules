static int parse_uint64(DeviceState *dev, Property *prop, const char *str)
{
    uint64_t *ptr = qdev_get_prop_ptr(dev, prop);
    char *end;

    /
    *ptr = strtoull(str, &end, 0);
    if ((*end != '\0') || (end == str)) {
        return -EINVAL;
    }

    return 0;
}
