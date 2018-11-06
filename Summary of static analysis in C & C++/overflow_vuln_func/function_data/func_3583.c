static inline void asv2_put_level(PutBitContext *pb, int level)
{
    unsigned int index = level + 31;

    if (index <= 62) {
        put_bits(pb, ff_asv2_level_tab[index][1], ff_asv2_level_tab[index][0]);
    } else {
        put_bits(pb, ff_asv2_level_tab[31][1], ff_asv2_level_tab[31][0]);
        asv2_put_bits(pb, 8, level & 0xFF);
    }
}
