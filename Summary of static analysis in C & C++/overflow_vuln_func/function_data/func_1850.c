void spapr_lmb_release(DeviceState *dev)
{
    HotplugHandler *hotplug_ctrl = qdev_get_hotplug_handler(dev);
    sPAPRMachineState *spapr = SPAPR_MACHINE(hotplug_ctrl);
    sPAPRDIMMState *ds = spapr_pending_dimm_unplugs_find(spapr, PC_DIMM(dev));

    /
    if (ds == NULL) {
        ds = spapr_recover_pending_dimm_state(spapr, PC_DIMM(dev));
        if (ds->nr_lmbs) {
            return;
        }
    } else if (--ds->nr_lmbs) {
        return;
    }

    spapr_pending_dimm_unplugs_remove(spapr, ds);

    /
    hotplug_handler_unplug(hotplug_ctrl, dev, &error_abort);
}
