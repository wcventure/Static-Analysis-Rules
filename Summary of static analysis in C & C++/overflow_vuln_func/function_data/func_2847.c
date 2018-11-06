ram_addr_t ppc405_set_bootinfo (CPUState *env, ppc4xx_bd_info_t *bd,
                                uint32_t flags)
{
    ram_addr_t bdloc;
    int i, n;

    /
    if (bd->bi_memsize >= 0x01000000UL)
        bdloc = 0x01000000UL - sizeof(struct ppc4xx_bd_info_t);
    else
        bdloc = bd->bi_memsize - sizeof(struct ppc4xx_bd_info_t);
    stl_phys(bdloc + 0x00, bd->bi_memstart);
    stl_phys(bdloc + 0x04, bd->bi_memsize);
    stl_phys(bdloc + 0x08, bd->bi_flashstart);
    stl_phys(bdloc + 0x0C, bd->bi_flashsize);
    stl_phys(bdloc + 0x10, bd->bi_flashoffset);
    stl_phys(bdloc + 0x14, bd->bi_sramstart);
    stl_phys(bdloc + 0x18, bd->bi_sramsize);
    stl_phys(bdloc + 0x1C, bd->bi_bootflags);
    stl_phys(bdloc + 0x20, bd->bi_ipaddr);
    for (i = 0; i < 6; i++)
        stb_phys(bdloc + 0x24 + i, bd->bi_enetaddr[i]);
    stw_phys(bdloc + 0x2A, bd->bi_ethspeed);
    stl_phys(bdloc + 0x2C, bd->bi_intfreq);
    stl_phys(bdloc + 0x30, bd->bi_busfreq);
    stl_phys(bdloc + 0x34, bd->bi_baudrate);
    for (i = 0; i < 4; i++)
        stb_phys(bdloc + 0x38 + i, bd->bi_s_version[i]);
    for (i = 0; i < 32; i++)
        stb_phys(bdloc + 0x3C + i, bd->bi_s_version[i]);
    stl_phys(bdloc + 0x5C, bd->bi_plb_busfreq);
    stl_phys(bdloc + 0x60, bd->bi_pci_busfreq);
    for (i = 0; i < 6; i++)
        stb_phys(bdloc + 0x64 + i, bd->bi_pci_enetaddr[i]);
    n = 0x6A;
    if (flags & 0x00000001) {
        for (i = 0; i < 6; i++)
            stb_phys(bdloc + n++, bd->bi_pci_enetaddr2[i]);
    }
    stl_phys(bdloc + n, bd->bi_opbfreq);
    n += 4;
    for (i = 0; i < 2; i++) {
        stl_phys(bdloc + n, bd->bi_iic_fast[i]);
        n += 4;
    }

    return bdloc;
}
