static int print_uint16(DeviceState *dev, Property *prop, char *dest, size_t len)
{
    uint16_t *ptr = qdev_get_prop_ptr(dev, prop);
    return snprintf(dest, len, "%" PRIu16, *ptr);
}
