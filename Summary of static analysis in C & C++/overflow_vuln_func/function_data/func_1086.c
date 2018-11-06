static void
eth_hash_to_texbuff(gpointer key, gpointer value, gpointer user_data)
{
    gchar string_buff[ADDRESS_STR_MAX];
    GtkTextBuffer *buffer = (GtkTextBuffer*)user_data;
    gint64 eth_as_gint64 = *(gint64*)key;
    hashether_t* tp = (hashether_t*)value;

    g_snprintf(string_buff, ADDRESS_STR_MAX, "%.2X:%.2X:%.2X:%.2X:%.2X:%.2X Status: %u %s %s\n",
               (guint8)(eth_as_gint64>>40&0xff),
               (guint8)(eth_as_gint64>>32&0xff),
               (guint8)((eth_as_gint64>>24)&0xff),
               (guint8)((eth_as_gint64>>16)&0xff),
               (guint8)((eth_as_gint64>>8)&0xff),
               (guint8)(eth_as_gint64&0xff),
               get_hash_ether_status(tp),
               get_hash_ether_hexaddr(tp),
               get_hash_ether_resolved_name(tp));
    gtk_text_buffer_insert_at_cursor (buffer, string_buff, -1);

}
