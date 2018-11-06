static int pci_rocker_init(PCIDevice *dev)
{
    Rocker *r = to_rocker(dev);
    const MACAddr zero = { .a = { 0, 0, 0, 0, 0, 0 } };
    const MACAddr dflt = { .a = { 0x52, 0x54, 0x00, 0x12, 0x35, 0x01 } };
    static int sw_index;
    int i, err = 0;

    /

    r->worlds[ROCKER_WORLD_TYPE_OF_DPA] = of_dpa_world_alloc(r);
    r->world_dflt = r->worlds[ROCKER_WORLD_TYPE_OF_DPA];

    for (i = 0; i < ROCKER_WORLD_TYPE_MAX; i++) {
        if (!r->worlds[i]) {
            goto err_world_alloc;
        }
    }

    /

    memory_region_init_io(&r->mmio, OBJECT(r), &rocker_mmio_ops, r,
                          "rocker-mmio", ROCKER_PCI_BAR0_SIZE);
    pci_register_bar(dev, ROCKER_PCI_BAR0_IDX,
                     PCI_BASE_ADDRESS_SPACE_MEMORY, &r->mmio);

    /

    memory_region_init(&r->msix_bar, OBJECT(r), "rocker-msix-bar",
                       ROCKER_PCI_MSIX_BAR_SIZE);
    pci_register_bar(dev, ROCKER_PCI_MSIX_BAR_IDX,
                     PCI_BASE_ADDRESS_SPACE_MEMORY, &r->msix_bar);

    /

    err = rocker_msix_init(r);
    if (err) {
        goto err_msix_init;
    }

    /

    if (!r->name) {
        r->name = g_strdup(ROCKER);
    }

    if (rocker_find(r->name)) {
        err = -EEXIST;
        goto err_duplicate;
    }

    /
#define ROCKER_IFNAMSIZ 16
#define MAX_ROCKER_NAME_LEN  (ROCKER_IFNAMSIZ - 1 - 3 - 3)
    if (strlen(r->name) > MAX_ROCKER_NAME_LEN) {
        fprintf(stderr,
                "rocker: name too long; please shorten to at most %d chars\n",
                MAX_ROCKER_NAME_LEN);
        return -EINVAL;
    }

    if (memcmp(&r->fp_start_macaddr, &zero, sizeof(zero)) == 0) {
        memcpy(&r->fp_start_macaddr, &dflt, sizeof(dflt));
        r->fp_start_macaddr.a[4] += (sw_index++);
    }

    if (!r->switch_id) {
        memcpy(&r->switch_id, &r->fp_start_macaddr,
               sizeof(r->fp_start_macaddr));
    }

    if (r->fp_ports > ROCKER_FP_PORTS_MAX) {
        r->fp_ports = ROCKER_FP_PORTS_MAX;
    }

    r->rings = g_malloc(sizeof(DescRing *) * rocker_pci_ring_count(r));
    if (!r->rings) {
        goto err_rings_alloc;
    }

    /

    err = -ENOMEM;
    for (i = 0; i < rocker_pci_ring_count(r); i++) {
        DescRing *ring = desc_ring_alloc(r, i);

        if (!ring) {
            goto err_ring_alloc;
        }

        if (i == ROCKER_RING_CMD) {
            desc_ring_set_consume(ring, cmd_consume, ROCKER_MSIX_VEC_CMD);
        } else if (i == ROCKER_RING_EVENT) {
            desc_ring_set_consume(ring, NULL, ROCKER_MSIX_VEC_EVENT);
        } else if (i % 2 == 0) {
            desc_ring_set_consume(ring, tx_consume,
                                  ROCKER_MSIX_VEC_TX((i - 2) / 2));
        } else if (i % 2 == 1) {
            desc_ring_set_consume(ring, NULL, ROCKER_MSIX_VEC_RX((i - 3) / 2));
        }

        r->rings[i] = ring;
    }

    for (i = 0; i < r->fp_ports; i++) {
        FpPort *port =
            fp_port_alloc(r, r->name, &r->fp_start_macaddr,
                          i, &r->fp_ports_peers[i]);

        if (!port) {
            goto err_port_alloc;
        }

        r->fp_port[i] = port;
        fp_port_set_world(port, r->world_dflt);
    }

    QLIST_INSERT_HEAD(&rockers, r, next);

    return 0;

err_port_alloc:
    for (--i; i >= 0; i--) {
        FpPort *port = r->fp_port[i];
        fp_port_free(port);
    }
    i = rocker_pci_ring_count(r);
err_ring_alloc:
    for (--i; i >= 0; i--) {
        desc_ring_free(r->rings[i]);
    }
    g_free(r->rings);
err_rings_alloc:
err_duplicate:
    rocker_msix_uninit(r);
err_msix_init:
    object_unparent(OBJECT(&r->msix_bar));
    object_unparent(OBJECT(&r->mmio));
err_world_alloc:
    for (i = 0; i < ROCKER_WORLD_TYPE_MAX; i++) {
        if (r->worlds[i]) {
            world_free(r->worlds[i]);
        }
    }
    return err;
}
