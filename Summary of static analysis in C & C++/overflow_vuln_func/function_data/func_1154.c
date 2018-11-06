static guint32 drx_lookup_inactivityTimer(guint32 idx)
{
  static const guint32 vals[] = {1,2,3,4,5,6,8,10,20,30,40,50,60,80,100,200,300,
                          500,750,1280,1920,2560,0};
  if (idx < sizeof(vals)) {
    return vals[idx];
  }
  else {
    return (sizeof(vals)/(sizeof(guint32)) - 1);
  }
}
