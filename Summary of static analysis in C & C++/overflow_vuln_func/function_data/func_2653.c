static int parse_vlan(DeviceState *dev, Property *prop, const char *str)
{
    VLANState **ptr = qdev_get_prop_ptr(dev, prop);
    int id;

    if (sscanf(str, "%d", &id) != 1)
        return -EINVAL;
    *ptr = qemu_find_vlan(id, 1);
    if (*ptr == NULL)
        return -ENOENT;
    return 0;
}
