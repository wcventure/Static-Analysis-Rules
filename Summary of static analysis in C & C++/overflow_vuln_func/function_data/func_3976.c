static int rac_get_model256_sym(RangeCoder *c, Model256 *m)
{
    int prob, prob2, helper, val;
    int start, end;
    int ssym;

    prob2      = c->range;
    c->range >>= MODEL_SCALE;

    helper     = c->low / c->range;
    ssym       = helper >> MODEL256_SEC_SCALE;
    val        = m->secondary[ssym];

    end = start = m->secondary[ssym + 1] + 1;
    while (end > val + 1) {
        ssym = (end + val) >> 1;
        if (m->freqs[ssym] <= helper) {
            end = start;
            val = ssym;
        } else {
            end   = (end + val) >> 1;
            start = ssym;
        }
    }
    prob = m->freqs[val] * c->range;
    if (val != 255)
        prob2 = m->freqs[val + 1] * c->range;

    c->low  -= prob;
    c->range = prob2 - prob;
    if (c->range < RAC_BOTTOM)
        rac_normalise(c);

    model256_update(m, val);

    return val;
}
