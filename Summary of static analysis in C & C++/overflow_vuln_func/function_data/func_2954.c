static int usb_host_open(USBHostDevice *dev, int bus_num,
                         int addr, const char *port,
                         const char *prod_name, int speed)
{
    int fd = -1, ret;

    trace_usb_host_open_started(bus_num, addr);

    if (dev->fd != -1) {
        goto fail;
    }

    fd = usb_host_open_device(bus_num, addr);
    if (fd < 0) {
        goto fail;
    }
    DPRINTF("husb: opened %s\n", buf);

    dev->bus_num = bus_num;
    dev->addr = addr;
    strcpy(dev->port, port);
    dev->fd = fd;

    /
    dev->descr_len = read(fd, dev->descr, sizeof(dev->descr));
    if (dev->descr_len <= 0) {
        perror("husb: reading device data failed");
        goto fail;
    }

#ifdef DEBUG
    {
        int x;
        printf("=== begin dumping device descriptor data ===\n");
        for (x = 0; x < dev->descr_len; x++) {
            printf("%02x ", dev->descr[x]);
        }
        printf("\n=== end dumping device descriptor data ===\n");
    }
#endif


    /
    if (!usb_host_claim_interfaces(dev, 0)) {
        goto fail;
    }

    usb_ep_init(&dev->dev);
    usb_linux_update_endp_table(dev);

    if (speed == -1) {
        struct usbdevfs_connectinfo ci;

        ret = ioctl(fd, USBDEVFS_CONNECTINFO, &ci);
        if (ret < 0) {
            perror("usb_host_device_open: USBDEVFS_CONNECTINFO");
            goto fail;
        }

        if (ci.slow) {
            speed = USB_SPEED_LOW;
        } else {
            speed = USB_SPEED_HIGH;
        }
    }
    dev->dev.speed = speed;
    dev->dev.speedmask = (1 << speed);
    if (dev->dev.speed == USB_SPEED_HIGH && usb_linux_full_speed_compat(dev)) {
        dev->dev.speedmask |= USB_SPEED_MASK_FULL;
    }

    trace_usb_host_open_success(bus_num, addr);

    if (!prod_name || prod_name[0] == '\0') {
        snprintf(dev->dev.product_desc, sizeof(dev->dev.product_desc),
                 "host:%d.%d", bus_num, addr);
    } else {
        pstrcpy(dev->dev.product_desc, sizeof(dev->dev.product_desc),
                prod_name);
    }

    ret = usb_device_attach(&dev->dev);
    if (ret) {
        goto fail;
    }

    /
    qemu_set_fd_handler(dev->fd, NULL, async_complete, dev);

    return 0;

fail:
    trace_usb_host_open_failure(bus_num, addr);
    if (dev->fd != -1) {
        close(dev->fd);
        dev->fd = -1;
    }
    return -1;
}
