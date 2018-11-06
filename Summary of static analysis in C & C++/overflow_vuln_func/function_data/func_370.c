static void
mgcpstat_draw(void *pms)
{
	mgcpstat_t *ms=(mgcpstat_t *)pms;
	
#ifdef G_HAVE_UINT64
	guint64 avg;
#else
	guint32 avg;
#endif
 

	/
	/
	/
	avg=(int)ms->rtd.tot.secs;
	avg=avg*100000+(int)ms->rtd.tot.nsecs/10000;
	if(ms->rtd.num){
		avg/=ms->rtd.num;
	} else {
		avg=0;
	}

	/
	printf("\n");
	printf("===================================================================\n");
	printf("MGCP Response Time Delay (RTD) Statistics:\n");
	printf("Filter: %s\n",ms->filter?ms->filter:"");
        printf("Duplicate requests: %ld\n",ms->req_dup_num);
        printf("Duplicate responses: %ld\n",ms->rsp_dup_num);
        printf("Open requests: %ld\n",ms->open_req_num);
        printf("Discarded responses: %ld\n",ms->disc_rsp_num);
        printf("Messages   |     Min RTD     |     Max RTD     |     Avg RTD \n");
        printf("%7ld    |  %5d.%03d msec |  %5d.%03d msec | %5d.%02d0 msec\n",
        	ms->rtd.num,
		(int)((ms->rtd.min.secs*1000)+(ms->rtd.min.nsecs/1000000)),(ms->rtd.min.nsecs%1000000)/1000,
		(int)((ms->rtd.max.secs*1000)+(ms->rtd.max.nsecs/1000000)),(ms->rtd.max.nsecs%1000000)/1000,
		avg/100, avg%100
	);
        printf("===================================================================\n");
}
