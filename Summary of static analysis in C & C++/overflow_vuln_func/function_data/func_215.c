void
ssl_private_key_free(gpointer id, gpointer key, gpointer dummy _U_)
{
    g_free(id);
    ssl_free_key((Ssl_private_key_t*) key);
}
