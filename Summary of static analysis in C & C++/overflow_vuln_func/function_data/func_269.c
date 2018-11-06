static oid_info_t* add_oid(const char* name, oid_kind_t kind, const oid_value_type_t* type, oid_key_t* key, guint oid_len, guint32 *subids) {
	guint i = 0;
	oid_info_t* c = &oid_root;
	
	if (!oid_root.children) {
		char* debug_env = getenv("WIRESHARK_DEBUG_MIBS");
		
		debuglevel = debug_env ? strtoul(debug_env,NULL,10) : 0;
				
		oid_root.children = pe_tree_create(EMEM_TREE_TYPE_RED_BLACK,"oid_root");
	}
	
	oid_len--;
	
	do {
		oid_info_t* n = emem_tree_lookup32(c->children,subids[i]);
		
		if(n) {
			if (i == oid_len) {
				if (n->name) {
					D(0,("RENAMING %s -> %s",n->name,name));
					g_free(n->name);
				}
				
				n->name = g_strdup(name);

				if (! n->value_type) {
					n->value_type = type;
				}
				
				return n;
			}
		} else {
			n = g_malloc(sizeof(oid_info_t));
			n->subid = subids[i];
			n->kind = kind;
			n->children = pe_tree_create(EMEM_TREE_TYPE_RED_BLACK,"oid_children");
			n->value_hfid = -2;
			n->key = key;
			n->parent = c;
			n->bits = NULL;

			emem_tree_insert32(c->children,n->subid,n);

			if (i == oid_len) {
				n->name = g_strdup(name);
				n->value_type = type;
				n->kind = kind;
				return n;
			} else {
				n->name = NULL;
				n->value_type = NULL;
				n->kind = OID_KIND_UNKNOWN;
			}
		}
		c = n;
	} while(++i);
	
	g_assert_not_reached();
	return NULL;
}
