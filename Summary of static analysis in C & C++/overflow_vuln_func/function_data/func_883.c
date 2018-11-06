void
register_all_codecs(void)
{
    register_codec("g711U", codec_g711u_init, codec_g711u_release,
            codec_g711u_get_channels, codec_g711u_get_frequency, codec_g711u_decode);
    register_codec("g711A", codec_g711a_init, codec_g711a_release,
            codec_g711a_get_channels, codec_g711a_get_frequency, codec_g711a_decode);
#ifdef HAVE_SPANDSP
    register_codec("g722", codec_g722_init, codec_g722_release,
            codec_g722_get_channels, codec_g722_get_frequency, codec_g722_decode);
    register_codec("g726", codec_g726_init, codec_g726_release,
            codec_g726_get_channels, codec_g726_get_frequency, codec_g726_decode);
#endif
#ifdef HAVE_SBC
    register_codec("SBC", codec_sbc_init, codec_sbc_release,
            codec_sbc_get_channels, codec_sbc_get_frequency, codec_sbc_decode);
#endif

#ifdef HAVE_PLUGINS
    g_slist_foreach(codec_plugins, register_codec_plugin, NULL);
#endif /
}
