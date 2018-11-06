PRIVATE void tplt_xfer(const char *name, FILE *in, FILE *out, int *lineno)
{
  int i, iStart;
  char line[LINESIZE];
  while( fgets(line,LINESIZE,in) && (line[0]!='%' || line[1]!='%') ){
    (*lineno)++;
    iStart = 0;
    if( name ){
      for(i=0; line[i] && i<LINESIZE; i++){
        if( line[i]=='P' && i<(LINESIZE-5) && strncmp(&line[i],"Parse",5)==0
          && (i==0 || !safe_isalpha(line[i-1]))
        ){
          if( i>iStart ) fprintf(out,"%.*s",i-iStart,&line[iStart]);
          fprintf(out,"%s",name);
          i += 4;
          iStart = i+1;
        }
      }
    }
    fprintf(out,"%s",&line[iStart]);
  }
}
