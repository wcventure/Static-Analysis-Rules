static int jp2_find_codestream(J2kDecoderContext *s)
{
    int32_t atom_size;
    int found_codestream = 0, search_range = 10;

    // skip jpeg2k signature atom
    s->buf += 12;

    while(!found_codestream && search_range) {
        atom_size = AV_RB32(s->buf);
        if(AV_RB32(s->buf + 4) == JP2_CODESTREAM) {
            found_codestream = 1;
            s->buf += 8;
        } else {
            s->buf += atom_size;
            search_range--;
        }
    }

    if(found_codestream)
        return 1;
    return 0;
}
