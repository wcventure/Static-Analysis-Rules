static inline target_ulong name (uint32_t opcode)                             \
{                                                                             \
    return (opcode >> (shift)) & ((1 << (nb)) - 1);                           \
}
