static int parse_bit(DeviceState *dev, Property *prop, const char *str)
{
    if (!strcasecmp(str, "on"))
        bit_prop_set(dev, prop, true);
    else if (!strcasecmp(str, "off"))
        bit_prop_set(dev, prop, false);
    else
        return -EINVAL;
    return 0;
}
