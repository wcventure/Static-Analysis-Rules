static inline int get_ue_code(GetBitContext *gb, int order)
{
    if (order) {
        int ret = get_ue_golomb(gb) << order;
        return ret + get_bits(gb, order);
    }
    return get_ue_golomb(gb);
}
