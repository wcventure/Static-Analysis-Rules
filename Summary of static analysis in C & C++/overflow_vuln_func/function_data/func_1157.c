static guint32 drx_lookup_shortCycle(guint32 idx)
{
  static const guint32 vals[] = {2,5,8,10,16,20,32,40,64,80,128,160,256,320,512,640};
  if (idx < sizeof(vals)) {
    return vals[idx];
  }
  else {
    return (sizeof(vals)/(sizeof(guint32)) - 1);
  }
}
