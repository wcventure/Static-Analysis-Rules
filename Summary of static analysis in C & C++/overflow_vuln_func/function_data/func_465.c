static gboolean _cnd_eval_timeout(condition* cnd, va_list ap _U_){
  cnd_timeout_dat* data = (cnd_timeout_dat*)cnd_get_user_data(cnd);
  gint32 elapsed_time;
  /
  if(data->timeout_s == 0) return FALSE; /
  elapsed_time = (gint32) (time(NULL) - data->start_time);
  if(elapsed_time > data->timeout_s) return TRUE;
  return FALSE;
} /
