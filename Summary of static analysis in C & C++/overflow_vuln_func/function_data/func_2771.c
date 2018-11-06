uint64_t pc_dimm_get_free_addr(uint64_t address_space_start,
                               uint64_t address_space_size,
                               uint64_t *hint, uint64_t size,
                               Error **errp)
{
    GSList *list = NULL, *item;
    uint64_t new_addr, ret = 0;
    uint64_t address_space_end = address_space_start + address_space_size;

    assert(address_space_end > address_space_size);
    object_child_foreach(qdev_get_machine(), pc_dimm_built_list, &list);

    if (hint) {
        new_addr = *hint;
    } else {
        new_addr = address_space_start;
    }

    /
    for (item = list; item; item = g_slist_next(item)) {
        PCDIMMDevice *dimm = item->data;
        uint64_t dimm_size = object_property_get_int(OBJECT(dimm),
                                                     PC_DIMM_SIZE_PROP,
                                                     errp);
        if (errp && *errp) {
            goto out;
        }

        if (ranges_overlap(dimm->addr, dimm_size, new_addr, size)) {
            if (hint) {
                DeviceState *d = DEVICE(dimm);
                error_setg(errp, "address range conflicts with '%s'", d->id);
                goto out;
            }
            new_addr = dimm->addr + dimm_size;
        }
    }
    ret = new_addr;

    if (new_addr < address_space_start) {
        error_setg(errp, "can't add memory [0x%" PRIx64 ":0x%" PRIx64
                   "] at 0x%" PRIx64, new_addr, size, address_space_start);
    } else if ((new_addr + size) > address_space_end) {
        error_setg(errp, "can't add memory [0x%" PRIx64 ":0x%" PRIx64
                   "] beyond 0x%" PRIx64, new_addr, size, address_space_end);
    }

out:
    g_slist_free(list);
    return ret;
}
