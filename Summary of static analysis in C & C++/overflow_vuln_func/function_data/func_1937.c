static int vfio_populate_device(VFIODevice *vbasedev)
{
    VFIOINTp *intp, *tmp;
    int i, ret = -1;
    VFIOPlatformDevice *vdev =
        container_of(vbasedev, VFIOPlatformDevice, vbasedev);

    if (!(vbasedev->flags & VFIO_DEVICE_FLAGS_PLATFORM)) {
        error_report("vfio: Um, this isn't a platform device");
        return ret;
    }

    vdev->regions = g_new0(VFIORegion *, vbasedev->num_regions);

    for (i = 0; i < vbasedev->num_regions; i++) {
        struct vfio_region_info reg_info = { .argsz = sizeof(reg_info) };
        VFIORegion *ptr;

        vdev->regions[i] = g_malloc0(sizeof(VFIORegion));
        ptr = vdev->regions[i];
        reg_info.index = i;
        ret = ioctl(vbasedev->fd, VFIO_DEVICE_GET_REGION_INFO, &reg_info);
        if (ret) {
            error_report("vfio: Error getting region %d info: %m", i);
            goto reg_error;
        }
        ptr->flags = reg_info.flags;
        ptr->size = reg_info.size;
        ptr->fd_offset = reg_info.offset;
        ptr->nr = i;
        ptr->vbasedev = vbasedev;

        trace_vfio_platform_populate_regions(ptr->nr,
                            (unsigned long)ptr->flags,
                            (unsigned long)ptr->size,
                            ptr->vbasedev->fd,
                            (unsigned long)ptr->fd_offset);
    }

    vdev->mmap_timer = timer_new_ms(QEMU_CLOCK_VIRTUAL,
                                    vfio_intp_mmap_enable, vdev);

    QSIMPLEQ_INIT(&vdev->pending_intp_queue);

    for (i = 0; i < vbasedev->num_irqs; i++) {
        struct vfio_irq_info irq = { .argsz = sizeof(irq) };

        irq.index = i;
        ret = ioctl(vbasedev->fd, VFIO_DEVICE_GET_IRQ_INFO, &irq);
        if (ret) {
            error_printf("vfio: error getting device %s irq info",
                         vbasedev->name);
            goto irq_err;
        } else {
            trace_vfio_platform_populate_interrupts(irq.index,
                                                    irq.count,
                                                    irq.flags);
            intp = vfio_init_intp(vbasedev, irq);
            if (!intp) {
                error_report("vfio: Error installing IRQ %d up", i);
                goto irq_err;
            }
        }
    }
    return 0;
irq_err:
    timer_del(vdev->mmap_timer);
    QLIST_FOREACH_SAFE(intp, &vdev->intp_list, next, tmp) {
        QLIST_REMOVE(intp, next);
        g_free(intp);
    }
reg_error:
    for (i = 0; i < vbasedev->num_regions; i++) {
        g_free(vdev->regions[i]);
    }
    g_free(vdev->regions);
    return ret;
}
