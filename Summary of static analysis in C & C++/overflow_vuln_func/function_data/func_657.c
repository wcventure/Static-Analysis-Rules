static int
iostat_packet(void *arg, packet_info *pinfo, epan_dissect_t *edt, const void *dummy _U_)
{
    io_stat_t *parent;
    io_stat_item_t *mit;
    io_stat_item_t *it;
    guint64 relative_time, rt;
    nstime_t *new_time;
    GPtrArray *gp;
    guint i;
    int ftype;

    mit = (io_stat_item_t *) arg;
    parent = mit->parent;
    relative_time = (guint64)((pinfo->fd->rel_ts.secs*1000000) + ((pinfo->fd->rel_ts.nsecs+500)/1000));
    if (mit->parent->start_time == 0) {
        mit->parent->start_time = pinfo->fd->abs_ts.secs - pinfo->fd->rel_ts.secs;
    }

    /
    it = mit->prev;

    /
    if(relative_time < it->time){
        return FALSE;
    }

    /
    rt = relative_time;
    while (rt >= it->time + parent->interval) {
        it->next = (io_stat_item_t *)g_malloc(sizeof(io_stat_item_t));
        it->next->prev = it;
        it->next->next = NULL;
        it = it->next;
        mit->prev = it;

        it->time = it->prev->time + parent->interval;
        it->frames = 0;
        it->counter = 0;
        it->float_counter = 0;
        it->double_counter = 0;
        it->num = 0;
        it->calc_type = it->prev->calc_type;
        it->hf_index = it->prev->hf_index;
        it->colnum = it->prev->colnum;
    }

    /
    it->frames++;

    switch(it->calc_type) {
    case CALC_TYPE_FRAMES:
    case CALC_TYPE_BYTES:
    case CALC_TYPE_FRAMES_AND_BYTES:
        it->counter += pinfo->fd->pkt_len;
        break;
    case CALC_TYPE_COUNT:
        gp=proto_get_finfo_ptr_array(edt->tree, it->hf_index);
        if(gp){
            it->counter += gp->len;
        }
        break;
    case CALC_TYPE_SUM:
        gp=proto_get_finfo_ptr_array(edt->tree, it->hf_index);
        if(gp){
            guint64 val;

            for(i=0;i<gp->len;i++){
                switch(proto_registrar_get_ftype(it->hf_index)){
                case FT_UINT8:
                case FT_UINT16:
                case FT_UINT24:
                case FT_UINT32:
                    it->counter += fvalue_get_uinteger(&((field_info *)gp->pdata[i])->value);
                    break;
                case FT_UINT64:
                    it->counter += fvalue_get_integer64(&((field_info *)gp->pdata[i])->value);
                    break;
                case FT_INT8:
                case FT_INT16:
                case FT_INT24:
                case FT_INT32:
                    it->counter += fvalue_get_sinteger(&((field_info *)gp->pdata[i])->value);
                    break;
                case FT_INT64:
                    it->counter += (gint64)fvalue_get_integer64(&((field_info *)gp->pdata[i])->value);
                    break;
                case FT_FLOAT:
                    it->float_counter +=
                        (gfloat)fvalue_get_floating(&((field_info *)gp->pdata[i])->value);
                    break;
                case FT_DOUBLE:
                    it->double_counter += fvalue_get_floating(&((field_info *)gp->pdata[i])->value);
                    break;
                case FT_RELATIVE_TIME:
                    new_time = (nstime_t *)fvalue_get(&((field_info *)gp->pdata[i])->value);
                    val = (guint64)((new_time->secs * NANOSECS_PER_SEC) + new_time->nsecs);
                    it->counter  +=  val;
                    break;
                default:
                    /
                    g_assert_not_reached();
                    break;
                }
            }
        }
        break;
    case CALC_TYPE_MIN:
        gp=proto_get_finfo_ptr_array(edt->tree, it->hf_index);
        if(gp){
            guint64 val;
            gfloat float_val;
            gdouble double_val;

            ftype=proto_registrar_get_ftype(it->hf_index);
            for(i=0;i<gp->len;i++){
                switch(ftype){
                case FT_UINT8:
                case FT_UINT16:
                case FT_UINT24:
                case FT_UINT32:
                    val = fvalue_get_uinteger(&((field_info *)gp->pdata[i])->value);
                    if ((it->frames==1 && i==0) || (val < it->counter)) {
                        it->counter=val;
                    }
                    break;
                case FT_UINT64:
                    val = fvalue_get_integer64(&((field_info *)gp->pdata[i])->value);
                    if((it->frames==1 && i==0) || (val < it->counter)){
                        it->counter=val;
                    }
                    break;
                case FT_INT8:
                case FT_INT16:
                case FT_INT24:
                case FT_INT32:
                    val = fvalue_get_sinteger(&((field_info *)gp->pdata[i])->value);
                    if((it->frames==1 && i==0) || ((gint32)val < (gint32)it->counter)) {
                        it->counter=val;
                    }
                    break;
                case FT_INT64:
                    val = fvalue_get_integer64(&((field_info *)gp->pdata[i])->value);
                    if((it->frames==1 && i==0) || ((gint64)val < (gint64)it->counter)) {
                        it->counter=val;
                    }
                    break;
                case FT_FLOAT:
                    float_val=(gfloat)fvalue_get_floating(&((field_info *)gp->pdata[i])->value);
                    if((it->frames==1 && i==0) || (float_val < it->float_counter)) {
                        it->float_counter=float_val;
                    }
                    break;
                case FT_DOUBLE:
                    double_val=fvalue_get_floating(&((field_info *)gp->pdata[i])->value);
                    if((it->frames==1 && i==0) || (double_val < it->double_counter)) {
                        it->double_counter=double_val;
                    }
                    break;
                case FT_RELATIVE_TIME:
                    new_time = (nstime_t *)fvalue_get(&((field_info *)gp->pdata[i])->value);
                    val = (guint64)new_time->secs * NANOSECS_PER_SEC + new_time->nsecs;
                    if((it->frames==1 && i==0) || (val < it->counter)) {
                        it->counter=val;
                    }
                    break;
                default:
                    /
                    g_assert_not_reached();
                    break;
                }
            }
        }
        break;
    case CALC_TYPE_MAX:
        gp=proto_get_finfo_ptr_array(edt->tree, it->hf_index);
        if(gp){
            guint64 val;
            gfloat float_val;
            gdouble double_val;

            ftype=proto_registrar_get_ftype(it->hf_index);
            for(i=0;i<gp->len;i++){
                switch(ftype){
                case FT_UINT8:
                case FT_UINT16:
                case FT_UINT24:
                case FT_UINT32:
                    val = fvalue_get_uinteger(&((field_info *)gp->pdata[i])->value);
                    if(val > it->counter)
                        it->counter=val;
                    break;
                case FT_UINT64:
                    val = fvalue_get_integer64(&((field_info *)gp->pdata[i])->value);
                    if(val > it->counter)
                        it->counter=val;
                    break;
                case FT_INT8:
                case FT_INT16:
                case FT_INT24:
                case FT_INT32:
                    val = fvalue_get_sinteger(&((field_info *)gp->pdata[i])->value);
                    if((gint32)val > (gint32)it->counter)
                        it->counter=val;
                    break;
                case FT_INT64:
                    val = fvalue_get_integer64(&((field_info *)gp->pdata[i])->value);
                    if ((gint64)val > (gint64)it->counter)
                        it->counter=val;
                    break;
                case FT_FLOAT:
                    float_val = (gfloat)fvalue_get_floating(&((field_info *)gp->pdata[i])->value);
                    if(float_val > it->float_counter)
                        it->float_counter=float_val;
                    break;
                case FT_DOUBLE:
                    double_val = fvalue_get_floating(&((field_info *)gp->pdata[i])->value);
                    if(double_val > it->double_counter)
                        it->double_counter=double_val;
                    break;
                case FT_RELATIVE_TIME:
                    new_time = (nstime_t *)fvalue_get(&((field_info *)gp->pdata[i])->value);
                    val = (guint64)((new_time->secs * NANOSECS_PER_SEC) + new_time->nsecs);
                    if (val>it->counter)
                        it->counter=val;
                    break;
                default:
                    /
                    g_assert_not_reached();
                    break;
                }
            }
        }
        break;
    case CALC_TYPE_AVG:
        gp=proto_get_finfo_ptr_array(edt->tree, it->hf_index);
        if(gp){
            guint64 val;

            ftype=proto_registrar_get_ftype(it->hf_index);
            for(i=0;i<gp->len;i++){
                it->num++;
                switch(ftype) {
                case FT_UINT8:
                case FT_UINT16:
                case FT_UINT24:
                case FT_UINT32:
                    val = fvalue_get_uinteger(&((field_info *)gp->pdata[i])->value);
                    it->counter += val;
                    break;
                case FT_UINT64:
                case FT_INT64:
                    val = fvalue_get_integer64(&((field_info *)gp->pdata[i])->value);
                    it->counter += val;
                    break;
                case FT_INT8:
                case FT_INT16:
                case FT_INT24:
                case FT_INT32:
                    val = fvalue_get_sinteger(&((field_info *)gp->pdata[i])->value);
                    it->counter += val;
                    break;
                case FT_FLOAT:
                    it->float_counter += (gfloat)fvalue_get_floating(&((field_info *)gp->pdata[i])->value);
                    break;
                case FT_DOUBLE:
                    it->double_counter += fvalue_get_floating(&((field_info *)gp->pdata[i])->value);
                    break;
                case FT_RELATIVE_TIME:
                    new_time = (nstime_t *)fvalue_get(&((field_info *)gp->pdata[i])->value);
                    val = (guint64)((new_time->secs * NANOSECS_PER_SEC) + new_time->nsecs);
                    it->counter += val;
                    break;
                default:
                    /
                    g_assert_not_reached();
                    break;
                }
            }
        }
        break;
    case CALC_TYPE_LOAD:
        gp = proto_get_finfo_ptr_array(edt->tree, it->hf_index);
        if (gp) {
            ftype = proto_registrar_get_ftype(it->hf_index);
            if (ftype != FT_RELATIVE_TIME) {
                fprintf(stderr,
                    "\ntshark: LOAD() is only supported for relative-time fields such as smb.time\n");
                exit(10);
            }
            for(i=0;i<gp->len;i++){
                guint64 val;
                int tival;
                io_stat_item_t *pit;

                new_time = (nstime_t *)fvalue_get(&((field_info *)gp->pdata[i])->value);
                val = (guint64)((new_time->secs*1000000) + (new_time->nsecs/1000));
                tival = (int)(val % parent->interval);
                it->counter += tival;
                val -= tival;
                pit = it->prev;
                while (val > 0) {
                    if (val < (guint64)parent->interval) {
                        pit->counter += val;
                        break;
                    }
                    pit->counter += parent->interval;
                    val -= parent->interval;
                    pit = pit->prev;
                }
            }
        }
        break;
    }
    /
    switch(it->calc_type) {
        case CALC_TYPE_FRAMES:
        case CALC_TYPE_FRAMES_AND_BYTES:
            parent->max_frame[it->colnum] =
                MAX(parent->max_frame[it->colnum], it->frames);
            if (it->calc_type==CALC_TYPE_FRAMES_AND_BYTES)
                parent->max_vals[it->colnum] =
                    MAX(parent->max_vals[it->colnum], it->counter);

        case CALC_TYPE_BYTES:
        case CALC_TYPE_COUNT:
        case CALC_TYPE_LOAD:
            parent->max_vals[it->colnum] = MAX(parent->max_vals[it->colnum], it->counter);
            break;
        case CALC_TYPE_SUM:
        case CALC_TYPE_MIN:
        case CALC_TYPE_MAX:
            ftype=proto_registrar_get_ftype(it->hf_index);
            switch(ftype) {
                case FT_FLOAT:
                    parent->max_vals[it->colnum] =
                        MAX(parent->max_vals[it->colnum], (guint64)(it->float_counter+0.5));
                    break;
                case FT_DOUBLE:
                    parent->max_vals[it->colnum] =
                        MAX(parent->max_vals[it->colnum],(guint64)(it->double_counter+0.5));
                    break;
                case FT_RELATIVE_TIME:
                    parent->max_vals[it->colnum] =
                        MAX(parent->max_vals[it->colnum], it->counter);
                    break;
                default:
                    /
                    parent->max_vals[it->colnum] =
                        MAX(parent->max_vals[it->colnum], it->counter);
                    break;
            }
            break;
        case CALC_TYPE_AVG:
            if (it->num==0) /
               break;
            ftype=proto_registrar_get_ftype(it->hf_index);
            switch(ftype) {
                case FT_FLOAT:
                    parent->max_vals[it->colnum] =
                        MAX(parent->max_vals[it->colnum], (guint64)it->float_counter/it->num);
                    break;
                case FT_DOUBLE:
                    parent->max_vals[it->colnum] =
                        MAX(parent->max_vals[it->colnum],(guint64)it->double_counter/it->num);
                    break;
                case FT_RELATIVE_TIME:
                    parent->max_vals[it->colnum] =
                        MAX(parent->max_vals[it->colnum], ((it->counter/it->num) + 500000000) / NANOSECS_PER_SEC);
                    break;
                default:
                    /
                    parent->max_vals[it->colnum] =
                        MAX(parent->max_vals[it->colnum], it->counter/it->num);
                    break;
            }
    }
    return TRUE;
}
