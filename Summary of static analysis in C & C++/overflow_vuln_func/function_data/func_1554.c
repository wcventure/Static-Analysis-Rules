static int TvbRange_ustringz_any(lua_State* L, gboolean little_endian) {
	/
    gint count;
    TvbRange tvbr = checkTvbRange(L,1);

    if ( !(tvbr && tvbr->tvb)) return 0;
    if (tvbr->tvb->expired) {
        luaL_error(L,"expired tvb");
        return 0;
    }

    if (tvb_find_guint8 (tvbr->tvb->ws_tvb, tvbr->offset, -1, 0) == -1) {
        luaL_error(L,"out of bounds");
        return 0;
    }

    lua_pushstring(L, (gchar*)tvb_get_stringz_enc(wmem_packet_scope(),tvbr->tvb->ws_tvb,tvbr->offset,&count,(little_endian ? ENC_UTF_16|ENC_LITTLE_ENDIAN : ENC_UTF_16|ENC_BIG_ENDIAN)) );
    lua_pushinteger(L,count);

    return 2; /
}
