static int parse_int32(DeviceState *dev, Property *prop, const char *str)
{
    int32_t *ptr = qdev_get_prop_ptr(dev, prop);
    char *end;

    *ptr = strtol(str, &end, 10);
    if ((*end != '\0') || (end == str)) {
        return -EINVAL;
    }

    return 0;
}
