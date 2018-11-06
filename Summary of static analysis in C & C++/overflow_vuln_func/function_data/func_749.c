static int
rpcprogs_packet(void *dummy _U_, packet_info *pinfo, epan_dissect_t *edt _U_, const void *arg)
{
	const rpc_call_info_value *ri = arg;
	nstime_t delta;
	rpc_program_t *rp;

	if(!prog_list){
		/
		rp=g_malloc(sizeof(rpc_program_t));
		add_new_program(rp);
		rp->next=NULL;
		rp->program=ri->prog;
		rp->version=ri->vers;
		prog_list=rp;
	} else if((ri->prog==prog_list->program)
		&&(ri->vers==prog_list->version)){
		rp=prog_list;
	} else if( (ri->prog<prog_list->program)
		||((ri->prog==prog_list->program)&&(ri->vers<prog_list->version))){
		/
		rp=g_malloc(sizeof(rpc_program_t));
		add_new_program(rp);
		rp->next=prog_list;
		rp->program=ri->prog;
		rp->version=ri->vers;
		prog_list=rp;
	} else {
		/
		for(rp=prog_list;rp;rp=rp->next){
			if((rp->next)
			&& (rp->next->program==ri->prog)
			&& (rp->next->version==ri->vers)){
				rp=rp->next;
				break;
			}
			if((!rp->next)
			|| (rp->next->program>ri->prog)
			|| (  (rp->next->program==ri->prog)
			    &&(rp->next->version>ri->vers))){
				rpc_program_t *trp;
				trp=g_malloc(sizeof(rpc_program_t));
				add_new_program(trp);
				trp->next=rp->next;
				trp->program=ri->prog;
				trp->version=ri->vers;
				rp->next=trp;
				rp=trp;
				break;
			}
		}
	}


	/
	if(ri->request || !rp){
		return 0;
	}

	/
	nstime_delta(&delta, &pinfo->fd->abs_ts, &ri->req_time);

	if((rp->max.secs==0)
	&& (rp->max.nsecs==0) ){
		rp->max.secs=delta.secs;
		rp->max.nsecs=delta.nsecs;
	}

	if((rp->min.secs==0)
	&& (rp->min.nsecs==0) ){
		rp->min.secs=delta.secs;
		rp->min.nsecs=delta.nsecs;
	}

	if( (delta.secs<rp->min.secs)
	||( (delta.secs==rp->min.secs)
	  &&(delta.nsecs<rp->min.nsecs) ) ){
		rp->min.secs=delta.secs;
		rp->min.nsecs=delta.nsecs;
	}

	if( (delta.secs>rp->max.secs)
	||( (delta.secs==rp->max.secs)
	  &&(delta.nsecs>rp->max.nsecs) ) ){
		rp->max.secs=delta.secs;
		rp->max.nsecs=delta.nsecs;
	}

	rp->tot.secs += delta.secs;
	rp->tot.nsecs += delta.nsecs;
	if(rp->tot.nsecs>1000000000){
		rp->tot.nsecs-=1000000000;
		rp->tot.secs++;
	}
	rp->num++;

	return 1;
}
