static char *decode_key_name(int unicode)
{
	static char key_name[10];

	if((unicode <= 0x20)
		|| (unicode == 0x7F)
		|| (unicode == 0xE0)
		|| (unicode == 0xE7)
		|| (unicode == 0xE8)
		|| (unicode == 0xE9)
		|| (unicode == 0xF9))
	{
		g_snprintf(key_name, 10, "%s", val_to_str(unicode, str_key_name, "Unknown"));
	}
	else if(unicode <= 0xFF)
	{
		g_snprintf(key_name, 10, "%c", unicode);
	}
	else
	{
		g_snprintf(key_name, 10, "%s", val_to_str(unicode, str_key_name, "Unknown"));
	}
	return key_name;
} 
