static int print_int32(DeviceState *dev, Property *prop, char *dest, size_t len)
{
    int32_t *ptr = qdev_get_prop_ptr(dev, prop);
    return snprintf(dest, len, "%" PRId32, *ptr);
}
