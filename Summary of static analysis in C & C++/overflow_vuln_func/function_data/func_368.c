void
rtd_stat_update(rtd_t *rtd,nstime_t delta)
{
	rtd->num++;
	if((rtd->max.secs==0)
	&& (rtd->max.nsecs==0) ){
		rtd->max.secs=delta.secs;
		rtd->max.nsecs=delta.nsecs;
	}
	
	if((rtd->min.secs==0)
	&& (rtd->min.nsecs==0) ){
		rtd->min.secs=delta.secs;
		rtd->min.nsecs=delta.nsecs;
	}
	
	if( (delta.secs<rtd->min.secs)
	||( (delta.secs==rtd->min.secs)
	  &&(delta.nsecs<rtd->min.nsecs) ) ){
		rtd->min.secs=delta.secs;
		rtd->min.nsecs=delta.nsecs;
	}
	
	if( (delta.secs>rtd->max.secs)
	||( (delta.secs==rtd->max.secs)
	  &&(delta.nsecs>rtd->max.nsecs) ) ){
		rtd->max.secs=delta.secs;
		rtd->max.nsecs=delta.nsecs;
	}
		
	rtd->tot.secs += delta.secs;
	rtd->tot.nsecs += delta.nsecs;
	if(rtd->tot.nsecs>1000000000){
		rtd->tot.nsecs-=1000000000;
		rtd->tot.secs++;	
	}
	
	
}
