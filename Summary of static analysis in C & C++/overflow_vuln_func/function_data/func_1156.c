static guint32 drx_lookup_longCycle(guint32 idx)
{
  static const guint32 vals[] = {10,20,32,40,64,80,128,160,256,320,512,640,1024,1280,2048,2560};
  if (idx < sizeof(vals)) {
    return vals[idx];
  }
  else {
    return (sizeof(vals)/(sizeof(guint32)) - 1);
  }
}
