static guint64
get_it_value(io_stat_t *io, int graph, int idx)
{
	guint64 value=0; /
	int adv_type;
	io_item_t *it;
	guint32 interval;

	it = &io->graphs[graph].items[idx];

	switch(io->count_type){
	case COUNT_TYPE_FRAMES:
		return it->frames;
	case COUNT_TYPE_BYTES:
		return it->bytes;
	case COUNT_TYPE_BITS:
		return (it->bytes * 8);
	}


	adv_type=proto_registrar_get_ftype(io->graphs[graph].hf_index);
	switch(adv_type){
	case FT_NONE:
		switch(io->graphs[graph].calc_type){
		case CALC_TYPE_COUNT_FRAMES:
			value=it->frames;
			break;
		case CALC_TYPE_COUNT_FIELDS:
			value=it->fields;
			break;
		default:
			break;
		}
		break;
	case FT_UINT8:
	case FT_UINT16:
	case FT_UINT24:
	case FT_UINT32:
	case FT_UINT64:
	case FT_INT8:
	case FT_INT16:
	case FT_INT24:
	case FT_INT32:
	case FT_INT64:
		switch(io->graphs[graph].calc_type){
		case CALC_TYPE_SUM:
			value=it->int_tot;
			break;
		case CALC_TYPE_COUNT_FRAMES:
			value=it->frames;
			break;
		case CALC_TYPE_COUNT_FIELDS:
			value=it->fields;
			break;
		case CALC_TYPE_MAX:
			value=it->int_max;
			break;
		case CALC_TYPE_MIN:
			value=it->int_min;
			break;
		case CALC_TYPE_AVG:
			if(it->fields){
				value=it->int_tot/it->fields;
			} else {
				value=0;
			}
			break;
		default:
			break;
		}
		break;
	case FT_FLOAT:
		switch(io->graphs[graph].calc_type){
		case CALC_TYPE_SUM:
			value=(guint64)it->float_tot;
			break;
		case CALC_TYPE_COUNT_FRAMES:
			value=it->frames;
			break;
		case CALC_TYPE_COUNT_FIELDS:
			value=it->fields;
			break;
		case CALC_TYPE_MAX:
			value=(guint64)it->float_max;
			break;
		case CALC_TYPE_MIN:
			value=(guint64)it->float_min;
			break;
		case CALC_TYPE_AVG:
			if(it->fields){
				value=(guint64)it->float_tot/it->fields;
			} else {
				value=0;
			}
			break;
		default:
			break;
		}
		break;
	case FT_DOUBLE:
		switch(io->graphs[graph].calc_type){
		case CALC_TYPE_SUM:
			value=(guint64)it->double_tot;
			break;
		case CALC_TYPE_COUNT_FRAMES:
			value=it->frames;
			break;
		case CALC_TYPE_COUNT_FIELDS:
			value=it->fields;
			break;
		case CALC_TYPE_MAX:
			value=(guint64)it->double_max;
			break;
		case CALC_TYPE_MIN:
			value=(guint64)it->double_min;
			break;
		case CALC_TYPE_AVG:
			if(it->fields){
				value=(guint64)it->double_tot/it->fields;
			} else {
				value=0;
			}
			break;
		default:
			break;
		}
		break;
	case FT_RELATIVE_TIME:
		switch(io->graphs[graph].calc_type){
		case CALC_TYPE_COUNT_FRAMES:
			value=it->frames;
			break;
		case CALC_TYPE_COUNT_FIELDS:
			value=it->fields;
			break;
		case CALC_TYPE_MAX:
			value=(guint64) (it->time_max.secs*1000000 + it->time_max.nsecs/1000);
			break;
		case CALC_TYPE_MIN:
			value=(guint64) (it->time_min.secs*1000000 + it->time_min.nsecs/1000);
			break;
		case CALC_TYPE_SUM:
			value=(guint64) (it->time_tot.secs*1000000 + it->time_tot.nsecs/1000);
			break;
		case CALC_TYPE_AVG:
			if(it->fields){
				guint64 t; /

				t=it->time_tot.secs;
				t=t*1000000+it->time_tot.nsecs/1000;
				value=(guint64) (t/it->fields);
			} else {
				value=0;
			}
			break;
		case CALC_TYPE_LOAD:
			if (idx==(int)io->num_items) {
				interval = (guint32)((cfile.elapsed_time.secs*1000) +
					   ((cfile.elapsed_time.nsecs+500000)/1000000));
				interval -= (io->interval * idx);
			} else {
				interval = io->interval;
			}
			value = (guint64) ((it->time_tot.secs*1000000 + it->time_tot.nsecs/1000) / interval);
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
	return value;
}
