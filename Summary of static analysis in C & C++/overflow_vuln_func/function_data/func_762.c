const char* oid_subid2string(guint32* subids, guint len) {
	char* s = ep_alloc0(len*11);
	char* w = s;
	
	DISSECTOR_ASSERT(subids);

	do {
		w += sprintf(w,"%u.",*subids++);
	} while(--len);
	
	if (w!=s) *(w-1) = '\0'; else *(w) = '\0';
	
	return s;
}
