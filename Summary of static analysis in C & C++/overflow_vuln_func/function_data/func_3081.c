static int ass_get_duration(const uint8_t *p)
{
    int sh, sm, ss, sc, eh, em, es, ec;
    uint64_t start, end;

    if (sscanf(p, "%*[^,],%d:%d:%d%*c%d,%d:%d:%d%*c%d",
               &sh, &sm, &ss, &sc, &eh, &em, &es, &ec) != 8)
        return 0;
    start = 3600000*sh + 60000*sm + 1000*ss + 10*sc;
    end   = 3600000*eh + 60000*em + 1000*es + 10*ec;
    return end - start;
}
