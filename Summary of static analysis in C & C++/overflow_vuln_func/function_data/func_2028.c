int tap_open(char *ifname, int ifname_size, int *vnet_hdr,
             int vnet_hdr_required, int mq_required)
{
    struct ifreq ifr;
    int fd, ret;
    int len = sizeof(struct virtio_net_hdr);

    TFR(fd = open(PATH_NET_TUN, O_RDWR));
    if (fd < 0) {
        error_report("could not open %s: %m", PATH_NET_TUN);
        return -1;
    }
    memset(&ifr, 0, sizeof(ifr));
    ifr.ifr_flags = IFF_TAP | IFF_NO_PI;

    if (*vnet_hdr) {
        unsigned int features;

        if (ioctl(fd, TUNGETFEATURES, &features) == 0 &&
            features & IFF_VNET_HDR) {
            *vnet_hdr = 1;
            ifr.ifr_flags |= IFF_VNET_HDR;
        } else {
            *vnet_hdr = 0;
        }

        if (vnet_hdr_required && !*vnet_hdr) {
            error_report("vnet_hdr=1 requested, but no kernel "
                         "support for IFF_VNET_HDR available");
            close(fd);
            return -1;
        }
        /
        ioctl(fd, TUNSETVNETHDRSZ, &len);
    }

    if (mq_required) {
        unsigned int features;

        if ((ioctl(fd, TUNGETFEATURES, &features) != 0) ||
            !(features & IFF_MULTI_QUEUE)) {
            error_report("multiqueue required, but no kernel "
                         "support for IFF_MULTI_QUEUE available");
            close(fd);
            return -1;
        } else {
            ifr.ifr_flags |= IFF_MULTI_QUEUE;
        }
    }

    if (ifname[0] != '\0')
        pstrcpy(ifr.ifr_name, IFNAMSIZ, ifname);
    else
        pstrcpy(ifr.ifr_name, IFNAMSIZ, "tap%d");
    ret = ioctl(fd, TUNSETIFF, (void *) &ifr);
    if (ret != 0) {
        if (ifname[0] != '\0') {
            error_report("could not configure %s (%s): %m", PATH_NET_TUN, ifr.ifr_name);
        } else {
            error_report("could not configure %s: %m", PATH_NET_TUN);
        }
        close(fd);
        return -1;
    }
    pstrcpy(ifname, ifname_size, ifr.ifr_name);
    fcntl(fd, F_SETFL, O_NONBLOCK);
    return fd;
}
