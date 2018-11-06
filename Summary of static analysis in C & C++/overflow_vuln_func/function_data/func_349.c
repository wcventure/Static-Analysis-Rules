static h223_lc_params* find_h223_lc_params(h223_vc_info* vc_info, int direction, guint32 framenum)
{
    h223_lc_params_listitem* li = vc_info->lc_params[direction];
    while( li && li->next && li->next->first_frame <= framenum )
        li = li->next;
    if( li )
        return li->lc_params;
    else
        return NULL;
}
