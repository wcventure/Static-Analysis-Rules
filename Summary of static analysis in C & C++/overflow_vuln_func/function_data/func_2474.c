static bool addrrange_intersects(AddrRange r1, AddrRange r2)
{
    return (r1.start >= r2.start && r1.start < r2.start + r2.size)
        || (r2.start >= r1.start && r2.start < r1.start + r1.size);
}
