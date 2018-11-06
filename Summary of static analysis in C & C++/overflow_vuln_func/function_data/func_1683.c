static int TreeItem_add_item_any(lua_State *L, gboolean little_endian) {
    TvbRange tvbr;
    Proto proto;
    ProtoField field;
    int hfid = -1;
    int ett = -1;
    ftenum_t type = FT_NONE;
    TreeItem tree_item  = shiftTreeItem(L,1);
    proto_item* item = NULL;

    if (!tree_item) {
        return luaL_error(L,"not a TreeItem!");
    }
    if (tree_item->expired) {
        luaL_error(L,"expired TreeItem");
        return 0;
    }

    if (! ( field = shiftProtoField(L,1) ) ) {
        if (( proto = shiftProto(L,1) )) {
            hfid = proto->hfid;
            type = FT_PROTOCOL;
            ett = proto->ett;
        }
    } else {
        hfid = field->hfid;
        type = field->type;
        ett = field->ett;
    }

    tvbr = shiftTvbRange(L,1);

    if (!tvbr) {
        tvbr = wmem_new(wmem_packet_scope(), struct _wslua_tvbrange);
        tvbr->tvb = wmem_new(wmem_packet_scope(), struct _wslua_tvb);
        tvbr->tvb->ws_tvb = lua_tvb;
        tvbr->offset = 0;
        tvbr->len = 0;
    }

    if (hfid > 0 ) {
        /

        if (lua_gettop(L)) {
            /

            switch(type) {
                case FT_PROTOCOL:
                    item = proto_tree_add_item(tree_item->tree,hfid,tvbr->tvb->ws_tvb,tvbr->offset,tvbr->len,ENC_NA);
                    lua_pushnumber(L,0);
                    lua_insert(L,1);
                    break;
                case FT_BOOLEAN:
                    {
                        /
                        guint32 val = (guint32) (wslua_tointeger(L,1));
                        item = proto_tree_add_boolean(tree_item->tree,hfid,tvbr->tvb->ws_tvb,tvbr->offset,tvbr->len,val);
                    }
                    break;
                case FT_UINT8:
                case FT_UINT16:
                case FT_UINT24:
                case FT_UINT32:
                case FT_FRAMENUM:
                    item = proto_tree_add_uint(tree_item->tree,hfid,tvbr->tvb->ws_tvb,tvbr->offset,tvbr->len,wslua_checkguint32(L,1));
                    break;
                case FT_INT8:
                case FT_INT16:
                case FT_INT24:
                case FT_INT32:
                    item = proto_tree_add_int(tree_item->tree,hfid,tvbr->tvb->ws_tvb,tvbr->offset,tvbr->len,wslua_checkguint32(L,1));
                    break;
                case FT_FLOAT:
                    item = proto_tree_add_float(tree_item->tree,hfid,tvbr->tvb->ws_tvb,tvbr->offset,tvbr->len,(float)luaL_checknumber(L,1));
                    break;
                case FT_DOUBLE:
                    item = proto_tree_add_double(tree_item->tree,hfid,tvbr->tvb->ws_tvb,tvbr->offset,tvbr->len,(double)luaL_checknumber(L,1));
                    break;
                case FT_ABSOLUTE_TIME:
                case FT_RELATIVE_TIME:
                    item = proto_tree_add_time(tree_item->tree,hfid,tvbr->tvb->ws_tvb,tvbr->offset,tvbr->len,checkNSTime(L,1));
                    break;
                case FT_STRING:
                    item = proto_tree_add_string(tree_item->tree,hfid,tvbr->tvb->ws_tvb,tvbr->offset,tvbr->len,luaL_checkstring(L,1));
                    break;
                case FT_STRINGZ:
                    item = proto_tree_add_string(tree_item->tree,hfid,tvbr->tvb->ws_tvb,tvbr->offset,tvb_strsize (tvbr->tvb->ws_tvb, tvbr->offset),luaL_checkstring(L,1));
                    break;
                case FT_BYTES:
                    item = proto_tree_add_bytes(tree_item->tree,hfid,tvbr->tvb->ws_tvb,tvbr->offset,tvbr->len, (const guint8*) luaL_checkstring(L,1));
                    break;
                case FT_UINT64:
                    item = proto_tree_add_uint64(tree_item->tree,hfid,tvbr->tvb->ws_tvb,tvbr->offset,tvbr->len,checkUInt64(L,1));
                    break;
                case FT_INT64:
                    item = proto_tree_add_int64(tree_item->tree,hfid,tvbr->tvb->ws_tvb,tvbr->offset,tvbr->len,checkInt64(L,1));
                    break;
                case FT_IPv4:
                    item = proto_tree_add_ipv4(tree_item->tree,hfid,tvbr->tvb->ws_tvb,tvbr->offset,tvbr->len,*((const guint32*)(checkAddress(L,1)->data)));
                    break;
                case FT_ETHER:
                case FT_UINT_BYTES:
                case FT_IPv6:
                case FT_IPXNET:
                case FT_GUID:
                case FT_OID:
                case FT_REL_OID:
                case FT_SYSTEM_ID:
                case FT_VINES:
                case FT_FCWWN:
                default:
                    luaL_error(L,"FT_ not yet supported");
                    return 0;
            }

            lua_remove(L,1);

        } else {
            if (type == FT_FRAMENUM) {
                luaL_error(L, "ProtoField FRAMENUM cannot fetch value from Tvb");
                return 0;
            }
            /
            if (type == FT_STRINGZ) tvbr->len = tvb_strsize (tvbr->tvb->ws_tvb, tvbr->offset);
            item = proto_tree_add_item(tree_item->tree, hfid, tvbr->tvb->ws_tvb, tvbr->offset, tvbr->len, little_endian ? ENC_LITTLE_ENDIAN : ENC_BIG_ENDIAN);
        }

        if ( lua_gettop(L) ) {
            /
            const gchar* s = lua_tostring(L,1);
            if (s) proto_item_set_text(item,"%s",s);
            lua_remove(L,1);
        }

    } else {
        /
        if (lua_gettop(L)) {
            const gchar* s = lua_tostring(L,1);
            const int hf = get_hf_wslua_text();
            if (hf > -1) {
                /
                item = proto_tree_add_item(tree_item->tree, hf, tvbr->tvb->ws_tvb, tvbr->offset, tvbr->len, ENC_NA);
                proto_item_set_text(item, "%s", s);
            } else {
                luaL_error(L,"Internal error: hf_wslua_text not registered");
            }
            lua_remove(L,1);
        } else {
            luaL_error(L,"Tree item ProtoField/Protocol handle is invalid (ProtoField/Proto not registered?)");
        }
    }

    while(lua_gettop(L)) {
        /
        const gchar* s = lua_tostring(L,1);
        if (s) proto_item_append_text(item, " %s", s);
        lua_remove(L,1);
    }

    push_TreeItem(L, proto_item_add_subtree(item,ett > 0 ? ett : wslua_ett), item);

    return 1;
}
