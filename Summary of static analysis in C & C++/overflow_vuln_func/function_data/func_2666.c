VLANClientState *qemu_new_vlan_client(VLANState *vlan,
                                      IOReadHandler *fd_read, void *opaque)
{
    VLANClientState *vc, **pvc;
    vc = qemu_mallocz(sizeof(VLANClientState));
    if (!vc)
        return NULL;
    vc->fd_read = fd_read;
    vc->opaque = opaque;
    vc->vlan = vlan;

    vc->next = NULL;
    pvc = &vlan->first_client;
    while (*pvc != NULL)
        pvc = &(*pvc)->next;
    *pvc = vc;
    return vc;
}
