int
dissect_nt_sid(tvbuff_t *tvb, int offset, proto_tree *parent_tree, char *name,
	       char **sid_str, int hf_sid)
{
	proto_item *item = NULL;
	proto_tree *tree = NULL;
	int old_offset = offset, sa_offset = offset;
	gboolean rid_present;
	guint rid=0;
	int rid_offset=0;
	guint8 revision;
	int rev_offset;
	guint8 num_auth;
	int na_offset;
        guint auth = 0;   /
	int i;
	GString *gstr;
	char sid_string[245];
	char *sid_name;

	if(hf_sid==-1){
		hf_sid=hf_nt_sid;
	}

	/
	revision = tvb_get_guint8(tvb, offset);
	rev_offset = offset;
	offset += 1;

	switch(revision){
	case 1:
	case 2:  /
	  /
	  num_auth = tvb_get_guint8(tvb, offset);
          na_offset = offset;
	  offset += 1;

	  /
	  /

	  for(i=0;i<6;i++){
	    auth = (auth << 8) + tvb_get_guint8(tvb, offset);

	    offset++;
	  }

          sa_offset = offset;

          gstr = g_string_new("");

	  CLEANUP_PUSH(free_g_string, gstr);

	  /
	  for(i=0; i < (num_auth > 4?(num_auth - 1):num_auth); i++){
	    /
             g_string_sprintfa(gstr, (i>0 ? "-%u" : "%u"),
                  tvb_get_letohl(tvb, offset));
             offset+=4;
	  }


	  if (num_auth > 4) {
	    rid = tvb_get_letohl(tvb, offset);
            rid_present=TRUE;
            rid_offset=offset;
	    offset+=4;
            sprintf(sid_string, "S-1-%u-%s-%u", auth, gstr->str, rid);
	  } else {
            rid_present=FALSE;
            sprintf(sid_string, "S-1-%u-%s", auth, gstr->str);
          }

          sid_name=NULL;
          if(sid_name_snooping){
            sid_name=find_sid_name(sid_string);
          }

          if(parent_tree){
            if(sid_name){
              item = proto_tree_add_string_format(parent_tree, hf_sid, tvb, old_offset, offset-old_offset, sid_string, "%s: %s (%s)", name, sid_string, sid_name);
            } else {
              item = proto_tree_add_string_format(parent_tree, hf_sid, tvb, old_offset, offset-old_offset, sid_string, "%s: %s", name, sid_string);
            }
            tree = proto_item_add_subtree(item, ett_nt_sid);
          }

          proto_tree_add_item(tree, hf_nt_sid_revision, tvb, rev_offset, 1, TRUE);
          proto_tree_add_item(tree, hf_nt_sid_num_auth, tvb, na_offset, 1, TRUE);
          proto_tree_add_text(tree, tvb, na_offset+1, 6, "Authority: %u", auth);
          proto_tree_add_text(tree, tvb, sa_offset, num_auth * 4, "Sub-authorities: %s", gstr->str);

          if(rid_present){
            proto_tree_add_text(tree, tvb, rid_offset, 4, "RID: %u", rid);
          }

          if(sid_str){
            if(sid_name){
              *sid_str = g_strdup_printf("%s (%s)", sid_string, sid_name);
            } else {
              *sid_str = g_strdup(sid_string);
            }
          }

	  CLEANUP_CALL_AND_POP;
	}


	return offset;
}
