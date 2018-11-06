static const value_string servers_vals[] = {
	{0x15,	"Call Server"},
	{0x16,	"Presentation Server"},
	{0, NULL}
};
static const value_string servers_short_vals[] = {
	{0x15,	"CS"},
	{0x16,	"PS"},
	{0, NULL}
};
enum
{
	METHOD_CREATE		= 0x00,
	METHOD_DELETE		= 0x01,
	METHOD_SET_PROPERTY	= 0x02,
	METHOD_GET_PROPERTY	= 0x03,
	METHOD_NOTIFY		= 0x04,
	METHOD_DELETE_ITEM	= 0x05,
	METHOD_INSERT_ITEM	= 0x06,
	METHOD_INVALID
};
