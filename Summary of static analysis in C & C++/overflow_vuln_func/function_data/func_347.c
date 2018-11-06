static h223_mux_element* find_h223_mux_element(h223_call_direction_data* direct, guint8 mc, guint32 framenum)
{
    h223_mux_element_listitem* li = direct->mux_table[mc];
    while( li && li->next && li->next->first_frame < framenum )
        li = li->next;
    while( li && li->next && li->next->first_frame == framenum && li->next->pdu_offset < pdu_offset )
        li = li->next;
    if( li ) {
        return li->me;
    } else {
        return NULL;
    }
}
