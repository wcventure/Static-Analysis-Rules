gchar*	
address_to_str(const address *addr)
{
#ifndef INET6_ADDRSTRLEN
#define INET6_ADDRSTRLEN 46
#endif
  static int i=0;
  static gchar *strp, str[16][INET6_ADDRSTRLEN];/

  i++;
  if(i>=16){
    i=0;
  }
  strp=str[i];

  address_to_str_buf(addr, strp);
  return strp;
}
