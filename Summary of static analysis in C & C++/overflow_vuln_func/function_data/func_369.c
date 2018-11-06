static int
mgcpstat_packet(void *pms, packet_info *pinfo, epan_dissect_t *edt _U_, void *pmi)
{
	mgcpstat_t *ms=(mgcpstat_t *)pms;
	mgcp_info_t *mi=pmi;
	nstime_t delta;

	switch (mi->mgcp_type) {
	
	case MGCP_REQUEST:
		if(mi->is_duplicate){
			/
			ms->req_dup_num++;
			return 0;
		}
		else {
			ms->open_req_num++;
			return 0;
		}
	break;
			
	case MGCP_RESPONSE:
		if(mi->is_duplicate){
			/
			ms->rsp_dup_num++;
			return 0;
		}
		else if (!mi->request_available) {
			/
			ms->disc_rsp_num++;
			return 0;
		}
		else {
			ms->open_req_num--;
			/
			delta.secs=pinfo->fd->abs_secs-mi->req_time.secs;
			delta.nsecs=pinfo->fd->abs_usecs*1000-mi->req_time.nsecs;
			if(delta.nsecs<0){
				delta.nsecs+=1000000000;
				delta.secs--;
			}
			
			rtd_stat_update(&(ms->rtd),delta);
			return 1;
		}
	break;

	default:
		return 0;
	break;
	}
}
