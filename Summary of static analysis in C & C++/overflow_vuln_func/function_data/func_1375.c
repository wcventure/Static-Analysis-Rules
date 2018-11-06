typedef struct {
	direction_enum	dir;
	int		cnt;
	gboolean	esc;
	guint8		buf[8192];
	long		id_offset;
} pkt_t;
