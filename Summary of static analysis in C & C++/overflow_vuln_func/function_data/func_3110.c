static int can_safely_read(GetBitContext* gb, int bits) {
    return get_bits_left(gb) >= bits;
}
