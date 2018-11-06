static void aeron_pos_add_length(aeron_pos_t * pos, guint32 length, guint32 term_length)
{
    guint32 next_term_offset = aeron_pos_roundup(pos->term_offset + length);

    if (next_term_offset >= term_length)
    {
        pos->term_offset = 0;
        pos->term_id++;
    }
    else
    {
        pos->term_offset = next_term_offset;
    }
}
