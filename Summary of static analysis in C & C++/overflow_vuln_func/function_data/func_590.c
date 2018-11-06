static condition* _cnd_constr_capturesize(condition* cnd, va_list ap){
  cnd_capturesize_dat *data = NULL;
  /
  if((data = (cnd_capturesize_dat*)g_malloc(sizeof(cnd_capturesize_dat))) == NULL)
    return NULL;
  /
  data->max_capture_size = va_arg(ap, long);
  cnd_set_user_data(cnd, (void*)data);
  return cnd;
} /
