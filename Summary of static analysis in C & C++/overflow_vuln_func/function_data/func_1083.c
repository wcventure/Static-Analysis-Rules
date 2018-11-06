static guint32 aeron_pos_delta(const aeron_pos_t * pos1, const aeron_pos_t * pos2, guint32 term_size)
{
    const aeron_pos_t * p1;
    const aeron_pos_t * p2;
    guint64 p1_val;
    guint64 p2_val;
    guint64 delta;
    int rc;

    rc = aeron_pos_compare(pos1, pos2);
    if (rc >= 0)
    {
        p1 = pos1;
        p2 = pos2;
    }
    else
    {
        p1 = pos2;
        p2 = pos1;
    }
    p1_val = (guint64) (p1->term_id * term_size) + ((guint64) p1->term_offset);
    p2_val = (guint64) (p2->term_id * term_size) + ((guint64) p2->term_offset);
    delta = p1_val - p2_val;
    return ((guint32) (delta & G_GUINT64_CONSTANT(0x00000000ffffffff)));
}
