static void pci_msix_write(void *opaque, hwaddr addr,
                           uint64_t val, unsigned size)
{
    XenPCIPassthroughState *s = opaque;
    XenPTMSIX *msix = s->msix;
    XenPTMSIXEntry *entry;
    int entry_nr, offset;

    entry_nr = addr / PCI_MSIX_ENTRY_SIZE;
    if (entry_nr < 0 || entry_nr >= msix->total_entries) {
        XEN_PT_ERR(&s->dev, "asked MSI-X entry '%i' invalid!\n", entry_nr);
        return;
    }
    entry = &msix->msix_entry[entry_nr];
    offset = addr % PCI_MSIX_ENTRY_SIZE;

    if (offset != PCI_MSIX_ENTRY_VECTOR_CTRL) {
        const volatile uint32_t *vec_ctrl;

        if (get_entry_value(entry, offset) == val
            && entry->pirq != XEN_PT_UNASSIGNED_PIRQ) {
            return;
        }

        /
        vec_ctrl = s->msix->phys_iomem_base + entry_nr * PCI_MSIX_ENTRY_SIZE
            + PCI_MSIX_ENTRY_VECTOR_CTRL;

        if (msix->enabled && !(*vec_ctrl & PCI_MSIX_ENTRY_CTRL_MASKBIT)) {
            XEN_PT_ERR(&s->dev, "Can't update msix entry %d since MSI-X is"
                       " already enabled.\n", entry_nr);
            return;
        }

        entry->updated = true;
    }

    set_entry_value(entry, offset, val);

    if (offset == PCI_MSIX_ENTRY_VECTOR_CTRL) {
        if (msix->enabled && !(val & PCI_MSIX_ENTRY_CTRL_MASKBIT)) {
            xen_pt_msix_update_one(s, entry_nr);
        }
    }
}
