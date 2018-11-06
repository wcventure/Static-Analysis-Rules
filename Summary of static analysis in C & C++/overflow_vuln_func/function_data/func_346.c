static void add_h223_mux_element(h223_call_direction_data *direct, guint8 mc, h223_mux_element *me, guint32 framenum)
{
    h223_mux_element_listitem *li = se_alloc(sizeof(h223_mux_element_listitem));
    h223_mux_element_listitem **old_li_ptr = &(direct->mux_table[mc]);
    h223_mux_element_listitem *old_li = *old_li_ptr;
    if( !old_li ) {
        direct->mux_table[mc] = li;
    } else {
        while( old_li->next ) {
            old_li_ptr = &(old_li->next);
            old_li = *old_li_ptr;
        }
        if( framenum < old_li->first_frame || (framenum == old_li->first_frame && pdu_offset < old_li->pdu_offset)  )
            return;
        else if ( framenum == old_li->first_frame && pdu_offset == old_li->pdu_offset )
            *old_li_ptr = li; // replace the tail of the list with the new item, since
                              // a single h223 pdu has just set the same MC twice..
        else
            old_li->next = li;
    }
    li->first_frame = framenum;
    li->pdu_offset = pdu_offset;
    li->next = 0;
    li->me = me;
}
