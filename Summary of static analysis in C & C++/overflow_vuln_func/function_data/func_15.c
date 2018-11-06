static guint64 decode_utf8(guint64 utf8)
{
	static guint64 unicode;

	if(utf8 <= G_GUINT64_CONSTANT(0xFF))
	{
		unicode =
			utf8 & G_GUINT64_CONSTANT(0x7F);
	}
	else if(utf8 <= G_GUINT64_CONSTANT(0xFFFF))
	{
		unicode =
			((utf8 & G_GUINT64_CONSTANT(0x1F00) >> 2) +
			(utf8 & G_GUINT64_CONSTANT(0x3F)));
	}
	else if(utf8 <= G_GUINT64_CONSTANT(0xFFFFFF))
	{
		unicode =
			((utf8 & G_GUINT64_CONSTANT(0x0F0000)) >> 4) +
			((utf8 & G_GUINT64_CONSTANT(0x3F00)) >> 2) +
			(utf8 & G_GUINT64_CONSTANT(0x3F));
	}
	else if(utf8 <= G_GUINT64_CONSTANT(0xFFFFFFFF))
	{
		unicode =
			((utf8 & G_GUINT64_CONSTANT(0x07000000)) >> 6) +
			((utf8 & G_GUINT64_CONSTANT(0x3F0000)) >> 4) +
			((utf8 & G_GUINT64_CONSTANT(0x3F00)) >> 2) +
			(utf8 & G_GUINT64_CONSTANT(0x3F));
	}
	else if(utf8 <= G_GUINT64_CONSTANT(0xFFFFFFFFFF))
	{
		unicode =
			((utf8 & G_GUINT64_CONSTANT(0x0300000000)) >> 8) +
			((utf8 & G_GUINT64_CONSTANT(0x3F000000)) >> 6) +
			((utf8 & G_GUINT64_CONSTANT(0x3F0000)) >> 4) +
			((utf8 & G_GUINT64_CONSTANT(0x3F00)) >> 2) +
			(utf8 & G_GUINT64_CONSTANT(0x3F));
	}
	else if(utf8 <= G_GUINT64_CONSTANT(0xFFFFFFFFFFFF))
	{
		unicode =
			((utf8 & G_GUINT64_CONSTANT(0x010000000000)) >> 10) +
			((utf8 & G_GUINT64_CONSTANT(0x3F00000000)) >> 8) +
			((utf8 & G_GUINT64_CONSTANT(0x3F000000)) >> 6) +
			((utf8 & G_GUINT64_CONSTANT(0x3F0000)) >> 4) +
			((utf8 & G_GUINT64_CONSTANT(0x3F00)) >> 2) +
			(utf8 & G_GUINT64_CONSTANT(0x3F));
	}
	else
	{
		unicode = G_GUINT64_CONSTANT(0);
	}
	return unicode;
} 
