static void
mgcpstat_init(char *optarg)
{
	mgcpstat_t *ms;
	char *filter=NULL;


	if(!strncmp(optarg,"mgcp,rtd,",9)){
		filter=optarg+9;
	} else {
		filter=NULL;
	}

	ms=g_malloc(sizeof(mgcpstat_t));
	if(filter){
		ms->filter=g_malloc(strlen(filter)+1);
		strcpy(ms->filter, filter);
	} else {
		ms->filter=NULL;
	}

	ms->rtd.num=0;
	ms->rtd.min.secs=0;
        ms->rtd.min.nsecs=0;
        ms->rtd.max.secs=0;
        ms->rtd.max.nsecs=0;
        ms->rtd.tot.secs=0;
        ms->rtd.tot.nsecs=0;

	ms->open_req_num=0;
	ms->disc_rsp_num=0;
	ms->req_dup_num=0;
	ms->rsp_dup_num=0;

	if(register_tap_listener("mgcp", ms, filter, NULL, mgcpstat_packet, mgcpstat_draw)){
		/
		g_free(ms->filter);
		g_free(ms);

		fprintf(stderr,"tethereal: mgcpstat_init() failed to attach to tap.\n");
		exit(1);
	}
}
