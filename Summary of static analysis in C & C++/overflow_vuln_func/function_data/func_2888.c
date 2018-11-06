static void init_timetables( FM_OPL *OPL , int ARRATE , int DRRATE )
{
	int i;
	double rate;

	/
	for (i = 0;i < 4;i++) OPL->AR_TABLE[i] = OPL->DR_TABLE[i] = 0;
	for (i = 4;i <= 60;i++){
		rate  = OPL->freqbase;						/
		if( i < 60 ) rate *= 1.0+(i&3)*0.25;		/
		rate *= 1<<((i>>2)-1);						/
		rate *= (double)(EG_ENT<<ENV_BITS);
		OPL->AR_TABLE[i] = rate / ARRATE;
		OPL->DR_TABLE[i] = rate / DRRATE;
	}
	for (i = 60;i < 76;i++)
	{
		OPL->AR_TABLE[i] = EG_AED-1;
		OPL->DR_TABLE[i] = OPL->DR_TABLE[60];
	}
#if 0
		rate *= (double)(EG_ENT<<ENV_BITS);
		OPL->AR_TABLE[i] = rate / ARRATE;
		OPL->DR_TABLE[i] = rate / DRRATE;
	}
	for (i = 60;i < 76;i++)
	{
		OPL->AR_TABLE[i] = EG_AED-1;
		OPL->DR_TABLE[i] = OPL->DR_TABLE[60];
	}
#if 0
	for (i = 0;i < 64 ;i++){	/
		LOG(LOG_WAR,("rate %2d , ar %f ms , dr %f ms \n",i,
			((double)(EG_ENT<<ENV_BITS) / OPL->AR_TABLE[i]) * (1000.0 / OPL->rate),
			((double)(EG_ENT<<ENV_BITS) / OPL->DR_TABLE[i]) * (1000.0 / OPL->rate) ));
	}
#endif
}
