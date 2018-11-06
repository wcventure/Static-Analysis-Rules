static int ipmi_register_netfn(IPMIBmcSim *s, unsigned int netfn,
                               const IPMINetfn *netfnd)
{
    if ((netfn & 1) || (netfn > MAX_NETFNS) || (s->netfns[netfn / 2])) {
        return -1;
    }
    s->netfns[netfn / 2] = netfnd;
    return 0;
}
