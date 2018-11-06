static int cond_seq(tvbparse_t* tt, int offset, const tvbparse_wanted_t * wanted, tvbparse_elem_t** tok) {
    guint i;
    int len = 0;
    int start = offset;
    tvbparse_elem_t* ret_tok = NULL;

    if ( offset > tt->end_offset )
        return -1;
#ifdef TVBPARSE_DEBUG
    if (TVBPARSE_DEBUG & TVBPARSE_DEBUG_SEQ) ws_g_warning("cond_seq: START");
#endif

    for(i=0; i < wanted->control.elems->len; i++) {
        tvbparse_wanted_t* w = (tvbparse_wanted_t *)g_ptr_array_index(wanted->control.elems,i);
        tvbparse_elem_t* new_elem = NULL;

        if ( offset + w->len > tt->end_offset )
            return -1;


        len = w->condition(tt, offset, w, &new_elem);

        if (len >= 0) {
            if (ret_tok) {
                if (new_elem->len)
                    ret_tok->len = (new_elem->offset - ret_tok->offset) + new_elem->len;
                ret_tok->sub->last->next = new_elem;
                ret_tok->sub->last = new_elem;
            } else {
                ret_tok = new_tok(tt, wanted->id, new_elem->offset, new_elem->len, wanted);
                ret_tok->sub = new_elem;
                new_elem->last = new_elem;
            }
        } else {
            return -1;
        }

        offset += len;
        offset += ignore_fcn(tt,offset);
    }

    *tok = ret_tok;

#ifdef TVBPARSE_DEBUG
    if (TVBPARSE_DEBUG & TVBPARSE_DEBUG_SEQ) ws_g_warning("cond_seq: GOT len=%i",offset - start);
#endif

    return offset - start;
}