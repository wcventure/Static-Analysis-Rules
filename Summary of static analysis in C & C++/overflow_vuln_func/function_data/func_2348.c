static inline target_long name (uint32_t opcode)                              \
{                                                                             \
    return (int16_t)((opcode >> (shift)) & ((1 << (nb)) - 1));                \
}
