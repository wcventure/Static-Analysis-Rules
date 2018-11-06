static void
rpcprogs_draw(void *dummy _U_)
{
	rpc_program_t *rp;
	int i;
#ifdef G_HAVE_UINT64
	guint64 td;
#else
	guint32 td;
#endif

	for(rp=prog_list,i=1;rp;rp=rp->next,i++){
		/
		/
		td=(int)rp->tot.secs;
		td=td*100000+(int)rp->tot.nsecs/10000;
		if(rp->num){
			td/=rp->num;
		} else {
			td=0;
		}

		g_snprintf(rp->sprogram, sizeof(rp->sprogram), "%s",rpc_prog_name(rp->program));
		gtk_label_set_text(GTK_LABEL(rp->wprogram), rp->sprogram);

		g_snprintf(rp->sversion, sizeof(rp->sversion), "%d",rp->version);
		gtk_label_set_text(GTK_LABEL(rp->wversion), rp->sversion);

		g_snprintf(rp->snum, sizeof(rp->snum), "%d",rp->num);
		gtk_label_set_text(GTK_LABEL(rp->wnum), rp->snum);

		g_snprintf(rp->smin, sizeof(rp->smin), "%3d.%05d",(int)rp->min.secs,(int)rp->min.nsecs/10000);
		gtk_label_set_text(GTK_LABEL(rp->wmin), rp->smin);

		g_snprintf(rp->smax, sizeof(rp->smax), "%3d.%05d",(int)rp->max.secs,(int)rp->max.nsecs/10000);
		gtk_label_set_text(GTK_LABEL(rp->wmax), rp->smax);

		g_snprintf(rp->savg, sizeof(rp->savg), "%3d.%05d",(int)td/100000,(int)td%100000);
		gtk_label_set_text(GTK_LABEL(rp->wavg), rp->savg);

	}
}
