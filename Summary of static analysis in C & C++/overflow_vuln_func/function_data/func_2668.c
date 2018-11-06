static int net_slirp_init(VLANState *vlan)
{
    if (!slirp_inited) {
        slirp_inited = 1;
        slirp_init();
    }
    slirp_vc = qemu_new_vlan_client(vlan, 
                                    slirp_receive, NULL);
    snprintf(slirp_vc->info_str, sizeof(slirp_vc->info_str), "user redirector");
    return 0;
}
