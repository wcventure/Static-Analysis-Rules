static const value_string opcodes_vals_term[] =
{
	{CS_NOP_ACK			, "NOP Acknowledge"},
	{CS_HANDSET_OFFHOOK		, "Handset Offhook"},	/
	{CS_HANDSET_ONHOOK		, "Hansdet Onhook"},	/
	{CS_DIGIT_DIALED		, "Digital Dialed"},	/
	{CS_SUBDEVICE_MSG		, "Subdevice Message"},
	{CS_HE_ROUTING			, "HE Routing Response Code"},	/
	{CS_LOOPBACK_ON			, "Loopback On Acknowledge"},	/
	{CS_LOOPBACK_OFF		, "Loopback Off Acknowledge"},	/
	{CS_VIDEO_ROUTING		, "Video Routing Response Code"},	/
	{CS_WARMSTART_ACK		, "Warmstart Acknowledge"},	/
	{CS_SUPER_MSG			, "Super Message"},	/
	{CS_SEGMENT_MSG			, "Segment Message"},	/
	{CS_REMOTE_UA_ROUTING		, "Remote UA Routing Response Code"},	/
	{CS_VERY_REMOTE_UA_R		, "Very Remote UA Routing Response Code"},	/
	{CS_OSI_ROUTING			, "OSI Response Code"},	/
	{CS_ABC_A_ROUTING		, "ABC-A Routing Response Code"},	/
	{CS_IBS_ROUTING			, "IBS Routing Response Code"},	/
	{CS_IP_DEVICE_ROUTING		, "IP Device Routing"},
	{CS_SUPER_MSG_2			, "Super Message 2"},	/
	{CS_DEBUG_IN_LINE		, "Debug Message"},
	{CS_UNSOLICITED_MSG		, "Unsolicited Message"},
	{CS_NON_DIGIT_KEY_PUSHED	, "Non-Digit Key Pushed"},	/
	{CS_VERSION_RESPONSE		, "Version Information"},
	{CS_I_M_HERE			, "I'm Here Response"},
	{CS_RSP_STATUS_INQUIRY		, "Response To Status Inquiry"},	/
	{CS_SUBDEVICE_STATE		, "Subdevice State Response"},
	{CS_DIGIT_KEY_RELEASED		, "Digit Key Released"},	/
	{CS_TRACE_ON_ACK		, "Trace On Acknowledge"},	/
	{CS_TRACE_OFF_ACK		, "Trace Off Acknowledge"},	/
	{CS_SPECIAL_KEY_STATUS		, "Special Key Status"},	/
	{CS_KEY_RELEASED		, "Key Released"},	/
	{CS_PERIPHERAL_CONTENT		, "Peripheral Content"},	/
	{CS_TM_KEY_PUSHED		, "TM Key Pushed"},	/
	{CS_UA_DWL_PROTOCOL		, "Download Protocol"},
	{0, NULL}
};
