static gboolean
tap_iostat_packet(void *g, packet_info *pinfo, epan_dissect_t *edt, const void *dummy _U_)
{
	io_stat_graph_t *graph = g;
	io_stat_t *io;
	io_item_t *it;
	nstime_t time_delta;
	int idx;

	/
	if(!graph->display){
		return FALSE;
	}

	io = graph->io;  /
	io->needs_redraw = TRUE;

	/
	time_delta = pinfo->fd->rel_ts;
	if(time_delta.nsecs<0){
		time_delta.secs--;
		time_delta.nsecs += 1000000000;
	}
	if(time_delta.secs<0){
		return FALSE;
	}
	idx = (int) ((time_delta.secs*1000 + time_delta.nsecs/1000000) / io->interval);

	/
	if((idx < 0) || (idx >= NUM_IO_ITEMS)) {
		io->num_items = NUM_IO_ITEMS-1;
		return FALSE;
	}

	/
	if((guint32)idx > io->num_items){
		io->num_items = (guint32) idx;
	}

	/
	if(io->start_time.secs == 0 && io->start_time.nsecs == 0) {
		nstime_delta (&io->start_time, &pinfo->fd->abs_ts, &pinfo->fd->rel_ts);
	}

	/
	it = &graph->items[idx];

	/
	if (it->first_frame_in_invl == 0) {
		it->first_frame_in_invl = pinfo->fd->num;
	}
	it->last_frame_in_invl = pinfo->fd->num;

	/
	if(io->count_type==COUNT_TYPE_ADVANCED){
		GPtrArray *gp;
		guint i;

		gp = proto_get_finfo_ptr_array(edt->tree, graph->hf_index);
		if(!gp){
			return FALSE;
		}

		/
		for(i=0; i<gp->len; i++) {
			int new_int;
			gint64 new_int64;
			float new_float;
			double new_double;
			nstime_t *new_time;

			switch(proto_registrar_get_ftype(graph->hf_index)){
			case FT_UINT8:
			case FT_UINT16:
			case FT_UINT24:
			case FT_UINT32:
				new_int = fvalue_get_uinteger(&((field_info *)gp->pdata[i])->value);

				if((new_int > it->int_max)||(it->fields==0)){
					it->int_max = new_int;
				}
				if((new_int < it->int_min)||(it->fields==0)){
					it->int_min = new_int;
				}
				it->int_tot += new_int;
				it->fields++;
				break;
			case FT_INT8:
			case FT_INT16:
			case FT_INT24:
			case FT_INT32:
				new_int = fvalue_get_sinteger(&((field_info *)gp->pdata[i])->value);
				if((new_int > it->int_max) || (it->fields==0)){
					it->int_max = new_int;
				}
				if((new_int < it->int_min) || (it->fields==0)){
					it->int_min = new_int;
				}
				it->int_tot += new_int;
				it->fields++;
				break;
			case FT_UINT64:
			case FT_INT64:
				new_int64 = fvalue_get_integer64(&((field_info *)gp->pdata[i])->value);
				if((new_int64 > it->int_max) || (it->fields==0)){
					it->int_max = new_int64;
				}
				if((new_int64 < it->int_min) || (it->fields==0)){
					it->int_min = new_int64;
				}
				it->int_tot += new_int64;
				it->fields++;
				break;
			case FT_FLOAT:
				new_float = (gfloat)fvalue_get_floating(&((field_info *)gp->pdata[i])->value);
				if((new_float > it->float_max) || (it->fields==0)){
					it->float_max = new_float;
				}
				if((new_float < it->float_min) || (it->fields==0)){
					it->float_min = new_float;
				}
				it->float_tot += new_float;
				it->fields++;
				break;
			case FT_DOUBLE:
				new_double = fvalue_get_floating(&((field_info *)gp->pdata[i])->value);
				if((new_double > it->double_max) || (it->fields==0)){
					it->double_max = new_double;
				}
				if((new_double < it->double_min) || (it->fields==0)){
					it->double_min = new_double;
				}
				it->double_tot += new_double;
				it->fields++;
				break;
			case FT_RELATIVE_TIME:
				new_time = fvalue_get(&((field_info *)gp->pdata[0])->value);

				switch(graph->calc_type){
					guint64 t, pt; /
					int j;
				case CALC_TYPE_LOAD:
					/
					t = new_time->secs;
					t = t * 1000000 + new_time->nsecs / 1000;
					j = idx;
					/
					pt = pinfo->fd->rel_ts.secs * 1000000 + pinfo->fd->rel_ts.nsecs / 1000;
					pt = pt % (io->interval * 1000);
					if(pt > t) {
						pt = t;
					}
					while(t){
						graph->items[j].time_tot.nsecs += (int) (pt * 1000);
						if(graph->items[j].time_tot.nsecs > 1000000000){
							graph->items[j].time_tot.secs++;
							graph->items[j].time_tot.nsecs -= 1000000000;
						}

						if(j==0){
							break;
						}
						j--;
						t -= pt;
						if(t > (guint64) io->interval * 1000) {
							pt = (guint64) io->interval * 1000;
						} else {
							pt = t;
						}
					}
					break;
				default:
					if( (new_time->secs > it->time_max.secs)
					||( (new_time->secs==it->time_max.secs)
					  &&(new_time->nsecs > it->time_max.nsecs))
					||(it->fields==0)){
						it->time_max = *new_time;
					}
					if( (new_time->secs<it->time_min.secs)
					||( (new_time->secs==it->time_min.secs)
					  &&(new_time->nsecs < it->time_min.nsecs))
					||(it->fields==0)){
						it->time_min = *new_time;
					}
					nstime_add(&it->time_tot, new_time);
					it->fields++;
				}
				break;
			default:
				/
				g_assert_not_reached();
				break;
			}
		}
	}

	it->frames++;
	it->bytes += pinfo->fd->pkt_len;

	return TRUE;
}
