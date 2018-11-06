PRIVATE void translate_code(struct lemon *lemp, struct rule *rp){
  char *cp, *xp;
  int i;
  char lhsused = 0;    /
  char used[MAXRHS];   /

  for(i=0; i<rp->nrhs; i++) used[i] = 0;
  lhsused = 0;

  if (!rp->code) {
    static char newlinestr[2] = { '\n', '\0' };
    rp->code = newlinestr;
    rp->line = rp->ruleline;
  }

  append_str(0,0,0,0);
  /
  for(cp=(char *)rp->code; *cp; cp++){
    if( safe_isalpha(*cp) && (cp==rp->code || (!safe_isalnum(cp[-1]) && cp[-1]!='_')) ){
      char saved;
      for(xp= &cp[1]; safe_isalnum(*xp) || *xp=='_'; xp++);
      saved = *xp;
      *xp = 0;
      if( rp->lhsalias && strcmp(cp,rp->lhsalias)==0 ){
        append_str("yygotominor.yy%d",0,rp->lhs->dtnum,0);
        cp = xp;
        lhsused = 1;
      }else{
        for(i=0; i<rp->nrhs; i++){
          if( rp->rhsalias[i] && strcmp(cp,rp->rhsalias[i])==0 ){
            if( cp!=rp->code && cp[-1]=='@' ){
              /
              append_str("yymsp[%d].major",-1,i-rp->nrhs+1,0);
            }else{
              struct symbol *sp = rp->rhs[i];
              int dtnum;
              if( sp->type==MULTITERMINAL ){
                dtnum = sp->subsym[0]->dtnum;
              }else{
                dtnum = sp->dtnum;
              }
              append_str("yymsp[%d].minor.yy%d",0,i-rp->nrhs+1, dtnum);
            }
            cp = xp;
            used[i] = 1;
            break;
          }
        }
      }
      *xp = saved;
    }
    append_str(cp, 1, 0, 0);
  } /

  /
  if( rp->lhsalias && !lhsused ){
    ErrorMsg(lemp->filename,rp->ruleline,
      "Label \"%s\" for \"%s(%s)\" is never used.",
        rp->lhsalias,rp->lhs->name,rp->lhsalias);
    lemp->errorcnt++;
  }

  /
  for(i=0; i<rp->nrhs; i++){
    if( rp->rhsalias[i] && !used[i] ){
      ErrorMsg(lemp->filename,rp->ruleline,
        "Label %s for \"%s(%s)\" is never used.",
        rp->rhsalias[i],rp->rhs[i]->name,rp->rhsalias[i]);
      lemp->errorcnt++;
    }else if( rp->rhsalias[i]==0 ){
      if( has_destructor(rp->rhs[i],lemp) ){
        append_str("  yy_destructor(yypParser,%d,&yymsp[%d].minor);\n", 0,
           rp->rhs[i]->index,i-rp->nrhs+1);
      }else{
        /
      }
    }
  }
  if( rp->code ){
    cp = append_str(0,0,0,0);
    rp->code = Strsafe(cp?cp:"");
  }
}
