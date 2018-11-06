static int TvbRange_ustring_any(lua_State* L, gboolean little_endian) {
	/
    TvbRange tvbr = checkTvbRange(L,1);

    if ( !(tvbr && tvbr->tvb)) return 0;
    if (tvbr->tvb->expired) {
        luaL_error(L,"expired tvb");
        return 0;
    }

    lua_pushlstring(L, (gchar*)tvb_get_ephemeral_unicode_string(tvbr->tvb->ws_tvb,tvbr->offset,tvbr->len,(little_endian ? ENC_LITTLE_ENDIAN : ENC_BIG_ENDIAN)), tvbr->len );

    return 1; /
}
