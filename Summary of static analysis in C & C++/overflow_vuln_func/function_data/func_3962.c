static void put_codebook_header(PutBitContext * pb, codebook_t * cb) {
    int i;
    int ordered = 0;

    put_bits(pb, 24, 0x564342); //magic
    put_bits(pb, 16, cb->ndimentions);
    put_bits(pb, 24, cb->nentries);

    for (i = 1; i < cb->nentries; i++) if (cb->entries[i].len < cb->entries[i-1].len) break;
    if (i == cb->nentries) ordered = 1;

    put_bits(pb, 1, ordered);
    if (ordered) {
        int len = cb->entries[0].len;
        put_bits(pb, 5, len);
        i = 0;
        while (i < cb->nentries) {
            int j;
            for (j = 0; j+i < cb->nentries; j++) if (cb->entries[j+i].len != len) break;
            put_bits(pb, ilog(cb->nentries - i), j);
            i += j;
            len++;
        }
    } else {
        int sparse = 0;
        for (i = 0; i < cb->nentries; i++) if (!cb->entries[i].len) break;
        if (i != cb->nentries) sparse = 1;
        put_bits(pb, 1, sparse);

        for (i = 0; i < cb->nentries; i++) {
            if (sparse) put_bits(pb, 1, !!cb->entries[i].len);
            if (cb->entries[i].len) put_bits(pb, 5, cb->entries[i].len - 1);
        }
    }

    put_bits(pb, 4, cb->lookup);
    if (cb->lookup) {
        int tmp = cb_lookup_vals(cb->lookup, cb->ndimentions, cb->nentries);
        int bits = ilog(cb->quantlist[0]);

        for (i = 1; i < tmp; i++) bits = FFMAX(bits, ilog(cb->quantlist[i]));

        put_float(pb, cb->min);
        put_float(pb, cb->delta);

        put_bits(pb, 4, bits - 1);
        put_bits(pb, 1, cb->seq_p);

        for (i = 0; i < tmp; i++) put_bits(pb, bits, cb->quantlist[i]);
    }
}
