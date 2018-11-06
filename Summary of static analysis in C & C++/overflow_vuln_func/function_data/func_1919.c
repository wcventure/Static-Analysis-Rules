static void get_pci_host_devaddr(Object *obj, Visitor *v, const char *name,
                                 void *opaque, Error **errp)
{
    DeviceState *dev = DEVICE(obj);
    Property *prop = opaque;
    PCIHostDeviceAddress *addr = qdev_get_prop_ptr(dev, prop);
    char buffer[] = "xxxx:xx:xx.x";
    char *p = buffer;
    int rc = 0;

    rc = snprintf(buffer, sizeof(buffer), "%04x:%02x:%02x.%d",
                  addr->domain, addr->bus, addr->slot, addr->function);
    assert(rc == sizeof(buffer) - 1);

    visit_type_str(v, name, &p, errp);
}
