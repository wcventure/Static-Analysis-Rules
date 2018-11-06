static void read_partition(uint8_t *p, struct partition_record *r)
{
    r->bootable = p[0];
    r->start_head = p[1];
    r->start_cylinder = p[3] | ((p[2] << 2) & 0x0300);
    r->start_sector = p[2] & 0x3f;
    r->system = p[4];
    r->end_head = p[5];
    r->end_cylinder = p[7] | ((p[6] << 2) & 0x300);
    r->end_sector = p[6] & 0x3f;
    r->start_sector_abs = p[8] | p[9] << 8 | p[10] << 16 | p[11] << 24;
    r->nb_sectors_abs = p[12] | p[13] << 8 | p[14] << 16 | p[15] << 24;
}
