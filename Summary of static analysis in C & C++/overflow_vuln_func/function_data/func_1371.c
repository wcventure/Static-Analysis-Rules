static gint compareAddresses(gconstpointer aa, gconstpointer bb)
{
      const address *a = (const address *)aa;
      const address *b = (const address *)bb;
      int c = b -> type - a -> type;
      if (c) return c;
      c = b -> len - a -> len;
      if (c) return c;
      return memcmp(b -> data, a -> data, a -> len);
}
