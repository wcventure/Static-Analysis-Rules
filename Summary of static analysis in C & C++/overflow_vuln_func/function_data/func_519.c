static char *
object_id_to_string(gint offset, tvbuff_t *tvb, char buff[])
{
  guint32        objectId = tvb_get_ntohl(tvb, offset);
  /

  if (objectId == OID_UNKNOWN)      { sprintf(buff,"Unknown ObjectId");
                                      return(buff);}
  if (objectId == OID_APP)          { sprintf(buff,"applicationSelf");
                                      return(buff);}
  if (objectId == OID_WRITE_APPSELF){ sprintf(buff,"writerApplicationSelf");
                                      return(buff);}
  if (objectId == OID_WRITE_APP)    { sprintf(buff,"writerApplications");
                                      return(buff);}
  if (objectId == OID_READ_APP)     { sprintf(buff,"readerApplications");
                                      return(buff);}
  if (objectId == OID_WRITE_MGR)    { sprintf(buff,"writerManagers");
                                      return(buff);}
  if (objectId == OID_READ_MGR)     { sprintf(buff,"readerManagers ");
                                      return(buff);}
  if (objectId == OID_WRITE_PUBL)   { sprintf(buff,"writerPublications");
                                      return(buff);}
  if (objectId == OID_READ_PUBL)    { sprintf(buff,"readerPublications");
                                      return(buff);}
  if (objectId == OID_WRITE_SUBS)   { sprintf(buff,"writerSubscriptions");
                                      return(buff);}
  if (objectId == OID_READ_SUBS)    { sprintf(buff,"readerSubscriptions");
                                      return(buff);}

  /
  sprintf(buff,"instanceId: 0x%X, objKind: 0x%X",
               (objectId >> 8),(objectId & 0xff));
  return(buff);

/
}
