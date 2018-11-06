typedef struct _mgcpstat_t {
	char *filter;
        rtd_t rtd;
	long int open_req_num;
	long int disc_rsp_num;
	long int req_dup_num;
	long int rsp_dup_num;
} mgcpstat_t;
