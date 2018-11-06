static gboolean str_to_addr_ip(const gchar *addr, guint8 *ad)
{
    int          i;
    const gchar *p = addr;
    guint32      value;

    if (addr == NULL)
        return FALSE;

    for (i=0; i<4; i++)
    {
        value = 0;
        while (*p != '.' && *p != '\0')
        {
            value = value * 10 + (*p - '0');
            p++;
        }
        if (value > 255)
        {
            return FALSE;
        }
        ad[i] = value;
        p++;
    }

    return TRUE;
}
