typedef struct _rpc_call_info {
	guint32	xid;
	conversation_t *conversation;
	guint32	req_num;	/
	guint32	rep_num;	/
	guint32	prog;
	guint32	vers;
	guint32	proc;
	guint32 flavor;
	guint32 gss_proc;
	guint32 gss_svc;
	rpc_proc_info_value*	proc_info;
} rpc_call_info;
