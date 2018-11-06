static void add_h223_lc_params(h223_vc_info* vc_info, int direction, h223_lc_params *lc_params, guint32 framenum )
{
    h223_lc_params_listitem *li = se_alloc(sizeof(h223_lc_params_listitem));
    h223_lc_params_listitem **old_li_ptr = &(vc_info->lc_params[direction]);
    h223_lc_params_listitem *old_li = *old_li_ptr;
    if( !old_li ) {
        vc_info->lc_params[direction] = li;
    } else {
        while( old_li->next ) {
            old_li_ptr = &(old_li->next);
            old_li = *old_li_ptr;
        }
        if( framenum < old_li->first_frame )
            return;
        else if( framenum == old_li->first_frame )
            *old_li_ptr = li;
        else {
            old_li->next = li;
            old_li->last_frame = framenum - 1;
        }
    }
    li->first_frame = framenum;
    li->last_frame = 0;
    li->next = 0;
    li->lc_params = lc_params;
}
