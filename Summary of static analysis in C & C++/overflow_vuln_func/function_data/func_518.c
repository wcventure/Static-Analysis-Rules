static char *
app_id_to_string(gint offset,tvbuff_t *tvb,char buff[])
{
  guint32        appId = tvb_get_ntohl(tvb, offset);
  /

  /
  guint32        instanceId = (appId >> 8);
  /
  guint8         appKind    = (appId & 0xff);

  if (appKind == MANAGEDAPPLICATION)
  {
    sprintf(buff,"Managed App, InstanceId: 0x%X",instanceId);
    return(buff);
  }

  if (appKind == MANAGER)
  {
    sprintf(buff,"Manager, InstanceId: 0x%X",instanceId);
    return(buff);
  }

  sprintf(buff,"Unknown");
  return(buff);

}
