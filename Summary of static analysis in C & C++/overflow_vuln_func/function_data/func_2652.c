static int print_uint64(DeviceState *dev, Property *prop, char *dest, size_t len)
{
    uint64_t *ptr = qdev_get_prop_ptr(dev, prop);
    return snprintf(dest, len, "%" PRIu64, *ptr);
}
