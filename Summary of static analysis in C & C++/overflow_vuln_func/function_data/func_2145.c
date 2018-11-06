static void shpc_interrupt_update(PCIDevice *d)
{
    SHPCDevice *shpc = d->shpc;
    int slot;
    int level = 0;
    uint32_t serr_int;
    uint32_t int_locator = 0;

    /
    for (slot = 0; slot < shpc->nslots; ++slot) {
        uint8_t event = shpc->config[SHPC_SLOT_EVENT_LATCH(slot)];
        uint8_t disable = shpc->config[SHPC_SLOT_EVENT_SERR_INT_DIS(d, slot)];
        uint32_t mask = 1 << SHPC_IDX_TO_LOGICAL(slot);
        if (event & ~disable) {
            int_locator |= mask;
        }
    }
    serr_int = pci_get_long(shpc->config + SHPC_SERR_INT);
    if ((serr_int & SHPC_CMD_DETECTED) && !(serr_int & SHPC_CMD_INT_DIS)) {
        int_locator |= SHPC_INT_COMMAND;
    }
    pci_set_long(shpc->config + SHPC_INT_LOCATOR, int_locator);
    level = (!(serr_int & SHPC_INT_DIS) && int_locator) ? 1 : 0;
    if (msi_enabled(d) && shpc->msi_requested != level)
        msi_notify(d, 0);
    else
        pci_set_irq(d, level);
    shpc->msi_requested = level;
}
