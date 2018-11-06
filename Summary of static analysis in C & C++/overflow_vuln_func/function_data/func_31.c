static const value_string opcodes_vals_sys[] =
{
	{NOP				, "NOP"},
	{PRODUCTION_TEST		, "Production Test"},	/
	{SUBDEVICE_ESCAPE		, "Subdevice Escape To Subdevice"},	/
	{SOFT_RESET			, "Software Reset"},
	{IP_PHONE_WARMSTART		, "IP-Phone Warmstart"},	/
	{HE_ROUTING			, "HE Routing Code"},	/
	{SUBDEVICE_RESET		, "Subdevice Reset"},
	{LOOPBACK_ON			, "Loopback On"},
	{LOOPBACK_OFF			, "Loopback Off"},
	{VIDEO_ROUTING			, "Video Routing Code"},	/
	{SUPER_MSG			, "Super Message"},
	{SEGMENT_MSG			, "Segment Message"},
	{REMOTE_UA_ROUTING		, "Remote UA Routing Code"},	/
	{VERY_REMOTE_UA_ROUTING		, "Very Remote UA Routing Code"},	/
	{OSI_ROUTING			, "OSI Routing Code"},	/
	{ABC_A_ROUTING			, "ABC-A Routing Code"},	/
	{IBS_ROUTING			, "IBS Routing Code"},	/
	{IP_DEVICE_ROUTING		, "IP Device Routing"},
	{M_REFLEX_HUB_ROUTING		, "Mutli-Reflex Hub Routing Code"},	/
	{SUPER_MSG_2			, "Super Message 2"},
	{DEBUG_IN_LINE			, "Debug In Line"},
	{LED_COMMAND			, "Led Command"},	/
	{START_BUZZER			, "Start Buzzer"},	/
	{STOP_BUZZER			, "Stop Buzzer"},	/
	{ENABLE_DTMF			, "Enable DTMF"},
	{DISABLE_DTMF			, "Disable DTMF"},
	{CLEAR_LCD_DISP			, "Clear LCD Display"},	/
	{LCD_LINE_1_CMD			, "LCD Line 1 Commands"},	/
	{LCD_LINE_2_CMD			, "LCD Line 2 Commands"},	/
	{MAIN_VOICE_MODE		, "Main Voice Mode"},
	{VERSION_INQUIRY		, "Version Inquiry"},
	{ARE_YOU_THERE			, "Are You There?"},
	{SUBDEVICE_METASTATE		, "Subdevice Metastate"},
	{VTA_STATUS_INQUIRY		, "VTA Status Inquiry"},	/
	{SUBDEVICE_STATE		, "Subdevice State?"},
	{DWL_DTMF_CLCK_FORMAT		, "Download DTMF & Clock Format"},	/
	{SET_CLCK			, "Set Clock"},	/
	{VOICE_CHANNEL			, "Voice Channel"},	/
	{EXTERNAL_RINGING		, "External Ringing"},
	{LCD_CURSOR			, "LCD Cursor"},	/
	{DWL_SPECIAL_CHAR		, "Download Special Character"},	/
	{SET_CLCK_TIMER_POS		, "Set Clock/Timer Position"},	/
	{SET_LCD_CONTRAST		, "Set LCD Contrast"},	/
	{AUDIO_IDLE			, "Audio Idle"},
	{SET_SPEAKER_VOL		, "Set Speaker Volume"},	/
	{BEEP				, "Beep"},
	{SIDETONE			, "Sidetone"},
	{RINGING_CADENCE		, "Set Programmable Ringing Cadence"},
	{MUTE				, "Mute"},
	{FEEDBACK			, "Feedback"},
	{KEY_RELEASE			, "Key Release"},	/
	{TRACE_ON			, "Trace On"},	/
	{TRACE_OFF			, "Trace Off"},	/
	{READ_PERIPHERAL		, "Read Peripheral"},	/
	{WRITE_PERIPHERAL		, "Write Peripheral"},	/
	{ALL_ICONS_OFF			, "All Icons Off"},	/
	{ICON_CMD			, "Icon Command"},	/
	{AMPLIFIED_HANDSET		, "Amplified Handset (Boost)"},	/
	{AUDIO_CONFIG			, "Audio Config"},
	{AUDIO_PADDED_PATH		, "Audio Padded Path"},	/
	{RELEASE_RADIO_LINK		, "Release Radio Link"},	/
	{DECT_HANDOVER			, "DECT External Handover Routing Code"},	/
	{LOUDSPEAKER			, "Loudspeaker"},
	{ANNOUNCE			, "Announce"},
	{RING				, "Ring"},
	{UA_DWL_PROTOCOL		, "UA Download Protocol"},
	{0, NULL}
};
