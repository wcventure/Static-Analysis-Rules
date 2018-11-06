void ide_drive_get(DriveInfo **hd, int max_bus)
{
    int i;

    if (drive_get_max_bus(IF_IDE) >= max_bus) {
        fprintf(stderr, "qemu: too many IDE bus: %d\n", max_bus);
        exit(1);
    }

    for(i = 0; i < max_bus * MAX_IDE_DEVS; i++) {
        hd[i] = drive_get(IF_IDE, i / MAX_IDE_DEVS, i % MAX_IDE_DEVS);
    }
}
