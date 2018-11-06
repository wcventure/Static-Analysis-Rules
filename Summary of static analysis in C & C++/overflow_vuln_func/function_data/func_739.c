char *
decode_bits_in_field(const gint bit_offset, const gint no_of_bits, const guint64 value)
{
	guint64 mask = 0,tmp;
	char *str;
	int bit;
	int i;

	mask = 1;
	mask = mask << (no_of_bits-1);

	/
	str=ep_alloc(256);
	str[0]='\0';
	for(bit=0;bit<((int)(bit_offset&0x07));bit++){
		if(bit&&(!(bit%4))){
			g_strlcat(str, " ", 256);
		}
		g_strlcat(str, ".", 256);
	}

	/
	for(i=0;i<no_of_bits;i++){
		if(bit&&(!(bit%4))){
			g_strlcat(str, " ", 256);
		}
		if(bit&&(!(bit%8))){
			g_strlcat(str, " ", 256);
		}
		bit++;
		tmp = value & mask;
		if(tmp != 0){
			g_strlcat(str, "1", 256);
		} else {
			g_strlcat(str, "0", 256);
		}
		mask = mask>>1;
	}

	for(;bit%8;bit++){
		if(bit&&(!(bit%4))){
			g_strlcat(str, " ", 256);
		}
		g_strlcat(str, ".", 256);
	}
	return str;
}
