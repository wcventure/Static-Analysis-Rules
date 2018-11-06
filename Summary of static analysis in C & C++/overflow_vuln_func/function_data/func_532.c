gchar *
col_format_to_string(gint fmt) {
  gchar *slist[] = { "%m", "%t", "%Rt", "%At", "%Yt", "%Tt", "%s", "%rs",
                     "%us","%hs", "%rhs", "%uhs", "%ns", "%rns", "%uns", "%d",
                     "%rd", "%ud", "%hd", "%rhd", "%uhd", "%nd", "%rnd",
                     "%und", "%S", "%rS", "%uS", "%D", "%rD", "%uD", "%p",
                     "%i", "%L", "%B", "%XO", "%XR", "%I", "%c", "%Xs", 
                     "%Xd", "%V", "%x", "%e", "%H", "%P" };
                     
  if (fmt < 0 || fmt > NUM_COL_FMTS)
    return NULL;

  return(slist[fmt]);
}
