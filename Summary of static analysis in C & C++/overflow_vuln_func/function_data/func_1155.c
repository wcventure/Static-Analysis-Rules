static guint32 drx_lookup_retransmissionTimer(guint32 idx)
{
  static const guint32 vals[] = {1,2,4,6,8,16,24,33};
  if (idx < sizeof(vals)) {
    return vals[idx];
  }
  else {
    return (sizeof(vals)/(sizeof(guint32)) - 1);
  }
}
