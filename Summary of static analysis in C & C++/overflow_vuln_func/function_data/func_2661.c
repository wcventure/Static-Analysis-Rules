static int ne2000_can_receive(void *opaque)
{
    NE2000State *s = opaque;
    int avail, index, boundary;
    
    if (s->cmd & E8390_STOP)
        return 0;
    index = s->curpag << 8;
    boundary = s->boundary << 8;
    if (index < boundary)
        avail = boundary - index;
    else
        avail = (s->stop - s->start) - (index - boundary);
    if (avail < (MAX_ETH_FRAME_SIZE + 4))
        return 0;
    return MAX_ETH_FRAME_SIZE;
}
