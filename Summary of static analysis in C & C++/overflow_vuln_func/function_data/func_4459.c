static int unpack_parse_unit(DiracParseUnit *pu, DiracParseContext *pc,
                             int offset)
{
    uint8_t *start = pc->buffer + offset;
    uint8_t *end   = pc->buffer + pc->index;
    if (start < pc->buffer || (start + 13 > end))
        return 0;
    pu->pu_type = start[4];

    pu->next_pu_offset = AV_RB32(start + 5);
    pu->prev_pu_offset = AV_RB32(start + 9);

    if (pu->pu_type == 0x10 && pu->next_pu_offset == 0)
        pu->next_pu_offset = 13;

    return 1;
}
