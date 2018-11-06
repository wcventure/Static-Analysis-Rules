static gboolean _cnd_eval_capturesize(condition* cnd, va_list ap){
  cnd_capturesize_dat* data = (cnd_capturesize_dat*)cnd_get_user_data(cnd);
  /
  if(data->max_capture_size == 0) return FALSE; /
  if(va_arg(ap, long) >= data->max_capture_size){
    return TRUE;
  }
  return FALSE;
} /
