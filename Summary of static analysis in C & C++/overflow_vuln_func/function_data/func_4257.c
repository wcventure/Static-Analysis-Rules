static void get_tree_codes(uint32_t *bits, int16_t *lens, uint8_t *xlat,
                           Node *nodes, int node,
                           uint32_t pfx, int pl, int *pos)
{
    int s;

    s = nodes[node].sym;
    if (s != -1) {
        bits[*pos] = (~pfx) & ((1U << FFMAX(pl, 1)) - 1);
        lens[*pos] = FFMAX(pl, 1);
        xlat[*pos] = s + (pl == 0);
        (*pos)++;
    } else {
        pfx <<= 1;
        pl++;
        get_tree_codes(bits, lens, xlat, nodes, nodes[node].l, pfx, pl,
                       pos);
        pfx |= 1;
        get_tree_codes(bits, lens, xlat, nodes, nodes[node].r, pfx, pl,
                       pos);
    }
}
